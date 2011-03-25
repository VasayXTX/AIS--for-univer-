#ifndef WORKSTATEMENT_H
#define WORKSTATEMENT_H

#include "../headers/report.h"

class WorkStatement: public Report
{
    Q_OBJECT

private:
    static const unsigned int COL_COUNT = 3;

    QVector<unsigned int> vecProjId;
    QVector<unsigned int> vecProbId;

    QCheckBox *chboxDev;
    QCheckBox *chboxProj;
    QCheckBox *chboxProb;
    QComboBox *cbboxDev;
    QComboBox *cbboxProj;
    QComboBox *cbboxProb;

    QVBoxLayout *_CreateWidgets();
    QString GetFilter();

private slots:
    void CreateReport();
    void UpdateCbBoxProblem(int aIndex);
    void SetProbEnabled(bool);

public:
    WorkStatement(QWidget *aParent);
};

#endif // WORKSTATEMENT_H
