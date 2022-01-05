#include <QTabWidget>

class QXDocument;
class QXShapingTabWidget : public QTabWidget {
    Q_OBJECT

public:
    explicit QXShapingTabWidget(QWidget * parent = nullptr);

public:
    void
    setDocument(QXDocument * document);

public slots:
    void
    addNewTab();

private slots:
    void
    onTabCloseRequested(int index);
    
private:
    QXDocument * document_ {};
    int tabTitleCounter_ {};
};
