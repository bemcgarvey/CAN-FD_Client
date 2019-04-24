#include "configurefifosdialog.h"
#include "ui_configurefifosdialog.h"

ConfigureFIFOsDialog::ConfigureFIFOsDialog(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint  | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::ConfigureFIFOsDialog)
{
    ui->setupUi(this);
}

ConfigureFIFOsDialog::~ConfigureFIFOsDialog()
{
    delete ui;
}


void ConfigureFIFOsDialog::Setup(const HardwareInfo &hardware) {
    ui->txQSpin->setValue(hardware.txQueueLength);
    ui->txFIFOSpin->setValue(hardware.txFIFOLength);
    ui->rxFIFOSpin->setValue(hardware.rxFIFOLength);
    setMemUsedText();
}

int ConfigureFIFOsDialog::GetTxQLength() {
    return ui->txQSpin->value();
}

int ConfigureFIFOsDialog::GetTxFIFOLength() {
    return ui->txFIFOSpin->value();
}

int ConfigureFIFOsDialog::GetRxFIFOLength() {
    return ui->rxFIFOSpin->value();
}

int ConfigureFIFOsDialog::calculateMemoryUsed(int txQ, int txF, int rxF) {
    int mem;
    mem = txQ * 72;
    mem += txF * 72;
    mem += rxF * 76;
    return mem;
}

void ConfigureFIFOsDialog::on_txQSpin_valueChanged(int arg1)
{
    int txF = ui->txFIFOSpin->value();
    int rxF = ui->rxFIFOSpin->value();
    if (calculateMemoryUsed(arg1, txF, rxF) > 2048) {
        do {
        --arg1;
        } while (calculateMemoryUsed(arg1, txF, rxF) > 2048);
        ui->txQSpin->setValue(arg1);
    }
    setMemUsedText();
}

void ConfigureFIFOsDialog::on_txFIFOSpin_valueChanged(int arg1)
{
    int txQ = ui->txQSpin->value();
    int rxF = ui->rxFIFOSpin->value();
    if (calculateMemoryUsed(txQ, arg1, rxF) > 2048) {
        do {
        --arg1;
        } while (calculateMemoryUsed(txQ, arg1, rxF) > 2048);
        ui->txFIFOSpin->setValue(arg1);
    }
    setMemUsedText();
}

void ConfigureFIFOsDialog::on_rxFIFOSpin_valueChanged(int arg1)
{
    int txF = ui->txFIFOSpin->value();
    int txQ = ui->txQSpin->value();
    if (calculateMemoryUsed(txQ, txF, arg1) > 2048) {
        do {
        --arg1;
        } while (calculateMemoryUsed(txQ, txF, arg1) > 2048);
        ui->rxFIFOSpin->setValue(arg1);
    }
    setMemUsedText();
}

void ConfigureFIFOsDialog::setMemUsedText() {
    ui->memoryUsed->setText(QString().number(calculateMemoryUsed(ui->txQSpin->value(),
                                                                 ui->txFIFOSpin->value(),
                                                                 ui->rxFIFOSpin->value())));
}
