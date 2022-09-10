/**
 *
 * Пример подсистемы логирования в Qt с использованием qInstallMessageHandler()
 * Время создания: 09.10.2021 12:44
 * Автор: xintrea
 * Текстовые метки: c++, Qt, Qt5, логирование, подсистема, qDebug, qWarning, отладка, консоль, файл, лог
 * Раздел: Компьютер - Программирование - Язык C++ (Си++) - Библиотека Qt - Принципы написания кода
 * Запись: xintrea/mytetra_syncro/master/base/1633772699ygnj6dzmz2/text.html на raw.github.com
 *
 * Файл взят с: https://webhamster.ru/mytetrashare/index/mtb0/1633772699va8ijtqw4e
 * и немного доработан.
 *
 * Ниже приведен код класса, который можно использовать в качестве подсистемы логирования
 * в приложении на Qt. Данный класс перехватывает все отладочные сообщения, генерируемые
 * через qDebug(), qWarning() и прочие функции потокового отладочного вывода, печатает
 * их в консоль и записывает в файл лога.
 *
 * Заголовок
 *
 * !!!ATT!!! Вся настройка подсистемы логирования производится через заголовочный файл
 *           log_helper_cfg.h
**/

#ifndef LOGHELPER_H
#define LOGHELPER_H

#include <QDebug>

class LogHelper
{
public:

    static void setDebugMessageHandler();
    static void myMessageOutput(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msgText);

protected:

    static void smartPrintDebugMessage(QString msg);
    static void printToLogFile(const QString &text);
    static void clearLogFile();
    static QString makeNameForNewFile ();
    static void makeNewFile ();
    static void removeNotUsedFiles ();

    static QString m_logFileName;

    static long int m_recCountInCurFile;
};


#endif // LOGHELPER_H


