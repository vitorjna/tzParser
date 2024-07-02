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

#include "TimeZone.h"
#include "ui/MainWindow.h"
#include "util/TimeUtils.h"

int main(int argc, char **argv)
{
    const QApplication tzParser(argc, argv);

    QList<TimeZone> myRows; // List to store timezone data
    myRows.reserve(550); // Reserve space for 550 timezones

    {
        // Read timezone data from CSV file
        QFile myFileZones(QString::fromLatin1("../data/zones2024a.csv"));
        myFileZones.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray caFileZones = myFileZones.readAll();
        const QList<QByteArray> myFileZonesList = caFileZones.split('\n');

        for (const QByteArray &caZone : myFileZonesList) {
            TimeZone myRow;

            QList<QByteArray> myLineSplit = caZone.split(','); // Split line into components

            if (myLineSplit.size() < 2) {
                continue; // Skip lines with insufficient data
            }

            myRow.data[TimeZone::ZONE_NAME] = myLineSplit.takeFirst(); // Assign timezone name
            myRow.data[TimeZone::ZONE_JAVA_NAME] = myRow.data[TimeZone::ZONE_NAME]; // Duplicate name for Java
            myRow.data[TimeZone::TRANSITION_RULE] = myLineSplit.join(','); // Remaining data as transition rule

            myRow.process();

            myRows.append(myRow);
        }

        myFileZones.close();
    }

    {
        // Read country data from JSON file
        QFile myFileJson(QString::fromLatin1("../data/2024a.json"));
        myFileJson.open(QIODevice::ReadOnly | QIODevice::Text);
        const QByteArray caJson = myFileJson.readAll();
        myFileJson.close();

        const QJsonDocument myJsonDoc = QJsonDocument::fromJson(caJson);
        const QJsonObject &myJsonData = myJsonDoc.object();
        const QJsonObject &myCountries = myJsonData["countries"].toObject();

        QList<TimeZone> myNewRows;
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

            for (TimeZone &myRow : myRows) {
                for (const QString &szZone : szaZoneList) {
                    if (szZone == myRow.data[TimeZone::ZONE_NAME]) {
                        if (myRow.data[TimeZone::COUNTRY].isEmpty() == false) {
                            // If country already assigned, duplicate row
                            TimeZone myNewRow(myRow);
                            myNewRow.data[TimeZone::COUNTRY] = szCountry;
                            myNewRows.append(myNewRow);

                        } else {
                            myRow.data[TimeZone::COUNTRY] = szCountry;
                        }
                    }
                }
            }
        }

        myRows.append(myNewRows);
    }


    {
        // Read Windows timezone data from text file
        QFile myFileWindows(QString::fromLatin1("../data/windows_timezones.txt")); //obtained in PowerShell with command: Get-TimeZone -ListAvailable
        myFileWindows.open(QIODevice::ReadOnly | QIODevice::Text);

        const QByteArray caFileZones = myFileWindows.readAll();
        myFileWindows.close();

        const QList<QByteArray> myFileZonesList = caFileZones.split('\n');

        struct WindowsZone {
            QString szZoneName;
            QString szDisplayName;
            QString szZoneNameDst;
            QString szOffset;
            bool bDstOn;
        };

        QList<WindowsZone> myWindowsZones;
        WindowsZone myWindowsZone;

        for (const QByteArray &caZone : myFileZonesList) {
            if (caZone.isEmpty()) {
                continue; // Skip empty lines
            }

            QList<QByteArray> myLineSplit = caZone.split(':');

            if (myLineSplit.size() < 2) {
                continue; // Skip lines with insufficient data
            }

            const QString szKey = myLineSplit.first().trimmed();

            if (szKey == "Id") {
                myWindowsZone.szZoneName = myLineSplit.last().trimmed();

            } else if (szKey == "DisplayName") {
                myWindowsZone.szDisplayName = myLineSplit.last().trimmed();

            } else if (szKey == "DaylightName") {
                myWindowsZone.szZoneNameDst = myLineSplit.last().trimmed();

            } else if (szKey == "BaseUtcOffset") {
                int nOffset = myLineSplit.at(1).trimmed().toInt() * 60;

                if (myLineSplit.at(2).trimmed() != "00") {
                    if (nOffset > 0) {
                        nOffset += myLineSplit.at(2).trimmed().toInt();

                    } else {
                        nOffset -= myLineSplit.at(2).trimmed().toInt();
                    }
                }

                if (nOffset > 0) {
                    myWindowsZone.szOffset = '+' + QString::number(nOffset);

                } else {
                    myWindowsZone.szOffset = QString::number(nOffset);
                }

            } else if (szKey == "SupportsDaylightSavingTime") {
                myWindowsZone.bDstOn = (myLineSplit.last().trimmed() == "True");

                myWindowsZones.push_back(myWindowsZone);
                myWindowsZone = {};
            }
        }

        // Match Windows timezones to existing timezone rows
        for (int nIndex = 0; nIndex < 3; ++nIndex) {
            // Loop multiple times, being less restrictive each time
            for (TimeZone &myRow : myRows) {
                if (myRow.data[TimeZone::ZONE_NET_NAME].isEmpty() == false) {
                    continue;
                }

                for (const WindowsZone &myWindowsZone : myWindowsZones) {

                    if (myRow.data[TimeZone::OFFSET] == myWindowsZone.szOffset
                        && myRow.data[TimeZone::OFFSET_DST].isEmpty() != myWindowsZone.bDstOn) {

                        QStringList szZoneNameSplit = myRow.data[TimeZone::ZONE_NAME].split('/');
                        const QString &szZoneContinent = szZoneNameSplit.first();
                        const QString &szZoneLocation  = szZoneNameSplit.last().replace('_', ' ');

                        bool bMatch = false;

                        switch (nIndex) {
                            case 0:
                                if (szZoneContinent.isEmpty() == false
                                    && (myWindowsZone.szDisplayName.contains(szZoneContinent)
                                        || myWindowsZone.szDisplayName.contains(szZoneLocation))) {
                                    bMatch = true;
                                }

                                break;

                            case 1:
                                if (szZoneContinent.isEmpty() == false
                                    && (myWindowsZone.szZoneName.contains(szZoneContinent)
                                        || myWindowsZone.szZoneName.contains(szZoneLocation))) {
                                    bMatch = true;
                                }

                                break;

                            case 2:
                                bMatch = true;
                                break;

                            default:
                                break;
                        }

                        if (bMatch) {
                            myRow.data[TimeZone::ZONE_NET_NAME] = myWindowsZone.szZoneName;
                            myRow.data[TimeZone::ZONE_JAR_NAME] = myWindowsZone.szZoneName;

                            if (myWindowsZone.bDstOn) {
                                myRow.data[TimeZone::ZONE_NAME_DST] = myWindowsZone.szZoneNameDst;
                            }
                        }
                    }
                }
            }
        }
    }

    // Iterate through timezone rows and make necessary adjustments
    QMutableListIterator<TimeZone> myIt(myRows);

    while (myIt.hasNext()) {
        if (myIt.next().data[TimeZone::COUNTRY].isEmpty()) {
            // qDebug() << "removed" << myIt.value().toString();
            myIt.remove(); // Remove rows without a country
            continue;
        }

        // invert offset values as current format expects "bias", not offset
        if (myIt.value().data[TimeZone::OFFSET_DST].isEmpty() == false) {
            myIt.value().data[TimeZone::OFFSET_DST_AMOUNT] = QString::number(myIt.value().data[TimeZone::OFFSET].toDouble() - myIt.value().data[TimeZone::OFFSET_DST].toDouble());
        }

        myIt.value().data[TimeZone::OFFSET]            = QString::number(0 - myIt.value().data[TimeZone::OFFSET].toDouble());
        myIt.value().data[TimeZone::OFFSET_DST]        = '0';
    }

    TimeZone::propagateZoneNames(myRows);

    // Sort timezone rows by offset
    std::sort(myRows.begin(), myRows.end(), [&](const TimeZone & row1, const TimeZone & row2) {
        return row1.data[TimeZone::OFFSET].toDouble() < row2.data[TimeZone::OFFSET].toDouble();
    });

    // {
    //     const QString szRowsHtml = TimeZone::rowsListToTableHtml(myRows);

    //     QFile myFileZonesTable(QString::fromLatin1("../data/zones2024aProcessed.html"));
    //     myFileZonesTable.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
    //     myFileZonesTable.write(szRowsHtml.toUtf8());
    //     myFileZonesTable.close();
    // }

    {
        const QString szRowsCsv = TimeZone::rowsListToTableCsv(myRows);

        QFile myFileZonesTable(QString::fromLatin1("../data/zones2024aProcessed.csv"));
        myFileZonesTable.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        myFileZonesTable.write(szRowsCsv.toUtf8());
        myFileZonesTable.close();
    }

    MainWindow myMainWindow(myRows);
    myMainWindow.show();

    return tzParser.exec();
}
