#include "filterdialog.h"
#include "filtervalidator.h"
#include "ui_filterdialog.h"
#include <QDebug>

FilterDialog::FilterDialog(const QVector<CANFilter> &list, QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint  | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::FilterDialog), filterList(list)
{
    ui->setupUi(this);
    ui->filterNumberSpinBox->setValue(0);
    ui->filterNumberSpinBox->setFocus();
    FilterValidator *v = new FilterValidator(this);
    ui->filterEdit->setValidator(v);
    ui->maskEdit->setValidator(v);
    filterList = list;
    updateDisplay();
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::on_removeAllButton_clicked()
{
    result = CLEAR_ALL;
    accept();
}


void FilterDialog::on_addButton_clicked()
{
    result = SET;
    accept();
}

void FilterDialog::updateDisplay()
{
    const CANFilter &f = filterList[ui->filterNumberSpinBox->value()];
    ui->maskEdit->setText("0x" + QString().number(f.mask, 16).toUpper());
    ui->filterEdit->setText("0x" + QString().number(f.filter, 16).toUpper());
    ui->EXIDEcheckBox->setChecked(f.EXIDE);
    ui->MIDEcheckBox->setChecked(f.MIDE);
    ui->filterEnabledCheckBox->setChecked(f.active);
    ui->maskEdit->setEnabled(f.active);
    ui->filterEdit->setEnabled(f.active);
    ui->EXIDEcheckBox->setEnabled(f.active);
    ui->MIDEcheckBox->setEnabled(f.active);
}

void FilterDialog::on_filterEnabledCheckBox_clicked(bool checked)
{
    filterList[ui->filterNumberSpinBox->value()].active = checked;
    updateDisplay();
}

void FilterDialog::on_MIDEcheckBox_clicked(bool checked)
{
    filterList[ui->filterNumberSpinBox->value()].MIDE = checked;
}

void FilterDialog::on_EXIDEcheckBox_clicked(bool checked)
{
    filterList[ui->filterNumberSpinBox->value()].EXIDE = checked;
}

void FilterDialog::on_filterEdit_editingFinished()
{
    bool ok;
    filterList[ui->filterNumberSpinBox->value()].filter = ui->filterEdit->text().toUInt(&ok, 16);
}

QVector<CANFilter> FilterDialog::getFilterList() const
{
    return filterList;
}


void FilterDialog::on_maskEdit_editingFinished()
{
    bool ok;
    filterList[ui->filterNumberSpinBox->value()].mask = ui->maskEdit->text().toUInt(&ok, 16);
}


