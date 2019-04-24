#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "toolstatus.h"

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    int GetNominalBaud();
    int GetDataBaud();
    int GetMode();
    void Setup(const ToolStatus *status, const HardwareInfo *hardware);
private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
