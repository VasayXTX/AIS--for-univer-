#include "../headers/settings.h"
#include "../headers/error.h"
#include "../headers/auth.h"

using namespace Settings;

extern void InitSettings();

void Connect()
{
    using namespace Database;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if (!db.isValid())
    {
        throw Error(QObject::tr("Error in connection with database ") + fullName);
    }
    db.setDatabaseName(fullName);
    if (!db.open())
    {
        throw Error(QObject::tr("Error in connection with database ") + fullName);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    try
    {
        InitSettings();
        Connect();
    }
    catch(Error &aErr)
    {
        QMessageBox::critical(0, QObject::tr("Error"), aErr.Text());
        return 1;
    }

    Auth *auth = new Auth;
    auth->show();

    return app.exec();
}

