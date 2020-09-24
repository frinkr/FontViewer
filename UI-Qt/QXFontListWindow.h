#pragma once

#include <QMainWindow>
#include "QXDocument.h"
#include "QXThemedWindow.h"
#include "QXFontListModel.h"

namespace Ui {
    class QXFontListWindow;
}

class QLineEdit;
class QMenu;
class QXFontListOptionsWidget;

class QXFontListWindow : public QXThemedWindow<QMainWindow>
{
    Q_OBJECT

public:
    explicit QXFontListWindow(QWidget *parent = nullptr);
    ~QXFontListWindow();

public:
    QXFontURI
    selectedFont() const;

    int
    selectFont(const QXFontURI & uri);

    QLineEdit *
    searchLineEdit() const;
    
signals:
    void
    fontSelected(const QXFontURI & uri);

protected:
    bool
    eventFilter(QObject * obj, QEvent * event) override;

private:
    QXFontListFilter
    fontListFilter() const;
    
private slots:
    void
    acceptFont();

    void
    onSearchLineEditReturnPressed();

    void
    onFilterChanged();

    void
    onSearchAction();

    void
    onOpenFileButtonClicked();

    void
    onPreviewTextChanged();
    
    void
    updatePreviewText();

private:
    Ui::QXFontListWindow * ui_;
    QMenu                * recentMenu_ {nullptr}; 
};
