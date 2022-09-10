#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QLibraryInfo>


#include "LogHelper/log_helper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // установка обработчика записей в программный лог
    LogHelper::setDebugMessageHandler ();

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qApp->installTranslator(&qtTranslator);
    }

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qApp->installTranslator(&qtBaseTranslator);
    }

    QFile file(":/styles/green.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);
    file.close();

    a.setApplicationName("Liga");

    QCommandLineParser parser;
    QCommandLineOption restore("restore", "restore unfinished experiments");
    parser.addOption(restore);
    parser.process(a.arguments());

    MainWindow w;
//    w.showMaximized();
    w.show();
    return a.exec();
}
