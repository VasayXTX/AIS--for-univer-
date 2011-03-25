#include "../headers/ganttChart.h"

using namespace Settings;

//----------------------------------- Gantt task -----------------------------------

GanttTask::GanttTask(unsigned int aId, const QString &aName, unsigned int aStatus, const QDateTime &aTBegin,
                     const QDateTime &aTEnd):
        id(aId),
        name(aName),
        status(aStatus),
        tBegin(aTBegin),
        tEnd(aTEnd)
{
    //empty
}

GanttTask::GanttTask(unsigned int aId, const QString &aName, unsigned int aStatus):
        id(aId),
        name(aName),
        status(aStatus)
{
    //empty
}

GanttTask::GanttTask(const GanttTask &aTask)
{
    *this = aTask;
}

unsigned int GanttTask::Id() const
{
    return id;
}

QString GanttTask::Name() const
{
    return name;
}

unsigned int GanttTask::Status() const
{
    return status;
}

unsigned int GanttTask::TimePlan() const
{
    return timePlan;
}

QDateTime GanttTask::TBegin() const
{
    return tBegin;
}

QDateTime GanttTask::TEnd() const
{
    return tEnd;
}

void GanttTask::SetTimePlan(unsigned int aTimePlan)
{
    timePlan = aTimePlan;
}

void GanttTask::SetTBegin(const QDateTime &aTBegin)
{
    tBegin = aTBegin;
}

void GanttTask::SetTEnd(const QDateTime &aTEnd)
{
    tEnd = aTEnd;
}

GanttTask &GanttTask::operator =(const GanttTask &aTask)
{
    id = aTask.id;
    name = aTask.name;
    status = aTask.status;
    tBegin = aTask.tBegin;
    tEnd = aTask.tEnd;
    return *this;
}

//----------------------------------- Gantt chart -----------------------------------

extern void InitVecId(QSqlTableModel *aModel, QVector<unsigned int> &aVecId, unsigned int aIndex);

GanttChart::GanttChart(QWidget *aParent):
        Report(aParent)
{
    CreateWidgets("EmptyGantt.html");
    setWindowTitle(tr("Gantt chart"));
}

inline QGroupBox *CreateGB(QWidget *aW)
{
    QHBoxLayout *lt = new QHBoxLayout;
    lt->addWidget(aW);
    QGroupBox *gb = new QGroupBox;
    gb->setLayout(lt);
    return gb;
}

QVBoxLayout *GanttChart::_CreateWidgets()
{
    cbboxProj = new QComboBox();
    QSqlTableModel *t = new QSqlTableModel(this);
    t->setTable("project");
    t->select();
    InitVecId(t, vecProjId, Project::iId);
    cbboxProj->setModel(t);
    cbboxProj->setModelColumn(Project::iName);

    QGroupBox *gbProj = CreateGB(cbboxProj);
    gbProj->setTitle(tr("Project:"));

    QVBoxLayout *lt1 = new QVBoxLayout;
    lt1->addStretch(0);
    lt1->addWidget(gbProj);

    return lt1;
}

QDateTime ProjBegin(unsigned int aProjId)
{
    QSqlQuery query;
    query.exec("SELECT date_begin FROM project WHERE proj_id = " + QString::number(aProjId));
    query.next();
    return QDateTime::fromString(query.record().value(0).toString(), Qt::ISODate);
}

QDateTime FuncDateFromRep(unsigned int aProbId, const QString &aFunc)
{
    QSqlQuery query;
    query.exec("SELECT " + aFunc + " FROM report WHERE prob_id = " + QString::number(aProbId));
    QString res;
    if (!query.next() || (res = query.record().value(0).toString()).isEmpty())
    {
        throw Error(QObject::tr("There are some problems in work, that haven\'t any  reports"));
    }
    return QDateTime::fromString(res, Qt::ISODate);
}

bool ProbIsDepend(unsigned int aProbId)
{
    QSqlQuery query;
    query.exec("SELECT * FROM problem_depend WHERE prob2_id = " + QString::number(aProbId));
    return query.next();
}

QDateTime DateTimeAddHours(QDateTime aDate, unsigned int aTimePlan)
{
    unsigned int time = aTimePlan % WorkingWeek::length;
    unsigned int dayCount = aTimePlan / WorkingWeek::length;
    aDate = aDate.addSecs(time * 60 * 60);
    if (aDate.time() > WorkingWeek::tEnd)
    {
        ++dayCount;
        int difSec = WorkingWeek::tEnd.secsTo(aDate.time());
        aDate.setTime(WorkingWeek::tBegin.addSecs(difSec));
    }
    unsigned int dw = aDate.date().dayOfWeek();
    unsigned int tmp = (dayCount +  dw) / (WorkingWeek::workingDayCount + 1) * WorkingWeek::weekendCount + dayCount;
    aDate = aDate.addDays(tmp);
    QMap<unsigned int, unsigned int>::const_iterator it = WorkingWeek::vecDif.find(aDate.date().dayOfWeek());
    if (it != WorkingWeek::vecDif.end())
    {
        aDate = aDate.addDays(it.value());
    }
    return aDate;
}

void AddTaskDep(MapTask &aMapTask, QList<GanttTask> &aLstTaskDep)
{
    const QDateTime tMin = QDateTime(QDate(1, 0, 0), QTime(0, 0, 0));
    QSqlQuery query;
    while (!aLstTaskDep.empty())
    {
        for (QList<GanttTask>::iterator it = aLstTaskDep.begin(); it != aLstTaskDep.end(); ++it)
        {
            query.exec("SELECT prob1_id FROM problem_depend WHERE prob2_id = " + QString::number((*it).Id()));
            QDateTime tMax(tMin);
            bool isMax = false;
            while (query.next())
            {
                MapTask::iterator it = aMapTask.find(query.record().value(0).toInt());
                if (it == aMapTask.end())
                {
                    continue;
                }
                if (it.value().TEnd() > tMax)
                {
                    tMax = it.value().TEnd();
                    isMax = true;
                }
            }
            if (isMax)
            {
                if (tMax.time() == WorkingWeek::tEnd)
                {
                    tMax = tMax.addDays(1);
                    tMax.setTime(WorkingWeek::tBegin);
                }
                (*it).SetTBegin(tMax);
                (*it).SetTEnd(DateTimeAddHours(tMax, (*it).TimePlan()));
                aMapTask.insert((*it).Id(), *it);
                aLstTaskDep.erase(it);
                break;
            }
        }
    }
}

inline QString FromQDateTime(const QDateTime &aT)
{
    QString res = QString::number(aT.date().day()) + "." +
                  QString::number(aT.date().month()) + "." +
                  QString::number(aT.date().year()) + " " +
                  QString::number(aT.time().hour()) + ":" +
                  QString::number(aT.time().minute()) + ":" +
                  QString::number(aT.time().second());
    return res;
}

void GanttChart::CreateHTML(const MapTask &aMapTask)
{
    typedef ReportHTML rHTML;

    QString htmlCode(qUncompress(rHTML::GetHTMLCodeFromRes("GanttChart.html")));
    rHTML::AddLineToHTML(htmlCode, "<body>");
    rHTML::AddLineToHTML(htmlCode, "<h1>Gantt chart for project " + cbboxProj->currentText() + "</h1>");
    rHTML::AddLineToHTML(htmlCode, "<div style=\"position:relative\" class=\"Gantt\" id=\"GanttChart\"></div>");
    rHTML::AddLineToHTML(htmlCode, "</body>");
    rHTML::AddLineToHTML(htmlCode, "<script>");
    rHTML::AddLineToHTML(htmlCode, "var g = new Gantt(document.all.GanttChart);");
    for (MapTask::const_iterator it = aMapTask.begin(); it != aMapTask.end(); ++it)
    {
        GanttTask gt = it.value();
        rHTML::AddLineToHTML(htmlCode, "g.AddTask(new Task(\'" + FromQDateTime(gt.TBegin()) + "\', \'" + FromQDateTime(gt.TEnd()) +
                             "\', \'" + gt.Name() + "\', " + QString::number(gt.Status()) + "));");
    }
    rHTML::AddLineToHTML(htmlCode, "g.Draw();");
    rHTML::AddLineToHTML(htmlCode, "</script>");
    rHTML::AddLineToHTML(htmlCode, "</html>");
    repHTML->setHtml(htmlCode);
}

void GanttChart::CreateReport()
{
    QSqlQuery query;
    query.exec("SELECT * FROM problem WHERE proj_id = " + QString::number(vecProjId[cbboxProj->currentIndex()]));
    MapTask mapTask;
    QList<GanttTask> lstTaskDep;     //контейнер из неначатых зависимых задач
    QDateTime tBegin, tEnd;
    try
    {
        while(query.next())
        {
            unsigned int id = query.record().value(Problem::iId).toInt();
            QString name = query.record().value(Problem::iName).toString();
            unsigned int timePlan = query.record().value(Problem::iTimePlan).toInt();
            unsigned int stat = query.record().value(Problem::iStatus).toInt();
            if (!stat)  //Если задача не начата
            {
                if (!ProbIsDepend(id))  //если задача независима
                {
                    tBegin = ProjBegin(vecProjId[cbboxProj->currentIndex()]);
                    tEnd = DateTimeAddHours(tBegin, timePlan);
                }
                else
                {
                    lstTaskDep.push_back(GanttTask(id, name, stat));
                    lstTaskDep.back().SetTimePlan(timePlan);
                    continue;
                }
            }
            else
            {
                tBegin = FuncDateFromRep(id, "MIN(date_begin)");
                if (stat == 1)  //если задача начата, но не закончена
                {
                    QDateTime t1 = FuncDateFromRep(id, "MAX(date_end)");
                    QDateTime t2 = DateTimeAddHours(tBegin, timePlan);;
                    tEnd = t1 > t2 ? t1 : t2;
                }
                else
                {
                    tEnd = FuncDateFromRep(id, "MAX(date_end)");
                }
            }
            mapTask.insert(id, GanttTask(id, name, stat, tBegin, tEnd));
        }
        AddTaskDep(mapTask, lstTaskDep);
        CreateHTML(mapTask);
        btnExport->setEnabled(true);
    }
    catch(Error &aErr)
    {
        QMessageBox::critical(0, QObject::tr("Error"), aErr.Text());
        repHTML->SetDefault("EmptyGantt.html");
        btnExport->setEnabled(false);
    }
}

