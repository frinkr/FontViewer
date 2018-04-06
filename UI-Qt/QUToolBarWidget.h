#pragma once
#include <QWidget>

class QLabel;

class QUToolBarWidget : public QWidget {
    Q_OBJECT

public:
    QUToolBarWidget(QWidget * child, const QString & label, QWidget * parent = nullptr);

    QString
        label() const;

    void
        setLabel(const QString & label) const;

protected:
    QLabel * label_;
};
