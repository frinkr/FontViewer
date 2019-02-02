#pragma once
#include <QObject>
#include "QXDocument.h"

struct QXRecentFontItem : public QXFontURI {
    QString fullName;

    friend QDataStream &
    operator << (QDataStream & arch, const QXRecentFontItem & item) {
        arch << static_cast<const QXFontURI&>(item);
        arch << item.fullName;
        return arch;
    }

    friend QDataStream &
    operator >> (QDataStream & arch, QXRecentFontItem & item) {
        arch >> static_cast<QXFontURI&>(item);
        arch >> item.fullName;
        return arch;
    }
};

Q_DECLARE_METATYPE(QXRecentFontItem);


class QXPreferences : public QObject {
    Q_OBJECT

public:
    static QStringList
    userFontFolders();

    static void
    setUserFontFolders(const QStringList & folders);

    static QList<QXRecentFontItem>
    recentFonts();

    static void
    setRecentFonts(const QList<QXRecentFontItem> & recentFonts);

private:
    using QObject::QObject;
};
