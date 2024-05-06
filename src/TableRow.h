#pragma once

#include "qstring.h"

class TableRow
{
    static const QString DEFAULT_DST_START;
    static const QString DEFAULT_DST_END;

public:
    enum RowField {
        COUNTRY,
        ZONE_ID,
        TRANSITION_RULE,
        ZONE_NAME,
        OFFSET,
        ZONE_NAME_DST,
        OFFSET_DST,
        DST_START,
        DST_END,
        COUNT_ROW_FIELDS
    };

    static QString translateRowField(const RowField eField);
    static QString getFieldTextAlign(const RowField eField);

    static QString rowsListToTableAscii(const QList<TableRow> &myRows);
    static QString rowsListToTableHtml(const QList<TableRow> &myRows);

    QString country;
    QString zoneID;
    QString transitionRule;
    QString zoneName;
    QString offset;
    QString zoneNameDst;
    QString offsetDst;
    QString dstStart;
    QString dstEnd;

    QString getField(const RowField eField) const;
    QString &getField(const RowField eField);

    void fixOffsets();

    bool processZoneName(const QString &szZoneNameFull);

    void process();

    QString toString() const;
};
