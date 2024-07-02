#pragma once

#include "qstring.h"

class TimeZone
{
    static const QString DEFAULT_DST_START;
    static const QString DEFAULT_DST_END;

public:
    enum RowField {
        ZONE_NAME,
        ZONE_CODE,
        ZONE_NAME_DST,
        ZONE_CODE_DST,
        ZONE_NET_NAME,
        ZONE_JAR_NAME,
        OFFSET,
        OFFSET_DST,
        OFFSET_DST_AMOUNT,
        DST_END,
        DST_START,
        IS_ONLINE,
        ZONE_JAVA_NAME,
        COUNTRY,
        TRANSITION_RULE,
        COUNT_ROW_FIELDS
    };

    static QString translateRowField(const RowField eField);
    static QString getFieldTextAlign(const RowField eField);

    static QString rowsListToTableAscii(const QList<TimeZone> &myRows);
    static QString rowsListToTableHtml(const QList<TimeZone> &myRows);
    static QString rowsListToTableCsv(const QList<TimeZone> &myRows);

    static void propagateZoneNames(QList<TimeZone> &myRows);

    TimeZone();

    QString data[COUNT_ROW_FIELDS] = {};

    void fixOffsets();

    void fixMinutes();

    void changeStartEndRules();

    bool processZoneName(const QString &szZoneNameFull);

    void process();

    QString toString() const;
};
