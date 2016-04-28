#ifndef AUTOCOMPLETER_H
#define AUTOCOMPLETER_H

#include <QCompleter>

class Autocompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit Autocompleter(QObject* parent = 0);
    explicit Autocompleter(QAbstractItemModel* model, QObject* parent = NULL);
    explicit Autocompleter(const QStringList& list, QObject* parent = NULL);
};

#endif // AUTOCOMPLETER_H
