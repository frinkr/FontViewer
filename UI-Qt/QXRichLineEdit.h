#include <QTextEdit>

class QXRichLineEdit : public QTextEdit {
    Q_OBJECT
public:
	explicit QXRichLineEdit(QWidget * parent = nullptr);

    void
    keyPressEvent(QKeyEvent *event) override;

	QSize
    sizeHint() const override;
};
