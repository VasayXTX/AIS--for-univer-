#include "../headers/workStatement.h"

using namespace Settings;

extern void InitVecId(QSqlTableModel *aModel, QVector<unsigned int> &aVecId, unsigned int aIndex);

WorkStatement::WorkStatement(QWidget *aParent):
        Report(aParent)
{
    CreateWidgets("EmptyWS.html");
    setWindowTitle(tr("Work statement"));
}

QGroupBox *AddWidgets(QCheckBox **aChBox, QComboBox **aCbBox, const QString &aStr)
{
    QHBoxLayout *lt = new QHBoxLayout;
    *aChBox = new QCheckBox;
    *aCbBox = new QComboBox;
    (*aCbBox)->setEnabled(false);
    lt->addWidget(*aChBox);
    lt->addWidget(*aCbBox);
    lt->setStretch(0, 0);
    lt->setStretch(1, 1);
    QGroupBox *gb = new QGroupBox(aStr);
    gb->setLayout(lt);
    return gb;
}

inline void SetModel(QWidget *aParent, QComboBox *aCbBox, const QString &aTable, unsigned int aColumn)
{
    QSqlTableModel *t = new QSqlTableModel(aParent);
    t->setTable(aTable);
    t->select();
    aCbBox->setModel(t);
    aCbBox->setModelColumn(aColumn);
}

QVBoxLayout *WorkStatement::_CreateWidgets()
{
    QGroupBox *gbDev = AddWidgets(&chboxDev, &cbboxDev, tr("Developer:"));
    QGroupBox *gbProj = AddWidgets(&chboxProj, &cbboxProj, tr("Project:"));
    QGroupBox *gbProb = AddWidgets(&chboxProb, &cbboxProb, tr("Problem:"));
    chboxProb->setEnabled(false);
    connect(chboxDev, SIGNAL(toggled(bool)), cbboxDev, SLOT(setEnabled(bool)));
    connect(chboxProj, SIGNAL(toggled(bool)), cbboxProj, SLOT(setEnabled(bool)));
    connect(chboxProj, SIGNAL(toggled(bool)), this, SLOT(SetProbEnabled(bool)));
    connect(chboxProb, SIGNAL(toggled(bool)), cbboxProb, SLOT(setEnabled(bool)));
    SetModel(this, cbboxDev, "developer", Developer::iLogin);
    SetModel(this, cbboxProj, "project", Project::iName);
    SetModel(this, cbboxProb, "problem", Problem::iName);
    InitVecId(static_cast<QSqlTableModel *>(cbboxProj->model()), vecProjId, Project::iId);
    connect(cbboxProj, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateCbBoxProblem(int)));
    UpdateCbBoxProblem(cbboxProj->currentIndex());

    QVBoxLayout *lt1 = new QVBoxLayout;
    lt1->addStretch(0);
    lt1->addWidget(gbDev);
    lt1->addWidget(gbProj);
    lt1->addWidget(gbProb);

    return lt1;
}

QString WorkStatement::GetFilter()
{
    if (chboxDev->isChecked())  //В отборе участвует разработчик
    {
        if (chboxProj->isChecked()) //В отборе участвует проект
        {
            if (chboxProb->isEnabled() && chboxProb->isChecked())
            {
                //Отбор происходит по разработчику, проекту и задаче (фактически только по 1 и 3)
                return "WHERE dev_login = \'" + cbboxDev->currentText() + "\' AND r.prob_id = " +
                        QString::number(vecProbId[cbboxProb->currentIndex()]) + " GROUP BY r.prob_id";
            }
            else    //Отбор происходит по разработчику и проекту
            {
                return "WHERE r.dev_login = \'" + cbboxDev->currentText() + "\' AND p.proj_id = " +
                        QString::number(vecProjId[cbboxProj->currentIndex()]) + " GROUP BY r.prob_id";
            }
        }
        else    //Отбор происходит только по разработчику
        {
            return "WHERE dev_login = \'" + cbboxDev->currentText() + "\' GROUP BY r.prob_id";
        }
    }
    else    //В отборе не участвует разработчик
    {
        if (chboxProj->isChecked()) //В отборе участвует проект
        {
            if (cbboxProb->isEnabled() && chboxProb->isChecked())
            {
                //Отбор происходит по проекту и  по задаче (фактически только по 2)
                return "WHERE r.prob_id = " + QString::number(vecProbId[cbboxProb->currentIndex()]) +
                        " GROUP BY dev_login, r.prob_id";
            }
            else    //Отбор происходит только по проекту
            {
                return "WHERE p.proj_id = " +
                        QString::number(vecProjId[cbboxProj->currentIndex()]) + " GROUP BY dev_login, r.prob_id";
            }
        }
        else    //Отбор происходит по всему (никто не участвует)
        {
            return "GROUP BY r.dev_login, r.prob_id";
        }
    }
}

double GetDifferenceInHours(const QString &aBegin, const QString aEnd)
{
    const unsigned int SEC_IN_HOUR = 3600;

    QStringList lstBegin = aBegin.split(Database::datetimeSplit);
    QStringList lstEnd = aEnd.split(Database::datetimeSplit);
    QStringList::const_iterator it1 = lstBegin.begin();
    QStringList::const_iterator it2 = lstEnd.begin();

    unsigned int r = 0;
    while (it1 != lstBegin.end())
    {
        QDateTime t1 = QDateTime::fromString(*it1, Qt::ISODate);
        QDateTime t2 = QDateTime::fromString(*it2, Qt::ISODate);
        r += t1.secsTo(t2);
        ++it1;
        ++it2;
    }
    return r / SEC_IN_HOUR;
}

void WorkStatement::CreateReport()
{
    QSqlQuery query;
    query.exec("SELECT r.dev_login, p.prob_name, group_concat(r.date_begin, \'" + Database::datetimeSplit +
               "\'), group_concat(r.date_end, \'" + Database::datetimeSplit + "\') " \
               "FROM report r JOIN problem p ON r.prob_id = p.prob_id " + GetFilter());

    QString htmlCode = ReportHTML::GetHTMLCodeFromRes("WorkStatement.html");
    ReportHTML::AddLineToHTML(htmlCode, "<body>");
    ReportHTML::AddLineToHTML(htmlCode, "<h1>Work statement</h1>");
    ReportHTML::AddLineToHTML(htmlCode, "<div class=\"CWS\" id=\"WS\"></div>");
    ReportHTML::AddLineToHTML(htmlCode, "</body>");
    ReportHTML::AddLineToHTML(htmlCode, "<script type=\"text/javascript\">");
    ReportHTML::AddLineToHTML(htmlCode, "var ws = new WorkStatement(document.all.WS);");
    while (query.next())
    {
        ReportHTML::AddLineToHTML(htmlCode, "ws.AddElem(new WSElem(\'" + query.value(0).toString() + "\', \'" +
                      query.value(1).toString() + "\', " +
                      QString::number(GetDifferenceInHours(query.value(2).toString(), query.value(3).toString())) + "));");
    }
    ReportHTML::AddLineToHTML(htmlCode, "ws.Draw();");
    ReportHTML::AddLineToHTML(htmlCode, "</script>");
    ReportHTML::AddLineToHTML(htmlCode, "</html>");
    repHTML->setHtml(htmlCode);

    btnExport->setEnabled(true);
}

void WorkStatement::UpdateCbBoxProblem(int aIndex)
{
    QSqlTableModel *t = static_cast<QSqlTableModel *>(cbboxProb->model());
    t->setFilter("proj_id = " + QString::number(vecProjId[aIndex]));
    t->select();
    InitVecId(t, vecProbId, Problem::iId);
}

void WorkStatement::SetProbEnabled(bool aB)
{
    chboxProb->setEnabled(aB);
    cbboxProb->setEnabled(aB && chboxProb->isChecked());
}

