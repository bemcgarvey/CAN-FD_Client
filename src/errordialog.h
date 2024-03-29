#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorDialog;
}

class ErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorDialog(QWidget *parent = 0);
    ~ErrorDialog();

public slots:
    void UpdateErrorStatus(unsigned char flags, unsigned char systemFlags);
signals:
    void ClearErrors();
private slots:
    void on_pushButton_clicked();

private:
    Ui::ErrorDialog *ui;
};

#endif // ERRORDIALOG_H
