#include "autocompleter.h"

Autocompleter::Autocompleter(QObject* parent) :
    QCompleter(parent)
{

}

Autocompleter::Autocompleter(QAbstractItemModel* model, QObject* parent) :
    QCompleter(model, parent)
{

}

Autocompleter::Autocompleter(const QStringList& list, QObject* parent) :
    QCompleter(list, parent)
{

}
