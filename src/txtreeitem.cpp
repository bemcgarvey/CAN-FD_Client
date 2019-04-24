#include "txtreeitem.h"
#include <QtDebug>

TxTreeItem::TxTreeItem(Type type, TxMessage msg, TxTreeItem *parentItem) : QObject()
{
    idDisplay = HEX;
    dataDisplay = BYTES;
    this->msg = msg;
    this->parentItem = parentItem;
    this->type = type;
    interval = 0;
    repeat = 0;
    sendButton = nullptr;
    timer = nullptr;
}

TxTreeItem::~TxTreeItem() {
    qDeleteAll(children);
}

void TxTreeItem::appendChild(TxTreeItem *child) {
    children.append(child);
}

TxTreeItem* TxTreeItem::child(int row) {
    return children[row];
}

int TxTreeItem::childCount() const {
    return children.length();
}

int TxTreeItem::columnCount() const {
    return 8;
}

QVariant TxTreeItem::data(int column) const {
    if (type == GROUP) {
        switch (column) {
        case 0: return row() + 1;
        case 2: return "Rpt:";
        case 3: return repeat;
        case 4: return "Int:";
        case 5: return interval;
        default: return QVariant();
        }
    } else if (type == MSG) {
        switch (column) {
        case 0: return QVariant();
        case 1:
            if (idDisplay == DECIMAL)
                if (!msg.ide) {
                    return msg.id & 0x7ff;
                } else {
                    return msg.id;
                }
            else {
                if (!msg.ide) {
                    return QString("0x%1").arg(static_cast<ulong>(msg.id & 0x7ff), 0, 16);
                } else {
                    return QString("0x%1").arg(static_cast<ulong>(msg.id), 0, 16);
                }
            }
        case 2:
        case 3:
        case 4:
        case 5:
            return QVariant();
        case 6:
            if (msg.dlc <= 8) {
                return msg.dlc;
            } else {
                return TxMessage::dlcToLen(msg.dlc);
            }
        case 7:
            return dataString();
        }
    }
    return QVariant();
}

int TxTreeItem::row() const {
    if (parentItem)
        return parentItem->children.indexOf(const_cast<TxTreeItem*>(this));
    return 0;
}

TxTreeItem* TxTreeItem::ParentItem() {
    return parentItem;
}

TxTreeItem::Type TxTreeItem::ItemType() {
    return type;
}

bool TxTreeItem::SetData(int column, const QVariant &data) {
    switch (column) {
    case 1:
        if (idDisplay == DECIMAL) {
            msg.id = data.toUInt();
        } else {
            QString s = data.toString();
            bool ok;
            msg.id = s.toUInt(&ok, 16);
        }
        break;
    case 2:
        msg.ide = data.toBool();
        break;
    case 3:
        msg.rtr = data.toBool();
        break;
    case 4:
        msg.fdf = data.toBool();
        break;
    case 5:
        msg.brs = data.toBool();
        break;
    case 6:
        msg.dlc = data.toInt();
        break;
    default: return false;
    }
    return true;
}

QVariant TxTreeItem::CheckState(int column) {
    switch (column) {
    case 2: return msg.ide ? Qt::Checked : Qt::Unchecked;
    case 3:
        if (!msg.fdf) {
            return msg.rtr ? Qt::Checked : Qt::Unchecked;
        } else {
            return QVariant();
        }
    case 4: return msg.fdf ? Qt::Checked : Qt::Unchecked;
    case 5:
        if (msg.fdf) {
            return msg.brs ? Qt::Checked : Qt::Unchecked;
        } else {
            return QVariant();
        }
    default: return QVariant();
    }
}

void TxTreeItem::RemoveChild(int row) {
    if (row >= children.length()) {
        return;
    }
    TxTreeItem *removed;
    removed = children[row];
    children.remove(row);
    delete removed;
}

QString TxTreeItem::dataString() const {
    int dataLen;
    if (msg.dlc <= 8) {
        dataLen = msg.dlc;
    } else {
        dataLen = static_cast<int>(TxMessage::dlcToLen(msg.dlc));
    }
    QString s = "";
    switch (dataDisplay) {
    case BYTES:
        for (int i = 0; i < dataLen; ++i) {
            s += QString("%1 ").arg(msg.data[i], 2, 16, QChar('0'));
        }
        return s;
    case UINT16:
        for (int i = 0; i < dataLen; i += sizeof(uint16_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const uint16_t *>(&msg.data[i]));
        }
        return s;
    case INT16:
        for (int i = 0; i < dataLen; i += sizeof(int16_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const int16_t *>(&msg.data[i]));
        }
        return s;
    case UINT32:
        for (int i = 0; i < dataLen; i += sizeof(uint32_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const uint32_t *>(&msg.data[i]));
        }
        return s;
    case INT32:
        for (int i = 0; i < dataLen; i += sizeof(int32_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const int32_t *>(&msg.data[i]));
        }
        return s;
    case FLOAT:
        for (int i = 0; i < dataLen; i += sizeof(float)) {
            s += QString("%1 ").arg(static_cast<double>(*reinterpret_cast<const float *>(&msg.data[i])));
        }
        return s;
    case DOUBLE:
        for (int i = 0; i < dataLen; i += sizeof(double)) {
            s += QString("%1 ").arg(*reinterpret_cast<const double *>(&msg.data[i]));
        }
        return s;
    case STRING:
        s = QString::fromUtf8(reinterpret_cast<const char *>(msg.data));
        if (s.length() > dataLen) {
            s.truncate(dataLen);
        }
        return s;
    }
    return "Unknown";
}

void TxTreeItem::SetMessageData(const uint8_t *data) {
    for (int i = 0; i < 64; ++i) {
        msg.data[i] = data[i];
    }
}

void TxTreeItem::SetButton(QPushButton *button)
{
    sendButton = button;
    connect(button, &QPushButton::clicked, this, &TxTreeItem::sendButtonPushed);
}

void TxTreeItem::ToDataStream(QDataStream &out)
{
    out << static_cast<quint8>(type);
    if (type == GROUP) {
        out << static_cast<qint32>(repeat);
        out << static_cast<qint32>(interval);
        out << static_cast<quint8>(idDisplay);
        out << static_cast<quint8>(dataDisplay);
        out << static_cast<qint32>(childCount());
        for (int i = 0; i < childCount(); ++i) {
            child(i)->ToDataStream(out);
        }
    } else {  //MSG
        out << static_cast<quint8>(idDisplay);
        out << static_cast<quint8>(dataDisplay);
        msg.ToDataStream(out);
    }
}

int TxTreeItem::FromDataStream(QDataStream &in)
{
    quint8 n;
    in >> n;
    type = static_cast<Type>(n);
    if (type == GROUP) {
        in >> repeat;
        in >> interval;
        in >> n;
        idDisplay = static_cast<IdDisplayType>(n);
        in >> n;
        dataDisplay = static_cast<DataDisplayType>(n);
        int count;
        in >> count;
        return count;
    } else {  //MSG
        in >> n;
        idDisplay = static_cast<IdDisplayType>(n);
        in >> n;
        dataDisplay = static_cast<DataDisplayType>(n);
        msg.FromDataStream(in);
        return 0;
    }
}

void TxTreeItem::timeout() {
    emit SendMe(this);
    if (sendCount > 0) {
        --sendCount;
        if (sendCount == 0) {
            timer->stop();
            sendButton->setText("Send");
        }
    }
}

void TxTreeItem::sendButtonPushed() {
    if (timer != nullptr && timer->isActive()) {
        timer->stop();
        sendButton->setText("Send");
        return;
    }
    if (interval > 0) {
        if (timer == nullptr) {
            timer = new QTimer(this);
            connect(timer, &QTimer::timeout, this, &TxTreeItem::timeout);
        }
        sendCount = repeat - 1;
        sendButton->setText("Stop");
        timer->start(interval);
        emit SendMe(this);
        return;
    } else {
        emit SendMe(this);
    }
}
