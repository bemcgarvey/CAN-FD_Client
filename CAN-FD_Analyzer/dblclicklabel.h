#ifndef DBLCLICKLABEL_H
#define DBLCLICKLABEL_H

#include <QLabel>

class DblClickLabel : public QLabel
{
    Q_OBJECT
public:
    DblClickLabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    // QWidget interface
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
signals:
    void DoubleClicked(void);
};

#endif // DBLCLICKLABEL_H
