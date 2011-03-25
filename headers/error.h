#ifndef ERROR_H
#define ERROR_H

#include <QString>

class Error
{
private:
    QString text;

public:
    Error(const QString &aText);

    QString Text() const;
};

#endif // ERROR_H
