#ifndef GANTTCHART_H
#define GANTTCHART_H

#include "../headers/report.h"

//----------------------------------- Gantt task -----------------------------------

class GanttTask
{
private:
    unsigned int id;
    QString name;
    unsigned int status;
    unsigned int timePlan;
    QDateTime tBegin;
    QDateTime tEnd;

public:
    GanttTask(unsigned int aId, const QString &aName, unsigned int aStatus, const QDateTime &aTBegin, const QDateTime &aTEnd);
    GanttTask(unsigned int aId, const QString &aName, unsigned int aStatus);
    GanttTask(const GanttTask &aTask);

    unsigned int Id() const;
    QString Name() const;
    unsigned int Status() const;
    unsigned int TimePlan() const;
    QDateTime TBegin() const;
    QDateTime TEnd() const;

    void SetTimePlan(unsigned int aTimePlan);
    void SetTBegin(const QDateTime &aTBegin);
    void SetTEnd(const QDateTime &aTEnd);

    GanttTask &operator =(const GanttTask &aTask);
};

typedef QMap<unsigned int, GanttTask> MapTask;

//----------------------------------- Gantt chart -----------------------------------

class GanttChart: public Report
{
    Q_OBJECT

private:
    QVector<unsigned int> vecProjId;

    QComboBox *cbboxProj;

    QVBoxLayout *_CreateWidgets();

    void CreateHTML(const MapTask &aMapTask);

private slots:
    void CreateReport();

public:
    GanttChart(QWidget *aParent);
};

#endif // GANTTCHART_H
