#ifndef TXDATAFRAME_H
#define TXDATAFRAME_H

#include <QFrame>
#include <txtreeitem.h>
#include <QVariant>
#include <QStringList>

namespace Ui {
class TxDataFrame;
}

class TxDataFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TxDataFrame(QWidget *parent = 0);
    ~TxDataFrame();
    void Setup(const TxTreeItem *item);
    const uint8_t* Data() const {return data;}
    int GetDLC() const;
    TxTreeItem::DataDisplayType GetDataDisplayType();
signals:
    void DataUpdated();
private slots:
    void on_doneButton_clicked();
    void on_dataTypeComboBox_currentIndexChanged(int index);
    void on_cancelButton_clicked();
    void on_text_textChanged();
    void on_dlcComboBox_currentIndexChanged(int index);

private:
    Ui::TxDataFrame *ui;
    const TxTreeItem *item;
    uint8_t data[64];
    void setupEdits(int type);
    void pullFromEdits(int type, int dataLen);
    int currentType;
    int previousSize;
    QStringList header;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // TXDATAFRAME_H
