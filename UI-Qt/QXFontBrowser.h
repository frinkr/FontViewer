#pragma once

#include <QDialog>
#include "QXDocument.h"
#include "QXThemedWindow.h"

namespace Ui {
    class QXFontBrowser;
}

class QModelIndex;

class QXSortFilterFontListModel;
class QXFontListModel;

class QXFontBrowser : public QXThemedWindow<QDialog> {
    Q_OBJECT

public:
    explicit QXFontBrowser(QWidget * parent = nullptr);
    ~QXFontBrowser();

    int
    selectedFontIndex() const;

    QXFontURI
    selectedFont() const;

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

private:
    Ui::QXFontBrowser * ui_;
};
