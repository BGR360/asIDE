#ifndef AUTOCOMPLETERMODEL_H
#define AUTOCOMPLETERMODEL_H

#include <QStringListModel>

class DocumentLabelIndex;

class AutocompleterModel : public QStringListModel
{
    Q_OBJECT
public:
    explicit AutocompleterModel(DocumentLabelIndex* labelIndex, QObject* parent = 0);

    void setLabelIndex(DocumentLabelIndex* labelIndex);
    DocumentLabelIndex* labelIndex() const;

private slots:
    void updateStringList();
    void onLabelAdded(const QString& label);
    void onLabelRemoved(const QString& label);

private:
    DocumentLabelIndex* mLabelIndex;
    QStringList mInstructions;
    QStringList mLabels;
};

#endif // AUTOCOMPLETERMODEL_H
