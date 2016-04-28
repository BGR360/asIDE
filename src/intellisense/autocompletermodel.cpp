#include "autocompletermodel.h"

#include <algorithm> //std::sort

#include "documentlabelindex.h"

AutocompleterModel::AutocompleterModel(DocumentLabelIndex* labelIndex, QObject* parent) :
    QStringListModel(parent),
    mLabelIndex(NULL)
{
    mInstructions << "halt" << "add" << "sub"
                  << "mult" << "div" << "cp"
                  << "and" << "or" << "not"
                  << "sl" << "sr" << "cpfa"
                  << "cpta" << "be" << "bne"
                  << "blt" << "call" << "ret"
                  << "#include";
    std::sort(mInstructions.begin(), mInstructions.end());

    setLabelIndex(labelIndex);
}

void AutocompleterModel::setLabelIndex(DocumentLabelIndex* labelIndex)
{
    if (mLabelIndex) {
        disconnect(mLabelIndex, SIGNAL(labelAdded(QString,int)), this, SLOT(onLabelAdded(QString)));
        disconnect(mLabelIndex, SIGNAL(labelRemoved(QString,int)), this, SLOT(onLabelRemoved(QString)));
        delete mLabelIndex;
    }

    mLabelIndex = labelIndex;

    if (mLabelIndex) {
        connect(mLabelIndex, SIGNAL(labelAdded(QString,int)), this, SLOT(onLabelAdded(QString)));
        connect(mLabelIndex, SIGNAL(labelRemoved(QString,int)), this, SLOT(onLabelRemoved(QString)));
        mLabels = mLabelIndex->labels();
        updateStringList();
    }
}

DocumentLabelIndex* AutocompleterModel::labelIndex() const
{
    return mLabelIndex;
}

void AutocompleterModel::updateStringList()
{
    std::sort(mLabels.begin(), mLabels.end());
    QStringList newList(mInstructions);
    newList.append(mLabels);
    setStringList(newList);
}

void AutocompleterModel::onLabelAdded(const QString& label)
{
    if (!mLabels.contains(label))
        mLabels.append(label);
    updateStringList();
}

void AutocompleterModel::onLabelRemoved(const QString& label)
{
    mLabels.removeAll(label);
    updateStringList();
}
