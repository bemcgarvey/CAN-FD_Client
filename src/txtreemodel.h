#ifndef TXTREEMODEL_H
#define TXTREEMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include "txtreeitem.h"
#include <QDataStream>

class TxTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    TxTreeModel(QObject *parent = 0);
    ~TxTreeModel();
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void AddGroup(TxTreeItem *item = nullptr);
    void AddMessage(int group, TxTreeItem *item = nullptr);
    void DeleteRow(QModelIndex index);
    void ToDataStream(QDataStream &out);
    void FromDataStream(QDataStream &in);
private:
    TxTreeItem *rootItem;

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};

#endif // TREEMODEL_H
