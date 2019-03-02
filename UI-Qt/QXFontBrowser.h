#pragma once

#include <QDialog>
#include "QXDocument.h"
#include "QXThemedWindow.h"

namespace Ui {
    class QXFontBrowser;
}

class QMenu;
class QModelIndex;
class QTextBrowser;
class QXFontListModel;
class QXPopoverWindow;
class QXSortFilterFontListModel;

class QXFontBrowser : public QXThemedWindow<QDialog> {
    Q_OBJECT

public:
    explicit QXFontBrowser(QWidget * parent = nullptr);
    ~QXFontBrowser();

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

private slots:
    void
    onFontDoubleClicked(const QModelIndex & index);

    void
    onSearchLineEditReturnPressed();

    void
    onSearchLineEditTextEdited();

    void
    onSearchAction();

    void
    onOpenFileButtonClicked();

    void
    scrollToCurrentIndex();

    void
    updatePreviewSettings();

    void
    quitApplication();
private:
    Ui::QXFontBrowser * ui_;
    QMenu             * recentMenu_ {nullptr}; 
    QXPopoverWindow   * popover_ {nullptr};
    QTextBrowser      * popoverWidget_{ nullptr };
};
