#ifndef TXIDDELEGATE_H
#define TXIDDELEGATE_H

#include <QStyledItemDelegate>
#include <QMenu>

class TxIdDelegate : public QStyledItemDelegate
{
public:
    TxIdDelegate(QObject *parent = nullptr);
    ~TxIdDelegate();
    // QAbstractItemDelegate interface
public:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
private:
    QMenu *menu;
};

#endif // TXIDDELEGATE_H
