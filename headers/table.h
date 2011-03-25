#ifndef TABLE_H
#define TABLE_H

#include "../headers/card.h"
#include "../headers/settings.h"

//----------------------------------- Таблица (базовый класс) -----------------------------------

class Table: public QMainWindow
{
    Q_OBJECT

private:
    Settings::ETable type;
    unsigned int countInsertCard;

    QToolBar *tb;
    QAction *actFirst;
    QAction *actNext;
    QAction *actPrev;
    QAction *actLast;
    QAction *actAdd;
    QAction *actEdit;
    QAction *actDel;

private slots:
    void SetFirst();
    void SetNext();
    void SetPrev();
    void SetLast();
    void Add();
    void Edit();
    void Del();
    void DestroyCard(QObject *aObj);
    void DecCountInsertCard();

protected:
    QMdiArea *mdiArea;
    QTableView *view;
    QSqlRelationalTableModel *model;

    typedef QList<Card *> LstCard;
    LstCard lstCard;

    Table(QWidget *aParent, Settings::ETable aType);

    void Init(const QString &aTitle, const QString &aTblName);

    void closeEvent(QCloseEvent *aE);

    virtual bool IsAccessible(QModelIndex &aIndex, bool aIsDel) const;

protected slots:
    void OpenCard(QModelIndex aIndex);
    virtual Card *CreateCard(int aRow) const = 0;
    Card *GetCard(const QString &aId) const;

public:
    Settings::ETable Type() const;
};

//----------------------------------- Таблица company -----------------------------------

class TblCompany: public Table
{
    Q_OBJECT

private:
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

public:
    explicit TblCompany(QWidget *aParent);
};

//----------------------------------- Таблица project -----------------------------------

class TblProject: public Table
{
    Q_OBJECT

private:
    bool lastIsManager;
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

    bool IsAccessible(QModelIndex &aIndex, bool aIsDel) const;

public:
    explicit TblProject(QWidget *aParent);
};

//----------------------------------- Таблица developer -----------------------------------

class TblDeveloper: public Table
{
    Q_OBJECT

private:
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

public:
    explicit TblDeveloper(QWidget *aParent);
};

//----------------------------------- Таблица problem -----------------------------------

class TblProblem: public Table
{
    Q_OBJECT

private:
    static bool isManager;
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

    bool IsAccessible(QModelIndex &aIndex, bool aIsDel) const;

public:
    explicit TblProblem(QWidget *aParent);
};

//----------------------------------- Таблица contract -----------------------------------

class TblContract: public Table
{
    Q_OBJECT

private:
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

public:
    explicit TblContract(QWidget *aParent);
};

//----------------------------------- Таблица report -----------------------------------

class TblReport: public Table
{
    Q_OBJECT

private:
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

    bool IsAccessible(QModelIndex &aIndex, bool aIsDel) const;

public:
    explicit TblReport(QWidget *aParent);
};

//----------------------------------- Таблица problem_depend -----------------------------------

class TblProblemDepend: public Table
{
    Q_OBJECT

private:
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

    bool IsAccessible(QModelIndex &aIndex, bool aIsDel) const;

public:
    explicit TblProblemDepend(QWidget *aParent);
};

//----------------------------------- Таблица distr_proj_dev -----------------------------------

class TblDistrProjDev: public Table
{
    Q_OBJECT

private:
    Card *CreateCard(int aRow) const;
    Card *GetCard(const QSqlRecord &aRec) const;

    bool IsAccessible(QModelIndex &aIndex, bool aIsDel) const;

public:
    explicit TblDistrProjDev(QWidget *aParent);
};

#endif // TABLE_H
