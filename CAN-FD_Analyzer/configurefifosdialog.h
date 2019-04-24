#ifndef CONFIGUREFIFOSDIALOG_H
#define CONFIGUREFIFOSDIALOG_H

#include <QDialog>
#include <toolstatus.h>

namespace Ui {
class ConfigureFIFOsDialog;
}

class ConfigureFIFOsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureFIFOsDialog(QWidget *parent = 0);
    ~ConfigureFIFOsDialog();
    void Setup(const HardwareInfo &hardware);
    int GetTxQLength();
    int GetTxFIFOLength();
    int GetRxFIFOLength();

private slots:
    void on_txQSpin_valueChanged(int arg1);
    void on_txFIFOSpin_valueChanged(int arg1);
    void on_rxFIFOSpin_valueChanged(int arg1);
private:
    Ui::ConfigureFIFOsDialog *ui;
    int calculateMemoryUsed(int txQ, int txF, int rxF);
    void setMemUsedText();
};

#endif // CONFIGUREFIFOSDIALOG_H
