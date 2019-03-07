#pragma once

#include <QWidget>
class QLabel;

class QXToastMessage: public QWidget {
public:
    explicit QXToastMessage(QWidget * parent = nullptr);

    void
    showToParent(const QIcon & icon, const QString & message);

protected:
    void
    showEvent(QShowEvent * event) override;

private slots:
    void
    fadeIn();

    void
    fadeOut();

private:
    QLabel        *icon_{nullptr};
    QLabel        *message_{nullptr};
};
