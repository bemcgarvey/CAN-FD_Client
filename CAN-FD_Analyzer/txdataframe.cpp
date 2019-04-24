#include "txdataframe.h"
#include "ui_txdataframe.h"
#include <QDebug>
#include <QByteArray>
#include <QKeyEvent>

TxDataFrame::TxDataFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TxDataFrame)
{
    ui->setupUi(this);
    setAutoFillBackground(true);
    previousSize = 0;
    for (int i = 0; i < 8; ++i) {
        header += QString().number(i);
    }
    ui->table->setHorizontalHeaderLabels(header);
}

TxDataFrame::~TxDataFrame()
{
    delete ui;
}

void TxDataFrame::on_doneButton_clicked()
{
    setVisible(false);
    pullFromEdits(ui->dataTypeComboBox->currentIndex(), ui->dlcComboBox->currentData().toInt());
    emit DataUpdated();
}

void TxDataFrame::on_dataTypeComboBox_currentIndexChanged(int index)
{
    pullFromEdits(currentType, previousSize);
    setupEdits(index);
    currentType = index;
}

int TxDataFrame::GetDLC() const {
    return ui->dlcComboBox->currentIndex();
}

TxTreeItem::DataDisplayType TxDataFrame::GetDataDisplayType() {
    return static_cast<TxTreeItem::DataDisplayType>(ui->dataTypeComboBox->currentIndex());
}

void TxDataFrame::Setup(const TxTreeItem *item) {
    this->item = item;
    const TxMessage *msg = item->Message();
    ui->dlcComboBox->clear();
    for (int i = 0; i <= 8; ++i) {
        ui->dlcComboBox->addItem(QString().number(i), i);
    }
    if (msg->fdf) {
        for (int i = 9; i <= 15; ++i) {
            ui->dlcComboBox->addItem(QString().number(TxMessage::dlcToLen(i)), TxMessage::dlcToLen(i));
        }
    }
    ui->dlcComboBox->setCurrentIndex(msg->dlc);
    currentType = item->GetDataDisplayType();
    ui->dataTypeComboBox->setCurrentIndex(currentType);
    for (int i = 0; i < 64; ++i) {
        data[i] = msg->data[i];
    }
    setupEdits(currentType);
    setFocus();
}

void TxDataFrame::on_cancelButton_clicked()
{
    setVisible(false);
}

void TxDataFrame::on_text_textChanged()
{
    int dataLen = ui->dlcComboBox->currentData().toInt();
    if(ui->text->toPlainText().length() > dataLen)
    {
        QString newStr = ui->text->toPlainText();
        newStr.truncate(dataLen);
        ui->text->setText(newStr);
        QTextCursor cursor(ui->text->textCursor());
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->text->setTextCursor(cursor);
    }
}

void TxDataFrame::on_dlcComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    on_dataTypeComboBox_currentIndexChanged(ui->dataTypeComboBox->currentIndex());
    previousSize = ui->dlcComboBox->currentData().toInt();
}

void TxDataFrame::setupEdits(int type) {
    int rows = 0;
    int columns = 1;
    int dataLen = ui->dlcComboBox->currentData().toInt();
    if (dataLen == 0) {
        ui->table->setVisible(false);
        ui->text->setVisible(false);
        return;
    }
    ui->text->setText("");
    if (type == 7) {
        QString s;
        s = ui->text->toPlainText();
        if (s.length() == 0) {
            s = QString::fromUtf8(reinterpret_cast<const char *>(data));
        }
        if (s.length() > dataLen) {
            s.truncate(dataLen);
        }
        ui->text->setText(s);
        ui->table->setVisible(false);
        ui->text->setVisible(true);
    } else {
        rows = ((dataLen - 1) / 8) + 1;
        switch (type) {
        case 0: //BYTES
            columns = 8;
            break;
        case 1: //UINT16
        case 2: //INT16
            columns = 4;
            break;
        case 3: //UINT32
        case 4: //INT32
        case 5: //FLOAT
            columns = 2;
            break;
        case 6: //DOUBLE
            columns = 1;
            break;
        }
        ui->table->setRowCount(rows);
        ui->table->setColumnCount(columns);
        int width = ui->table->width();
        width -= ui->table->verticalHeader()->width() + 1;
        width /= columns;
        for (int i = 0; i < columns; ++i) {
            ui->table->setColumnWidth(i, width);
        }
        ui->table->setVerticalHeaderLabels(header);
        uint8_t *d = data;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < columns; ++c) {
                QTableWidgetItem *item = nullptr;
                switch (type) {
                case 0:
                    item = new QTableWidgetItem(QString().number(*d, 16));
                    ++d;
                    break;
                case 1:
                    item = new QTableWidgetItem(QString().number(*reinterpret_cast<uint16_t *>(d), 10));
                    d += 2;
                    break;
                case 2:
                    item = new QTableWidgetItem(QString().number(*reinterpret_cast<int16_t *>(d), 10));
                    d += 2;
                    break;
                case 3:
                    item = new QTableWidgetItem(QString().number(*reinterpret_cast<uint32_t *>(d), 10));
                    d += 4;
                    break;
                case 4:
                    item = new QTableWidgetItem(QString().number(*reinterpret_cast<int32_t *>(d), 10));
                    d += 4;
                    break;
                case 5:
                    item = new QTableWidgetItem(QString().number(*reinterpret_cast<float *>(d), 'g', 6));
                    d += 4;
                    break;
                case 6:
                    item = new QTableWidgetItem(QString().number(*reinterpret_cast<double *>(d), 'g', 6));
                    d += 8;
                    break;
                }
                ui->table->setItem(r, c, item);
            }
        }
        ui->table->setVisible(true);
        ui->text->setVisible(false);
    }
}

void TxDataFrame::pullFromEdits(int type, int dataLen) {
    int rows = 0;
    int columns = 0;
    //int dataLen = ui->dlcComboBox->currentData().toInt();

    if (type == 7) {
        QString s = ui->text->toPlainText();
        s.truncate(dataLen);
        QByteArray bytes = s.toUtf8();
        for (int i = 0; i < bytes.length() && i < 64; ++i) {
            data[i] = bytes[i];
        }
    } else {
        if (dataLen == 0) {
            return;
        }
        rows = ((dataLen - 1) / 8) + 1;
        switch (type) {
        case 0: //BYTES
            columns = 8;
            break;
        case 1: //UINT16
        case 2: //INT16
            columns = 4;
            break;
        case 3: //UINT32
        case 4: //INT32
        case 5: //FLOAT
            columns = 2;
            break;
        case 6: //DOUBLE
            columns = 1;
            break;
        }
        uint8_t *d = data;
        bool ok = false;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < columns; ++c) {
                QTableWidgetItem *item;
                switch (type) {
                case 0:
                    item = ui->table->item(r, c);
                    *d = item->text().toUInt(&ok, 16);
                    ++d;
                    break;
                case 1:
                    item = ui->table->item(r, c);
                    *reinterpret_cast<uint16_t *>(d) = item->text().toUInt();
                    d += 2;
                    break;
                case 2:
                    item = ui->table->item(r, c);
                    *reinterpret_cast<int16_t *>(d) = item->text().toInt();
                    d += 2;
                    break;
                case 3:
                    item = ui->table->item(r, c);
                    *reinterpret_cast<uint32_t *>(d) = item->text().toUInt();
                    d += 4;
                    break;
                case 4:
                    item = ui->table->item(r, c);
                    *reinterpret_cast<int32_t *>(d) = item->text().toInt();
                    d += 4;
                    break;
                case 5:
                    item = ui->table->item(r, c);
                    *reinterpret_cast<float *>(d) = item->text().toFloat();
                    d += 4;
                    break;
                case 6:
                    item = ui->table->item(r, c);
                    *reinterpret_cast<double *>(d) = item->text().toDouble();
                    d += 8;
                    break;
                }
            }
        }
    }
}


void TxDataFrame::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        on_cancelButton_clicked();
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        on_doneButton_clicked();
    } else {
        event->ignore();
    }
}
