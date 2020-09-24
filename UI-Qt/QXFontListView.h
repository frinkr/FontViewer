#pragma once

#include <QListView>
#include <QStyledItemDelegate>
#include "FontX/FXFaceDatabase.h"
#include "QXDocument.h"

class QTextBrowser;
class QXFontListFilter;
class QXFontListModel;
class QXPopoverWindow;
class QXSortFilterFontListModel;

struct QXFontListViewPreview {
    QString sampleText;
    qreal   fontSize;

    static const QXFontListViewPreview &
    defaultPreview();
};

class QXFontListView : public QListView {
    Q_OBJECT

public:
    explicit QXFontListView(QWidget * parent = nullptr);

public:

    /**
     * Return the selected index in the font db
     */
    int
    selectedFontIndex() const;

    /**
     * Return the selected font uri
     */
    QXFontURI
    selectedFont() const;

    /**
     * Select the font by index.
     * @param index: index in font db
     */
    int
    selectFont(int index);

    /**
     * Select the font by uri
     */
    int
    selectFont(const QXFontURI & uri);

    /**
     * Filter
     */
    void
    setFilter(const QXFontListFilter & filter);
    
    void
    clearFilter();

    const QXFontListFilter &
    filter() const;
    
    /**
     * Preview
     */
    void
    setPreview(const QXFontListViewPreview & preview);

    /**
     * The database
     */
    void
    setDb(FXPtr<FXFaceDatabase> db);

    FXPtr<FXFaceDatabase>
    db() const;
    
public:
    void
    showFontInfoPopover(const QModelIndex & index, const QRect & globalRect);

    bool
    isFontInfoPopoverVisible() const;

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
    onFontContextMenuRequested(const QPoint & pos);

    void
    scrollToCurrentIndex();
    
private:
    QXPopoverWindow       * popover_  {nullptr} ;
    QTextBrowser          * popoverWidget_{ nullptr };
};
