#include "../headers/card.h"

using namespace Settings;

inline void AddToLayout(QGridLayout *aLt, unsigned int aRow, QLabel *aLbl, QWidget *aWidget)
{
    aLt->addWidget(aLbl, aRow, 0);
    aLt->addWidget(aWidget, aRow, 1);
    aLbl->setBuddy(aWidget);
}

void AddWidgetsInput(QGridLayout *aLt, unsigned int aRow, QLabel **aLbl, const QString &aStrLbl, QLineEdit **aEdt)
{
    *aLbl = new QLabel(aStrLbl);
    *aEdt = new QLineEdit;
    AddToLayout(aLt, aRow, *aLbl, *aEdt);
}

inline QString GetFilter(const QString &aField, QSqlRelationalTableModel *aModel,
    int aRow, unsigned int aI, bool aIsString = false)
{
    QString value;
    if (aIsString)
    {
        value = "\'" + (aRow != -1 ?  aModel->record(aRow).value(aI).toString() : "") + "\'";
    }
    else
    {
        value = (aRow != -1 ?  aModel->record(aRow).value(aI).toString() : "-1");
    }
    return aField + " = " + value;
}

bool IsMistake(bool aCond, const QString &aMsgErr)
{
    if (aCond)
    {
        QMessageBox::critical(0, QObject::tr("Error"), aMsgErr);
        return true;
    }
    return false;
}
//Фильтр, отбирающий только задачи, относящиеся к проектам, на которых текущий пользователь является менеджером
inline QString GetFilterProjForManager()
{
    return "proj_id IN (SELECT proj_id FROM distr_proj_dev WHERE is_manager = 1 AND dev_login = \'" + Developer::curLogin + "\')";
}

//----------------------------------- Карточка для таблиц (базовая) -----------------------------------

Card::Card(QWidget *aParent, QSqlRelationalTableModel *aModel, ETable aType, const QString &aFilter, bool aIsInsert):
        QDialog(aParent),
        mdiArea(static_cast<QMdiArea *>(aParent)),
        type(aType),
        model(aModel)
{
    setAttribute(Qt::WA_DeleteOnClose);

    proxyModel.setTable(model->tableName());
    proxyModel.setFilter(aFilter);
    for (int i = 0; i < model->columnCount(); ++i)
    {
        QSqlRelation tmpRelation = model->relation(i);
        if (tmpRelation.isValid())
        {
            proxyModel.setRelation(i, tmpRelation);
        }
    }
    proxyModel.select();
    if (aIsInsert)
    {
        proxyModel.insertRow(proxyModel.rowCount());
    }

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(&proxyModel);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
}

void Card::CreateWidgetsBase(QLayout *aLt)
{
    QHBoxLayout *lt1 = new QHBoxLayout;
    grpBox = new QGroupBox;
    grpBox->setLayout(aLt);
    lt1->addWidget(grpBox);

    QHBoxLayout *lt2 = new QHBoxLayout;
    btnOk = new QPushButton(tr("Ok"));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnOk, SIGNAL(clicked()), this, SLOT(Ok()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(Cancel()));
    lt2->addWidget(btnOk);
    lt2->addWidget(btnCancel);

    QVBoxLayout *ltMain = new QVBoxLayout;
    ltMain->addLayout(lt1);
    ltMain->addLayout(lt2);

    setLayout(ltMain);
}

void Card::Ok()
{
    if (!IsValid())
    {
        return;
    }
    SubmitForSheckBox();
    if (!mapper->submit())
    {
        return;
    }
    model->select();
    close();
}

void Card::Cancel()
{
    close();
}

void Card::closeEvent(QCloseEvent *aE)
{
    proxyModel.revert();
    mdiArea->closeActiveSubWindow();
    aE->accept();
}

bool Card::IsValid() const
{
    return true;
}

QString Card::Id() const
{
    return proxyModel.record(0).value(0).toString();   //с учетом, что первичные ключи во всех таблицах - это первый атрибут
}

Settings::ETable Card::Type() const
{
    return type;
}

//Небольшая заглушка, необходимая для submita в случае с QCheckBox'ом
//Из-за нехватки времени на то, чтобы разобраться с изменением взаимодействия QDataWidgetMapper и QCheckBox,
//пришлось пойти на такую хитрость
//Начало заглушки
inline void Card::_SubmitForSheckBox_(QCheckBox *aChBox, unsigned int aI)
{
    mapper->removeMapping(aChBox);
    QSpinBox *tmp = new QSpinBox(this);
    mapper->addMapping(tmp, aI);
    tmp->setValue(aChBox->checkState() ? 1 : 0);
}

//Функция перегружается для всех классов, где есть QCheckBox, посредством полиморфизма
void Card::SubmitForSheckBox()
{
    //empty
}
//Конец заглушки

//----------------------------------- Карточка для таблицы company -----------------------------------

CardCompany::CardCompany(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tCompany, GetFilter("comp_id", aModel, aRow, Company::iId), aRow == -1)
{
    using namespace Company;

    setWindowTitle(tr("Company"));

    CreateWidgets();

    mapper->addMapping(edtName, iName);
    mapper->toFirst();

    setFixedSize(sizeHint());
}

void CardCompany::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;
    AddWidgetsInput(lt1, 0, &lblName, tr("Name"), &edtName);
    edtName->setMaxLength(Company::maxLenName);
    CreateWidgetsBase(lt1);
}

bool CardCompany::IsValid() const
{
    return !IsMistake(edtName->text().isEmpty(), tr("Company name must not be empty"));
}

//----------------------------------- Карточка для таблицы project -----------------------------------

CardProject::CardProject(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tProject, GetFilter("proj_id", aModel, aRow, Project::iId), aRow == -1)
{
    using namespace Project;

    setWindowTitle(tr("Project"));

    CreateWidgets();

    mapper->addMapping(edtName, iName);
    mapper->addMapping(edtDateBegin, iDateBegin);
    mapper->addMapping(edtDateEndExpect, iDateEndExpect);
    mapper->addMapping(chboxIsComplete, iIsComplete);
    mapper->toFirst();

    if (!Developer::curIsAdm)
    {
        //Не администраторам доступна только возможность установки статуса проекта
        //Примечание: карточка может быть открыта только менеджером этого проета
        edtName->setEnabled(false);
        edtDateEndExpect->setEnabled(false);
        edtDateBegin->setEnabled(false);
    }
    else
    {
        if (aRow == -1) //если происходит вставка
        {
            edtDateBegin->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0)));
            edtDateEndExpect->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0)));
        }
    }
}

void CardProject::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;

    AddWidgetsInput(lt1, 0, &lblName, tr("Name"), &edtName);
    edtName->setMaxLength(Project::maxLenName);

    lblDateBegin = new QLabel(tr("Begin date"));
    edtDateBegin = new QDateTimeEdit;
    AddToLayout(lt1, 1, lblDateBegin, edtDateBegin);
    edtDateBegin->setCalendarPopup(true);

    lblDateEndExpect = new QLabel(tr("End expected date"));
    edtDateEndExpect = new QDateTimeEdit;
    AddToLayout(lt1, 2, lblDateEndExpect, edtDateEndExpect);
    edtDateEndExpect->setCalendarPopup(true);

    QHBoxLayout *lt2 = new QHBoxLayout;
    chboxIsComplete = new QCheckBox(tr("Is complete"));
    lt2->addWidget(chboxIsComplete);

    QVBoxLayout *lt3 = new QVBoxLayout;
    lt3->addLayout(lt1);
    lt3->addLayout(lt2);

    CreateWidgetsBase(lt3);
}

bool CardProject::IsValid() const
{
    return !IsMistake(edtName->text().isEmpty(), tr("Project name must not be empty")) &&
        !IsMistake(edtDateBegin->dateTime() >= edtDateEndExpect->dateTime(), tr("Begin date must be grater tnan end date"));
}

void CardProject::SubmitForSheckBox()
{
    _SubmitForSheckBox_(chboxIsComplete, Project::iIsComplete);
}

//----------------------------------- Карточка для таблицы developer -----------------------------------

CardDeveloper::CardDeveloper(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tDeveloper, GetFilter("dev_login", aModel, aRow, Developer::iLogin, true), aRow == -1)
{
    using namespace Developer;

    setWindowTitle(tr("Developer"));

    CreateWidgets();

    cbboxCompany->setModel(proxyModel.relationModel(iCompanyId));
    cbboxCompany->setModelColumn(Company::iName);

    mapper->addMapping(edtLogin, iLogin);
    mapper->addMapping(edtPassw, iPassword);
    mapper->addMapping(edtFirstName, iFirstName);
    mapper->addMapping(edtLastName, iLastName);
    mapper->addMapping(chboxIsAdm, iIsAdm);
    mapper->addMapping(cbboxCompany, iCompanyId);
    mapper->toFirst();

    oldLogin = edtLogin->text();
    edtPasswRep->setText(edtPassw->text());
}

void CardDeveloper::CreateWidgets()
{
    using namespace Developer;

    QGridLayout *lt1 = new QGridLayout;
    AddWidgetsInput(lt1, 0, &lblLogin, tr("Login"), &edtLogin);
    edtLogin->setMaxLength(maxLenLogin);

    AddWidgetsInput(lt1, 1, &lblPassw, tr("Password"), &edtPassw);
    edtPassw->setMaxLength(maxLenPassword);
    edtPassw->setEchoMode(QLineEdit::Password);

    AddWidgetsInput(lt1, 2, &lblPasswRep, tr("Repeat password"), &edtPasswRep);
    edtPasswRep->setMaxLength(maxLenPassword);
    edtPasswRep->setEchoMode(QLineEdit::Password);

    AddWidgetsInput(lt1, 3, &lblFirstName, tr("First name"), &edtFirstName);
    edtFirstName->setMaxLength(maxLenFirstName);

    AddWidgetsInput(lt1, 4, &lblLastName, tr("Last name"), &edtLastName);
    edtLastName->setMaxLength(maxLenLastName);

    lblCompany = new QLabel(tr("Company"));
    cbboxCompany = new QComboBox;
    AddToLayout(lt1, 5, lblCompany, cbboxCompany);

    QHBoxLayout *lt2 = new QHBoxLayout;
    chboxIsAdm = new QCheckBox(tr("Is administrator"));
    lt2->addWidget(chboxIsAdm);

    QVBoxLayout *lt = new QVBoxLayout;
    lt->addLayout(lt1);
    lt->addLayout(lt2);
    CreateWidgetsBase(lt);
}

bool CardDeveloper::IsValid() const
{
    if (IsMistake(edtLogin->text().isEmpty(), tr("Login must not be empty")) ||
        IsMistake(edtPassw->text() != edtPasswRep->text(), tr("Passwords do not match")) ||
        IsMistake(cbboxCompany->currentText().isEmpty(), tr("Select company")))
    {
        return false;
    }
    QSqlQuery query;
    query.exec("SELECT * FROM developer WHERE dev_login = \'" + edtLogin->text() + "\' AND dev_login <> \'" + oldLogin + "\'");
    return !IsMistake(query.next(), tr("Login already in use"));
}

void CardDeveloper::SubmitForSheckBox()
{
    _SubmitForSheckBox_(chboxIsAdm, Developer::iIsAdm);
}

//----------------------------------- Карточка для таблицы problem -----------------------------------

CardProblem::CardProblem(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow, bool aIsManager):
        Card(aParent, aModel, tProblem, GetFilter("prob_id", aModel, aRow, Problem::iId), aRow == -1),
        probId(aModel->record(aRow).value(Problem::iId).toInt())
{
    using namespace Problem;

    setWindowTitle(tr("Problem"));

    CreateWidgets();

    QSqlTableModel *t = proxyModel.relationModel(iProjectId);
    if (!Developer::curIsAdm && aIsManager)
    {
        t->setFilter(GetFilterProjForManager());
    }
    cbboxProject->setModel(t);
    cbboxProject->setModelColumn(Project::iName);

    mapper->addMapping(edtName, iName);
    mapper->addMapping(edtDeclaration, iDeclaration);
    mapper->addMapping(spnboxTimePlan, iTimePlan);
    mapper->addMapping(sldrStatus, iStatus);
    mapper->addMapping(cbboxProject, iProjectId);
    mapper->toFirst();

    SetStatus(sldrStatus->value());

    if (!Developer::curIsAdm && !aIsManager)
    {
        //Если пользователь не является администратором или менеджером,
        //то он может изменять только статус задачи
        edtName->setEnabled(false);
        edtDeclaration->setEnabled(false);
        spnboxTimePlan->setEnabled(false);
        cbboxProject->setEnabled(false);
    }
}

void CardProblem::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;
    AddWidgetsInput(lt1, 0, &lblName, tr("Name"), &edtName);
    edtName->setMaxLength(Problem::maxLenName);

    lblDeclaration = new QLabel(tr("Decaration"));
    edtDeclaration = new QLineEdit;
    AddToLayout(lt1, 1, lblDeclaration, edtDeclaration);
    edtDeclaration->setMaxLength(Problem::maxLenDeclaration);

    lblTimePlan = new QLabel(tr("Estimated time"));
    spnboxTimePlan = new QSpinBox;
    AddToLayout(lt1, 2, lblTimePlan, spnboxTimePlan);
    spnboxTimePlan->setMinimum(1);

    lblProject = new QLabel(tr("Project"));
    cbboxProject = new QComboBox;
    AddToLayout(lt1, 3, lblProject, cbboxProject);

    lblStatus = new QLabel(tr("Status"));
    sldrStatus = new QSlider(Qt::Horizontal);
    AddToLayout(lt1, 4, lblStatus, sldrStatus);
    connect(sldrStatus, SIGNAL(valueChanged(int)), this, SLOT(SetStatus(int)));
    sldrStatus->setRange(0, 2);

    lblStatusVal = new QLabel;
    lt1->addWidget(lblStatusVal, 5, 1, Qt::AlignHCenter);

    CreateWidgetsBase(lt1);
}

bool CardProblem::IsValid() const
{
    if (IsMistake(edtName->text().isEmpty(), tr("Problem name must not be empty")) ||
        IsMistake(cbboxProject->currentText().isEmpty(), tr("Select project")))
    {
        return false;
    }
    if (sldrStatus->value() < 2)    //Если задача не помечена как завершенная, то проверка на завершение задач,
    {                               //от которых она зависит, не требуется
        return true;
    }
    QSqlQuery query;
    query.exec("SELECT pd.prob1_id FROM problem_depend pd JOIN problem p ON pd.prob1_id = p.prob_id WHERE pd.prob2_id = " +
               QString::number(probId) + " AND p.prob_status < 2");
    return !IsMistake(query.next(), tr("The problem can not be completed"));
}

void CardProblem::SetStatus(int aStatus)
{
    lblStatusVal->setText(Problem::vecStatus[aStatus]);
}

//----------------------------------- Карточка для таблицы contract -----------------------------------

CardContract::CardContract(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tContract, GetFilter("cont_num", aModel, aRow, Contract::iNum), aRow == -1)
{
    using namespace Contract;

    setWindowTitle(tr("Contract"));

    CreateWidgets();

    cbboxCompany->setModel(proxyModel.relationModel(iCompanyId));
    cbboxCompany->setModelColumn(Company::iName);
    cbboxProject->setModel(proxyModel.relationModel(iProjectId));
    cbboxProject->setModelColumn(Project::iName);

    mapper->addMapping(edtNum, iNum);
    mapper->addMapping(edtDateConclusion, iDateConclusion);
    mapper->addMapping(sldrStatus, iStatus);
    mapper->addMapping(cbboxCompany, iCompanyId);
    mapper->addMapping(cbboxProject, iProjectId);
    mapper->toFirst();

    SetStatus(sldrStatus->value());

    if (aRow == -1) //если происходит вставка
    {
        edtDateConclusion->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0)));
    }
}

void CardContract::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;

    AddWidgetsInput(lt1, 0, &lblNum, tr("Number"), &edtNum);
    edtNum->setValidator(new QIntValidator(1, 999999, this));

    lblDateConclusion = new QLabel(tr("Date conclusion"));
    edtDateConclusion = new QDateTimeEdit;
    AddToLayout(lt1, 1, lblDateConclusion, edtDateConclusion);
    edtDateConclusion->setCalendarPopup(true);

    lblCompany = new QLabel(tr("Company"));
    cbboxCompany = new QComboBox;
    AddToLayout(lt1, 2, lblCompany, cbboxCompany);

    lblProject = new QLabel(tr("Project"));
    cbboxProject = new QComboBox;
    AddToLayout(lt1, 3, lblProject, cbboxProject);

    lblStatus = new QLabel(tr("Status"));
    sldrStatus = new QSlider(Qt::Horizontal);
    AddToLayout(lt1, 4, lblStatus, sldrStatus);
    connect(sldrStatus, SIGNAL(valueChanged(int)), this, SLOT(SetStatus(int)));
    sldrStatus->setRange(0, 2);

    lblStatusVal = new QLabel;
    lt1->addWidget(lblStatusVal, 5, 1, Qt::AlignHCenter);

    CreateWidgetsBase(lt1);
}

bool CardContract::IsValid() const
{
    int num = edtNum->text().toInt();
    edtNum->setText(QString::number(num));
    return !IsMistake(!num, tr("Invalid number of contract")) &&
        !IsMistake(cbboxCompany->currentText().isEmpty(), tr("Select company")) &&
        !IsMistake(cbboxProject->currentText().isEmpty(), tr("Select project"));
}

void CardContract::SetStatus(int aStatus)
{
    lblStatusVal->setText(Contract::vecStatus[aStatus]);
}

//----------------------------------- Карточка для таблицы report -----------------------------------

QVector<unsigned int> CardReport::vecIdProb;

CardReport::CardReport(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tReport, GetFilter("rep_id", aModel, aRow, Report::iId), aRow == -1),
        repId(aModel->record(aRow).value(Report::iId).toInt())
{
    using namespace Report;

    setWindowTitle(tr("Report"));

    CreateWidgets();

    QSqlTableModel *t = proxyModel.relationModel(iDeveloperLogin);
    /*if (aIsManager) //если менеджер -> отбор сотрудников, привязанных к проектам, где текущий пользователь есть менеджер
    {
        t->setFilter("dev_login IN (SELECT d1.dev_login FROM distr_proj_dev d1 JOIN distr_proj_dev d2 ON d1.proj_id = " \
                     "d2.proj_id WHERE d2.is_manager = 1 AND d2.dev_login = \'" + Developer::curLogin + "\')");
    }
    else    //иначе -> отбор только текущего пользователя
    {
        t->setFilter("dev_login = \'" + Developer::curLogin + "\'");
    }*/
    cbboxDeveloper->setModel(t);
    cbboxDeveloper->setModelColumn(Developer::iLogin);

    t = proxyModel.relationModel(iProblemId);
    if (!Developer::curIsAdm)
    {
        t->setFilter("proj_id IN (SELECT proj_id FROM distr_proj_dev WHERE dev_login = \'" + Developer::curLogin + "\')");
    }

    //Запоминаем все id -> ускоряем поиск при проверке:
    vecIdProb.resize(t->rowCount());
    for (int i = 0; i < t->rowCount(); ++i)
    {
        vecIdProb[i] = t->record(i).value(Problem::iId).toInt();
    }

    cbboxProblem->setModel(t);
    cbboxProblem->setModelColumn(Problem::iName);

    mapper->addMapping(edtDateBegin, iDateBegin);
    mapper->addMapping(edtDateEnd, iDateEnd);
    mapper->addMapping(edtDeclaration, iDeclaration);
    mapper->addMapping(cbboxDeveloper, iDeveloperLogin);
    mapper->addMapping(cbboxProblem, iProblemId);
    mapper->toFirst();

    if (aRow == -1) //если происходит вставка
    {
        edtDateBegin->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0)));
        edtDateEnd->setDateTime(QDateTime(QDate::currentDate(), QTime(0, 0)));
    }
}

void CardReport::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;

    lblDeveloper = new QLabel(tr("Developer (login)"));
    cbboxDeveloper = new QComboBox;
    AddToLayout(lt1, 1, lblDeveloper, cbboxDeveloper);

    lblProblem = new QLabel(tr("Problem"));
    cbboxProblem = new QComboBox;
    AddToLayout(lt1, 2, lblProblem, cbboxProblem);

    lblDateBegin = new QLabel(tr("Begin date"));
    edtDateBegin = new QDateTimeEdit;
    AddToLayout(lt1, 3, lblDateBegin, edtDateBegin);
    edtDateBegin->setCalendarPopup(true);

    lblDateEnd = new QLabel(tr("End date"));
    edtDateEnd = new QDateTimeEdit;
    AddToLayout(lt1, 4, lblDateEnd, edtDateEnd);
    edtDateEnd->setCalendarPopup(true);

    AddWidgetsInput(lt1, 5, &lblDeclaration, tr("Declaration"), &edtDeclaration);
    edtDeclaration->setMaxLength(Report::maxLenDeclaration);

    CreateWidgetsBase(lt1);
}

bool CardReport::IsValid() const
{
    if  (IsMistake(edtDateBegin->dateTime() >= edtDateEnd->dateTime(), tr("Begin date must be grater tnan end date")) ||
         IsMistake(cbboxDeveloper->currentText().isEmpty(), tr("Select developer")) ||
         IsMistake(cbboxProblem->currentText().isEmpty(), tr("Select problem")))
    {
        return false;
    }

    QSqlQuery query;

    //Проверка на коллизии по времени. Не занят ли разработчик уже:

    QString strD1 = edtDateBegin->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strD2 = edtDateEnd->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    query.exec("SELECT * FROM report WHERE rep_id <> " + QString::number(repId) +
               " AND dev_login = \'" + cbboxDeveloper->currentText() + "\' AND ((datetime(\'" +
               strD1 + "\') BETWEEN datetime(date_begin) AND datetime(date_end)) OR (datetime(\'" + strD2 +
               "\') BETWEEN datetime(date_begin) AND datetime(date_end)))");
    if (IsMistake(query.next(), tr("Developer is already busy at this time")))
    {
        return false;
    }

    if (Developer::curLogin == cbboxDeveloper->currentText())   //Если текущий пользователь заполняет свой отчет,
    {                                                           //то все нормально, поскольку проект не надо проверять
        return true;
    }

    //Посколько текущий разработчик пытается заполнить отчет не за себя, то надо определить является ли он
    //менеджером на проекте либо админом, к которому принадлежит выбранная задача (т.к. это может сделать только менеджер):

    if (!Developer::curIsAdm)
    {
        query.exec("SELECT is_manager FROM distr_proj_dev d JOIN problem p ON d.proj_id = p.proj_id WHERE p.prob_id = \'" +
                   QString::number(vecIdProb[cbboxProblem->currentIndex()]) + "\' AND d.dev_login = \'" +
                   Developer::curLogin + "\'");
        if (IsMistake(!query.next() || !query.record().value(0).toBool(),
                      tr("The current user is not manager of the selected project")))
        {
            return false;
        }
    }

    //Мы уже выяснили, что менеджер заполняет отчет за другого разработчика
    //Теперь проверяем может ли этот разработчик работать над выбранной задачей:

    query.exec("SELECT * FROM distr_proj_dev d JOIN problem p ON d.proj_id = p.proj_id WHERE p.prob_id = \'" +
               QString::number(vecIdProb[cbboxProblem->currentIndex()]) + "\' AND d.dev_login = \'" +
               cbboxDeveloper->currentText() + "\'");
    return !IsMistake(!query.next(), tr("The selected developer can not work on this problem"));
}

//----------------------------------- Карточка для таблицы problem_depend -----------------------------------


QVector<unsigned int> CardProblemDepend::vecIdProb;

CardProblemDepend::CardProblemDepend(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tProblemDepend, GetFilter("depend_id", aModel, aRow, ProblemDepend::iId), aRow == -1)
{
    using namespace ProblemDepend;

    setWindowTitle(tr("Dependence"));

    CreateWidgets();

    QSqlTableModel *t1 = proxyModel.relationModel(iProblem1Id);
    QSqlTableModel *t2 = proxyModel.relationModel(iProblem2Id);
    if (!Developer::curIsAdm)
    {
        t1->setFilter(GetFilterProjForManager());
        t2->setFilter(GetFilterProjForManager());
    }
    //Запоминаем все id -> ускоряем поиск при проверке:
    vecIdProb.resize(t1->rowCount());
    for (int i = 0; i < t1->rowCount(); ++i)
    {
        vecIdProb[i] = t1->record(i).value(Problem::iId).toInt();
    }
    cbboxProblem1->setModel(t1);
    cbboxProblem1->setModelColumn(Problem::iName);
    cbboxProblem2->setModel(t2);
    cbboxProblem2->setModelColumn(Problem::iName);

    mapper->addMapping(cbboxProblem1, iProblem1Id);
    mapper->addMapping(cbboxProblem2, iProblem2Id);

    mapper->toFirst();
}

void CardProblemDepend::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;

    lblProblem1 = new QLabel(tr("Problem"));
    cbboxProblem1 = new QComboBox;
    AddToLayout(lt1, 0, lblProblem1, cbboxProblem1);

    lblProblem2 = new QLabel(tr("Dependent problem"));
    cbboxProblem2 = new QComboBox;
    AddToLayout(lt1, 1, lblProblem2, cbboxProblem2);

    CreateWidgetsBase(lt1);
}

bool IsDependence(unsigned int aProb1Id, unsigned int aProb2Id)
{
    if (aProb1Id == aProb2Id)
    {
        return true;
    }
    QSqlQuery query;
    query.exec("SELECT prob1_id FROM problem_depend WHERE prob2_id = " + QString::number(aProb1Id));
    while (query.next())
    {
        unsigned int probId = query.record().value(0).toUInt();
        if (probId == aProb2Id || IsDependence(probId, aProb2Id))
        {
            return true;
        }

    }
    return false;   //здесь нет зависимости
}

inline unsigned int GetProjIdByProbId(unsigned int aProbId)
{
    QSqlQuery query;
    query.exec("SELECT proj_id FROM problem WHERE prob_id = " + QString::number(aProbId));
    query.next();
    return query.record().value(0).toInt();
}

bool CardProblemDepend::IsValid() const
{
    if (IsMistake(cbboxProblem1->currentText().isEmpty(), tr("Select problem")) ||
        IsMistake(cbboxProblem2->currentText().isEmpty(), tr("Select dependent problem")))
    {
        return false;
    }

    unsigned int projId1 = GetProjIdByProbId(vecIdProb[cbboxProblem1->currentIndex()]);
    unsigned int projId2 = GetProjIdByProbId(vecIdProb[cbboxProblem2->currentIndex()]);
    if (IsMistake(projId1 != projId2, tr("Problems must be from one project")))
    {
        return false;
    }
    return !IsMistake(IsDependence(vecIdProb[cbboxProblem1->currentIndex()], vecIdProb[cbboxProblem2->currentIndex()]),
                      tr("Cycle dependence"));
}

//----------------------------------- Карточка для таблицы distr_proj_dev -----------------------------------

CardDistrProjDev::CardDistrProjDev(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow):
        Card(aParent, aModel, tDistrProjDev, GetFilter("distr_id", aModel, aRow, DistrProjDev::iId), aRow == -1)
{
    using namespace DistrProjDev;

    setWindowTitle(tr("Distribution"));

    CreateWidgets();

    QSqlTableModel *t = proxyModel.relationModel(iProjectId);
    if (!Developer::curIsAdm)
    {
        t->setFilter(GetFilterProjForManager());
    }
    cbboxProject->setModel(t);
    cbboxProject->setModelColumn(Project::iName);
    cbboxDeveloper->setModel(proxyModel.relationModel(iDeveloperLogin));
    cbboxDeveloper->setModelColumn(Developer::iLogin);

    mapper->addMapping(cbboxProject, iProjectId);
    mapper->addMapping(cbboxDeveloper, iDeveloperLogin);
    mapper->addMapping(chboxIsManager, iIsManager);
    mapper->toFirst();
}

void CardDistrProjDev::CreateWidgets()
{
    QGridLayout *lt1 = new QGridLayout;

    lblProject = new QLabel(tr("Project"));
    cbboxProject = new QComboBox;
    AddToLayout(lt1, 0, lblProject, cbboxProject);

    lblDeveloper = new QLabel(tr("Developer (login)"));
    cbboxDeveloper = new QComboBox;
    AddToLayout(lt1, 1, lblDeveloper, cbboxDeveloper);

    QHBoxLayout *lt2 = new QHBoxLayout;
    chboxIsManager = new QCheckBox(tr("Is manager"));
    lt2->addWidget(chboxIsManager);

    QVBoxLayout *lt = new QVBoxLayout;
    lt->addLayout(lt1);
    lt->addLayout(lt2);

    CreateWidgetsBase(lt);
}

bool CardDistrProjDev::IsValid() const
{
    if (IsMistake(cbboxProject->currentText().isEmpty(), tr("Select project")) ||
        IsMistake(cbboxDeveloper->currentText().isEmpty(), tr("Select developer")))
    {
        return false;
    }
    QSqlQuery query;
    query.exec("SELECT cont_num FROM contract c, project p JOIN developer dev ON c.comp_id = dev.comp_id AND c.proj_id = " \
               "p.proj_id WHERE c.cont_status = 1 AND dev.dev_login = \'" + cbboxDeveloper->currentText() +
               "\' AND p.proj_name = \'" + cbboxProject->currentText() + "\'");
    return !IsMistake(!query.next(), tr("The developer can not work of the selected " \
                                        "project because his company has no contract for it"));
}

void CardDistrProjDev::SubmitForSheckBox()
{
    _SubmitForSheckBox_(chboxIsManager, DistrProjDev::iIsManager);
}
