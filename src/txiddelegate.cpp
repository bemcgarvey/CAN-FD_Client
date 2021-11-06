#include "txiddelegate.h"
#include <QMouseEvent>
#include "txtreemodel.h"

TxIdDelegate::TxIdDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    menu = new QMenu();
    menu->addAction("Hex");
    menu->addAction("Decimal");
    menu->actions().at(0)->setCheckable(true);
    menu->actions().at(1)->setCheckable(true);
}

TxIdDelegate::~TxIdDelegate() {
    delete menu;
}

bool TxIdDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if (me->button() == Qt::RightButton) {
            TxTreeItem *item = static_cast<TxTreeItem *>(index.internalPointer());
            if (item->GetIdType() == TxTreeItem::HEX) {
                menu->actions().at(0)->setChecked(true);
                menu->actions().at(1)->setChecked(false);
            } else {
                menu->actions().at(1)->setChecked(true);
                menu->actions().at(0)->setChecked(false);
            }
            QAction *result = menu->exec(me->globalPosition().toPoint());
            if (result != nullptr) {
                TxTreeItem::IdDisplayType type;
                if (result->text() == "Hex") {
                    type = TxTreeItem::HEX;
                } else {
                    type = TxTreeItem::DECIMAL;
                }
                if (item->ItemType() == TxTreeItem::GROUP) {
                    for (int i = 0; i < item->childCount(); ++i) {
                        item->child(i)->SetIdType(type);
                    }
                }
                item->SetIdType(type);
                emit model->dataChanged(index, index);
            }
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
