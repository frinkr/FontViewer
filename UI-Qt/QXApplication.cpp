#include <QApplicationStateChangeEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <QtDebug>
#include <QtGui>

#include "QXAboutDialog.h"
#include "QXApplication.h"
#include "QXMenuBar.h"
#include "QXDocumentWindowManager.h"
#include "QXPreferencesDialog.h"

QXApplication::QXApplication(int & argc, char ** argv) : QApplication(argc, argv) {
    setOrganizationName("DANIEL JIANG");
    setOrganizationDomain("frinkr.top");
    setApplicationName("FontViewer");
    
#ifdef Q_OS_MAC
    setQuitOnLastWindowClosed(false);
#endif
}

bool
QXApplication::darkMode() const {
	QColor textColor = palette().color(QPalette::Normal, QPalette::Text);
    QColor gray(55, 55, 55);
    return textColor.toRgb().value() > gray.value();
}

QIcon
QXApplication::loadIcon(const QString & path) {
    auto itr = iconCache_.find(path);
    if (itr != iconCache_.end())
        return itr.value();

    QIcon icon;
	if (darkMode()) {
		QFileInfo fi(path);
		QString d = fi.dir().filePath(fi.baseName() + "_d." + fi.completeSuffix());
		fi = QFileInfo(d);
		if (fi.exists())
			icon = QIcon(d);
	}
    if (icon.isNull())
        icon = QIcon(path);
    iconCache_.insert(path, icon);
    return icon;
}

QXApplication::~QXApplication() {
}

void
QXApplication::about() const {
    QXAboutDialog::showAbout();    
}

void
QXApplication::preferences() const {
    QXPreferencesDialog::showPreferences();
}

#ifdef Q_OS_MAC
bool
QXApplication::event(QEvent * event) {
    if (event->type() == QEvent::FileOpen) {
        // This opens a document using this application from the Finder.
        QXDocumentWindowManager::instance()->openFontFile(static_cast<QFileOpenEvent *>(event)->file());
        return true;
    }
    else if (event->type() == QEvent::ApplicationStateChange) {
        // This open the dialog when clicking the dock
        auto changeEvent = static_cast<QApplicationStateChangeEvent *>(event);
        if (changeEvent->applicationState() == Qt::ApplicationActive) {
            if (QXDocumentWindowManager::instance()->documents().empty()) {
                bool hasWindow = false;
                for (QWidget * widget: qApp->allWidgets()) {
                    QWidget * window = widget->window();
                    if (window && window->isWindow() && !qobject_cast<QMenuBar*>(window) && !qobject_cast<QMenu*>(window)) {
                        hasWindow = true;
                        break;
                    }
                }
                if (!hasWindow)
                    QXDocumentWindowManager::instance()->doOpenFontDialog();
            }
        }
    }

    return QApplication::event(event);
}
#endif

QXApplication * qxApp = nullptr;
