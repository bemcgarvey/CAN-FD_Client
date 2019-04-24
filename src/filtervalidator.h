#ifndef FILTERVALIDATOR_H
#define FILTERVALIDATOR_H

#include <QValidator>

class FilterValidator : public QValidator
{
public:
    FilterValidator(QObject *parent);

    // QValidator interface
public:
    State validate(QString &, int &) const override;
    void fixup(QString &) const override;
};

#endif // FILTERVALIDATOR_H
