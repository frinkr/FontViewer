#pragma once

#include <QDialog>
#include "QXDocument.h"
#include "QXThemedWindow.h"

namespace Ui {
    class QXFontBrowser;
}

class QMenu;
class QModelIndex;
class QXFontListModel;
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
    void
    accept() override;

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
    onSearchLineEditTextEdited(const QString & text);

    void
    onSearchAction();

    void
    onOpenFileButtonClicked();

    void
    scrollToCurrentIndex();

private:
    Ui::QXFontBrowser * ui_;
    QMenu             * recentMenu_; 

};
