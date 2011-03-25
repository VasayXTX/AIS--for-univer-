#ifndef CARD_H
#define CARD_H

#include "../headers/settings.h"

//----------------------------------- Карточка для таблиц (базовая) -----------------------------------

class Card: public QDialog
{
    Q_OBJECT

private:
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QMdiArea *mdiArea;
    Settings::ETable type;

private slots:
    void Ok();
    void Cancel();

protected:
    QGroupBox *grpBox;
    QSqlRelationalTableModel *model;
    QSqlRelationalTableModel proxyModel;
    QDataWidgetMapper *mapper;

    void CreateWidgetsBase(QLayout *aLt);
    void closeEvent(QCloseEvent *aE);
    virtual bool IsValid() const;

    inline void _SubmitForSheckBox_(QCheckBox *aChBox, unsigned int aI);
    virtual void SubmitForSheckBox();

public:
    Card(QWidget *aParent, QSqlRelationalTableModel *aModel, Settings::ETable aType, const QString &aFilter, bool aIsInsert);
    QString Id() const;
    Settings::ETable Type() const;
};

//----------------------------------- Карточка для таблицы company -----------------------------------

class CardCompany: public Card
{
    Q_OBJECT

private:
    QLabel *lblName;
    QLineEdit *edtName;

    void CreateWidgets();
    bool IsValid() const;

public:
    CardCompany(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

//----------------------------------- Карточка для таблицы project -----------------------------------

class CardProject: public Card
{
    Q_OBJECT

private:
    QLabel *lblName;
    QLabel *lblDateBegin;
    QLabel *lblDateEndExpect;
    QLineEdit *edtName;
    QDateTimeEdit *edtDateBegin;
    QDateTimeEdit *edtDateEndExpect;
    QCheckBox *chboxIsComplete;

    void CreateWidgets();

    bool IsValid() const;
    void SubmitForSheckBox();

public:
    CardProject(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

//----------------------------------- Карточка для таблицы developer -----------------------------------

class CardDeveloper: public Card
{
    Q_OBJECT

private:
    QString oldLogin;

    QLabel *lblLogin;
    QLabel *lblPassw;
    QLabel *lblPasswRep;
    QLabel *lblFirstName;
    QLabel *lblLastName;
    QLabel *lblCompany;
    QLineEdit *edtLogin;
    QLineEdit *edtPassw;
    QLineEdit *edtPasswRep;
    QLineEdit *edtFirstName;
    QLineEdit *edtLastName;
    QCheckBox *chboxIsAdm;
    QComboBox *cbboxCompany;

    void CreateWidgets();
    bool IsValid() const;
    void SubmitForSheckBox();

public:
    CardDeveloper(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

//----------------------------------- Карточка для таблицы problem -----------------------------------

class CardProblem: public Card
{
    Q_OBJECT

private:
    QString probName;
    unsigned int probId;

    QLabel *lblName;
    QLabel *lblDeclaration;
    QLabel *lblTimePlan;
    QLabel *lblStatus;
    QLabel *lblStatusVal;
    QLabel *lblProject;
    QLineEdit *edtName;
    QLineEdit *edtDeclaration;
    QSpinBox *spnboxTimePlan;
    QSlider *sldrStatus;
    QComboBox *cbboxProject;

    void CreateWidgets();
    bool IsValid() const;

private slots:
    void SetStatus(int aStatus);

public:
    CardProblem(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow, bool aIsManager);
};

//----------------------------------- Карточка для таблицы contract -----------------------------------

class CardContract: public Card
{
    Q_OBJECT

private:
    QLabel *lblNum;
    QLabel *lblDateConclusion;
    QLabel *lblStatus;
    QLabel *lblStatusVal;
    QLabel *lblCompany;
    QLabel *lblProject;
    QLineEdit *edtNum;
    QDateTimeEdit *edtDateConclusion;
    QSlider *sldrStatus;
    QComboBox *cbboxCompany;
    QComboBox *cbboxProject;

    void CreateWidgets();
    bool IsValid() const;

private slots:
    void SetStatus(int aStatus);

public:
    CardContract(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

//----------------------------------- Карточка для таблицы report -----------------------------------

class CardReport: public Card
{
    Q_OBJECT

private:
    unsigned int repId;
    static QVector<unsigned int> vecIdProb;
    QLabel *lblDateBegin;
    QLabel *lblDateEnd;
    QLabel *lblDeclaration;
    QLabel *lblDeveloper;
    QLabel *lblProblem;
    QDateTimeEdit *edtDateBegin;
    QDateTimeEdit *edtDateEnd;
    QLineEdit *edtDeclaration;
    QComboBox *cbboxDeveloper;
    QComboBox *cbboxProblem;

    void CreateWidgets();
    bool IsValid() const;

public:
    CardReport(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

//----------------------------------- Карточка для таблицы problem_depend -----------------------------------

class CardProblemDepend: public Card
{
    Q_OBJECT

private:
    static QVector<unsigned int> vecIdProb;
    QLabel *lblProblem1;
    QLabel *lblProblem2;
    QComboBox *cbboxProblem1;
    QComboBox *cbboxProblem2;

    void CreateWidgets();
    bool IsValid() const;

public:
    CardProblemDepend(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

//----------------------------------- Карточка для таблицы distr_proj_dev -----------------------------------

class CardDistrProjDev: public Card
{
    Q_OBJECT

private:
    QLabel *lblDeveloper;
    QLabel *lblProject;
    QComboBox *cbboxDeveloper;
    QComboBox *cbboxProject;
    QCheckBox *chboxIsManager;

    void CreateWidgets();
    bool IsValid() const;
    void SubmitForSheckBox();

public:
    CardDistrProjDev(QWidget *aParent, QSqlRelationalTableModel *aModel, int aRow);
};

#endif // CARD_H
