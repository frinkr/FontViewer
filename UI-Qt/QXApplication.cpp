#include <QApplicationStateChangeEvent>
#include <QClipBoard>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>
#include <QtDebug>
#include <QtGui>

#include "QXAboutDialog.h"
#include "QXAboutFontsDialog.h"
#include "QXApplication.h"
#include "QXDocumentWindowManager.h"
#include "QXPreferencesDialog.h"
#include "QXSplash.h"
#include "QXThemedWindow.h"
#include "QXToastMessage.h"

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
    QXToastMessage * message = new QXToastMessage(parent);
    message->showToParent(style()->standardIcon(QStyle::SP_MessageBoxWarning), text);
}

void
QXApplication::message(QWidget * parent, const QString & title, const QString & text) const {
    QXToastMessage * message = new QXToastMessage(parent);
    message->showToParent(style()->standardIcon(QStyle::SP_MessageBoxInformation), text);
}

void
QXApplication::showInGraphicalShell(QWidget * parent, const QString & path) {
    const QFileInfo fileInfo(path);

#if defined(Q_OS_WIN)
    const FileName explorer = Environment::systemEnvironment().searchInPath(QLatin1String("explorer.exe"));
    if (explorer.isEmpty()) {
        QMessageBox::warning(parent,
                             QApplication::translate("Core::Internal",
                                                     "Launching Windows Explorer Failed"),
                             QApplication::translate("Core::Internal",
                                                     "Could not find explorer.exe in path to launch Windows Explorer."));
        return;
    }
    QStringList param;
    if (!fileInfo.isDir())
        param += QLatin1String("/select,");
    param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
    QProcess::startDetached(explorer.toString(), param);

#elif defined(Q_OS_MACOS)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
        .arg(fileInfo.canonicalFilePath());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
#else

#endif
}

void
QXApplication::copyTextToClipBoard(const QString & text) {
    clipboard()->setText(text);
}


void
QXApplication::showSplashScreen() {
    if (!splash_) {
        splash_ = new QXSplash(nullptr);
    }
    splash_->show();
}

void
QXApplication::dismissSplashScreen(QWidget * widget) {
    if (splash_)
        splash_->dismiss(widget);
    //delete splash_;
    //splash_ = nullptr;
}
    
void
QXApplication::splashScreenShowProgress(int value, int maximum, const QString & message) {
    if (splash_)
        splash_->showProgress(value, maximum, message);
}
 
                                                                            
