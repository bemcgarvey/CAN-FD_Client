#include "txdatadelegate.h"
#include "txdataframe.h"
#include <QDebug>
#include <QMouseEvent>
#include "txtreeitem.h"

TxDataDelegate::TxDataDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    frame = new TxDataFrame(parent);
    frame->setVisible(false);
    connect(frame, &TxDataFrame::DataUpdated, this, &TxDataDelegate::onFrameClosed);
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

bool TxDataDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(option)
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = dynamic_cast<QMouseEvent *>(event);
        if (me->button() == Qt::LeftButton) {
            this->model = model;
            currentItem = static_cast<TxTreeItem *>(index.internalPointer());
            if (currentItem->ItemType() == TxTreeItem::GROUP) {
                return true;
            }
            QPoint pos = me->globalPos();
            QWidget *parent = dynamic_cast<QWidget *>(this->parent());
            pos = parent->mapFromGlobal(pos);
            if (pos.ry() + frame->height() > parent->height()) {
                pos.ry() -= (pos.ry() + frame->height()) - parent->height();
            }
            if (pos.rx() + frame->width() > parent->width()) {
                pos.rx() -= (pos.rx() + frame->width()) - parent->width();
            }
            frame->move(pos);
            frame->Setup(currentItem);
            frame->show();
        } else if (me->button() == Qt::RightButton) {
            currentItem = static_cast<TxTreeItem *>(index.internalPointer());
            if (currentItem->ItemType() != TxTreeItem::GROUP) {
                return true;
            }
            for (int i = 0; i < 8; ++i) {
                menu->actions().at(i)->setChecked(false);
            }
            menu->actions().at(currentItem->GetDataDisplayType())->setChecked(true);
            QAction *result = menu->exec(me->globalPos());
            TxTreeItem::DataDisplayType type;
            if (result != nullptr) {
                if (result->text() == "bytes") {
                    type = TxTreeItem::BYTES;
                } else if (result->text() == "uint16") {
                    type = TxTreeItem::UINT16;
                } else if (result->text() == "int16") {
                    type = TxTreeItem::INT16;
                } else if (result->text() == "uint32") {
                    type = TxTreeItem::UINT32;
                } else if (result->text() == "int32") {
                    type = TxTreeItem::INT32;
                } else if (result->text() == "string") {
                    type = TxTreeItem::STRING;
                } else if (result->text() == "float") {
                    type = TxTreeItem::FLOAT;
                } else if (result->text() == "double") {
                    type = TxTreeItem::DOUBLE;
                }
                currentItem->SetDataDisplayType(type);
                for (int i = 0; i < currentItem->childCount(); ++i) {
                    currentItem->child(i)->SetDataDisplayType(type);
                }
                emit model->dataChanged(index, index);
            }
        }
        return true;
    } else {
        return false;
    }
}

void TxDataDelegate::onFrameClosed() {
    currentItem->SetMessageData(frame->Data());
    currentItem->SetData(6, frame->GetDLC());
    currentItem->SetDataDisplayType(frame->GetDataDisplayType());
    emit model->dataChanged(model->index(currentItem->row(), 6), model->index(currentItem->row(), 7));
}

