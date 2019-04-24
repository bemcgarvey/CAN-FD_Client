#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <stdint.h>
#include <QVector>
#include "canfilter.h"

namespace Ui {
class FilterDialog;
}

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(const QVector<CANFilter> &list, QWidget *parent = 0);
    ~FilterDialog();
    enum ResultType {SET, CLEAR_ALL};
    QVector<CANFilter> getFilterList() const;
    ResultType getResult() const {return result;}
private slots:
    void on_removeAllButton_clicked();
    void on_addButton_clicked();
    void updateDisplay();
    void on_filterEnabledCheckBox_clicked(bool checked);
    void on_MIDEcheckBox_clicked(bool checked);
    void on_EXIDEcheckBox_clicked(bool checked);
    void on_filterEdit_editingFinished();
    void on_maskEdit_editingFinished();
private:
    Ui::FilterDialog *ui;
    ResultType result;
    QVector<CANFilter> filterList;
};

#endif // FILTERDIALOG_H
