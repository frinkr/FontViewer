#pragma once
#include <QStringList>
#include <QObject>
#include "FontX/FXFace.h"
#include "FontX/FXFaceDatabase.h"

class QUFontManager : public QObject {
    Q_OBJECT
public:
    static QUFontManager &
    get();

    FXPtr<FXFaceDatabase>
    db() const;

private:
    QUFontManager();

    void
    initDirectories();
    
    void
    scanDirectory(const QString & directory);

    QStringList             directories_;
    FXPtr<FXFaceDatabase>   db_;
};
