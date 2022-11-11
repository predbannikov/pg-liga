#ifndef UPLOADER_H
#define UPLOADER_H

#include <QDebug>
#include <QDir>
#include <QJsonObject>
#include <QDataStream>

#include "clientrepositarion.h"


class Uploader : public QObject
{
    Q_OBJECT
    ClientRepositarion *repos;
public:
    Uploader(QObject *parent = nullptr);
    ~Uploader();

public slots:
    void connectedClientRepositarion();

};

#endif // UPLOADER_H
