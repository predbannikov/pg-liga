#ifndef ABSTRACTSTEPPEDMODEL_H
#define ABSTRACTSTEPPEDMODEL_H

#include <QVector>
#include <QAbstractTableModel>
#include <QJsonObject>

class AbstractSteppedModel : public QAbstractTableModel
{
public:
    AbstractSteppedModel(QObject *parent = nullptr):
        QAbstractTableModel(parent)
    {}

    /* Data editing API */
    virtual int insertStep(const QModelIndex &index = QModelIndex()) = 0;
    virtual int removeStep(const QModelIndex &index = QModelIndex()) = 0;
    virtual int moveStep(const QModelIndex &index, int moveAmount) = 0;
    virtual int duplicateStep(const QModelIndex &index = QModelIndex()) = 0;

    virtual QJsonObject getJsonModel() = 0;
};

#endif // ABSTRACTSTEPPEDMODEL_H
