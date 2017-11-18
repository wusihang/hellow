#include<QtWidgets>
#include "cell.h"
#include "spreadsheet.h"

Spreadsheet::Spreadsheet(QWidget *parent)
    :QTableWidget(parent)
{
    autoRecalc =true;
    //通常情况下， 当用户在一个空单元格输入一些文本时，QTableWidget会自动创建一个QTableWidgetItem来保存这些文本。
    //通过setItemPrototype可以替代为我们希望使用的Cell，实际上，QTableWidget会在每次需要新项是，把所传递的项以原型形式clone出来
    setItemPrototype(new Cell);
    //选择使用QAbstractItemView::ContiguousSelection,允许简单矩形选择框
    setSelectionMode(ContiguousSelection);
    connect(this,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(somethingChanged()));
    clear();
}

void Spreadsheet::clear()
{
    //如果直接使用QTableWidget::clear()来重置，那么当前的大小尺寸会被留下。
    setRowCount(0);
    setColumnCount(0);
    setRowCount(ROW_COUNT);
    setColumnCount(COLUMN_COUNT);
    //设置行的标题A-Z，列的标题默认1~999，无需重新设置
    for(int i=0;i<COLUMN_COUNT;i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(QChar('A'+i)));
        setHorizontalHeaderItem(i,item);
    }
    //光标移动到A1
    setCurrentCell(0,0);
}

Cell *Spreadsheet::cell(int row,int column) const
{
    return static_cast<Cell *>(item(row,column));
}

QString Spreadsheet::text(int row,int column) const
{
    Cell *c = cell(row,column);
    if(c){
        return c->text();
    }else{
        return "";
    }
}

QString Spreadsheet::formula(int row,int column) const
{
    Cell *c = cell(row,column);
    if(c){
        return c->formula();
    }else{
        return "";
    }
}

void Spreadsheet::setFormula(int row,int column,const QString &formula){
    Cell *c = cell(row,column);
    if(!c){
        c= new Cell;
        setItem(row,column,c);
    }
    c->setFormula(formula);
}

QString Spreadsheet::currentLocation() const
{
    return QChar('A'+currentColumn())+QString::number(currentRow()+1);
}

QString Spreadsheet::currentFormula() const
{
    return formula(currentRow(),currentColumn());
}

void Spreadsheet::somethingChanged()
{
    if(autoRecalc){
        recalculate();
    }
    emit modified();
}

bool Spreadsheet::writeFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,tr("Spreadsheet"),tr("cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_8);
    out << quint32(MAGIC_NUMBER);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for(int row=0;row<ROW_COUNT;row++){
        for(int column=0;column<COLUMN_COUNT;column++){
            QString str = formula(row,column);
            if(!str.isEmpty()){
                out<<quint16(row)<<quint16(column)<<str;
            }
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::readFile(const QString &fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,tr("Spreadsheet"),tr("cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_8);
    quint32 magic;
    in>>magic;
    if(magic!=MAGIC_NUMBER){
        QMessageBox::warning(this,tr("Spreadsheet"),tr("The file is not a Spreadsheet file"));
        return false;
    }
    clear();
    quint16 row;
    quint16 column;
    QString str;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while(!in.atEnd()){
        in >> row >>column>>str;
        setFormula(row,column,str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void Spreadsheet::cut()
{
    copy();
    del();
}

void Spreadsheet::copy()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str;
    for(int i=0;i<range.rowCount();i++)
    {
        if(i>0){
            str+='\n';
        }
        for(int j=0;j<range.columnCount();j++){
            if(j>0){
                str+='\t';
            }
            str+=formula(range.topRow()+i,range.leftColumn()+j);
        }
    }
    //系统剪切板
    QApplication::clipboard()->setText(str);
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const
{
       QList<QTableWidgetSelectionRange> ranges = selectedRanges();
       if(ranges.isEmpty()){
           return QTableWidgetSelectionRange();
       }
       return ranges.first();

}

void Spreadsheet::paste(){
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split("\n");
    int numRows = rows.count();
    int numColumns = rows.first().count('\t')+1;
    if(range.rowCount()*range.columnCount()!=1&&(range.rowCount()!=numRows||range.columnCount()!=numColumns)){
        QMessageBox::information(this,tr("Spreadsheet"),tr("the infomation cannot be pasted because the copy"
                                                           "and paste areas aren't the same size"));
        return;
    }
    for(int i=0;i<numRows;i++)
    {
        if(!rows[i].isEmpty()){
            QStringList columns = rows[i].split("\t");
            for(int j=0;j<numColumns;j++){
                int row = range.topRow()+i;
                int column = range.leftColumn()+j;
                if(row<ROW_COUNT&&column<COLUMN_COUNT&&!columns[j].isEmpty()){
                    setFormula(row,column,columns[j]);
                }
            }
        }
    }
    somethingChanged();
}

void Spreadsheet::del()
{
    QList<QTableWidgetItem *> items = selectedItems();
    if(!items.isEmpty()){
        foreach (QTableWidgetItem *item, items) {
            delete item;
        }
        somethingChanged();
    }
}

void Spreadsheet::selectCurrentRow(){
    selectRow(currentRow());
}

void Spreadsheet::selectCurrentColumn()
{
    selectColumn(currentColumn());
}

void Spreadsheet::findNext(const QString &str,Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn()+1;
    while(row<ROW_COUNT){
        while(column<COLUMN_COUNT){
            if(text(row,column).contains(str,cs)){
                clearSelection();
                setCurrentCell(row,column);
                activateWindow();
                return;
            }
            column++;
        }
        column=0;
        row++;
    }
    //发出哔，表示搜索结束
    QApplication::beep();
}

void Spreadsheet::findPrevious(const QString &str,Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn()-1;
    while(row>=0){
        while(column>=0){
            if(text(row,column).contains(str,cs)){
                clearSelection();
                setCurrentCell(row,column);
                activateWindow();
                return;
            }
            column--;
        }
        column = COLUMN_COUNT-1;
        row--;
    }
    QApplication::beep();
}


void Spreadsheet::recalculate(){
    //循环遍历每一个格子，并将实例化过的格子设置为dirty，这样会重新计算值。
    for(int row=0;row<ROW_COUNT;row++){
        for(int column=0;column<COLUMN_COUNT;column++){
            if(cell(row,column)){
                cell(row,column)->setDirty();
            }
        }
    }
    //更新视口
    viewport()->update();
}

void Spreadsheet::setAutoRecalculate(bool recalc)
{
    autoRecalc = recalc;
    if(autoRecalc){
        recalculate();
    }
}


void Spreadsheet::sort(const SpreadsheetCompare &compare)
{
    QList<QStringList> rows;
    QTableWidgetSelectionRange range = selectedRange();
    int i;
    for(i=0;i<range.rowCount();i++){
        QStringList row;
        for(int j=0;j<range.columnCount();j++){
            row.append(formula(range.topRow()+i,range.leftColumn()+j));
        }
        rows.append(row);
    }
//    qStableSort(rows.begin(),rows.end(),compare);
    std::stable_sort(rows.begin(),rows.end(),compare);
    //性能很差，因为setFormula会去创建每一个cell
    for(int i=0;i<range.rowCount();i++){
        for(int j=0;j<range.columnCount();j++){
            setFormula(range.topRow()+i,range.leftColumn()+j,rows[i][j]);
        }
    }
    clearSelection();
    somethingChanged();
}


bool SpreadsheetCompare::operator ()(const QStringList &row1,const QStringList &row2) const
{
    for(int i=0;i<KEY_COUNT;i++){
        int column = keys[i];
        if(column!=-1){
            if(row1[column]!=row2[column]){
                if(ascending[i]){
                    return row1[column]<row2[column];
                }else{
                    return row1[column]>row2[column];
                }
            }
        }
    }
    return false;
}
