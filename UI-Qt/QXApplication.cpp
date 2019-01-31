#include <QFileInfo>
#include <QMessageBox>
#include <QtDebug>
#include <QtGui>

#include "QXAboutDialog.h"
#include "QXApplication.h"
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
// This opens a document using this application from the Finder.
bool
QXApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QXDocumentWindowManager::instance()->openFontFile(static_cast<QFileOpenEvent *>(event)->file());

        return true;
    }

    return QApplication::event(event);
}
#endif

QXApplication * qxApp = nullptr;
