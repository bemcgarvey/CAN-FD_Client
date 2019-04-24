#include "filtervalidator.h"
#include <stdint.h>
#include <QDebug>

FilterValidator::FilterValidator(QObject *parent) : QValidator(parent)
{

}


QValidator::State FilterValidator::validate(QString &str, int &) const
{
    bool ok;
    if (str.length() == 0) {
        return Intermediate;
    }
    if (str == "0x" || str == "0X") {
        return Intermediate;
    }
    uint32_t v = str.toUInt(&ok, 16);
    if (!ok) {
        return Invalid;
    }
    if (v <= 0x1fffffff) {
        return Acceptable;
    } else {
        return Intermediate;
    }
}

void FilterValidator::fixup(QString &str) const
{
    bool ok;
    uint32_t v = str.toUInt(&ok, 16);
    v &= 0x1fffffff;
    qDebug() << v;
    if (!ok) {
        str = "0x0";
    } else {
        str = "0x" + QString().number(v, 16);
    }

}
