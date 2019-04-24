#ifndef RXDATACOLUMNDELEGATE_H
#define RXDATACOLUMNDELEGATE_H

#include <QStyledItemDelegate>
#include <QMenu>

class RxDataColumnDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    RxDataColumnDelegate(QObject *parent = nullptr);
    ~RxDataColumnDelegate();
protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
private:
    QMenu *menu;
};

#endif // RXDATACOLUMNDELEGATE_H
