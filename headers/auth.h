#ifndef AUTH_H
#define AUTH_H

#include "../headers/mainWindow.h"
#include "../headers/card.h"
#include "../headers/settings.h"

class Auth: public QDialog
{
    Q_OBJECT

public:
    explicit Auth(QWidget *aParent = 0);

private:
    QLabel *lblLogin;
    QLabel *lblPassw;
    QLineEdit *edtLogin;
    QLineEdit *edtPassw;
    QPushButton *pbOk;
    QPushButton *pbCancel;
    QCheckBox *chboxPassw;

    void CreateWidgets();

private slots:
    void ShowPassword(bool aB);
    void Authorization();
};

#endif // AUTH_H
