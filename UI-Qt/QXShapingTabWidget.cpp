#include "QXShapingTabWidget.h"
#include "QXShapingWidget.h"
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QTabBar>
#include "QXDocument.h"
#include "QXPreferences.h"

QXShapingTabWidget::QXShapingTabWidget(QWidget * parent)
    : QTabWidget(parent) {
    setTabsClosable(true);
    setUsesScrollButtons(true);
        setTabPosition(QTabWidget::South);
        setTabShape(QTabWidget::Triangular);
    QToolButton * corner = new QToolButton(this);
    corner->setText("+");

    //addTab(new QLabel("Hello"), QString("+"));
    //setTabEnabled(0, false);
    //tabBar()->setTabButton(0, QTabBar::RightSide, corner);
    
    //setCornerWidget(corner);
    connect(corner, &QPushButton::clicked, this, &QXShapingTabWidget::addNewTab);
    
    connect(this, &QXShapingTabWidget::tabCloseRequested, this, &QXShapingTabWidget::onTabCloseRequested);

    setContentsMargins(0, 0, 0, 0);
    //tabBar()->setDocumentMode(true);
}

                   
void
QXShapingTabWidget::setDocument(QXDocument * document) {
    document_ = document;
    addNewTab();
}

void
QXShapingTabWidget::addNewTab() {
    ++ tabTitleCounter_;
    
    QXShapingWidget * tab = new QXShapingWidget(this);
    tab->setDocument(document_);
    
    addTab(tab, QString("%1 %2")
           .arg(document_->displayName(QXPreferences::fontDisplayLanguage()))
           .arg(tabTitleCounter_));
}

void
QXShapingTabWidget::onTabCloseRequested(int index) {
    if (1 == count())
        return;
    QWidget * w = widget(index);
    removeTab(index);
    w->deleteLater();
}
