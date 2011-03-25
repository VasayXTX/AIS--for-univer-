#ifndef REPORT_H
#define REPORT_H

#include "../headers/settings.h"
#include "../headers/error.h"
#include "../headers/reportHTML.h"

class Report: public QDialog
{
    Q_OBJECT

protected:
    QPushButton *btnCreate;
    QPushButton *btnExport;
    ReportHTML *repHTML;

    void CreateWidgets(const QString &aStrHTMLEmpty);

    virtual QVBoxLayout *_CreateWidgets() = 0;

protected slots:
    virtual void CreateReport() = 0;
    void ExportHTML();

public:
    Report(QWidget *aParent = 0);
};

#endif // REPORT_H
