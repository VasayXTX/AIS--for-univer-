#include "../headers/table.h"

using namespace Settings;

inline bool IsEditAdminOnly(ETable aType)
{
    return aType == tDeveloper || aType == tCompany || aType == tContract || aType ==tProject;
}

Table::Table(QWidget *aParent, ETable aType):
        QMainWindow(aParent, Qt::SubWindow),
        type(aType),
        countInsertCard(0)
{
    setAttribute(Qt::WA_DeleteOnClose);

    //Создание модели с поддержкой внешних кючей и таблицы для отображения этой модели
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnRowChange);
    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection); //Режим выделения - одиночное
    view->setSelectionBehavior(QAbstractItemView::SelectRows);  //Режим работы выделения - строка полностью
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);   //отключение возможности переименования элемента

    //Создание ToolBar'а на форме
    tb = new QToolBar;
    tb->setFloatable(false);       //отключаем произвольный перенос
    actFirst = tb->addAction(tr("&First"), this, SLOT(SetFirst()));
    actFirst->setIcon(QIcon(":/Resource/Images/First.ico"));
    actPrev = tb->addAction(tr("&Prev"), this, SLOT(SetPrev()));
    actPrev->setIcon(QIcon(":/Resource/Images/Prev.ico"));
    actNext = tb->addAction(tr("&Next"), this, SLOT(SetNext()));
    actNext->setIcon(QIcon(":/Resource/Images/Next.ico"));
    actLast = tb->addAction(tr("&Last"), this, SLOT(SetLast()));
    actLast->setIcon(QIcon(":/Resource/Images/Last.ico"));

    if (Developer::curIsAdm || !IsEditAdminOnly(aType))     //права админа
    {
        tb->addSeparator();
        actAdd = tb->addAction(tr("&Add"), this, SLOT(Add()));
        actAdd->setIcon(QIcon(":/Resource/Images/Add.ico"));
        actEdit = tb->addAction(tr("&Edit"), this, SLOT(Edit()));
        actEdit->setIcon(QIcon(":/Resource/Images/Edit.png"));
        actDel = tb->addAction(tr("&Del"), this, SLOT(Del()));
        actDel->setIcon(QIcon(":/Resource/Images/Delete.ico"));

        connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OpenCard(QModelIndex)));
    }
    else if (aType == tProject)
    {
        tb->addSeparator();
        actEdit = tb->addAction(tr("&Edit"), this, SLOT(Edit()));
        actEdit->setIcon(QIcon(":/Resource/Images/Edit.png"));
        connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OpenCard(QModelIndex)));
    }

    addToolBar(tb);
    setCentralWidget(view);

    mdiArea = static_cast<QMdiArea *>(aParent); //можно наверное и получать, но не работает член-функция parent()
}

ETable Table::Type() const
{
    return type;
}

void Table::Init(const QString &aTitle, const QString &aTblName)
{
    setWindowTitle(tr("Table \"") + aTitle + tr("\""));
    model->setTable(aTblName);
}

void Table::SetFirst()
{
    view->setCurrentIndex(model->index(0, 0));
}

void Table::SetNext()
{
    if (view->currentIndex().row() < model->rowCount() - 1)
    {
        view->setCurrentIndex(model->index(view->currentIndex().row() + 1, 0));
    }
    else
    {
        QApplication::beep();
    }
}

void Table::SetPrev()
{
    if (view->currentIndex().row() > 0)
    {
        view->setCurrentIndex(model->index(view->currentIndex().row() - 1, 0));
    }
    else
    {
        QApplication::beep();
    }
}

void Table::SetLast()
{
    view->setCurrentIndex(model->index(model->rowCount() - 1, 0));
}

void Table::Add()
{
    Card *newCard = CreateCard(-1);
    connect(newCard, SIGNAL(destroyed()), this, SLOT(DecCountInsertCard()));
    mdiArea->addSubWindow(newCard)->show();
    ++countInsertCard;
}

void Table::Edit()
{
    OpenCard(view->currentIndex());
}

void Table::Del()
{
    QModelIndex curIndex = view->currentIndex();
    Card *card = GetCard(model->record(curIndex.row()).value(0).toString());  //с учетом, что первичные ключи во всех таблицах - это первый атрибут
    if (card)   //Если выделенная запись сейчас редактируется
    {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("This record is edited now!"));
        card->showNormal();
        card->setFocus();
        return;
    }
    if (!Developer::curIsAdm && !IsAccessible(curIndex, true))
    {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Access denied"));
        return;
    }
    int ret = QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Do you want to delete this record?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (ret == QMessageBox::No)
    {
        return;
    }
    int row = view->currentIndex().row();
    model->removeRow(row);
    view->setCurrentIndex(model->index((row == model->rowCount() ? row - 1 : row), 0));
    //закрытие карты
}

void Table::OpenCard(QModelIndex aIndex)
{
    if (!Developer::curIsAdm && !IsAccessible(aIndex, false))
    {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Access denied"));
        return;
    }
    Card *card = GetCard(model->record(aIndex.row()).value(0).toString());  //с учетом, что первичные ключи во всех таблицах - это первый атрибут
    if (card)
    {
        card->showNormal();
        card->setFocus();
        return;
    }
    Card *newCard = CreateCard(aIndex.row());
    QMdiSubWindow *sw = mdiArea->addSubWindow(newCard);
    sw->show();
    sw->setFixedSize(sw->sizeHint());
    sw->setWindowFlags(Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    connect(newCard, SIGNAL(destroyed(QObject *)), this, SLOT(DestroyCard(QObject *)));
    lstCard.push_back(newCard);
}

Card *Table::GetCard(const QString &aId) const
{
    for (LstCard::const_iterator it = lstCard.begin(); it != lstCard.end(); ++it)
    {
        if ((*it)->Type() == type && (*it)->Id() == aId)
        {
            return *it;
        }
    }
    return 0;
}

void Table::DestroyCard(QObject *aObj)
{
    for (LstCard::iterator it = lstCard.begin(); it != lstCard.end(); ++it)
    {
        if (*it == aObj)
        {
            lstCard.erase(it);
            break;
        }
    }
}

void Table::DecCountInsertCard()
{
    --countInsertCard;
}

void Table::closeEvent(QCloseEvent *aE)
{
    if (countInsertCard || lstCard.size())
    {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("There are some cards of this table opening now"));
        aE->ignore();
        return;
    }
    aE->accept();
}

bool Table::IsAccessible(QModelIndex &aIndex, bool aIsDel) const
{
    return true;
}

//----------------------------------- Таблица company -----------------------------------

TblCompany::TblCompany(QWidget *aParent):
        Table(aParent, tCompany)
{
    using namespace Company;

    Init(tr("Companies"), "company");
    model->setHeaderData(iName, Qt::Horizontal, tr("Name"));
    model->select();
    view->setColumnHidden(iId, true);
}

Card *TblCompany::CreateCard(int aRow) const
{
    return new CardCompany(mdiArea, model, aRow);
}

//----------------------------------- Таблица project -----------------------------------

TblProject::TblProject(QWidget *aParent):
        Table(aParent, tProject)
{
    using namespace Project;

    Init(tr("Projects"), "project");
    model->setHeaderData(iName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(iDateBegin, Qt::Horizontal, tr("Begin date"));
    model->setHeaderData(iDateEndExpect, Qt::Horizontal, tr("Expected end date"));
    model->setHeaderData(iIsComplete, Qt::Horizontal, tr("Is complete"));
    model->select();
    view->setColumnHidden(iId, true);
}

Card *TblProject::CreateCard(int aRow) const
{
    return new CardProject(mdiArea, model, aRow);
}

bool TblProject::IsAccessible(QModelIndex &aIndex, bool aIsDel) const
{
    QSqlQuery query;
    query.exec(
            "SELECT is_manager FROM distr_proj_dev d JOIN project p ON d.proj_id = p.proj_id WHERE d.dev_login = \'" +
            Developer::curLogin + "\' AND p.proj_id = " + model->record(aIndex.row()).value(Project::iId).toString());
    return query.next() && query.record().value(0).toBool();
}

//----------------------------------- Таблица developer -----------------------------------

TblDeveloper::TblDeveloper(QWidget *aParent):
        Table(aParent, tDeveloper)
{
    using namespace Developer;

    Init(tr("Developers"), "developer");
    model->setRelation(iCompanyId, QSqlRelation("company", "comp_id", "comp_name"));
    model->setHeaderData(iLogin, Qt::Horizontal, tr("Login"));
    model->setHeaderData(iPassword, Qt::Horizontal, tr("Password"));
    model->setHeaderData(iFirstName, Qt::Horizontal, tr("First name"));
    model->setHeaderData(iLastName, Qt::Horizontal, tr("Last name"));
    model->setHeaderData(iIsAdm, Qt::Horizontal, tr("Is administrator"));
    model->setHeaderData(iCompanyId, Qt::Horizontal, tr("Company"));
    model->select();
    view->setColumnHidden(iPassword, true);
}

Card *TblDeveloper::CreateCard(int aRow) const
{
    return new CardDeveloper(mdiArea, model, aRow);
}

//----------------------------------- Таблица problem -----------------------------------

bool TblProblem::isManager = true;

TblProblem::TblProblem(QWidget *aParent):
        Table(aParent, tProblem)
{
    using namespace Problem;

    Init(tr("Problems"), "problem");
    model->setRelation(iProjectId, QSqlRelation("project", "proj_id", "proj_name"));
    model->setHeaderData(iName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(iDeclaration, Qt::Horizontal, tr("Declaration"));
    model->setHeaderData(iTimePlan, Qt::Horizontal, tr("Estimated time (in hours)"));
    model->setHeaderData(iStatus, Qt::Horizontal, tr("Status"));
    model->setHeaderData(iProjectId, Qt::Horizontal, tr("Project"));
    model->select();

    view->setColumnHidden(iId, true);
}

Card *TblProblem::CreateCard(int aRow) const
{
    return new CardProblem(mdiArea, model, aRow, isManager);
}

bool TblProblem::IsAccessible(QModelIndex &aIndex, bool aIsDel) const
{
    QSqlQuery query;
    query.exec("SELECT is_manager FROM distr_proj_dev d JOIN problem p ON d.proj_id = p.proj_id WHERE d.dev_login = \'" +
               Developer::curLogin + "\' AND p.prob_id = " + model->record(aIndex.row()).value(Problem::iId).toString());

    if (!query.next())  //Если нет вообще такого распределения "текущий пользователь - выбранная задача"
    {
        return false;
    }
    isManager = query.record().value(0).toBool();
    return isManager || !aIsDel;    //разработчик не имеет права удалять задачи, в отличие от менеджера
}

//----------------------------------- Таблица contract -----------------------------------

TblContract::TblContract(QWidget *aParent):
        Table(aParent, tContract)
{

    using namespace Contract;

    Init(tr("Contracts"), "contract");
    model->setRelation(iCompanyId, QSqlRelation("company", "comp_id", "comp_name"));
    model->setRelation(iProjectId, QSqlRelation("project", "proj_id", "proj_name"));
    model->setHeaderData(iNum, Qt::Horizontal, tr("Number"));
    model->setHeaderData(iDateConclusion, Qt::Horizontal, tr("Date of conclusion"));
    model->setHeaderData(iStatus, Qt::Horizontal, tr("Status"));
    model->setHeaderData(iCompanyId, Qt::Horizontal, tr("Company"));
    model->setHeaderData(iProjectId, Qt::Horizontal, tr("Project"));
    model->select();
}

Card *TblContract::CreateCard(int aRow) const
{
    return new CardContract(mdiArea, model, aRow);
}


//----------------------------------- Таблица report -----------------------------------

TblReport::TblReport(QWidget *aParent):
        Table(aParent, tReport)
{
    using namespace Report;

    Init(tr("Reports"), "report");
    model->setRelation(iDeveloperLogin, QSqlRelation("developer", "dev_login", "dev_login"));
    model->setRelation(iProblemId, QSqlRelation("problem", "prob_id", "prob_name"));
    model->setHeaderData(iDateBegin, Qt::Horizontal, tr("Begin date"));
    model->setHeaderData(iDateEnd, Qt::Horizontal, tr("End date"));
    model->setHeaderData(iDeclaration, Qt::Horizontal, tr("Declaration"));
    model->setHeaderData(iDeveloperLogin, Qt::Horizontal, tr("Developer (login)"));
    model->setHeaderData(iProblemId, Qt::Horizontal, tr("Problem"));
    model->select();
    view->setColumnHidden(iId, true);
}

Card *TblReport::CreateCard(int aRow) const
{
    return new CardReport(mdiArea, model, aRow);
}

bool TblReport::IsAccessible(QModelIndex &aIndex, bool aIsDel) const
{
    QSqlQuery query;
    QString str = "SELECT is_manager, r.dev_login FROM distr_proj_dev d JOIN problem p, report r ON d.proj_id  = p.proj_id AND " \
                  "p.prob_id = r.prob_id WHERE d.dev_login = \'" + Developer::curLogin + "\' AND r.rep_id = " +
                  model->record(aIndex.row()).value(Report::iId).toString();
    query.exec(str);
    if (!query.next())  //Если нет вообще такого распределения "текущий пользователь - выбранная задача (в отчете)"
    {
        return false;
    }

    //Доступ разрешен менеджеру (над всеми задачами из проектов, где он менеджер) и разработчику, если это его отчет
    return query.record().value(0).toBool() || Developer::curLogin == query.record().value(1).toString();
}

//----------------------------------- Таблица problem_depend -----------------------------------

TblProblemDepend::TblProblemDepend(QWidget *aParent):
        Table(aParent, tProblemDepend)
{
    using namespace ProblemDepend;

    Init(tr("Dependence problems"), "problem_depend");
    model->setRelation(iProblem1Id, QSqlRelation("problem", "prob_id", "prob_name"));
    model->setRelation(iProblem2Id, QSqlRelation("problem", "prob_id", "prob_name"));
    model->setHeaderData(iProblem1Id, Qt::Horizontal, tr("Problem"));
    model->setHeaderData(iProblem2Id, Qt::Horizontal, tr("Dependent problem"));
    view->setColumnHidden(iId, true);
    model->select();
}

Card *TblProblemDepend::CreateCard(int aRow) const
{
    return new CardProblemDepend(mdiArea, model, aRow);
}

//Доступ к записи (редактирование и удаление) возможен только если текущий пользователь является менеджером на проекте,
//к которому принадлежат задачи.
//Проверка идет на принадлежность к проекту только первой задачи,
//т.к. зависимости можно устанавливать только между задачами, принадлежащими к одному проекту.
bool TblProblemDepend::IsAccessible(QModelIndex &aIndex, bool aIsDel) const
{
    QSqlQuery query;
    query.exec("SELECT is_manager FROM distr_proj_dev d JOIN problem p, problem_depend pd ON d.proj_id = " \
               "p.proj_id AND p.prob_id = pd.prob1_id WHERE d.dev_login = \'" + Developer::curLogin +
               "\' AND pd.depend_id = " + model->record(aIndex.row()).value(ProblemDepend::iId).toString());
    return query.next() && query.record().value(0).toBool();
}

//----------------------------------- Таблица distr_proj_dev -----------------------------------

TblDistrProjDev::TblDistrProjDev(QWidget *aParent):
        Table(aParent, tDistrProjDev)
{
    using namespace DistrProjDev;

    Init(tr("Distributions (projects - developers)"), "distr_proj_dev");
    model->setRelation(DistrProjDev::iProjectId, QSqlRelation("project", "proj_id", "proj_name"));
    model->setRelation(DistrProjDev::iDeveloperLogin, QSqlRelation("developer", "dev_login", "dev_login"));
    model->setHeaderData(iProjectId, Qt::Horizontal, tr("Project"));
    model->setHeaderData(iDeveloperLogin, Qt::Horizontal, tr("Developer (login)"));
    model->setHeaderData(iIsManager, Qt::Horizontal, tr("Is manager"));
    view->setColumnHidden(iId, true);
    model->select();
}

Card *TblDistrProjDev::CreateCard(int aRow) const
{
    return new CardDistrProjDev(mdiArea, model, aRow);
}

//Доступ к записи (редактирование и удаление) возможен только если текущий пользователь является менеджером на проекте,
//распределение которого содержит запись.
bool TblDistrProjDev::IsAccessible(QModelIndex &aIndex, bool aIsDel) const
{
    QSqlQuery query;
    query.exec("SELECT is_manager FROM distr_proj_dev WHERE dev_login = \'" + Developer::curLogin +
               "\' AND proj_id IN (SELECT proj_id FROM distr_proj_dev WHERE distr_id = " +
               model->record(aIndex.row()).value(DistrProjDev::iId).toString() + ")");
    return query.next() && query.record().value(0).toBool();
}
