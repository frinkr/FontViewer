#pragma once

#include <QWidget.h>

enum QUPopoverEdge {
    QUPopoverLeft    = 0x00,
    QUPopoverTop     = 0x01,
    QUPopoverRight   = 0x02,
    QUPopoverBottom  = 0x04,
    QUPopoverAnyEdge = 0xFF,
};

Q_DECLARE_FLAGS(QUPopoverEdges, QUPopoverEdge);

class QUPopoverWindow : public QWidget {
    Q_OBJECT
public:
    explicit QUPopoverWindow(QWidget * parent = nullptr);

    void
    setWidget(QWidget * widget);

    QWidget *
    widget() const;

    void
    showRelativeTo(const QRect & rect,
                   QUPopoverEdges preferedEgdes = QUPopoverAnyEdge);
    
    void
    showRelativeTo(QWidget * widget,
                   QUPopoverEdges preferedEgdes = QUPopoverAnyEdge);

public:
    virtual QSize
    sizeHint() const;

    virtual QSize
    minimumSizeHint() const;
    
    virtual void
    resizeEvent(QResizeEvent * event);
    
protected:
        
    QRect
    geometryRelativeTo(const QRect & rect, QUPopoverEdge edge);

    QRegion
    localRegion();
    
protected:
    QRegion   mask_;
    QUPopoverEdge edge_;

    QWidget * widget_;
};
