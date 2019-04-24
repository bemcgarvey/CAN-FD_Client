#ifndef RXROLLINGTABLEMODEL_H
#define RXROLLINGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QString>
#include <QHash>
#include "rxmessage.h"
#include <QTextStream>

class RxTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RxTableModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    enum DataTypes {BYTE = 0, UINT16, INT16, UINT32, INT32, FLOAT, DOUBLE, STRING};
    enum IDRadix {HEX, DECIMAL};
    IDRadix GetIdRadix(int row);
    DataTypes GetDataType(int row);
    void ToCsv(QTextStream &out);
public slots:
    void AddMessage(RxMessage *m);
    void SetDataType(int row, DataTypes type);
    void SetIdRadix(int row, IDRadix radix);
    void Clear();
    void Pause(bool pause);
    void Fixed(bool f);
signals:
    void ActiveRow(int row);
private:
    class fixedEntry {
    public:
        int count;
        int rollingRow;
        int fixedRow;
    };
    QVector<RxMessage> messages;
    QVector<DataTypes> dataTypes;
    QVector<IDRadix> idDisplay;
    bool paused;
    QString dataString(int row, const RxMessage *msg) const;
    QHash<uint32_t, fixedEntry> fixedList;
    QVector<uint32_t> fixedIds;
    bool fixed;
};

#endif // RXROLLINGTABLEMODEL_H
