#include "rxtablemodel.h"
#include <QColor>
#include <QBrush>
#include <QDebug>

RxTableModel::RxTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    messages.reserve(100);
    dataTypes.reserve(100);
    idDisplay.reserve(100);
    paused = false;
    fixed = false;
}

int RxTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    if (!fixed) {
        return messages.length();
    } else {
        return fixedIds.length();
    }
}

int RxTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 8;
}

QVariant RxTableModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    const RxMessage *msg;
    fixedEntry e;
    if (!fixed) {
        msg = &messages[row];
    } else {
        uint32_t id = fixedIds[row];
        e = fixedList.value(id);
        row = e.rollingRow;
        msg = &messages[row];
    }
    if (role == Qt::ForegroundRole && index.column() == 1) {
        if (msg->ide) {
            return QBrush(Qt::red);
        } else {
            return QVariant();
        }
    }
    if (role == Qt::BackgroundRole && index.column() == 1) {
        if (msg->fdf) {
            return QBrush(Qt::yellow);
        } else {
            return QVariant();
        }
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            if (!fixed)
                return row + 1;
            else
                return e.count;
        case 1:
            if (idDisplay[row] == DECIMAL)
                return msg->id;
            else
                return QString("0x%1").arg(static_cast<ulong>(msg->id), 0, 16);
        case 2:
        case 3:
        case 4:
            return QVariant();
        case 5:
            if (msg->dlc <= 8) {
                return msg->dlc;
            } else {
                return RxMessage::dlcToLen(msg->dlc);
            }
        case 7: return dataString(row, msg);
        case 6: return QString("%1 s").arg((msg->timeStamp - messages[0].timeStamp) / 1000000.0, 0, 'f', 6);
        }
    }
    if (role == Qt::CheckStateRole) {
        switch (index.column()) {
        case 2:
            if (!msg->fdf)
                return msg->rtr ? Qt::Checked : Qt::Unchecked;
            break;
        case 3:
            return msg->fdf ? Qt::Checked : QVariant();
        case 4:
            if (msg->fdf)
                return msg->brs ? Qt::Checked : Qt::Unchecked;
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant RxTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::TextAlignmentRole) {
        if (section == 7) {
            return Qt::AlignLeft;
        } else {
            return QVariant();
        }
    }
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            if (fixed) {
                return "Count";
            } else {
                return QVariant();
            }
        case 1: return "Id";
        case 2: return "RTR";
        case 3: return "FDF";
        case 4: return "BRS";
        case 5: return "DLC";
        case 7: return "Data";
        case 6: return "Time";
        default: return QVariant();
        }
    } else {
        return QVariant();
    }
}

Qt::ItemFlags RxTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (index.column() == 2 || index.column() == 3 || index.column() == 4) {
        return f | Qt::ItemIsUserCheckable;
    }
    return f;
}

void RxTableModel::AddMessage(RxMessage *m) {
    bool willInsertRow = true;
    int fixedRow = 0;
    int oldRollingRow = -1;
    uint32_t id = m->id;
    if (m->ide) {
        id |= 0x80000000;
    }
    if (fixedList.contains(id)) {
        fixedEntry e = fixedList.value(id);
        ++e.count;
        oldRollingRow = e.rollingRow;
        e.rollingRow = messages.length();
        fixedList.insert(id, e);
        fixedRow = e.fixedRow;
        if (fixed) {
            willInsertRow = false;
        }
    } else {
        fixedRow = fixedIds.length();
        fixedIds.append(id);
        fixedEntry e = {1, messages.length(), fixedRow};
        fixedList.insert(id, e);
    }
    if (!paused && willInsertRow) {
        beginInsertRows(QModelIndex(), messages.length(), messages.length());
    }
    messages.append(*m);
    delete m;
    if (!fixed || oldRollingRow < 0) {
        if (!dataTypes.isEmpty()) {
            dataTypes.append(dataTypes.last());
            idDisplay.append(idDisplay.last());
        } else {
            dataTypes.append(BYTE);
            idDisplay.append(HEX);
        }
    } else {
        dataTypes.append(dataTypes[oldRollingRow]);
        idDisplay.append(idDisplay[oldRollingRow]);
    }
    if (!paused && willInsertRow) {
        endInsertRows();
        if (!fixed)
            emit ActiveRow(messages.length() - 1);
    }
    if (!paused && fixed && !willInsertRow) {
        emit dataChanged(createIndex(fixedRow, 0), createIndex(fixedRow, 7));
    }
}

void RxTableModel::SetDataType(int row, DataTypes type) {
    if (fixed) {
        uint32_t id = fixedIds[row];
        fixedEntry e = fixedList.value(id);
        row = e.rollingRow;
    }
    dataTypes[row] = type;
}

void RxTableModel::SetIdRadix(int row, IDRadix radix) {
    if (fixed) {
        uint32_t id = fixedIds[row];
        fixedEntry e = fixedList.value(id);
        row = e.rollingRow;
    }
    idDisplay[row] = radix;
}

void RxTableModel::Clear() {
    beginResetModel();
    messages.clear();
    dataTypes.clear();
    idDisplay.clear();
    fixedList.clear();
    fixedIds.clear();
    endResetModel();
}

QString RxTableModel::dataString(int row, const RxMessage *msg) const {
    int dataLen;
    if (msg->dlc <= 8) {
        dataLen = msg->dlc;
    } else {
        dataLen = static_cast<int>(RxMessage::dlcToLen(msg->dlc));
    }
    QString s = "";
    switch (dataTypes[row]) {
    case BYTE:
        for (int i = 0; i < dataLen; ++i) {
            s += QString("%1 ").arg(msg->data[i], 2, 16, QChar('0'));
        }
        return s;
    case UINT16:
        for (int i = 0; i < dataLen; i += sizeof(uint16_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const uint16_t *>(&msg->data[i]));
        }
        return s;
    case INT16:
        for (int i = 0; i < dataLen; i += sizeof(int16_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const int16_t *>(&msg->data[i]));
        }
        return s;
    case UINT32:
        for (int i = 0; i < dataLen; i += sizeof(uint32_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const uint32_t *>(&msg->data[i]));
        }
        return s;
    case INT32:
        for (int i = 0; i < dataLen; i += sizeof(int32_t)) {
            s += QString("%1 ").arg(*reinterpret_cast<const int32_t *>(&msg->data[i]));
        }
        return s;
    case FLOAT:
        for (int i = 0; i < dataLen; i += sizeof(float)) {
            s += QString("%1 ").arg(static_cast<double>(*reinterpret_cast<const float *>(&msg->data[i])));
        }
        return s;
    case DOUBLE:
        for (int i = 0; i < dataLen; i += sizeof(double)) {
            s += QString("%1 ").arg(*reinterpret_cast<const double *>(&msg->data[i]));
        }
        return s;
    case STRING:
        s = QString::fromUtf8(reinterpret_cast<const char *>(msg->data));
        if (s.length() > dataLen) {
            s.truncate(dataLen);
        }
        return s;
    }
    return "Unknown";
}


void RxTableModel::Pause(bool pause) {
    if (pause && !paused) {
        paused = true;
    } else if (!pause && paused) {
        beginResetModel();
        endResetModel();
        if (!fixed) {
            emit ActiveRow(messages.length() - 1);
        }
        paused = false;
    }
}

void RxTableModel::Fixed(bool f) {
    if (f != fixed) {
        beginResetModel();
        fixed = f;
        endResetModel();
    }
}

RxTableModel::IDRadix RxTableModel::GetIdRadix(int row) {
    if (fixed) {
        uint32_t id = fixedIds[row];
        fixedEntry e = fixedList.value(id);
        row = e.rollingRow;
    }
    return idDisplay[row];
}

RxTableModel::DataTypes RxTableModel::GetDataType(int row) {
    if (fixed) {
        uint32_t id = fixedIds[row];
        fixedEntry e = fixedList.value(id);
        row = e.rollingRow;
    }
    return dataTypes[row];
}

void RxTableModel::ToCsv(QTextStream &out)
{
    out << "Id, IDE, RTR, FDF, BRS, DLC, Time Stamp, Data" << endl;
    for (int i = 0; i < messages.length(); ++i) {
        if (idDisplay[i] == HEX) {
            out << hex << showbase;
        } else {
            out << dec << noshowbase;
        }
        out << messages[i].id <<",";
        out << dec << noshowbase;
        out << (messages[i].ide ? "1" : "0") << ",";
        out << (messages[i].rtr ? "1" : "0") << ",";
        out << (messages[i].fdf ? "1" : "0") << ",";
        out << (messages[i].brs ? "1" : "0") << ",";
        out << messages[i].dlc << ",";
        out << (messages[i].timeStamp - messages[0].timeStamp) / 1000000.0 << ",";
        out << dataString(i, &messages[i]) << endl;
    }
}
