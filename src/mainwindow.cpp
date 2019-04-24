#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "configdialog.h"
#include "mcp2517fd.h"
#include "txiddelegate.h"
#include "txdatadelegate.h"
#include "configurefifosdialog.h"
#include "QSettings"
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDataStream>
#include <QTextStream>
#include "aboutdialog.h"
#include "filterdialog.h"
#include "dblclicklabel.h"

//BUG When sending both ways a USB TX buffer overrun occurred.
//BUG Definite bug when sending ID's of 3, 2, 1 repeatedly from Tx Queue.  Three or more messages. May be in firmware
//TODO Add toolbar buttons?


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), filterList(32)
{
    ui->setupUi(this);
    rxThread = nullptr;
    usb = new USBDevice();
    usbWatcher = new USBDeviceWatcher(this, 0x301);
    connect(usbWatcher, &USBDeviceWatcher::connected, this, &MainWindow::onUSBDeviceConnected, Qt::QueuedConnection);
    connect(usbWatcher, &USBDeviceWatcher::removed, this, &MainWindow::onUSBDeviceRemoved, Qt::QueuedConnection);
    errorDialog = new ErrorDialog(this);
    connect(this, &MainWindow::ErrorStatusChanged, errorDialog, &ErrorDialog::UpdateErrorStatus);
    connect(errorDialog, &ErrorDialog::ClearErrors, this, &MainWindow::onClearErrors);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::red);
    toolLabel = new QLabel();
    toolLabel->setAutoFillBackground(true);
    toolLabel->setPalette(pal);
    toolLabel->setText("Not Connected");
    toolLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(toolLabel, 0);
    baudLabel = new DblClickLabel();
    connect(dynamic_cast<DblClickLabel *>(baudLabel), &DblClickLabel::DoubleClicked, this, &MainWindow::on_actionConfigureHardware_triggered);
    baudLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(baudLabel, 0);
    modeLabel = new DblClickLabel();
    connect(dynamic_cast<DblClickLabel *>(modeLabel), &DblClickLabel::DoubleClicked, this, &MainWindow::on_actionConfigureHardware_triggered);
    modeLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(modeLabel, 0);
    fifoLengthLabel = new DblClickLabel();
    connect(dynamic_cast<DblClickLabel *>(fifoLengthLabel), &DblClickLabel::DoubleClicked, this, &MainWindow::on_actionFIFO_Sizes_triggered);
    fifoLengthLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(fifoLengthLabel, 0);
    QLabel *spacer = new QLabel();
    spacer->setMinimumWidth(100);
    statusBar()->addPermanentWidget(spacer, 1);
    rxErrorLabel = new QLabel();
    rxErrorLabel->setAutoFillBackground(true);
    rxErrorLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    statusBar()->addPermanentWidget(rxErrorLabel, 0);
    txErrorLabel = new QLabel();
    txErrorLabel->setAutoFillBackground(true);
    txErrorLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);    
    statusBar()->addPermanentWidget(txErrorLabel, 0);
    statusLabel = new DblClickLabel();
    connect(dynamic_cast<DblClickLabel *>(statusLabel), &DblClickLabel::DoubleClicked, this, &MainWindow::on_actionErrorDetails_triggered);
    statusLabel->setAutoFillBackground(true);
    statusLabel->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    statusLabel->setMinimumWidth(60);
    statusBar()->addPermanentWidget(statusLabel, 0);
    rxTableModel = new RxTableModel(this);
    ui->rxTableView->setModel(rxTableModel);
    ui->rxTableView->setColumnWidth(0, 48);
    ui->rxTableView->setColumnWidth(1, 100);
    ui->rxTableView->setColumnWidth(2, 30);
    ui->rxTableView->setColumnWidth(3, 30);
    ui->rxTableView->setColumnWidth(4, 30);
    ui->rxTableView->setColumnWidth(5, 30);
    ui->rxTableView->setColumnWidth(6, 80);
    ui->rxTableView->setColumnWidth(7, 350);
    QStyledItemDelegate *d = new RxDataColumnDelegate(this);
    ui->rxTableView->setItemDelegateForColumn(7, d);
    d = new RxIdDelegate();
    ui->rxTableView->setItemDelegateForColumn(1, d);
    connect(rxTableModel, &RxTableModel::ActiveRow, this, &MainWindow::onNewRxItem);
    txQModel = new TxTreeModel(this);
    ui->txQTreeView->setModel(txQModel);
    connect(txQModel, &TxTreeModel::rowsInserted, this, &MainWindow::onNewTxQItem);
    ui->txQTreeView->setColumnWidth(0, 48);
    ui->txQTreeView->setColumnWidth(1, 100);
    ui->txQTreeView->setColumnWidth(2, 30);
    ui->txQTreeView->setColumnWidth(3, 30);
    ui->txQTreeView->setColumnWidth(4, 30);
    ui->txQTreeView->setColumnWidth(5, 30);
    ui->txQTreeView->setColumnWidth(6, 30);
    d = new TxIdDelegate(this);
    ui->txQTreeView->setItemDelegateForColumn(1, d);
    d = new TxDataDelegate(this);
    ui->txQTreeView->setItemDelegateForColumn(7, d);
    ui->txQTreeView->setItemDelegateForColumn(6, d);
    txFModel = new TxTreeModel(this);
    ui->txFTreeView->setModel(txFModel);
    connect(txFModel, &TxTreeModel::rowsInserted, this, &MainWindow::onNewTxFItem);
    ui->txFTreeView->setColumnWidth(0, 48);
    ui->txFTreeView->setColumnWidth(1, 100);
    ui->txFTreeView->setColumnWidth(2, 30);
    ui->txFTreeView->setColumnWidth(3, 30);
    ui->txFTreeView->setColumnWidth(4, 30);
    ui->txFTreeView->setColumnWidth(5, 30);
    ui->txFTreeView->setColumnWidth(6, 30);
    d = new TxIdDelegate(this);
    ui->txFTreeView->setItemDelegateForColumn(1, d);
    d = new TxDataDelegate(this);
    ui->txFTreeView->setItemDelegateForColumn(7, d);
    ui->txFTreeView->setItemDelegateForColumn(6, d);
    statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    onUSBDeviceConnected();
    updateStatusBar();
    loadWindowSettings();
    filterList[0].active = true;
    filterList[0].EXIDE = true;
}

MainWindow::~MainWindow()
{
    saveWindowSettings();
    saveHardwareSettings();
    closeUsb();
    delete ui;
    delete usb;
}


void MainWindow::onUSBDeviceConnected() {
    closeUsb();
    if (usb->Open()) {
        loadHardwareSettings();
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::green);
        toolLabel->setText("Connected");
        toolLabel->setPalette(pal);
        rxThread = new RxThread(usb);
        connect(rxThread, &RxThread::MessageReady, rxTableModel, &RxTableModel::AddMessage, Qt::QueuedConnection);
        rxThread->start(QThread::TimeCriticalPriority);
        statusTimer->start(200);
    }
    updateHardwareDisplay();
}

void MainWindow::onUSBDeviceRemoved() {
    statusTimer->stop();
    closeUsb();
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::red);
    toolLabel->setText("Not Connected");
    toolLabel->setPalette(pal);
    saveHardwareSettings();
}

void MainWindow::closeUsb() {
    if (rxThread != nullptr) {
        rxThread->disconnect();
        rxThread->Stop();
        while (!rxThread->isFinished());
        delete rxThread;
        rxThread = nullptr;
    }
    if (usb->IsConnected()) {
        usb->Close();
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_RxClearButton_clicked()
{
    rxTableModel->Clear();
}

void MainWindow::on_RxPauseButton_toggled(bool checked)
{
    rxTableModel->Pause(checked);
}

void MainWindow::on_fixedCheckBox_clicked(bool checked)
{
    rxTableModel->Fixed(checked);
}

void MainWindow::on_actionConfigureHardware_triggered()
{
    if (!usb->IsConnected()) {
        return;
    }
    status.Update(usb);
    hardware.Update(usb);
    ConfigDialog *dlg = new ConfigDialog(this);
    dlg->Setup(&status, &hardware);
    if (dlg->exec() == QDialog::Accepted) {
        if (usb->IsConnected()) {
            uint8_t data[8];
            data[0] = static_cast<uint8_t>(dlg->GetMode());
            data[1] = static_cast<uint8_t>(dlg->GetNominalBaud());
            data[2] = static_cast<uint8_t>(dlg->GetDataBaud());
            usb->SendControl(SET_HARDWARE_CONFIG, data, 8);
            updateStatusBar();
            updateHardwareDisplay();
        }
    }
    delete dlg;
}

void MainWindow::updateStatusBar() {
    status.Update(usb);
    emit ErrorStatusChanged(static_cast<unsigned char>(status.errorFlags),
                            static_cast<unsigned char>(status.systemErrors));
    QString labelText;
    QPalette pal = palette();
    if (status.busState & ToolStatus::TX_WARN) {
        pal.setColor(QPalette::Window, Qt::lightGray);
    } else if (status.busState & ToolStatus::TX_PASSIVE) {
        pal.setColor(QPalette::Window, Qt::yellow);
    } else if (status.busState & ToolStatus::TX_BUS_OFF) {
        pal.setColor(QPalette::Window, Qt::red);
    } else {
        pal.setColor(QPalette::Window, Qt::white);
    }
    txErrorLabel->setPalette(pal);
    labelText = "TX Errors: ";
    txErrorLabel->setText(labelText + QString().number(status.txErrors));
    if (status.busState & ToolStatus::RX_WARN) {
        pal.setColor(QPalette::Window, Qt::lightGray);
    } else if (status.busState & ToolStatus::RX_PASSIVE) {
        pal.setColor(QPalette::Background, Qt::yellow);
    } else {
        pal.setColor(QPalette::Window, Qt::white);
    }
    labelText = "RX Errors: ";
    rxErrorLabel->setText(labelText + QString().number(status.rxErrors));
    rxErrorLabel->setPalette(pal);
    if (status.errorFlags == 0 && status.systemErrors == 0) {
        pal.setColor(QPalette::Window, Qt::green);
        statusLabel->setText("Ok");
    } else {
        pal.setColor(QPalette::Window, Qt::red);
        statusLabel->setText("Error");
    }
    statusLabel->setPalette(pal);
    switch (status.mode) {
    case NORMAL_MODE: labelText = "CAN-FD Mode";
        break;
    case CAN_2_0_MODE: labelText = "CAN 2.0 Mode";
        break;
    case LISTEN_ONLY_MODE: labelText = "Listen Only Mode";
        break;
    case EXT_LOOPBACK_MODE: labelText = "External Loopback Mode";
        break;
    case INT_LOOPBACK_MODE: labelText = "Internal Loopback Mode";
        break;
    case RESTRICTED_MODE: labelText = "Restricted Mode";
        break;
    case SLEEP_MODE: labelText = "Sleep Mode";
        break;
    case CONFIG_MODE: labelText = "Config Mode";
        break;
    default: labelText = "???";
    }
    modeLabel->setText(labelText);

}

void MainWindow::updateHardwareDisplay() {
    hardware.Update(usb);
    QString labelText;
    switch (hardware.nominalBaud) {
    case N_50K: labelText = "50kbps/";
        break;
    case N_100K: labelText = "100kbps/";
        break;
    case N_125K: labelText = "125kbps/";
        break;
    case N_250K: labelText = "250kbps/";
        break;
    case N_500K: labelText = "500kbps/";
        break;
    case N_800K: labelText = "800kbps/";
        break;
    case N_1M: labelText = "1Mbps/";
        break;
    default: labelText = "???";
    }
    switch (hardware.dataBaud) {
    case D_500K: labelText += "500kbps";
        break;
    case D_1M: labelText += "1Mbps";
        break;
    case D_2M: labelText += "2Mbps";
        break;
    case D_5M: labelText += "5Mbps";
        break;
    case D_8M: labelText += "8Mbps";
        break;
    default: labelText = "???";
    }
    baudLabel->setText(labelText);
    labelText = "FIFO's: %1/%2/%3";
    labelText = labelText.arg(hardware.txQueueLength).arg(hardware.txFIFOLength).arg(hardware.rxFIFOLength);
    fifoLengthLabel->setText(labelText);
    ui->txQFrame->setEnabled(hardware.txQueueLength != 0);
}

void MainWindow::on_actionAbout_triggered()
{
    QString firmwareVersion = "Not Connected";
    if (usb->IsConnected()) {
        uint8_t data[2];
        usb->ReadControl(GET_VERSION, data, 2);
        firmwareVersion = QString().number(data[1], 16) + "." + QString().number(data[0], 16);
    }
    AboutDialog *dlg = new AboutDialog(this);
    dlg->SetFirmwareVersion(firmwareVersion);
    dlg->exec();
}

void MainWindow::onClearErrors() {
    if (usb->IsConnected()) {
        usb->SendControl(CLEAR_ERRORS, 0, 0);
    }
    status.errorFlags = 0;
    status.systemErrors = 0;
}

void MainWindow::on_actionErrorDetails_triggered()
{
    errorDialog->show();
}

void MainWindow::on_txQAddGroupButton_clicked()
{
    txQModel->AddGroup();
    txQModel->AddMessage(txQModel->rowCount() - 1);
}

void MainWindow::on_txQAddMsgButton_clicked()
{
    int group;
    QModelIndex index = ui->txQTreeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    TxTreeItem *t = static_cast<TxTreeItem *>(index.internalPointer());
    if (t->ItemType() == TxTreeItem::GROUP) {
        group = t->row();
    } else {
        group = t->ParentItem()->row();
    }
    txQModel->AddMessage(group);
}

void MainWindow::onNewTxQItem(const QModelIndex &parent, int first, int last) {
    for (int i = first; i <= last; ++i) {
        QModelIndex index = txQModel->index(i, 6, parent);
        TxTreeItem *t = static_cast<TxTreeItem *>(index.internalPointer());
        if (t->ItemType() == TxTreeItem::GROUP) {
            QPushButton *button = new QPushButton(this);
            button->setText("Send");
            t->SetButton(button);
            connect(t, &TxTreeItem::SendMe, this, &MainWindow::onTxQSend);
            ui->txQTreeView->setIndexWidget(index, button);
        } else {
            ui->txQTreeView->setCurrentIndex(txQModel->index(i, 0, parent));
        }
    }
}

void MainWindow::onTxQSend(TxTreeItem *group) {
    //qDebug() << "Send TxQ " << group->row();
    if (!usb->IsConnected()) {
        return;
    }
    const int txSize = 76;
    uint8_t buffer[txSize];
    for (int i = 0; i < group->childCount(); ++i) {
        buffer[0] = group->GetChild(i)->Message()->ToBytes(buffer + 4);
        if (i == group->childCount() - 1) {
            buffer[1] = 1;
        } else {
            buffer[1] = 0;
        }
        usb->WritePipe(USBDevice::TXQ, buffer, txSize);
    }
}

void MainWindow::on_txFAddGroupButton_clicked()
{
    txFModel->AddGroup();
    txFModel->AddMessage(txFModel->rowCount() - 1);
}

void MainWindow::on_txFAddMsgButton_clicked()
{
    int group;
    QModelIndex index = ui->txFTreeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    TxTreeItem *t = static_cast<TxTreeItem *>(index.internalPointer());
    if (t->ItemType() == TxTreeItem::GROUP) {
        group = t->row();
    } else {
        group = t->ParentItem()->row();
    }
    txFModel->AddMessage(group);
}

void MainWindow::onNewTxFItem(const QModelIndex &parent, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        QModelIndex index = txFModel->index(i, 6, parent);
        TxTreeItem *t = static_cast<TxTreeItem *>(index.internalPointer());
        if (t->ItemType() == TxTreeItem::GROUP) {
            QPushButton *button = new QPushButton(this);
            button->setText("Send");
            t->SetButton(button);
            connect(t, &TxTreeItem::SendMe, this, &MainWindow::onTxFSend);
            ui->txFTreeView->setIndexWidget(index, button);
        } else {
            ui->txFTreeView->setCurrentIndex(txFModel->index(i, 0, parent));
        }
    }
}

void MainWindow::onTxFSend(TxTreeItem *group)
{
    if (!usb->IsConnected()) {
        return;
    }
    const int txSize = 76;
    uint8_t buffer[txSize];
    for (int i = 0; i < group->childCount(); ++i) {
        buffer[0] = group->GetChild(i)->Message()->ToBytes(buffer + 4);
        if (i == group->childCount() - 1) {
            buffer[1] = 1;
        } else {
            buffer[1] = 0;
        }
        usb->WritePipe(USBDevice::TXFIFO, buffer, txSize);
    }
}

void MainWindow::onNewRxItem(int row) {
    ui->rxTableView->setCurrentIndex(rxTableModel->index(row, 0, QModelIndex()));
}

void MainWindow::on_txQDeleteButton_clicked()
{
    QModelIndex index = ui->txQTreeView->currentIndex();
    if (index.isValid()) {
        txQModel->DeleteRow(index);
    }
}

void MainWindow::on_txFDeleteButton_clicked()
{
    QModelIndex index = ui->txFTreeView->currentIndex();
    if (index.isValid()) {
        txFModel->DeleteRow(index);
    }
}

void MainWindow::on_actionFIFO_Sizes_triggered()
{
    if (!usb->IsConnected()) {
        return;
    }
    hardware.Update(usb);
    ConfigureFIFOsDialog *dlg = new ConfigureFIFOsDialog(this);
    dlg->Setup(hardware);
    if (dlg->exec() == QDialog::Accepted) {
        uint8_t buffer[8];
        buffer[0] = static_cast<uint8_t>(dlg->GetTxQLength());
        buffer[1] = static_cast<uint8_t>(dlg->GetTxFIFOLength());
        buffer[2] = static_cast<uint8_t>(dlg->GetRxFIFOLength());
        usb->SendControl(SET_FIFO_LENGTHS, buffer, 8);
    }
    delete dlg;
    updateHardwareDisplay();
}

void MainWindow::saveWindowSettings() {
    QSettings settings("QES", "CAN-FD Analyzer");
    settings.beginGroup("MainWindow");
    settings.setValue("Maximized", this->isMaximized());
    if (!this->isMinimized()) {
        settings.setValue("Position", this->pos());
        settings.setValue("Size", this->size());
    }
    settings.setValue("VSplitter", ui->vsplitter->saveState());
    settings.setValue("HSplitter", ui->hsplitter->saveState());
    settings.endGroup();
}

void MainWindow::loadWindowSettings() {
    QSettings settings("QES", "CAN-FD Analyzer");
    settings.beginGroup("MainWindow");
    if (settings.value("Maximized", false).toBool()) {
        this->setWindowState(Qt::WindowMaximized);
    }
    if (settings.contains("Position")) {
        this->move(settings.value("Position", this->pos()).toPoint());
    }
    if (settings.contains("Size")) {
        this->resize(settings.value("Size", this->size()).toSize());
    }
    if (settings.contains("VSplitter")) {
        ui->vsplitter->restoreState(settings.value("VSplitter").toByteArray());
    }
    if (settings.contains("HSplitter")) {
        ui->hsplitter->restoreState(settings.value("HSplitter").toByteArray());
    }
    settings.endGroup();
}

void MainWindow::saveHardwareSettings() {
    if (usb->IsConnected()) {
        hardware.Update(usb);
        status.Update(usb);
    }
    QSettings settings("QES", "CAN-FD Analyzer");
    settings.beginGroup("MCP2517FD");
    settings.setValue("Nominal Baud", hardware.nominalBaud);
    settings.setValue("Data Baud", hardware.dataBaud);
    settings.setValue("Mode", status.mode);
    settings.setValue("TX Queue Length", hardware.txQueueLength);
    settings.setValue("TX FIFO Length", hardware.txFIFOLength);
    settings.setValue("RX FIFO Length", hardware.rxFIFOLength);
    settings.endGroup();
}

void MainWindow::loadHardwareSettings() {
    QSettings settings("QES", "CAN-FD Analyzer");
    settings.beginGroup("MCP2517FD");
    if (usb->IsConnected()) {
        uint8_t data[8];
        data[0] = static_cast<uint8_t>(settings.value("Mode", 0).toInt());
        data[1] = static_cast<uint8_t>(settings.value("Nominal Baud", 6).toInt());
        data[2] = static_cast<uint8_t>(settings.value("Data Baud", 2).toInt());
        usb->SendControl(SET_HARDWARE_CONFIG, data, 8);
        data[0] = static_cast<uint8_t>(settings.value("TX Queue Length", 8).toInt());
        data[1] = static_cast<uint8_t>(settings.value("TX FIFO Length", 7).toInt());
        data[2] = static_cast<uint8_t>(settings.value("RX FIFO Length", 12).toInt());
        usb->SendControl(SET_FIFO_LENGTHS, data, 8);
    }
}

void MainWindow::on_actionTxQLoad_triggered()
{
    QSettings settings("QES", "CAN-FD Analyzer");
    QString lastDir = settings.value("LastDir", ".").toString();
    QString fileName = QFileDialog::getOpenFileName(this, "Load Tx Queue Items", lastDir, "CAN Messages (*.cfd)");
    if (fileName != "") {
        QFileInfo info(fileName);
        settings.setValue("LastDir", info.absoluteDir().absolutePath());
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        // Read a header with a "magic number" and a version
        quint32 magic;
        in >> magic;
        if (magic != 0x4D630301) {
            QMessageBox::critical(this, "CAN-FD Anlyzer", "Invalid file format", QMessageBox::Ok);
        } else {
            quint32 version;
            in >> version;
            in.setVersion(QDataStream::Qt_5_0);
            txQModel->disconnect();
            TxTreeModel *newModel = new TxTreeModel(this);
            ui->txQTreeView->setModel(newModel);
            connect(newModel, &TxTreeModel::rowsInserted, this, &MainWindow::onNewTxQItem);
            delete txQModel;
            txQModel = newModel;
            txQModel->FromDataStream(in);
        }
        file.close();
    }
}

void MainWindow::on_actionTxQSave_triggered()
{
    QSettings settings("QES", "CAN-FD Analyzer");
    QString lastDir = settings.value("LastDir", ".").toString();
    QString fileName = QFileDialog::getSaveFileName(this, "Save Tx Queue Items", lastDir, "CAN Messages (*.cfd)");
    if (fileName != "") {
        QFileInfo info(fileName);
        settings.setValue("LastDir", info.absoluteDir().absolutePath());
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        // Write a header with a "magic number" and a version
        out << static_cast<quint32>(0x4D630301);
        out << static_cast<qint32>(100);
        out.setVersion(QDataStream::Qt_5_0);
        txQModel->ToDataStream(out);
        file.close();
    }
}

void MainWindow::on_actionTxFSave_triggered()
{
    QSettings settings("QES", "CAN-FD Analyzer");
    QString lastDir = settings.value("LastDir", ".").toString();
    QString fileName = QFileDialog::getSaveFileName(this, "Save Tx FIFO Items", lastDir, "CAN Messages (*.cfd)");
    if (fileName != "") {
        QFileInfo info(fileName);
        settings.setValue("LastDir", info.absoluteDir().absolutePath());
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        // Write a header with a "magic number" and a version
        out << static_cast<quint32>(0x4D630301);
        out << static_cast<qint32>(100);
        out.setVersion(QDataStream::Qt_5_0);
        txFModel->ToDataStream(out);
        file.close();
    }
}

void MainWindow::on_actionTxFLoad_triggered()
{
    QSettings settings("QES", "CAN-FD Analyzer");
    QString lastDir = settings.value("LastDir", ".").toString();
    QString fileName = QFileDialog::getOpenFileName(this, "Load Tx Queue Items", lastDir, "CAN Messages (*.cfd)");
    if (fileName != "") {
        QFileInfo info(fileName);
        settings.setValue("LastDir", info.absoluteDir().absolutePath());
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        // Read a header with a "magic number" and a version
        quint32 magic;
        in >> magic;
        if (magic != 0x4D630301) {
            QMessageBox::critical(this, "CAN-FD Anlyzer", "Invalid file format", QMessageBox::Ok);
        } else {
            quint32 version;
            in >> version;
            in.setVersion(QDataStream::Qt_5_0);
            txFModel->disconnect();
            TxTreeModel *newModel = new TxTreeModel(this);
            ui->txFTreeView->setModel(newModel);
            connect(newModel, &TxTreeModel::rowsInserted, this, &MainWindow::onNewTxFItem);
            delete txFModel;
            txFModel = newModel;
            txFModel->FromDataStream(in);
        }
        file.close();
    }
}

void MainWindow::on_actionSave_Rx_Log_triggered()
{
    QSettings settings("QES", "CAN-FD Analyzer");
    QString lastDir = settings.value("LastDir", ".").toString();
    QString fileName = QFileDialog::getSaveFileName(this, "Save Rx Log", lastDir, "CSV file (*.csv);;Text file (*.txt)");
    if (fileName != "") {
        QFileInfo info(fileName);
        settings.setValue("LastDir", info.absoluteDir().absolutePath());
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        rxTableModel->ToCsv(out);
        file.close();
    }
}

void MainWindow::on_actionFilter_triggered()
{
    if (usb->IsConnected()) {
        FilterDialog *dlg = new FilterDialog(filterList, this);
        if (dlg->exec() == QDialog::Accepted) {
            filterList = dlg->getFilterList();
            if (dlg->getResult() == FilterDialog::SET) {
                uint32_t buffer[2];
                for (int i = 0; i < 32; ++i) {
                    if (filterList[i].active) {
                        buffer[0] = filterList[i].mask;
                        if (filterList[i].MIDE) {
                            buffer[0] |= 0x40000000;
                        }
                        buffer[1] = filterList[i].filter;
                        if (filterList[i].EXIDE) {
                            buffer[1] |= 0x40000000;
                        }
                        usb->SendControl(SET_FILTER, reinterpret_cast<uint8_t *>(buffer), 8, static_cast<uint16_t>(i));
                    } else {
                        usb->SendControl(CLEAR_FILTER, 0, 0, static_cast<uint16_t>(i));
                    }
                }
            }  else if(dlg->getResult() == FilterDialog::CLEAR_ALL) {
                usb->SendControl(CLEAR_FILTER, 0, 0, 0xffff);
                for (int i = 1; i < 32; ++i) {
                    filterList[i].active = false;
                }
                filterList[0].active = true;
                filterList[0].filter = filterList[0].mask = 0;
                filterList[0].EXIDE = true;
            }
        }
    }
}
