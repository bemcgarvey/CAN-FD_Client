#ifndef TXDATADELEGATE_H
#define TXDATADELEGATE_H

#include <QStyledItemDelegate>
#include "txdataframe.h"
#include <QAbstractItemModel>
#include <QMenu>

class TxDataDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TxDataDelegate(QWidget *parent = nullptr);
    // QAbstractItemDelegate interface
public:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private slots:
    void onFrameClosed();

private:
    TxTreeItem *currentItem;
    TxDataFrame *frame;
    QAbstractItemModel *model;
    QMenu *menu;
};

#endif // TXDATADELEGATE_H
