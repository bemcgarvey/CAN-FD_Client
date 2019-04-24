#include "rxiddelegate.h"
#include "rxtablemodel.h"
#include <QMouseEvent>

RxIdDelegate::RxIdDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    menu = new QMenu();
    menu->addAction("Hex");
    menu->addAction("Decimal");
    menu->actions().at(0)->setCheckable(true);
    menu->actions().at(1)->setCheckable(true);
}

RxIdDelegate::~RxIdDelegate() {
    delete menu;
}

bool RxIdDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if (me->button() == Qt::RightButton) {
            RxTableModel *mod = dynamic_cast<RxTableModel *>(model);
            if (mod->GetIdRadix(index.row()) == RxTableModel::HEX) {
                menu->actions().at(0)->setChecked(true);
                menu->actions().at(1)->setChecked(false);
            } else {
                menu->actions().at(1)->setChecked(true);
                menu->actions().at(0)->setChecked(false);
            }
            QAction *result = menu->exec(me->globalPos());
            if (result != nullptr) {
                if (result->text() == "Hex") {
                    mod->SetIdRadix(index.row(), RxTableModel::HEX);
                }
                if (result->text() == "Decimal") {
                    mod->SetIdRadix(index.row(), RxTableModel::DECIMAL);
                }
                emit model->dataChanged(index, index);
            }
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
