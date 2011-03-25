#include "../headers/reportHTML.h"

ReportHTML::ReportHTML(QWidget *aParent):
        QWebView(aParent)
{
    //empty
}

void ReportHTML::AddLineToHTML(QString &aHTMLCode, const QString &aStr)
{
    aHTMLCode += QChar(10) + aStr;
}

QByteArray ReportHTML::GetHTMLCodeFromRes(const QString &aHTMLName)
{
    QResource res(":/Resource/Html/" + aHTMLName);
    QByteArray tmp((const char*)res.data(), res.size());
    return tmp;
}

inline void CheckFName(QString &aFName)
{
    unsigned int i = aFName.size();
    while (i > 0 && aFName[i-1] != '.')
    {
        --i;
    }
    if (!i)
    {
        aFName += ".html";
    }
}

void ReportHTML::ExportHTML()
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFilter("*.html *.htm");
    dialog.setNameFilter(QObject::tr("Web pages (*.html *.htm)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec())
    {
        QString fName = dialog.selectedFiles().front();
        try
        {
            if (fName.isEmpty())
            {
                throw Error(QObject::tr("File name is not chosen"));
            }
            CheckFName(fName);
            QFile file(fName);
            if (!file.open(QFile::WriteOnly))
            {
                throw Error(QObject::tr("Failed to create file ") + fName);
            }
            QTextStream out(&file);
            QString str = page()->currentFrame()->toHtml();
            out << str;
            file.close();
        }
        catch(Error &aErr)
        {
            QMessageBox::critical(0, QObject::tr("Error"), aErr.Text());
        }
    }
}

void ReportHTML::SetDefault(const QString &aHTMLName)
{
    setHtml(GetHTMLCodeFromRes(aHTMLName));
}
