#include "../headers/settings.h"

using namespace Settings;

QString Database::configFile = "Config.ais";
QString Database::fullName;
QString Database::datetimeSplit = "#";

unsigned int Company::maxLenName = 100;

unsigned int Project::maxLenName = 100;

unsigned int Developer::maxLenLogin = 20;
unsigned int Developer::maxLenPassword = 20;
unsigned int Developer::maxLenFirstName = 20;
unsigned int Developer::maxLenLastName = 20;
bool Developer::curIsAdm;
QString Developer::curLogin;

unsigned int Problem::maxLenName = 100;
unsigned int Problem::maxLenDeclaration = 200;
QVector<QString> Problem::vecStatus;

QVector<QString> Contract::vecStatus;

unsigned int Report::maxLenDeclaration = 200;

unsigned int WorkingWeek::length = 8;
QTime WorkingWeek::tBegin = QTime(9, 0, 0);
QTime WorkingWeek::tEnd = QTime(17, 0, 0);
unsigned int WorkingWeek::weekendCount = 2;
unsigned int WorkingWeek::workingDayCount = 5;
QMap<unsigned int, unsigned int> WorkingWeek::vecDif;

void InitSettings()
{
    QFile f(Database::configFile);
    if (!f.open(QFile::ReadOnly))
    {
        throw Error(QObject::tr("Failed to open file ") + Database::configFile);
    }
    QTextStream in(&f);
    Database::fullName = in.readLine();
    f.close();

    Problem::vecStatus.push_back(QObject::tr("not started"));
    Problem::vecStatus.push_back(QObject::tr("in work"));
    Problem::vecStatus.push_back(QObject::tr("completed"));

    Contract::vecStatus.push_back(QObject::tr("not concluded"));
    Contract::vecStatus.push_back(QObject::tr("concluded"));
    Contract::vecStatus.push_back(QObject::tr("terminated"));

    WorkingWeek::vecDif[6] = 2;
    WorkingWeek::vecDif[7] = 1;
}

void InitVecId(QSqlTableModel *aModel, QVector<unsigned int> &aVecId, unsigned int aIndex)
{
    aVecId.resize(aModel->rowCount());
    for (int i = 0; i < aModel->rowCount(); ++i)
    {
        aVecId[i] = aModel->record(i).value(aIndex).toInt();
    }
}
