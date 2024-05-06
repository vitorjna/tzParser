//#include <cstdio>
//#include <cstdlib>
//#include <cstring>
//#include <iostream>
//#include <unistd.h>

#include <algorithm>

#include "qapplication.h"
#include "qbytearray.h"
#include "qdebug.h"
#include "qfile.h"
#include "qglobal.h"
#include "qiodevice.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "qlist.h"
#include "qstring.h"
#include <QDebug>

#include "TableRow.h"
#include "ui/MainWindow.h"

int main(int argc, char **argv)
{
    const QApplication tzParser(argc, argv);

    QList<TableRow> myRows;
    myRows.reserve(550);

    {
        QFile myFileZones(QString::fromLatin1("../zones2024a.csv"));
        myFileZones.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray caFileZones = myFileZones.readAll();
        const QList<QByteArray> myFileZonesList = caFileZones.split('\n');

        for (const QByteArray &caZone : myFileZonesList) {
            TableRow myRow;

            QList<QByteArray> myLineSplit = caZone.split(',');

            if (myLineSplit.size() < 2) {
                continue; // not a timezone line
            }

            myRow.zoneID = myLineSplit.takeFirst();
            myRow.transitionRule = myLineSplit.join(',');

            myRow.process();

            myRows.append(myRow);
        }

        myFileZones.close();
    }

    {
        QFile myFileJson(QString::fromLatin1("../2024a.json"));
        myFileJson.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray caJson = myFileJson.readAll();
        myFileJson.close();

        const QJsonDocument myJsonDoc = QJsonDocument::fromJson(caJson);
        const QJsonObject &myJsonData = myJsonDoc.object();
        const QJsonObject &myCountries = myJsonData["countries"].toObject();

        QList<TableRow> myNewRows;
        QStringList szaZoneList;

        for (const QJsonValueConstRef myCountry : myCountries) {
            const QJsonObject myCountryObj = myCountry.toObject();
            const QString szCountry = myCountryObj["name"].toString();
            const QJsonArray myArrayZones = myCountryObj["zones"].toArray();

            szaZoneList.clear();
            szaZoneList.reserve(myArrayZones.size());

            for (const QJsonValueConstRef myZone : myArrayZones) {
                szaZoneList.append(myZone.toString());
            }

            for (TableRow &myRow : myRows) {
                for (const QString &szZone : szaZoneList) {
                    if (szZone == myRow.zoneID) {
                        if (myRow.country.isEmpty() == false) {

                            TableRow myNewRow(myRow);
                            myNewRow.country = szCountry;
                            myNewRows.append(myNewRow);

                        } else {
                            myRow.country = szCountry;
                        }
                    }
                }
            }
        }

        myRows.append(myNewRows);
    }

    QMutableListIterator<TableRow> myIt(myRows);

    while (myIt.hasNext()) {
        if (myIt.next().country.isEmpty()) {
            // qDebug() << "removed" << myIt.value().toString();
            myIt.remove();
        }
    }

    std::sort(myRows.begin(), myRows.end(), [&](const TableRow & row1, const TableRow & row2) { return row1.offset.toDouble() < row2.offset.toDouble(); });

    const QString szRowsHtml = TableRow::rowsListToTableHtml(myRows);

    QFile myFileZonesTable(QString::fromLatin1("../zones2024aTable.html"));
    myFileZonesTable.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
    myFileZonesTable.write(szRowsHtml.toUtf8());
    myFileZonesTable.close();

    MainWindow myMainWindow(myRows);
    myMainWindow.show();

    return tzParser.exec();
}
