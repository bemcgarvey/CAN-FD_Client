#include "dblclicklabel.h"

DblClickLabel::DblClickLabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
}


void DblClickLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit DoubleClicked();
}
