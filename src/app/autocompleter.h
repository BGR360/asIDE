#ifndef AUTOCOMPLETER_H
#define AUTOCOMPLETER_H

#include <QCompleter>

class DocumentLabelIndex;

class Autocompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit Autocompleter(DocumentLabelIndex* labelIndex, QObject* parent = 0);
    ~Autocompleter();

private:
    QAbstractItemModel* model;
};

#endif // AUTOCOMPLETER_H
