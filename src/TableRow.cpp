#include "TableRow.h"
#include "qglobal.h"
#include "qlist.h"
#include "qstring.h"
#include <QDebug>

const QString TableRow::DEFAULT_DST_START = QStringLiteral("M3.2.0");
const QString TableRow::DEFAULT_DST_END = QStringLiteral("M11.1.0");

QString TableRow::translateRowField(const RowField eField)
{
    switch (eField) {
        case COUNTRY:
            return QStringLiteral("COUNTRY");

        case ZONE_ID:
            return QStringLiteral("ZONE_ID");

        case TRANSITION_RULE:
            return QStringLiteral("TRANSITION_RULE");

        case ZONE_NAME:
            return QStringLiteral("ZONE_NAME");

        case OFFSET:
            return QStringLiteral("OFFSET");

        case ZONE_NAME_DST:
            return QStringLiteral("ZONE_NAME_DST");

        case OFFSET_DST:
            return QStringLiteral("OFFSET_DST");

        case DST_START:
            return QStringLiteral("DST_START");

        case DST_END:
            return QStringLiteral("DST_END");

        case COUNT_ROW_FIELDS:
            return QStringLiteral("COUNT_ROW_FIELDS");
    }
}

QString TableRow::getFieldTextAlign(const RowField eField)
{
    switch (eField) {
        case COUNTRY:
        case ZONE_ID:
        case TRANSITION_RULE:
        case DST_START:
        case DST_END:
            return "class='aleft'";

        case ZONE_NAME:
        case OFFSET:
        case ZONE_NAME_DST:
        case OFFSET_DST:
        case COUNT_ROW_FIELDS:
            return "class='acenter'";
    }
}

QString TableRow::rowsListToTableAscii(const QList<TableRow> &myRows)
{
    QList<TableRow> myRowsCopy = myRows;
    TableRow myRow;

    for (int eField = 0; eField < TableRow::COUNT_ROW_FIELDS; ++eField) {
        myRow.getField(static_cast<TableRow::RowField>(eField)) = TableRow::translateRowField(static_cast<TableRow::RowField>(eField));
    }

    myRowsCopy.prepend(myRow);

    QStringList szaValues;
    szaValues.reserve(myRows.size() + 1);

    qsizetype naMaxValues[TableRow::COUNT_ROW_FIELDS] = { 0 };

    // get max field size
    for (int eField = 0; eField < TableRow::COUNT_ROW_FIELDS; ++eField) {
        for (const TableRow &myRow : myRowsCopy) {
            const qsizetype nSize = myRow.getField(static_cast<TableRow::RowField>(eField)).size();

            if (nSize > naMaxValues[eField]) {
                naMaxValues[eField] = nSize;
            }
        }
    }

    // format fields with spaces to fill up to the max size
    QStringList szaRow;
    szaRow.reserve(TableRow::COUNT_ROW_FIELDS);

    for (const TableRow &myRow : myRowsCopy) {
        for (int eField = 0; eField < TableRow::COUNT_ROW_FIELDS; ++eField) {
            const QString szField = myRow.getField(static_cast<TableRow::RowField>(eField));
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

QString TableRow::rowsListToTableHtml(const QList<TableRow> &myRows)
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

    for (int eField = 0; eField < TableRow::COUNT_ROW_FIELDS; ++eField) {
        szTable.append("<th>");
        szTable.append(TableRow::translateRowField(static_cast<TableRow::RowField>(eField)));
        szTable.append("</th>");
    }

    szTable.append("</tr></thead><tbody>");
    int nRowIndex = 0;

    for (const TableRow &myRow : myRows) {
        QString szTableRow = nRowIndex++ % 2 == 0 ? szRowEven : szRowOdd;

        for (int eField = 0; eField < TableRow::COUNT_ROW_FIELDS; ++eField) {
            const QString szField = myRow.getField(static_cast<TableRow::RowField>(eField));
            szTableRow.append("<td " + getFieldTextAlign(static_cast<TableRow::RowField>(eField)) + ">");
            szTableRow.append(szField.toHtmlEscaped());
            szTableRow.append("</td>");
        }

        szTableRow.append("</tr>");
        szTable.append(szTableRow);
    }

    szTable.append("</tbody></table></body></html>");

    return szTable;
}

QString TableRow::getField(const RowField eField) const
{
    switch (eField) {
        case COUNTRY:
            return country;

        case ZONE_ID:
            return zoneID;

        case TRANSITION_RULE:
            return transitionRule;

        case ZONE_NAME:
            return zoneName;

        case OFFSET:
            return offset;

        case ZONE_NAME_DST:
            return zoneNameDst;

        case OFFSET_DST:
            return offsetDst;

        case DST_START:
            return dstStart;

        case DST_END:
            return dstEnd;

        case COUNT_ROW_FIELDS:
            return QString::number(COUNT_ROW_FIELDS);
    }
}

QString &TableRow::getField(const RowField eField)
{
    switch (eField) {
        case COUNTRY:
            return country;

        case ZONE_ID:
            return zoneID;

        case TRANSITION_RULE:
            return transitionRule;

        case ZONE_NAME:
            return zoneName;

        case OFFSET:
            return offset;

        case ZONE_NAME_DST:
            return zoneNameDst;

        case OFFSET_DST:
            return offsetDst;

        case DST_START:
            return dstStart;

        case DST_END:
            return dstEnd;

        case COUNT_ROW_FIELDS:
            static QString szRowCount = QString::number(COUNT_ROW_FIELDS);
            return szRowCount;
    }
}

void TableRow::fixOffsets()
{
    if (offsetDst.isEmpty() == false) {
        double dOffsetDst = offsetDst.toDouble();

        if (dOffsetDst != 0.0) {
            dOffsetDst = -dOffsetDst;

            if (dOffsetDst > 0) {
                offsetDst = '+' + QString::number(dOffsetDst);

            } else {
                offsetDst = QString::number(dOffsetDst);
            }
        }
    }

    double dOffset = offset.toDouble();

    if (dOffset == 0.0) {
        if (zoneNameDst.isEmpty() == false
            && offsetDst.isEmpty()) {

            offsetDst = "+1";
        }

    } else {
        dOffset = -dOffset;

        if (zoneNameDst.isEmpty() == false
            && offsetDst.isEmpty()) {

            if (dOffset > 0) {
                offsetDst = '+' + QString::number(dOffset + 1);

            } else {
                offsetDst = QString::number(dOffset + 1);
            }
        }

        if (dOffset > 0) {
            offset = '+' + QString::number(dOffset);

        } else {
            offset = QString::number(dOffset + 1);
        }
    }
}

bool TableRow::processZoneName(const QString &szZoneNameFull)
{
    if (szZoneNameFull.startsWith('<')) {
        // tz not named
        qsizetype nIndex = szZoneNameFull.indexOf('>');

        zoneName = szZoneNameFull.first(nIndex + 1);

        for (++nIndex; nIndex < szZoneNameFull.size(); ++nIndex) {
            const QChar cChar = szZoneNameFull[nIndex];

            if (cChar == '<') {
                //found start of DST TZ

                qsizetype nIndexEnd = szZoneNameFull.indexOf('>', nIndex);

                if (nIndexEnd != -1) {
                    ++nIndexEnd;
                    zoneNameDst = szZoneNameFull.mid(nIndex, nIndexEnd - nIndex);
                    offsetDst = szZoneNameFull.last(szZoneNameFull.size() - nIndexEnd);

                } else {
                    zoneNameDst = szZoneNameFull.last(szZoneNameFull.size() - nIndex);
                }

                return true;

            } else {
                offset.append(cChar);
            }
        }

        return false;

    } else {
        // tz named
        for (int nIndex = 0; nIndex < szZoneNameFull.size(); ++nIndex) {
            if (szZoneNameFull[nIndex].isLetter() == false) {
                //found end of non-DST TZ
                zoneName = szZoneNameFull.first(nIndex);

                for (; nIndex < szZoneNameFull.size(); ++nIndex) {
                    const QChar cChar = szZoneNameFull[nIndex];

                    if (cChar.isLetter() == true) {
                        //found start of DST TZ
                        zoneNameDst = szZoneNameFull.last(szZoneNameFull.size() - nIndex);
                        return true;

                    } else {
                        offset.append(cChar);
                    }
                }

                return false;
            }
        }
    }

    return false;
}

void TableRow::process()
{
    zoneID.replace('\"', "");
    transitionRule.replace('\"', "");
    transitionRule.replace(':', '.');
    bool bHasDst = true;

    if (transitionRule.contains(',')) {
        const QStringList szTransitionSplit = transitionRule.split(',');

        if (szTransitionSplit.size() >= 3) {
            bHasDst = processZoneName(szTransitionSplit.at(0));
            dstStart = szTransitionSplit.at(1);
            dstEnd = szTransitionSplit.at(2);

        } else {
            qDebug() << "Wrong format" << this->toString();
        }

    } else {
        bHasDst = processZoneName(transitionRule);
    }

    if (bHasDst) {
        if (dstStart.isEmpty()) {
            dstStart = DEFAULT_DST_START;
        }

        if (dstEnd.isEmpty()) {
            dstEnd = DEFAULT_DST_END;
        }

    } else {
        transitionRule.clear();
    }

    fixOffsets();
}

QString TableRow::toString() const
{
    QStringList szaResult;
    szaResult.reserve(TableRow::COUNT_ROW_FIELDS * 3);

    for (int eField = 0; eField < TableRow::COUNT_ROW_FIELDS; ++eField) {
        szaResult.append(translateRowField(static_cast<TableRow::RowField>(eField)));
        szaResult.append(": ");
        szaResult.append(getField(static_cast<TableRow::RowField>(eField)));
    }

    return szaResult.join("; ");
}
