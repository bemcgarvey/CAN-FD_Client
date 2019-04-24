#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "usbdevice.h"
#include "USBDeviceWatcher.h"
#include <QLabel>
#include "rxtablemodel.h"
#include "rxdatacolumndelegate.h"
#include "rxiddelegate.h"
#include "rxthread.h"
#include "toolstatus.h"
#include <QTimer>
#include "errordialog.h"
#include "txtreemodel.h"
#include "txtreeitem.h"
#include "canfilter.h"
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onUSBDeviceConnected();
    void onUSBDeviceRemoved();
    void on_actionExit_triggered();
    void on_RxClearButton_clicked();
    void on_RxPauseButton_toggled(bool checked);
    void on_fixedCheckBox_clicked(bool checked);
    void on_actionConfigureHardware_triggered();
    void updateStatusBar();
    void on_actionAbout_triggered();
    void on_actionErrorDetails_triggered();
    void onClearErrors();
    void on_txQAddGroupButton_clicked();
    void on_txQAddMsgButton_clicked();
    void onNewTxQItem(const QModelIndex &parent, int first, int last);
    void onTxQSend(TxTreeItem *group);
    void on_txFAddGroupButton_clicked();
    void on_txFAddMsgButton_clicked();
    void onNewTxFItem(const QModelIndex &parent, int first, int last);
    void onTxFSend(TxTreeItem *group);
    void onNewRxItem(int row);
    void on_txQDeleteButton_clicked();
    void on_txFDeleteButton_clicked();
    void on_actionFIFO_Sizes_triggered();
    void updateHardwareDisplay();
    void on_actionTxQLoad_triggered();
    void on_actionTxQSave_triggered();
    void on_actionTxFSave_triggered();
    void on_actionTxFLoad_triggered();
    void on_actionSave_Rx_Log_triggered();
    void on_actionFilter_triggered();

signals:
    void ErrorStatusChanged(unsigned char, unsigned char);

private:
    Ui::MainWindow *ui;
    USBDevice *usb;
    USBDeviceWatcher *usbWatcher;
    QLabel *baudLabel;
    QLabel *modeLabel;
    QLabel *rxErrorLabel;
    QLabel *txErrorLabel;
    QLabel *toolLabel;
    QLabel *statusLabel;
    QLabel *fifoLengthLabel;
    RxTableModel *rxTableModel;
    RxThread *rxThread;
    ToolStatus status;
    HardwareInfo hardware;
    QTimer *statusTimer;
    ErrorDialog *errorDialog;
    TxTreeModel *txQModel;
    TxTreeModel *txFModel;
    QVector<CANFilter> filterList;
    void closeUsb();
    void saveWindowSettings();
    void loadWindowSettings();
    void saveHardwareSettings();
    void loadHardwareSettings();
};

#endif // MAINWINDOW_H
