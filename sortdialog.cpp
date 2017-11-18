#include "sortdialog.h"

SortDialog::SortDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    secondaryGroupBox->hide();
    tertiaryGroupBox->hide();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    setColumnRange('A','Z');
}

void SortDialog::setColumnRange(QChar first,QChar last)
{
    primaryCombo->clear();
    secondaryCombo->clear();
    tertiaryCombo->clear();

    secondaryCombo->addItem(tr("None"));
    tertiaryCombo->addItem(tr("None"));
    primaryCombo->setMinimumSize(secondaryCombo->sizeHint());
    QChar ch = first;
    while(ch<=last)
    {
        primaryCombo->addItem(QString(ch));
        secondaryCombo->addItem(QString(ch));
        tertiaryCombo->addItem(QString(ch));
        ch = ch.unicode()+1;
    }
}
