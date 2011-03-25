#include "../headers/mainWindow.h"

using namespace Settings;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mdiArea = new QMdiArea;
    setWindowTitle(tr("AIS"));
    setWindowIcon(QIcon(":/Resource/Images/Main.png"));
    setCentralWidget(mdiArea);

    CreateMenu();
    CreateToolBars();
}

void MainWindow::CreateMenu()
{
    mnBar = new QMenuBar;
    setMenuBar(mnBar);

    mnFile = new QMenu(tr("&File"));
    actExit = mnFile->addAction(tr("&Exit"), this, SLOT(close()));
    actExit->setIcon(QIcon(":/Resource/Images/Exit.ico"));
    mnBar->addMenu(mnFile);

    mnTables = new QMenu(tr("&Tables"));
    actTblCompany = mnTables->addAction(tr("&Companies"), this, SLOT(CreateTableCompany()));
    actTblCompany->setIcon(QIcon(":/Resource/Images/Company.ico"));
    actTblProject = mnTables->addAction(tr("&Projects"), this, SLOT(CreateTableProject()));
    actTblProject->setIcon(QIcon(":/Resource/Images/Project.ico"));
    actTblDeveloper = mnTables->addAction(tr("&Developers"), this, SLOT(CreateTableDeveloper()));
    actTblDeveloper->setIcon(QIcon(":/Resource/Images/Developer.ico"));
    actTblProblem = mnTables->addAction(tr("&Problems"), this, SLOT(CreateTableProblem()));
    actTblProblem->setIcon(QIcon(":/Resource/Images/Problem.ico"));
    actTblContract = mnTables->addAction(tr("&Contracts"), this, SLOT(CreateTableContract()));
    actTblContract->setIcon(QIcon(":/Resource/Images/Contract.ico"));
    actTblReport = mnTables->addAction(tr("&Reports"), this, SLOT(CreateTableReport()));
    actTblReport->setIcon(QIcon(":/Resource/Images/Report.ico"));
    mnTables->addSeparator();
    actTblProblemDepend = mnTables->addAction(tr("&Problems (depending)"), this, SLOT(CreateTableProblemDepend()));
    actTblProblemDepend->setIcon(QIcon(":/Resource/Images/Depending.ico"));
    actTblDistrProjDev = mnTables->addAction(tr("&Distributions (projects - developers)"), this, SLOT(CreateTableDistrProjDev()));
    actTblDistrProjDev->setIcon(QIcon(":/Resource/Images/Distribution.ico"));
    mnBar->addMenu(mnTables);

    mnReports = new QMenu(tr("&Reports"));
    actRepWorking = mnReports->addAction(tr("&Work statement"), this, SLOT(CreateWorkStatement()));
    actRepWorking->setIcon(QIcon(":/Resource/Images/WorkStatement.ico"));
    actGanttChart = mnReports->addAction(tr("Gantt chart"), this, SLOT(CreateGanttChart()));
    actGanttChart->setIcon(QIcon(":/Resource/Images/GanttChart.png"));
    mnBar->addMenu(mnReports);

    mnAbout = new QMenu(tr("&About"));
    actAbout = mnAbout->addAction(tr("&About"), this, SLOT(ShowAbout()));
    actAbout->setIcon(QIcon(":/Resource/Images/About.ico"));
    mnBar->addMenu(mnAbout);

}

void MainWindow::CreateToolBars()
{
    tbTables = addToolBar(tr("Tables"));
    tbTables->addAction(actTblCompany);
    tbTables->addAction(actTblProject);
    tbTables->addAction(actTblDeveloper);
    tbTables->addAction(actTblProblem);
    tbTables->addAction(actTblContract);
    tbTables->addAction(actTblReport);
    tbTables->addSeparator();
    tbTables->addAction(actTblProblemDepend);
    tbTables->addAction(actTblDistrProjDev);
    tbTables->addSeparator();
    tbTables->addAction(actRepWorking);
    tbTables->addAction(actGanttChart);
    tbTables->addSeparator();
    tbTables->addAction(actAbout);
    tbTables->setFloatable(false);
}

inline QMdiSubWindow *MainWindow::CreateTable(ETable aType, Table *aTable)
{
    QMdiSubWindow *sw = mdiArea->addSubWindow(aTable);
    sw->show();
    mapTable.insert(aType, aTable);
    connect(aTable, SIGNAL(destroyed(QObject *)), this, SLOT(DestroyTable(QObject *)));
    return sw;
}

void MainWindow::DestroyTable(QObject *aTable)
{
    MapTable::iterator it = mapTable.find(static_cast<Table *>(aTable)->Type());
    if (it != mapTable.end())
    {
        mapTable.erase(it);
    }
}

bool MainWindow::SetActiveTable(ETable aType)
{
    MapTable::iterator it = mapTable.find(aType);
    if (it == mapTable.end())
    {
        return false;
    }
    it.value()->setFocus();
    return true;
}

void MainWindow::CreateTableCompany()
{
    if (!SetActiveTable(tCompany))
    {
        QMdiSubWindow *sw = CreateTable(tCompany, new TblCompany(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Company.ico"));
    }
}

void MainWindow::CreateTableProject()
{
    if (!SetActiveTable(tProject))
    {
        QMdiSubWindow *sw = CreateTable(tProject, new TblProject(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Project.ico"));
    }
}

void MainWindow::CreateTableDeveloper()
{
    if (!SetActiveTable(tDeveloper))
    {
        QMdiSubWindow *sw = CreateTable(tDeveloper, new TblDeveloper(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Developer.ico"));
    }
}

void MainWindow::CreateTableProblem()
{
    if (!SetActiveTable(tProblem))
    {
        QMdiSubWindow *sw = CreateTable(tProblem, new TblProblem(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Problem.ico"));
    }
}

void MainWindow::CreateTableContract()
{
    if (!SetActiveTable(tContract))
    {
        QMdiSubWindow *sw = CreateTable(tContract, new TblContract(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Contract.ico"));
    }
}

void MainWindow::CreateTableReport()
{
    if (!SetActiveTable(tReport))
    {
        QMdiSubWindow *sw = CreateTable(tReport, new TblReport(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Report.ico"));
    }
}

void MainWindow::CreateTableProblemDepend()
{
    if (!SetActiveTable(tProblemDepend))
    {
        QMdiSubWindow *sw = CreateTable(tProblemDepend, new TblProblemDepend(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Depending.ico"));
    }
}

void MainWindow::CreateTableDistrProjDev()
{
    if (!SetActiveTable(tDistrProjDev))
    {
        QMdiSubWindow *sw = CreateTable(tDistrProjDev, new TblDistrProjDev(mdiArea));
        sw->setWindowIcon(QIcon(":/Resource/Images/Distribution.ico"));
    }
}

void MainWindow::CreateWorkStatement()
{
    QMdiSubWindow *sw = mdiArea->addSubWindow(new WorkStatement(mdiArea));
    sw->show();
    sw->setWindowIcon(QIcon(":/Resource/Images/WorkStatement.ico"));
}

void MainWindow::CreateGanttChart()
{
    QMdiSubWindow *sw = mdiArea->addSubWindow(new GanttChart(mdiArea));
    sw->show();
    sw->setWindowIcon(QIcon(":/Resource/Images/GanttChart.png"));
}

void MainWindow::ShowAbout()
{
    QMessageBox::about(this, tr("About AIS"),
            tr("<h2>AIS 1.0</h2>"
               "<p>Copyright &copy; 2011 Vasiliy Tsubenko.</p>"
               "<p>AIS (Automated Information System) is a small application "
               "to monitor development for projects and accounting activities of "
               "developers in the company.</p>"));
}
