#include "ui/MainWindow.h"
#include "qboxlayout.h"
#include "qdebug.h"
#include "qfont.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qlist.h"
#include "qwidget.h"

MainWindow::MainWindow(const QList<TableRow> &myRows, QWidget *parent)
    : QWidget(parent)
    , myRows(myRows)
{
    this->hide();

    setupUi();

    this->show();
    myTextEdit->setPlainText(TableRow::rowsListToTableAscii(myRows));
}

void MainWindow::setupUi()
{
    QVBoxLayout *myMainLayout = new QVBoxLayout(this);

    myTextEdit = new QTextEdit(this);
    myTextEdit->setReadOnly(true);
    myTextEdit->setWordWrapMode(QTextOption::NoWrap);
    myTextEdit->setUndoRedoEnabled(false);

    // qDebug() << QFontDatabase::families();
    QFont myFontMono("Monospace");
    myFontMono.setStyleHint(QFont::TypeWriter);
    myTextEdit->setFont(myFontMono);
    // myTextEdit->setAcceptRichText(false);
    // myTextEdit->setContextMenuPolicy(Qt::NoContextMenu);

    myMainLayout->addWidget(myTextEdit);

    QHBoxLayout *mySearchLayout = new QHBoxLayout();
    {
        {
            QLabel *labelFilter = new QLabel(this);
            labelFilter->setText(tr("Filter table:"));
            mySearchLayout->addWidget(labelFilter);

            QLineEdit *lineEditFilter = new QLineEdit(this);
            lineEditFilter->setToolTip(tr("Filter table"));
            mySearchLayout->addWidget(lineEditFilter);

            connect(lineEditFilter, &QLineEdit::textEdited,
            this, [ this] (const QString & szText) {
                this->searchTextChanged(szText);
            });
        }

        mySearchLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

        {
            QLabel *labelSearchText = new QLabel(this);
            labelSearchText->setText(tr("Search:"));
            mySearchLayout->addWidget(labelSearchText);

            QLineEdit *lineEditServerName = new QLineEdit(this);
            lineEditServerName->setToolTip(tr("Search Timezone"));
            mySearchLayout->addWidget(lineEditServerName);

            QStringList szaSearchText;
            szaSearchText.reserve(myRows.size());

            for (const TableRow &myRow : myRows) {
                szaSearchText.append(myRow.country + " - " + myRow.zoneID);
            }

            QCompleter *completerTimezones = new QCompleter(szaSearchText, this);
            completerTimezones->setCompletionMode(QCompleter::PopupCompletion);
            completerTimezones->setCaseSensitivity(Qt::CaseInsensitive);
            completerTimezones->setFilterMode(Qt::MatchContains);
            lineEditServerName->setCompleter(completerTimezones);
        }
    }
    myMainLayout->addLayout(mySearchLayout);

    this->resize(1000, 800);
    this->setLayout(myMainLayout);
}

void MainWindow::searchTextChanged(const QString &szText)
{
    QList<TableRow> myRowsFilteredBase;
    const QString szTextLower = szText.toLower();

    if (szSearchText.isEmpty() == false
        && szTextLower.startsWith(szSearchText)) {

        myRowsFilteredBase = myRowsFiltered;

    } else {
        myRowsFilteredBase = myRows;
    }

    this->szSearchText = szTextLower;
    myRowsFiltered.clear();

    for (const TableRow &myRow : myRowsFilteredBase) {
        if (myRow.country.contains(this->szSearchText, Qt::CaseInsensitive)
            || myRow.zoneID.contains(this->szSearchText, Qt::CaseInsensitive)) {

            myRowsFiltered.append(myRow);
        }
    }

    const QString szHtml = TableRow::rowsListToTableAscii(myRowsFiltered);
    myTextEdit->clear();
    myTextEdit->setPlainText(szHtml);
}
