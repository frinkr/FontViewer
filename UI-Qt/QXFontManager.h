#pragma once
#include <QStringList>
#include <QObject>
#include "FontX/FXFace.h"
#include "FontX/FXFaceDatabase.h"

class QXFontManager : public QObject {
    Q_OBJECT
public:

    static void
    resetDatabase();

    static QString
    dbFilePath();

    static QXFontManager &
    instance();

    FXPtr<FXFaceDatabase>
    db() const;

    double
    dbInitSeconds() const;
    
    const QStringList &
    systemFontFolders() const;

    const QStringList &
    userFontFolders() const;

    void
    setUserFontFolders(const QStringList & folders);

private:
    QXFontManager();

    void
    initDirectories();
    
    void
    scanDirectory(const QString & directory);

    QStringList             systemFontFolders_;
    QStringList             userFontFolders_;
    FXPtr<FXFaceDatabase>   db_;
    double                  dbInitSeconds_ {};
};
