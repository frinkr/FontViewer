#pragma once

#include <QDialog>
#include "QXDocument.h"
#include "QXThemedWindow.h"
#include "QXFontListModel.h"

namespace Ui {
    class QXFontListDialog;
}

class QMenu;
class QModelIndex;
class QTextBrowser;
class QXFontListModel;
class QXFontListOptionsWidget;
class QXPopoverWindow;
class QXSortFilterFontListModel;

class QXFontListWidget : public QXThemedWindow<QWidget> {
    Q_OBJECT

public:
    explicit QXFontListWidget(QWidget * parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~QXFontListWidget();

public slots:
    void
    accept();

    void
    reject();

public:
    int
    selectedFontIndex() const;

    QXFontURI
    selectedFont() const;

    int
    selectFont(int index);

    int
    selectFont(const QXFontURI & uri);

    void
    clearFilter();
    
    void
    showFontInfoPopover(const QModelIndex & index, const QRect & globalRect);

    bool
    isFontInfoPopoverVisible() const;

signals:
    void
    accepted();
    
protected:
    bool
    eventFilter(QObject * obj, QEvent * event) override;

private:
    QXSortFilterFontListModel *
    proxyModel() const;

    QXFontListModel *
    sourceModel() const;

    QModelIndex
    currentProxyIndex() const;

    QModelIndex
    currentSourceIndex() const;

    QXFontListFilter
    fontListFilter() const;
    
private slots:
    void
    onFontDoubleClicked(const QModelIndex & index);

    void
    onFontContextMenuRequested(const QPoint & pos);
    
    void
    onSearchLineEditReturnPressed();

    void
    onFilterChanged();

    void
    onSearchAction();

    void
    onOpenFileButtonClicked();

    void
    scrollToCurrentIndex();

    void
    onPreviewTextChanged();
    
    void
    updatePreviewText();

private:
    Ui::QXFontListDialog * ui_;
    QMenu             * recentMenu_ {nullptr}; 
    QXPopoverWindow   * popover_ {nullptr};
    QTextBrowser      * popoverWidget_{ nullptr };
};
