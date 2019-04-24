#ifndef RXIDDELEGATE_H
#define RXIDDELEGATE_H

#include <QStyledItemDelegate>
#include <QMenu>

class RxIdDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RxIdDelegate(QObject *parent = nullptr);
    ~RxIdDelegate();
protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
    QMenu *menu;
};

#endif // RXIDDELEGATE_H
