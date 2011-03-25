#ifndef REPORTHTML_H
#define REPORTHTML_H

#include "../headers/settings.h"

class ReportHTML: public QWebView
{
    Q_OBJECT

public:
    ReportHTML(QWidget *aParent = 0);

    static void AddLineToHTML(QString &aHTMLCode, const QString &aStr);
    static QByteArray GetHTMLCodeFromRes(const QString &aHTMLName);

    void ExportHTML();

    void SetDefault(const QString &aHTMLName);
};

#endif // REPORTHTML_H
