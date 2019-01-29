#include <QtGui>
#include <QtDebug>
#include <QMessageBox>
#include <QFileInfo>

#include "QUAboutDialog.h"
#include "QUDocumentWindowManager.h"
#include "QUApplication.h"

QUApplication::QUApplication(int & argc, char ** argv) : QUSingleApplication(argc, argv) {
    setOrganizationName("DANIEL JIANG");
    setOrganizationDomain("frinkr.top");
    setApplicationName("FontViewer");
    
#ifdef Q_OS_MAC
    setQuitOnLastWindowClosed(false);
#endif
}

bool
QUApplication::darkMode() const {
	QColor textColor = palette().color(QPalette::Normal, QPalette::Text);
    QColor gray(55, 55, 55);
    return textColor.toRgb().value() > gray.value();
}

QIcon
QUApplication::loadIcon(const QString & path) const {
	if (darkMode()) {
		QFileInfo fi(path);
		QString d = fi.dir().filePath(fi.baseName() + "_d." + fi.completeSuffix());
		fi = QFileInfo(d);
		if (fi.exists())
			return QIcon(d);
	}
	return QIcon(path);
}

QUApplication::~QUApplication() {
}

void
QUApplication::about() const {
    QUAboutDialog::showAbout();    
}

#ifdef Q_OS_MAC
// This opens a document using this application from the Finder.
bool
QUApplication::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen)
    {
        QUDocumentWindowManager::instance()->openFile(static_cast<QFileOpenEvent *>(event)->file());

        return true;
    }

    return QApplication::event(event);
}
#endif

QUApplication * quApp = nullptr;
