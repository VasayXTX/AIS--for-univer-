#include "../headers/error.h"

Error::Error(const QString &aText):
        text(aText)
{
    //empty
}

QString Error::Text() const
{
    return text;
}
