#ifndef TXTREEITEM_H
#define TXTREEITEM_H

#include "txmessage.h"
#include <QVector>
#include <QVariant>
#include <QPushButton>
#include <QTimer>
#include <QDataStream>

class TxTreeItem : public QObject
{
    Q_OBJECT
public:
    enum Type {ROOT = 0, GROUP = 1, MSG = 2};
    enum IdDisplayType {HEX, DECIMAL};
    enum DataDisplayType {BYTES = 0, UINT16, INT16, UINT32, INT32, FLOAT, DOUBLE, STRING};

    explicit TxTreeItem(Type type, TxMessage msg, TxTreeItem *ParentItem = 0);
    ~TxTreeItem();

    void appendChild(TxTreeItem *child);
    TxTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TxTreeItem *ParentItem();
    void SetParentItem(TxTreeItem *parentItem) {this->parentItem = parentItem;}
    Type ItemType();
    IdDisplayType GetIdType() const {return idDisplay;}
    void SetIdType(IdDisplayType type) {idDisplay = type;}
    DataDisplayType GetDataDisplayType() const {return dataDisplay;}
    void SetDataDisplayType(DataDisplayType type) {dataDisplay = type;}
    bool SetData(int column, const QVariant &data);
    QVariant CheckState(int column);
    void RemoveChild(int row);
    const TxMessage* Message() const {return &msg;}
    void SetMessageData(const uint8_t *data);
    TxTreeItem* GetChild(int n) {return children[n];}
    int GetRepeat() const {return repeat;}
    void SetRepeat(int repeat) {this->repeat = repeat;}
    int GetInterval() const {return interval;}
    void SetInterval(int interval) {this->interval = interval;}
    void SetButton(QPushButton *button);
    void ToDataStream(QDataStream &out);
    int FromDataStream(QDataStream &in);
signals:
    void SendMe(TxTreeItem *group);

private slots:
    void timeout();
    void sendButtonPushed();

 private:
    Type type;
    TxTreeItem *parentItem;
    IdDisplayType idDisplay;
    DataDisplayType dataDisplay;
    TxMessage msg;
    QVector<TxTreeItem *> children;
    QString dataString() const;
    int repeat;
    int interval;
    int sendCount;
    QTimer *timer;
    QPushButton *sendButton;
};

#endif // TREEITEM_H
