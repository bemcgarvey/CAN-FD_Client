#include "txtreemodel.h"
#include "txtreeitem.h"
#include "txmessage.h"
#include <QDebug>
#include <QColor>
#include <QBrush>

TxTreeModel::TxTreeModel(QObject *parent) : QAbstractItemModel (parent)
{
    rootItem = new TxTreeItem(TxTreeItem::ROOT, TxMessage());
}

TxTreeModel::~TxTreeModel()
{
    delete rootItem;
}

QModelIndex TxTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TxTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TxTreeItem*>(parent.internalPointer());

    TxTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TxTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    TxTreeItem *childItem = static_cast<TxTreeItem*>(index.internalPointer());
    TxTreeItem *parentItem = childItem->ParentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TxTreeModel::rowCount(const QModelIndex &parent) const {
    TxTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TxTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TxTreeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 8;
}

QVariant TxTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();
    TxTreeItem *item = static_cast<TxTreeItem*>(index.internalPointer());
    if (item->ItemType() == TxTreeItem::MSG && role == Qt::CheckStateRole) {
        switch (index.column()) {
        case 2:
        case 3:
        case 4:
        case 5:
            return item->CheckState(index.column());
        default:
            return QVariant();
        }
    }
    if (role == Qt::ForegroundRole && item->ItemType() == TxTreeItem::MSG && index.column() == 1) {
        if (item->CheckState(2).toBool()) {
            return QBrush(Qt::red);
        } else {
            return QVariant();
        }
    }
    if (role == Qt::BackgroundRole && item->ItemType() == TxTreeItem::MSG && index.column() == 1) {
        if (item->CheckState(4).toBool()) {
            return QBrush(Qt::yellow);
        } else {
            return QVariant();
        }
    }
    if (role != Qt::DisplayRole)
        return QVariant();
    return item->data(index.column());
}

Qt::ItemFlags TxTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return nullptr;
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    TxTreeItem *item = static_cast<TxTreeItem*>(index.internalPointer());
    if (item->ItemType() == TxTreeItem::MSG) {
        if (index.column() > 0) {
            f |= Qt::ItemIsEditable;
        }
        if (index.column() >= 2 && index.column() <= 5) {
            f |= Qt::ItemIsUserCheckable;
            f &= ~Qt::ItemIsSelectable;
        }
    } else if (item->ItemType() == TxTreeItem::GROUP) {
        if (index.column() == 3 || index.column() == 5) {
            f |= Qt::ItemIsEditable;
        }
    }
    return f;
}

QVariant TxTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::TextAlignmentRole) {
        if (section == 7) {
            return Qt::AlignLeft;
        } else {
            return Qt::AlignCenter;
        }
    }
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch(section) {
        case 1: return "Id";
        case 2: return "IDE";
        case 3: return "RTR";
        case 4: return "FDF";
        case 5: return "BRS";
        case 6: return "DLC";
        case 7: return "Payload";
        }
    return QVariant();
}

void TxTreeModel::AddGroup(TxTreeItem *item) {
    beginInsertRows(QModelIndex(), rootItem->childCount(), rootItem->childCount());
    if (item == nullptr) {
        item = new TxTreeItem(TxTreeItem::GROUP, TxMessage(), rootItem);
    } else {
        item->SetParentItem(rootItem);
    }
    rootItem->appendChild(item);
    endInsertRows();
}


void TxTreeModel::AddMessage(int group, TxTreeItem *item) {
    if (group < 0) {
        return;
    }
    TxTreeItem *parent = rootItem->child(group);
    beginInsertRows(createIndex(parent->row(), 0, parent), parent->childCount(), parent->childCount());
    if (item == nullptr) {
        item = new TxTreeItem(TxTreeItem::MSG, TxMessage(), parent);
        item->SetIdType(parent->GetIdType());
    } else {
        item->SetParentItem(parent);
    }
    parent->appendChild(item);
    endInsertRows();
}


bool TxTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result;
    if (role == Qt::EditRole || role == Qt::CheckStateRole) {
        TxTreeItem *item = static_cast<TxTreeItem *>(index.internalPointer());
        if (item->ItemType() == TxTreeItem::GROUP) {
            if (index.column() == 3) {
               item->SetRepeat(value.toInt());
            } else if (index.column() == 5) {
                item->SetInterval(value.toInt());
            }
            return true;
        }
        result = item->SetData(index.column(), value);
        if (index.column() == 4) {
            emit dataChanged(createIndex(index.row(), 3, index.internalPointer()), createIndex(index.row(), 3, index.internalPointer()));
            emit dataChanged(createIndex(index.row(), 5, index.internalPointer()), createIndex(index.row(), 5, index.internalPointer()));
            emit dataChanged(createIndex(index.row(), 1, index.internalPointer()), createIndex(index.row(), 1, index.internalPointer()));
        }
        if (index.column() == 2) {
            emit dataChanged(createIndex(index.row(), 1, index.internalPointer()), createIndex(index.row(), 1, index.internalPointer()));
        }
        return result;
    } else {
        return false;
    }
}

void TxTreeModel::DeleteRow(QModelIndex index) {
    TxTreeItem *t = static_cast<TxTreeItem *>(index.internalPointer());
    TxTreeItem *p = t->ParentItem();
    if (t->ItemType() == TxTreeItem::MSG && p->childCount() == 1) {
        DeleteRow(parent(index));
        return;
    }
    if (t->row() >= 0) {
        beginRemoveRows(parent(index), t->row(), t->row());
        t->ParentItem()->RemoveChild(t->row());
        endRemoveRows();
    }
}

void TxTreeModel::ToDataStream(QDataStream &out)
{
    out << static_cast<qint32>(rootItem->childCount());
    for (int i = 0; i < rootItem->childCount(); ++i) {
        rootItem->child(i)->ToDataStream(out);
    }
}

void TxTreeModel::FromDataStream(QDataStream &in)
{
    int groups;
    in >> groups;
    for (int i = 0; i < groups; ++i) {
        TxTreeItem *item = new TxTreeItem(TxTreeItem::GROUP, TxMessage(), rootItem);
        int msgCount = item->FromDataStream(in);
        AddGroup(item);
        for (int j = 0; j < msgCount; ++j) {
            item = new TxTreeItem(TxTreeItem::MSG, TxMessage());
            item->FromDataStream(in);
            AddMessage(rowCount() - 1, item);
        }
    }
}

