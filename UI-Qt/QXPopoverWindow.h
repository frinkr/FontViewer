#pragma once

#include <QWidget>

class QBoxLayout;

enum QXPopoverEdge {
    QXPopoverLeft    = 0x01,
    QXPopoverTop     = 0x02,
    QXPopoverRight   = 0x04,
    QXPopoverBottom  = 0x08,
    QXPopoverAnyEdge = 0xFF,
};

Q_DECLARE_FLAGS(QXPopoverEdges, QXPopoverEdge);
Q_DECLARE_OPERATORS_FOR_FLAGS(QXPopoverEdges);

class QXPopoverWindow : public QWidget {
    Q_OBJECT
public:
    explicit QXPopoverWindow(QWidget * parent = nullptr);

    void
    setWidget(QWidget * widget);

    QWidget *
    widget() const;

    QXPopoverEdge
    edge() const;
    
    void
    showRelativeTo(const QRect & rect,
                   QXPopoverEdges preferedEgdes = QXPopoverAnyEdge);
    
    void
    showRelativeTo(QWidget * widget,
                   QXPopoverEdges preferedEgdes = QXPopoverAnyEdge);

    
public:
    virtual QSize
    sizeHint() const;

    virtual QSize
    minimumSizeHint() const;
    
    virtual void
    resizeEvent(QResizeEvent * event);

    virtual void
    paintEvent(QPaintEvent * event);

    virtual void
    showEvent(QShowEvent * event);

protected:
    void
    setEdge(QXPopoverEdge edge);

    QXPopoverEdge
    edgeRelativeTo(const QRect & rect, QXPopoverEdges preferedEgdes);
        
    QRect
    geometryRelativeTo(const QRect & rect, QXPopoverEdge edge);

    QRegion
    localRegion(qreal border);

    QPolygonF
    localPolygon(qreal border);
    
protected:
    QRegion   mask_;
    QXPopoverEdge edge_;

    QWidget * widget_;
    QBoxLayout * layout_;
};
