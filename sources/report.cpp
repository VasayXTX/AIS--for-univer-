#include "../headers/report.h"

Report::Report(QWidget *aParent):
        QDialog(aParent)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

void Report::CreateWidgets(const QString &aStrHTMLEmpty)
{
    QVBoxLayout *lt1 = _CreateWidgets();

    btnCreate = new QPushButton(tr("Create report"));
    connect(btnCreate, SIGNAL(clicked()), this, SLOT(CreateReport()));
    btnCreate->setIcon(QIcon(":/Resource/Images/Create.ico"));
    btnCreate->setIconSize(QSize(30, 30));

    btnExport = new QPushButton(tr("Export HTML"));
    btnExport->setEnabled(false);
    connect(btnExport, SIGNAL(clicked()), this, SLOT(ExportHTML()));
    btnExport->setIcon(QIcon(":/Resource/Images/ExportHTML.ico"));
    btnExport->setIconSize(QSize(30, 30));

    lt1->addWidget(btnCreate);
    lt1->addWidget(btnExport);
    lt1->addStretch(1);
    lt1->setSpacing(20);

    QGroupBox *gbPar = new QGroupBox;
    gbPar->setLayout(lt1);

    repHTML = new ReportHTML;
    repHTML->SetDefault(aStrHTMLEmpty);

    QHBoxLayout *lt2 = new QHBoxLayout;
    lt2->addWidget(repHTML);
    QGroupBox *gbRepHTML = new QGroupBox;
    gbRepHTML->setLayout(lt2);

    QHBoxLayout *lt = new QHBoxLayout;
    lt->addWidget(gbPar);
    lt->addWidget(gbRepHTML);
    lt->setStretch(0, 0);
    lt->setStretch(1, 1);

    setLayout(lt);
}

void Report::ExportHTML()
{
    repHTML->ExportHTML();
}
