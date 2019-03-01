#include <QApplicationStateChangeEvent>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QtDebug>
#include <QtGui>
#include <QTimer>

#include "QXAboutDialog.h"
#include "QXAboutFontsDialog.h"
#include "QXApplication.h"
#include "QXDocumentWindowManager.h"
#include "QXPreferencesDialog.h"
#include "QXThemedWindow.h"

QXApplication::QXApplication(int & argc, char ** argv)
    : QApplication(argc, argv) {
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
QXApplication::aboutFonts() const {
    QXAboutFontsDialog::showAbout();
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
    return QApplication::event(event);
}
#endif


bool
QXApplication::userRequiredToResetAppData() const {
    bool keyPressed = queryKeyboardModifiers().testFlag(Qt::ShiftModifier);
    if (keyPressed) {
        return question(
            nullptr,
            tr("Reset font database"),
            tr("Are you sure to reset the database? The font database will be deleted and rebuilt."));
    }
    return false;
}

bool
QXApplication::question(QWidget * parent, const QString & title, const QString & text) const {
    QXThemedWindow<QMessageBox> box(parent);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setIcon(QMessageBox::Question);
    return box.exec() == QMessageBox::Yes;
}

void
QXApplication::warning(QWidget * parent, const QString & title, const QString & text) const {
    QXThemedWindow<QMessageBox> box(parent);
    box.setWindowTitle(title);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.setIcon(QMessageBox::Warning);
    box.exec();
}
