#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../headers/settings.h"
#include "../headers/table.h"
#include "../headers/workStatement.h"
#include "../headers/ganttChart.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QMenuBar *mnBar;

    QMenu *mnFile;
    QMenu *mnTables;
    QMenu *mnReports;
    QMenu *mnAbout;

    QToolBar *tbTables;

    QAction *actExit;
    QAction *actTblCompany;
    QAction *actTblProject;
    QAction *actTblDeveloper;
    QAction *actTblProblem;
    QAction *actTblContract;
    QAction *actTblReport;
    QAction *actTblProblemDepend;
    QAction *actTblDistrProjDev;
    QAction *actRepWorking;
    QAction *actGanttChart;
    QAction *actAbout;

    QMdiArea *mdiArea;

    typedef QMap<Settings::ETable, Table *> MapTable;
    MapTable mapTable;

    void CreateMenu();
    void CreateToolBars();

    bool SetActiveTable(Settings::ETable aType);
    inline QMdiSubWindow *CreateTable(Settings::ETable aType, Table *aTable);

private slots:
    void DestroyTable(QObject *aTable);
    void CreateTableCompany();
    void CreateTableProject();
    void CreateTableDeveloper();
    void CreateTableProblem();
    void CreateTableContract();
    void CreateTableReport();
    void CreateTableProblemDepend();
    void CreateTableDistrProjDev();
    void CreateWorkStatement();
    void CreateGanttChart();
    void ShowAbout();

public:
    MainWindow(QWidget *parent = 0);
};

#endif // MAINWINDOW_H
