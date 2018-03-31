#ifndef DIAGRAM_WINDOW_H
#define DIAGRAM_WINDOW_H
#include<QMainWindow>
#include<QPair>
class Node;
class Link;
class QGraphicsScene;
class QGraphicsView;
class QAction;
class QMenu;
class QToolBar;
class DiagramWindow:public QMainWindow{
    Q_OBJECT
public:
    DiagramWindow();
private slots:
    void addNode();
    void addLink();
    void del();
    void cut();
    void copy();
    void paste();
    void bringToFront();
    void sendToBack();
    void properties();
    void updateActions();

private:
    typedef QPair<Node *,Node *> NodePair;
    void createActions();
    void createMenus();
    void createToolBars();
    void setZValue(int z);
    void setupNode(Node* node);
    Node* selectedNode() const;
    Link* selectedLink() const;
    NodePair selectedNodePair() const;

    QMenu *fileMenu;
    QMenu *editMenu;
    QToolBar *editToolBar;
    QAction *exitAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *delAction;
    QAction *addLinkAction;
    QAction *addNodeAction;
    QAction *bringToFrontAction;
    QAction *sendToBackAction;
    QAction *propertiesAction;
    QGraphicsScene *scene;
    QGraphicsView *view;
    int minZ;
    int maxZ;
    int seqNumber;
};

#endif // DIAGRAM_WINDOW_H
