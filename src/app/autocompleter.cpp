#include "autocompleter.h"

#include <autocompletermodel.h>

Autocompleter::Autocompleter(DocumentLabelIndex* labelIndex, QObject* parent) :
    QCompleter(parent),
    model(new AutocompleterModel(labelIndex, this))
{
    setModel(model);
}

Autocompleter::~Autocompleter()
{
    delete model;
}
