#include "configdialog.h"
#include "ui_configdialog.h"
#include "mcp2517fd.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint  | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

int ConfigDialog::GetNominalBaud() {
    return ui->nominalBaudComboBox->currentIndex();
}

int ConfigDialog::GetDataBaud() {
    return ui->dataBaudComboBox->currentIndex();
}

int ConfigDialog::GetMode() {
    switch (ui->modeComboBox->currentIndex()) {
    case 0: return NORMAL_MODE;
    case 1: return CAN_2_0_MODE;
    case 2: return LISTEN_ONLY_MODE;
    case 3: return RESTRICTED_MODE;
    case 4: return INT_LOOPBACK_MODE;
    case 5: return EXT_LOOPBACK_MODE;
    default: return CONFIG_MODE;
    }
}

void ConfigDialog::Setup(const ToolStatus *status, const HardwareInfo *hardware) {
    ui->nominalBaudComboBox->setCurrentIndex(hardware->nominalBaud);
    ui->dataBaudComboBox->setCurrentIndex(hardware->dataBaud);
    int modeIndex = 0;
    switch (status->mode) {
    case NORMAL_MODE: modeIndex = 0;
        break;
    case CAN_2_0_MODE: modeIndex = 1;
        break;
    case LISTEN_ONLY_MODE: modeIndex = 2;
        break;
    case RESTRICTED_MODE: modeIndex = 3;
        break;
    case INT_LOOPBACK_MODE: modeIndex = 4;
        break;
    case EXT_LOOPBACK_MODE: modeIndex = 5;
        break;
    case CONFIG_MODE: modeIndex = 6;
        break;
    default: modeIndex = 6;
    }
    ui->modeComboBox->setCurrentIndex(modeIndex);
}
