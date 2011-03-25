#include "../headers/auth.h"

using namespace Settings::Developer;

extern void AddWidgetsInput(QGridLayout *aLt, unsigned int aRow, QLabel **aLbl, const QString &aStrLbl, QLineEdit **aEdt);

Auth::Auth(QWidget *aParent):
        QDialog(aParent, Qt::WindowTitleHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Authorization");

    CreateWidgets();

    setMaximumHeight(150);
    setMaximumWidth(300);
    setWindowIcon(QIcon(":/Resource/Images/Main.png"));
}

void Auth::CreateWidgets()
{
    using namespace Settings::Developer;

    QGridLayout *lt1 = new QGridLayout;
    AddWidgetsInput(lt1, 0, &lblLogin, tr("Name"), &edtLogin);
    AddWidgetsInput(lt1, 1, &lblPassw, tr("Password"), &edtPassw);
    edtLogin->setMaxLength(maxLenLogin);
    edtPassw->setMaxLength(maxLenPassword);
    edtPassw->setEchoMode(QLineEdit::Password);

    QHBoxLayout *lt2 = new QHBoxLayout;
    chboxPassw = new QCheckBox(tr("Show password"));
    lt2->addWidget(chboxPassw);
    chboxPassw->setChecked(false);
    connect(chboxPassw, SIGNAL(toggled(bool)), this, SLOT(ShowPassword(bool)));

    QVBoxLayout *lt3 = new QVBoxLayout;
    lt3->addLayout(lt1);
    lt3->addLayout(lt2);

    QGroupBox *gb = new QGroupBox;
    gb->setLayout(lt3);

    QHBoxLayout *lt4 = new QHBoxLayout;
    pbOk = new QPushButton(tr("OK"));
    pbCancel = new QPushButton(tr("Cancel"));
    lt4->addStretch();
    lt4->addWidget(pbOk);
    lt4->addStretch();
    lt4->addWidget(pbCancel);
    lt4->addStretch();
    connect(pbOk, SIGNAL(clicked()), this, SLOT(Authorization()));
    connect(pbCancel, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *ltMain = new QVBoxLayout;
    ltMain->addWidget(gb);
    ltMain->addLayout(lt4);

    setLayout(ltMain);
}

void Auth::Authorization()
{
    QSqlQuery query;
    QString strQuery = "SELECT * FROM developer WHERE dev_login = \'" + edtLogin->text() + "\'";
    query.exec(strQuery);

    if (!query.next())
    {
        QMessageBox::critical(this, "Error", "Illegal login");
        return;
    }

    if (query.value(iPassword).toString() != edtPassw->text())
    {
        QMessageBox::critical(this, "Error", "Incorrect password");
        return;
    }

    curLogin = query.value(iLogin).toString();
    curIsAdm = query.value(iIsAdm).toBool();
    MainWindow *mw = new MainWindow;
    mw->show();

    close();
}

void Auth::ShowPassword(bool aB)
{
    edtPassw->setEchoMode(!aB ? QLineEdit::Password : QLineEdit::Normal);
}

