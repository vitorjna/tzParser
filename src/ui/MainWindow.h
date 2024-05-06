#pragma once

#include "qcompleter.h"
#include "qlist.h"
#include "qobject.h"
#include "qstring.h"
#include "qtextedit.h"
#include "qwidget.h"

#include "TableRow.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(const QList<TableRow> &myRows, QWidget *parent = nullptr);

protected:
    void setupUi();

private:
    void searchTextChanged(const QString &szText);

    QTextEdit *myTextEdit = {};
    QList<TableRow> myRows;
    QList<TableRow> myRowsFiltered;

    QString szSearchText;
};
