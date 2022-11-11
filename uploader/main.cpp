#include <QCoreApplication>

#include "uploader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Uploader uploader;

    return a.exec();
}
