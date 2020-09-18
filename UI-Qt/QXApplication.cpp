#include <QApplicationStateChangeEvent>
#include <QClipboard>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>
#include <QtDebug>
#include <QtGui>

#include "FontX/FXBenchmark.h"
#include "QXAboutDialog.h"
#include "QXAboutFontsDialog.h"
#include "QXApplication.h"
#include "QXDocumentWindowManager.h"
#include "QXIconEngine.h"
#include "QXPreferencesDialog.h"
#include "QXSplash.h"
#include "QXThemedWindow.h"
#include "QXToastMessage.h"

#if defined(Q_OS_MACOS)
#  include "MacHelper.h"
#elif defined(Q_OS_WIN)
#  include "WinHelper.h"
#endif

QXApplication::QXApplication(int & argc, char ** argv)
    : QApplication(argc, argv) {
    setOrganizationName("FontViewer");
    setOrganizationDomain("frinkr.top");
    setApplicationName("FontViewer");
    setQuitOnLastWindowClosed(false);
}

bool
QXApplication::darkMode() const {
	return isDarkColor(palette().color(QPalette::Normal, QPalette::Text));
}

bool
QXApplication::isDarkColor(const QColor & color) const {
    QColor gray(55, 55, 55);
    return color.toRgb().value() > gray.value();
}

QIcon
QXApplication::loadIcon(const QString & path) {
    auto itr = iconCache_.find(path);
    if (itr != iconCache_.end())
        return itr.value();

    QIcon icon(new QXIconEngine(path));
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
#if defined(Q_OS_WIN)
    WinHelper::showInExplorer(path);
#elif defined(Q_OS_MACOS)
    MacHelper::revealFileInFinder(path);
#else
#endif
}

void
QXApplication::copyTextToClipBoard(const QString & text) {
    clipboard()->setText(text);
}

FXBenchmark &
QXApplication::benchmark() {
    if (!benchmark_)
        benchmark_ = new FXBenchmark(false);
    return *benchmark_;
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
 
                                                                            
