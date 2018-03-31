#include "diagram_window.h"
#include "node.h"
#include "link.h"
#include<QGraphicsView>
#include<QGraphicsScene>
#include<QtWidgets>
#include<typeinfo.h>

DiagramWindow::DiagramWindow(){
    scene = new QGraphicsScene(0,0,600,500);
    view = new QGraphicsView(scene);
    view->setDragMode(QGraphicsView::RubberBandDrag);
    view->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
    setCentralWidget(view);

    minZ = 0;
    maxZ = 0;
    seqNumber = 0;

    createActions();
    createMenus();
    createToolBars();
    connect(scene,SIGNAL(selectionChanged()),this,SLOT(updateActions()));
    setWindowTitle(tr("Diagram"));
    updateActions();
}

void DiagramWindow::addNode(){
    Node* node = new Node;
    node->setText(tr("Node %1").arg(seqNumber+1));
    setupNode(node);
}

void DiagramWindow::setupNode(Node* node){
    node->setPos(QPoint(80+(100*(seqNumber%5)),80+(50*((seqNumber/5)%7))));
    scene->addItem(node);
    ++seqNumber;
    scene->clearSelection();
    node->setSelected(true);
    bringToFront();
}

void DiagramWindow::bringToFront(){
    ++maxZ;
    setZValue(maxZ);
}

void DiagramWindow::sendToBack(){
    --minZ;
    setZValue(minZ);
}

void DiagramWindow::setZValue(int z){
    Node* node = selectedNode();
    if(node){
        node->setZValue(z);
    }
}

Node* DiagramWindow::selectedNode() const{
    QList<QGraphicsItem *> items = scene->selectedItems();
    if(items.count()==1){
        return dynamic_cast<Node *>(items.first());
    }else{
        return 0;
    }
}

Link* DiagramWindow::selectedLink() const{
    QList<QGraphicsItem *> items = scene->selectedItems();
    if(items.count()==1){
        return dynamic_cast<Link *>(items.first());
    }else{
        return 0;
    }
}

void DiagramWindow::addLink(){
    NodePair nodes = selectedNodePair();
    if(nodes != NodePair()){
        Link* link = new Link(nodes.first,nodes.second);
        scene->addItem(link);
    }
}

DiagramWindow::NodePair DiagramWindow::selectedNodePair() const{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if(items.count()==2){
        Node* first = dynamic_cast<Node *>(items.first());
        Node* second = dynamic_cast<Node *>(items.last());
        if(first&&second){
            return NodePair(first,second);
        }
    }
    return NodePair();
}

void DiagramWindow::del(){
    QList<QGraphicsItem*> items = scene->selectedItems();
    QMutableListIterator<QGraphicsItem*> i(items);
    while(i.hasNext()){
        Link* link = dynamic_cast<Link*>(i.next());
        if(link){
            delete link;
            i.remove();
        }
    }
    qDeleteAll(items);
}

void DiagramWindow::properties(){
    Node* node = selectedNode();
    Link* link = selectedLink();
    if(node){

    }else if(link){
        QColor color = QColorDialog::getColor(link->color(),this);
        if(color.isValid()){
            link->setColor(color);
        }
    }
}


void DiagramWindow::cut(){
    Node* node = selectedNode();
    if(node){
        copy();
        delete node;
    }
}

void DiagramWindow::copy(){
    Node* node = selectedNode();
    if(node){
        QString str = QString("Node %1 %2 %3 %4")
                .arg(node->textColor().name())
                .arg(node->outlineColor().name())
                .arg(node->backgroundColor().name())
                .arg(node->text());
        QApplication::clipboard()->setText(str);
    }
}


void DiagramWindow::paste()
{
    QString str = QApplication::clipboard()->text();
    QStringList parts = str.split(" ");
    if(parts.count()>=5&&parts.first()=="Node"){
        Node* node = new Node;
        node->setText(QStringList(parts.mid(4)).join(" "));
        node->setTextColor(QColor(parts[1]));
        node->setOutlineColor(QColor(parts[2]));
        node->setBackgroundColor(QColor(parts[3]));
        setupNode(node);
    }
}


void DiagramWindow::createActions()
{
        exitAction = new  QAction(tr("&Exit"),this);
        connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
        cutAction = new  QAction(tr("&Cut"),this);
        connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
        copyAction= new  QAction(tr("&Copy"),this);
        connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
        pasteAction= new  QAction(tr("&Paste"),this);
        connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
        delAction = new  QAction(tr("&Del"),this);
        connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
        addLinkAction= new  QAction(tr("AddLink"),this);
        connect(addLinkAction, SIGNAL(triggered()), this, SLOT(addLink()));
        addNodeAction= new  QAction(tr("AddNode"),this);
        connect(addNodeAction, SIGNAL(triggered()), this, SLOT(addNode()));
        bringToFrontAction= new  QAction(tr("BringToFront"),this);
        connect(bringToFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));
        sendToBackAction= new  QAction(tr("SendToBack"),this);
        connect(sendToBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));
        propertiesAction = new  QAction(tr("Properties"),this);
        connect(propertiesAction, SIGNAL(triggered()), this, SLOT(properties()));
}
void DiagramWindow::createMenus(){

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addLinkAction);
    fileMenu->addAction(addNodeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(delAction);
    editMenu->addAction(bringToFrontAction);
    editMenu->addAction(sendToBackAction);
    editMenu->addAction(propertiesAction);

}
void DiagramWindow::createToolBars(){

}

void DiagramWindow::updateActions(){
    bool hasSelection = !scene->selectedItems().isEmpty();
    bool isNode = (selectedNode()!=0);
    bool isNodePair = (selectedNodePair()!=NodePair());
    cutAction->setEnabled(isNode);
    copyAction->setEnabled(isNode);
    addLinkAction->setEnabled(isNodePair);
    delAction->setEnabled(hasSelection);
    bringToFrontAction->setEnabled(isNode);
    sendToBackAction->setEnabled(isNode);
    propertiesAction->setEnabled(isNode);
    foreach (QAction* action, view->actions()) {
        view->removeAction(action);
    }
    foreach (QAction* action, editMenu->actions()) {
        if(action->isEnabled()){
            view->addAction(action);
        }
    }
}
