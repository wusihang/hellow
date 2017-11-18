#include<QtWidgets>
#include "cell.h"
Cell::Cell()
{
    setDirty();
}

QTableWidgetItem *Cell::clone() const
{
    //使用拷贝构造函数拷贝一个格子
    return new Cell(*this);
}

void Cell::setFormula(const QString &formula)
{
    //设置公式，以编辑模式编辑单元格
    setData(Qt::EditRole,formula);
}

QString Cell::formula() const
{
    //获取公式，如 =A1+B1
    return data(Qt::EditRole).toString();
}

void Cell::setData(int role,const QVariant &value)
{
    QTableWidgetItem::setData(role,value);
    if(role==Qt::EditRole){
        //如果设置值的模式是编辑模式，那么将格子置为dirty
        setDirty();
    }
}

void Cell::setDirty(){
    cacheIsDirty = true;
}

QVariant Cell::data(int role) const{
    if(role==Qt::DisplayRole){
        //合法公式展示对应的值，非法则展示####
        if(value().isValid()){
            return value().toString();
        }
        else{
            return "####";
        }
    }else if(role == Qt::TextAlignmentRole){
        if(value().type() == QVariant::String){
            return int(Qt::AlignLeft|Qt::AlignVCenter);
        }else{
            return int(Qt::AlignRight|Qt::AlignVCenter);
        }
    }else{
        return QTableWidgetItem::data(role);
    }
}

const QVariant Invaild;
QVariant Cell::value() const{
    if(cacheIsDirty){
        cacheIsDirty = false;
        //获取公式
        QString formulaStr = formula();
        //如果开头是'，那么就返回字符串
        if(formulaStr.startsWith('\'')){
            cachedValue=formulaStr.mid(1);
        }else if(formulaStr.startsWith("=")){
            //如果开头是 = ，那么解析公式
            cachedValue=Invaild;
            QString expr = formulaStr.mid(1);
            expr.replace(" ","");
            //给公式后面加上\0,比如： A1+B1\0
            expr.append(QChar::Null);
            int pos = 0;
            cachedValue = evalExpression(expr,pos);
            if(expr[pos]!=QChar::Null){
                cachedValue=Invaild;
            }
        }else{
            //既不是'，也不是=，那么如果是数字，返回数字，其他返回字符串
            bool ok;
            double d=formulaStr.toDouble(&ok);
            if(ok){
                cachedValue=d;
            }else{
                cachedValue=formulaStr;
            }
        }
    }
    return cachedValue;
}


QVariant Cell::evalExpression(const QString &str,int &pos) const
{
    //左表达式的值，比如 A1+B1 那么就是A1的值
    QVariant result = evalTerm(str,pos);
    while(pos<str.size()&&str[pos]!=QChar::Null){
        QChar op = str[pos];
        if(op!='+'&&op!='-'){
            return result;
        }
        ++pos;
        QVariant term = evalTerm(str,pos);
        if(result.type()==QVariant::Double&&term.type()==QVariant::Double){
            if(op=='+'){
                result=result.toDouble()+term.toDouble();
            }else{
                result = result.toDouble()-term.toDouble();
            }
        }else{
            result=Invaild;
        }
    }
    return result;
}

QVariant Cell::evalTerm(const QString &str,int &pos) const
{
    QVariant result = evalFactor(str,pos);
    while(pos<str.size()&&str[pos]!=QChar::Null){
        QChar op = str[pos];
        if(op!='*'&&op!='/'){
            return result;
        }
        pos++;
        QVariant factor = evalFactor(str,pos);
        if(result.type()==QVariant::Double&&factor.type()==QVariant::Double){
            if(op=='*'){
                result = result.toDouble()*factor.toDouble();
            }else{
                if(factor.toDouble()==0.0){
                    result = Invaild;
                }else{
                    result = result.toDouble()/factor.toDouble();
                }
            }
        }else{
            result = Invaild;
        }
    }
    return result;
}

QVariant Cell::evalFactor(const QString &str,int &pos) const
{
    QVariant result;
    bool negative = false;
    if(str[pos]=='-'){
        negative = true;
        ++pos;
    }
    if(str[pos]=='('){
        ++pos;
        result = evalExpression(str,pos);
        if(str[pos]!=')'){
            result=Invaild;
        }
        ++pos;
    }else{
        QRegExp regExp("[A-Za-z][1-9][0-9]{0,2}");
        QString token;
        while(str[pos].isLetterOrNumber()||str[pos]=='.'){
            token+=str[pos];
            ++pos;
        }
        if(regExp.exactMatch(token)){
            int column = token[0].toUpper().unicode()-'A';
            int row = token.mid(1).toInt()-1;
            Cell *c = static_cast<Cell *>(tableWidget()->item(row,column));
            if(c){
                result=c->value();
            }else{
                result=0.0;
            }
        }else{
            bool ok;
            result = token.toDouble(&ok);
            if(!ok){
                result=Invaild;
            }
        }
    }
    if(negative){
        if(result.type()==QVariant::Double)
        {
            result = -result.toDouble();
        }else{
            result = Invaild;
        }
    }
    return result;
}

