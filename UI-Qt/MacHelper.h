#pragma once

class QWidget;
class QString;

class MacHelper {
public:
    static void
    installNSApplicationDelegate();

    static void
    revealFileInFinder(const QString & path);
};
