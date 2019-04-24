#include "errordialog.h"
#include "ui_errordialog.h"
#include "toolstatus.h"
#include <QDebug>

ErrorDialog::ErrorDialog(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint  | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::ErrorDialog)
{
    ui->setupUi(this);
    ui->IVMcheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->IVMcheckBox->setFocusPolicy(Qt::NoFocus);
    ui->CERcheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->CERcheckBox->setFocusPolicy(Qt::NoFocus);
    ui->SERcheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->SERcheckBox->setFocusPolicy(Qt::NoFocus);
    ui->RXOVcheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->RXOVcheckBox->setFocusPolicy(Qt::NoFocus);
    ui->USBTXcheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->USBTXcheckBox->setFocusPolicy(Qt::NoFocus);
    ui->USBRXcheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->USBRXcheckBox->setFocusPolicy(Qt::NoFocus);
    ui->ModeChangecheckBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->ModeChangecheckBox->setFocusPolicy(Qt::NoFocus);
}

ErrorDialog::~ErrorDialog()
{
    delete ui;
}

void ErrorDialog::UpdateErrorStatus(unsigned char flags, unsigned char systemFlags) {
    ui->CERcheckBox->setChecked(flags & ToolStatus::ERROR_CER);
    ui->IVMcheckBox->setChecked(flags & ToolStatus::ERROR_IVM);
    ui->SERcheckBox->setChecked(flags & ToolStatus::ERROR_SER);
    ui->RXOVcheckBox->setChecked(flags & ToolStatus::ERROR_RXOV);
    ui->USBTXcheckBox->setChecked(systemFlags & ToolStatus::ERROR_USB_TX);
    ui->USBRXcheckBox->setChecked(systemFlags & ToolStatus::ERROR_USB_RX);
    ui->ModeChangecheckBox->setChecked(systemFlags & ToolStatus::ERROR_MODE_CHANGE);
}

void ErrorDialog::on_pushButton_clicked()
{
    emit ClearErrors();
}
