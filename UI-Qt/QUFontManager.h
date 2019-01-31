#pragma once
#include <QStringList>
#include <QObject>
#include "FontX/FXFace.h"
#include "FontX/FXFaceDatabase.h"

class QUFontManager : public QObject {
    Q_OBJECT
public:
    static bool
    checkResetDatabase();

    static void
    resetDatabase();

    static QString
    dbFilePath();

    static QUFontManager &
    get();

    FXPtr<FXFaceDatabase>
    db() const;

    const QStringList &
    systemFontFolders() const;

    const QStringList &
    userFontFolders() const;

    void
    setUserFontFolders(const QStringList & folders);

private:
    QUFontManager();

    void
    initDirectories();
    
    void
    scanDirectory(const QString & directory);

    QStringList             systemFontFolders_;
    QStringList             userFontFolders_;
    FXPtr<FXFaceDatabase>   db_;
};
