#include "rxdatacolumndelegate.h"
#include <QMouseEvent>
#include "rxtablemodel.h"

RxDataColumnDelegate::RxDataColumnDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    menu = new QMenu();
    menu->addAction("bytes");
    menu->addAction("uint16");
    menu->addAction("int16");
    menu->addAction("uint32");
    menu->addAction("int32");
    menu->addAction("float");
    menu->addAction("double");
    menu->addAction("string");
    for (int i = 0; i < 8; ++i) {
        menu->actions().at(i)->setCheckable(true);
    }
}


RxDataColumnDelegate::~RxDataColumnDelegate() {
    delete menu;
}


bool RxDataColumnDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if (me->button() == Qt::RightButton) {
            RxTableModel *mod = dynamic_cast<RxTableModel *>(model);
            for (int i = 0; i < 8; ++i) {
                menu->actions().at(i)->setChecked(false);
            }
            menu->actions().at(mod->GetDataType(index.row()))->setChecked(true);
            QAction *result = menu->exec(me->globalPosition().toPoint());
            if (result != nullptr) {
                if (result->text() == "bytes") {
                    mod->SetDataType(index.row(), RxTableModel::BYTE);
                } else if (result->text() == "uint16") {
                    mod->SetDataType(index.row(), RxTableModel::UINT16);
                } else if (result->text() == "int16") {
                    mod->SetDataType(index.row(), RxTableModel::INT16);
                } else if (result->text() == "uint32") {
                    mod->SetDataType(index.row(), RxTableModel::UINT32);
                } else if (result->text() == "int32") {
                    mod->SetDataType(index.row(), RxTableModel::INT32);
                } else if (result->text() == "string") {
                    mod->SetDataType(index.row(), RxTableModel::STRING);
                } else if (result->text() == "float") {
                    mod->SetDataType(index.row(), RxTableModel::FLOAT);
                } else if (result->text() == "double") {
                    mod->SetDataType(index.row(), RxTableModel::DOUBLE);
                }
                emit model->dataChanged(index, index);
            }
            return true;
        } else if (me->button() == Qt::LeftButton) {
            //Show label with values
            return QStyledItemDelegate::editorEvent(event, model, option, index); //return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
