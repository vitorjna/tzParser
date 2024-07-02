#include "TimeZone.h"
#include "qglobal.h"
#include "qlist.h"
#include "qstring.h"
#include <QDebug>

const QString TimeZone::DEFAULT_DST_START = QStringLiteral("M3.2.0");
const QString TimeZone::DEFAULT_DST_END = QStringLiteral("M11.1.0");

QString TimeZone::translateRowField(const RowField eField)
{
    switch (eField) {
        case ZONE_NAME:
            return QStringLiteral("timezone_name");

        case ZONE_CODE:
            return QStringLiteral("timezone_code");

        case ZONE_NAME_DST:
            return QStringLiteral("daylight_name");

        case ZONE_CODE_DST:
            return QStringLiteral("daylight_code");

        case ZONE_NET_NAME:
            return QStringLiteral("timezone_netname");

        case ZONE_JAR_NAME:
            return QStringLiteral("timezone_jarname");

        case OFFSET:
            return QStringLiteral("utc_bias");

        case OFFSET_DST:
            return QStringLiteral("utc_stdbias");

        case OFFSET_DST_AMOUNT:
            return QStringLiteral("utc_dltbias");

        case DST_START:
            return QStringLiteral("date_dltstart");

        case DST_END:
            return QStringLiteral("date_stdstart");

        case IS_ONLINE:
            return QStringLiteral("is_online");

        case ZONE_JAVA_NAME:
            return QStringLiteral("timezone_javaname");

        case COUNTRY:
            return QStringLiteral("country");

        case TRANSITION_RULE:
            return QStringLiteral("dst_rule");

        case COUNT_ROW_FIELDS:
            return QStringLiteral("COUNT_ROW_FIELDS");
    }
}

QString TimeZone::getFieldTextAlign(const RowField eField)
{
    switch (eField) {
        case COUNTRY:
        case ZONE_NAME:
        case TRANSITION_RULE:
        case DST_START:
        case DST_END:
            return "class='aleft'";

        case ZONE_NAME_DST:
        case ZONE_NET_NAME:
        case ZONE_JAVA_NAME:
        case ZONE_JAR_NAME:
        case ZONE_CODE:
        case OFFSET:
        case ZONE_CODE_DST:
        case OFFSET_DST:
        case OFFSET_DST_AMOUNT:
        case IS_ONLINE:
        case COUNT_ROW_FIELDS:
            return "class='acenter'";
    }
}

QString TimeZone::rowsListToTableAscii(const QList<TimeZone> &myRows)
{
    QList<TimeZone> myRowsCopy = myRows;
    TimeZone myRow;

    for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
        myRow.data[static_cast<TimeZone::RowField>(eField)] = TimeZone::translateRowField(static_cast<TimeZone::RowField>(eField));
    }

    myRowsCopy.prepend(myRow);

    QStringList szaValues;
    szaValues.reserve(myRows.size() + 1);

    qsizetype naMaxValues[TimeZone::COUNT_ROW_FIELDS] = { 0 };

    // get max field size
    for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
        for (const TimeZone &myRow : myRowsCopy) {
            const qsizetype nSize = myRow.data[static_cast<TimeZone::RowField>(eField)].size();

            if (nSize > naMaxValues[eField]) {
                naMaxValues[eField] = nSize;
            }
        }
    }

    // format fields with spaces to fill up to the max size
    QStringList szaRow;
    szaRow.reserve(TimeZone::COUNT_ROW_FIELDS);

    for (const TimeZone &myRow : myRowsCopy) {
        for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
            const QString szField = myRow.data[static_cast<TimeZone::RowField>(eField)];
            szaRow.append(szField + QString(naMaxValues[eField] - szField.size(), ' '));
        }

        QString szRow("| ");
        szRow.append(szaRow.join(" | "));
        szRow.append(" |");

        szaRow.clear();
        szaValues.append(szRow);
    }

    szaValues.insert(1, QString(szaValues.first().size(), '_'));

    return szaValues.join('\n');
}

QString TimeZone::rowsListToTableHtml(const QList<TimeZone> &myRows)
{
    static const QString szRowEven("<tr class='even'>");
    static const QString szRowOdd("<tr class='odd'>");

    QString szTable("<html>"
                    "<head>"
                    "<style>"
                    "table, th, td {"
                    "    border: 1px solid black;"
                    "    border-collapse: collapse;"
                    "}"
                    ".aleft {"
                    "    text-align: left;"
                    "}"
                    ".acenter {"
                    "    text-align: center;"
                    "}"
                    "th, td {"
                    "    padding-left: 5px;"
                    "    padding-right: 5px;"
                    "}"
                    "tr.odd {"
                    "    background-color: #FFFFFF;"
                    "}"
                    "tr.even {"
                    "    background-color: #DDDDDD;"
                    "}"
                    "</style>"
                    "</head><body>"
                   );

    szTable.append("<table><thead><tr>");

    for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
        szTable.append("<th>");
        szTable.append(TimeZone::translateRowField(static_cast<TimeZone::RowField>(eField)));
        szTable.append("</th>");
    }

    szTable.append("</tr></thead><tbody>");
    int nRowIndex = 0;

    for (const TimeZone &myRow : myRows) {
        QString szTimeZone = nRowIndex++ % 2 == 0 ? szRowEven : szRowOdd;

        for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
            const QString szField = myRow.data[static_cast<TimeZone::RowField>(eField)];
            szTimeZone.append("<td " + getFieldTextAlign(static_cast<TimeZone::RowField>(eField)) + ">");
            szTimeZone.append(szField.toHtmlEscaped());
            szTimeZone.append("</td>");
        }

        szTimeZone.append("</tr>");
        szTable.append(szTimeZone);
    }

    szTable.append("</tbody></table></body></html>");

    return szTable;
}

QString TimeZone::rowsListToTableCsv(const QList<TimeZone> &myRows)
{
    QStringList szaValues;
    szaValues.reserve(myRows.size() + 1);

    QStringList szaRow;
    szaRow.reserve(TimeZone::COUNT_ROW_FIELDS);

    for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
        szaRow.append(TimeZone::translateRowField(static_cast<TimeZone::RowField>(eField)));
    }

    szaValues.append(szaRow.join(','));
    szaRow.clear();

    for (const TimeZone &myRow : myRows) {
        for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
            szaRow.append("\"" + myRow.data[static_cast<TimeZone::RowField>(eField)] + "\"");
        }

        szaValues.append(szaRow.join(','));
        szaRow.clear();
    }

    return szaValues.join('\n');
}

void TimeZone::propagateZoneNames(QList<TimeZone> &myRows)
{
    for (TimeZone &myRow : myRows) {
        if (myRow.data[ZONE_NET_NAME].isEmpty()) {
            for (const TimeZone &myRowNamed : myRows) {
                if (myRowNamed.data[ZONE_NET_NAME].isEmpty() == true) {
                    continue;
                }

                if (myRow.data[OFFSET] == myRowNamed.data[OFFSET]
                    && myRow.data[OFFSET_DST] == myRowNamed.data[OFFSET_DST]
                    && myRow.data[OFFSET_DST_AMOUNT] == myRowNamed.data[OFFSET_DST_AMOUNT]
                   ) {

                    // qDebug() << myRow.toString() << "\nwill get values from\n" << myRowNamed.toString();
                    myRow.data[TimeZone::ZONE_NET_NAME] = myRowNamed.data[ZONE_NET_NAME];
                    myRow.data[TimeZone::ZONE_JAR_NAME] = myRowNamed.data[ZONE_JAR_NAME];
                    myRow.data[TimeZone::ZONE_NAME_DST] = myRowNamed.data[ZONE_NAME_DST];
                }
            }
        }
    }
}

TimeZone::TimeZone()
{
    data[OFFSET_DST_AMOUNT] = '0';
    data[IS_ONLINE] = '1';
}

void TimeZone::fixOffsets()
{
    fixMinutes();

    if (data[OFFSET_DST].isEmpty() == false) {
        double dOffsetDst = data[OFFSET_DST].toDouble();

        if (dOffsetDst != 0.0) {
            dOffsetDst = -dOffsetDst;

            if (dOffsetDst > 0) {
                data[OFFSET_DST] = '+' + QString::number(dOffsetDst * 60);

            } else {
                data[OFFSET_DST] = QString::number(dOffsetDst * 60);
            }
        }
    }

    double dOffset = data[OFFSET].toDouble();

    if (dOffset == 0.0) {
        if (data[ZONE_CODE_DST].isEmpty() == false
            && data[OFFSET_DST].isEmpty()) {

            data[OFFSET_DST] = "+60";
        }

    } else {
        dOffset = -dOffset;

        if (data[ZONE_CODE_DST].isEmpty() == false
            && data[OFFSET_DST].isEmpty()) {

            if (dOffset > 0) {
                data[OFFSET_DST] = '+' + QString::number((dOffset + 1) * 60);

            } else {
                data[OFFSET_DST] = QString::number((dOffset + 1) * 60);
            }
        }

        if (dOffset > 0) {
            data[OFFSET] = '+' + QString::number(dOffset * 60);

        } else {
            data[OFFSET] = QString::number(dOffset * 60);
        }
    }
}

void TimeZone::fixMinutes()
{
    if (data[OFFSET].endsWith(".15")) {
        data[OFFSET].replace(".15", ".25");

    } else if (data[OFFSET].endsWith(".30")) {
        data[OFFSET].replace(".30", ".5");

    } else if (data[OFFSET].endsWith(".45")) {
        data[OFFSET].replace(".45", ".75");
    }

    if (data[OFFSET_DST].endsWith(".15")) {
        data[OFFSET_DST].replace(".15", ".25");

    } else if (data[OFFSET_DST].endsWith(".30")) {
        data[OFFSET_DST].replace(".30", ".5");

    } else if (data[OFFSET_DST].endsWith(".45")) {
        data[OFFSET_DST].replace(".45", ".75");
    }
}

void TimeZone::changeStartEndRules()
{
    for (const RowField eField : {DST_START, DST_END}) {
        const QString szRule = data[eField];

        if (szRule.isEmpty()) {
            continue;
        }

        QString szTime(32, '0');
        QStringList szaRule = szRule.split('.');

        // month
        szTime[5] = QString::number(szaRule[0].remove('M').toInt(), 16)[0].toUpper();
        // week
        szTime[13] = QString::number(szaRule[1].toInt(), 16)[0].toUpper();
        // day
        QStringList szaRuleChange = szaRule[2].split('/');
        szTime[15] = QString::number(szaRuleChange[0].toInt(), 16)[0].toUpper();

        // hour
        if (szaRuleChange.size() == 2) {
            szTime[17] = QString::number(szaRuleChange[1].toInt(), 16)[0].toUpper();

        } else {
            szTime[17] = '2';
        }

        szTime.prepend("0x");
        data[eField] = szTime;
    }
}

bool TimeZone::processZoneName(const QString &szZoneNameFull)
{
    if (szZoneNameFull.startsWith('<')) {
        // tz not named
        qsizetype nIndex = szZoneNameFull.indexOf('>');

        data[ZONE_CODE] = szZoneNameFull.first(nIndex + 1);

        for (++nIndex; nIndex < szZoneNameFull.size(); ++nIndex) {
            const QChar cChar = szZoneNameFull[nIndex];

            if (cChar == '<') {
                //found start of DST TZ

                qsizetype nIndexEnd = szZoneNameFull.indexOf('>', nIndex);

                if (nIndexEnd != -1) {
                    ++nIndexEnd;
                    data[ZONE_CODE_DST] = szZoneNameFull.mid(nIndex, nIndexEnd - nIndex);
                    data[OFFSET_DST] = szZoneNameFull.last(szZoneNameFull.size() - nIndexEnd);

                } else {
                    data[ZONE_CODE_DST] = szZoneNameFull.last(szZoneNameFull.size() - nIndex);
                }

                return true;

            } else {
                data[OFFSET].append(cChar);
            }
        }

        return false;

    } else {
        // tz named
        for (int nIndex = 0; nIndex < szZoneNameFull.size(); ++nIndex) {
            if (szZoneNameFull[nIndex].isLetter() == false) {
                //found end of non-DST TZ
                data[ZONE_CODE] = szZoneNameFull.first(nIndex);

                for (; nIndex < szZoneNameFull.size(); ++nIndex) {
                    const QChar cChar = szZoneNameFull[nIndex];

                    if (cChar.isLetter() == true) {
                        //found start of DST TZ
                        data[ZONE_CODE_DST] = szZoneNameFull.last(szZoneNameFull.size() - nIndex);
                        return true;

                    } else {
                        data[OFFSET].append(cChar);
                    }
                }

                return false;
            }
        }
    }

    return false;
}

void TimeZone::process()
{
    data[ZONE_NAME].remove('\"');
    data[ZONE_JAVA_NAME].remove('\"');
    data[TRANSITION_RULE].remove('\"');
    data[TRANSITION_RULE].replace(':', '.');
    bool bHasDst = true;

    if (data[TRANSITION_RULE].contains(',')) {
        const QStringList szTransitionSplit = data[TRANSITION_RULE].split(',');

        if (szTransitionSplit.size() >= 3) {
            bHasDst = processZoneName(szTransitionSplit.at(0));
            data[DST_START] = szTransitionSplit.at(1);
            data[DST_END] = szTransitionSplit.at(2);

        } else {
            qDebug() << "Wrong format" << this->toString();
        }

    } else {
        bHasDst = processZoneName(data[TRANSITION_RULE]);
    }

    if (bHasDst) {
        if (data[DST_START].isEmpty()) {
            data[DST_START] = DEFAULT_DST_START;
        }

        if (data[DST_END].isEmpty()) {
            data[DST_END] = DEFAULT_DST_END;
        }

    } else {
        data[TRANSITION_RULE].clear();
    }

    fixOffsets();
    changeStartEndRules();
}

QString TimeZone::toString() const
{
    QStringList szaResult;
    szaResult.reserve(TimeZone::COUNT_ROW_FIELDS * 3);

    for (int eField = 0; eField < TimeZone::COUNT_ROW_FIELDS; ++eField) {
        QStringList szaRow;
        szaRow.append(translateRowField(static_cast<TimeZone::RowField>(eField)));
        szaRow.append(data[static_cast<TimeZone::RowField>(eField)]);

        szaResult.append(szaRow.join(": "));
    }

    return szaResult.join("; ");
}
