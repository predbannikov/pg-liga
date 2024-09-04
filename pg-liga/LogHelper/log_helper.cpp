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
 * Реализация
 *
**/

#include <QDebug>
#include <QObject>
#include <QApplication>
#include <QString>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>

#include <iostream>

#include "log_helper.h"
#include "log_helper_cfg.h"

QString  LogHelper::m_logFileName;
long int LogHelper::m_recCountInCurFile = 0;

static QDateTime getDateTimeFromFileName (QString a_fileName);

void LogHelper::clearLogFile()
{
    // Создание пустого файла лога
    FILE *pFile=fopen(qPrintable(m_logFileName), "w+");
    if(!pFile)
    {
        fclose(pFile);
    }
    m_recCountInCurFile = 0;
}

void LogHelper::printToLogFile(const QString &text)
{
    // Лог-файл открывается в режиме добавления данных
    FILE *pFile=fopen(qPrintable(m_logFileName), "a+");

    if(!pFile)
    {
        printf("Log %s file not writable\n", qPrintable(m_logFileName));
        return;
    }

    // Быстрый вывод текста в файл лога
    fprintf(pFile, "%s", qPrintable(text));

    // Лог-файл закрывается
    fclose(pFile);

    // Учёт произведённого добавления запси
    m_recCountInCurFile ++ ;

    static bool recurce_defence = false;
    if (recurce_defence) return;

    recurce_defence = true ;

    // если текущий файл лог-а уже переполнен, то создаём новый и
    // проверяем, не нужно ли удалять какие-то из старых файлов
    if (m_recCountInCurFile > MAX_RECORDS_IN_ONE_FILE)
    {
        // создание нового файла
        makeNewFile ();

        // удаление (если необходимо) ненужных файлов
        removeNotUsedFiles ();
    }

    recurce_defence = false;
}


void LogHelper::smartPrintDebugMessage(QString msg)
{
    // Подготовка текста для вывода
    QDate currDate = QDate::currentDate();
    QString dateText = currDate.toString("yyyy_MM_dd");
    QTime currTime = QTime::currentTime();
    QString timeText = currTime.toString("hh:mm:ss.zzz");
    msg = dateText + "-" + timeText + " " + msg;

    // Быстрый вывод в консоль
    fprintf(stderr, "%s", qPrintable(msg));

    // Вывод в файл лога
    printToLogFile(msg);
}



// Обработчик (хендлер) вызовов qDebug()
// Внутри этого обработчика нельзя использовать вызовы qDebug(), т. к. получится рекурсия
void LogHelper::myMessageOutput(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msgText)
{
    Q_UNUSED(context)

    switch (type) {

     case QtDebugMsg:
         // Отладочный вывод возможен только при компиляции в режиме отладки
//       #ifdef QT_DEBUG
         smartPrintDebugMessage("[DBG] "+msgText+"\n");
//       #endif
         break;

     case QtWarningMsg:
         smartPrintDebugMessage("[WRN] "+msgText+"\n");
//       QMessageBox::warning(nullptr, "Application warning message",
//                            msgText,
//                            QMessageBox::Ok);
         break;

    case QtCriticalMsg:
         smartPrintDebugMessage("[CRERR] "+msgText+"\n");
//       QMessageBox::warning(nullptr, "Application critical message",
//                            msgText,
//                            QMessageBox::Ok);
         break;

     case QtFatalMsg:
         smartPrintDebugMessage("[FTERR] "+msgText+"\n");
//       abort();
         break;

     case QtInfoMsg:
         smartPrintDebugMessage("[INF] "+msgText+"\n");
         break;
    }
}


void LogHelper::setDebugMessageHandler()
{
    // Создаём новый лог-файл
    makeNewFile ();

    // Устанавливается Qt-обработчик консольного вывода
    qInstallMessageHandler(myMessageOutput);
}

QString LogHelper :: makeNameForNewFile ()
{
    // Формируется имя файла лога (без указания пути)

    QDate currDate = QDate::currentDate();
    QString dateText = currDate.toString(FILENAME_DATE_FORMAT);
    QTime currTime = QTime::currentTime();
    QString timeText = currTime.toString(FILENAME_TIME_FORMAT);

    QString fileName = QString(FILENAME_PREFIX) + "_" + dateText + "_" + timeText + "." + FILENAME_EXTENSION;
    fileName.replace(":", "-");    // знаки ":", которые могут разделять часы/минуты/секунды не допускаются в имени файла

    return fileName;
}

void LogHelper :: makeNewFile ()
{
    QString log_dir = qApp->applicationDirPath() + "/" + PRGLOG_FOLDER;

    // Проверяется наличие директории лога программы
    bool dir_exist = QDir(log_dir).exists();

    // Если директория ещё не создана, она создаётся
    if (!dir_exist)
    {
        QDir().mkdir(log_dir);
    }

    // Задается статическое значение имени файла лога
    m_logFileName = log_dir + "/" + makeNameForNewFile ();
    printf("\nSet log file to %s\n", qPrintable(m_logFileName));

    // Лог-файл очищается
    clearLogFile();
}

QDateTime getDateTimeFromFileName (QString a_fileName)
{
    QString   format = QString (FILENAME_DATE_FORMAT) + "_" + QString (FILENAME_TIME_FORMAT);
    QFileInfo fileInfo (a_fileName);
    QString   dateTimeString = fileInfo.fileName();
    int prefix_size = QString(FILENAME_PREFIX).size ();
    dateTimeString.remove (0, prefix_size + 1);    // обрезка начала имени - префикса и знака "_"
//  int extension_size = QString(FILENAME_EXTENSION).size ();
//  dateTimeString.truncate (dateTimeString.size() - extension_size - 1);    // обрезка расширения и знака "."
    dateTimeString.truncate (format.size());    // обрезка по длине формата даты/времени

    format        .replace("-", ":");
    dateTimeString.replace("-", ":");

    QDateTime dateTime;
    dateTime = QDateTime::fromString (dateTimeString, format);

    return dateTime;
}

typedef struct
{
        qint64  delta_t ;
        QString fileName;

} exist_log_file;

static int log_file_compare (const void *arg1, const void *arg2)
{
   if (((exist_log_file *)arg1)->delta_t < ((exist_log_file *)arg2)->delta_t) return -1;
   if (((exist_log_file *)arg1)->delta_t > ((exist_log_file *)arg2)->delta_t) return  1;
   else                                                                       return  0;
}

void LogHelper :: removeNotUsedFiles ()
{
    // чтобы определять, какие файлы являются ненужными, требуется
    // знать сколько времени прошло с момента их создания до
    // момента создания текущего активного log-файла

    // время создания текущего активного log-файла
    QDateTime dt_current_file = getDateTimeFromFileName (m_logFileName);

    // создаём список всех имеющихся в директоии log-файлов
    // с указанием "давности" - разницы между временем их создания и
    // временем создания текущего активного log-файла
/*
    class exist_log_file // класс - элемент списка описателей
    {
      public:
        exist_log_file (qint64 a_delta_t, QString a_fileName) {delta_t = a_delta_t; fileName = a_fileName;}
        qint64  getDelta_t  () {return delta_t ;}
        QString getFileName () {return fileName;}
        bool operator <(const exist_log_file & f2) const
        {
            return delta_t < f2.delta_t;
        }
      private:
        qint64  delta_t ;
        QString fileName;
    };

    QList <exist_log_file> exist_log_file_list; // список описателей имеющихся log-файлов
    exist_log_file_list.clear ();
*/
    QList <exist_log_file> exist_log_file_list; // список описателей имеющихся log-файлов

    QString log_dir = qApp->applicationDirPath() + "/" + PRGLOG_FOLDER;
    QDir dir (log_dir);  //объявляем объект работы с папками
    dir.setFilter  (QDir::Files | QDir::Hidden | QDir::NoSymLinks);   // устанавливаем фильтр выводимых файлов/папок (см ниже)
    dir.setSorting (QDir::Name  | QDir::Reversed);   // устанавливаем сортировку "от меньшего к большему"
    QFileInfoList all_files_in_dir_list = dir.entryInfoList ();       // получаем список файлов директории
//    std::cout << "     Bytes Filename" << std::endl;   //выводим заголовок

    for (int i = 0; i < all_files_in_dir_list.size(); ++i)    // цикл по всем файлам, находящимся в директории лог-а
    {
        QFileInfo fileInfo = all_files_in_dir_list.at(i);
        QDateTime dt = getDateTimeFromFileName (fileInfo.fileName());    // время создания рассматриваемого файла
        if (dt.isValid())
        {
            // в цикле выводим сведения о файлах
//            std::cout << dt.toString("dd/MM/yyyy hh:mm:ss").toStdString () << std::endl;
            // находим "давность" - разницу времени создания этого файла и времени создания текущего активного файла
            qint64 delta_t = dt.secsTo (dt_current_file);
            // заносим описание log-файла в список
            exist_log_file f;
            f.delta_t = delta_t;
            f.fileName = fileInfo.fileName();
            exist_log_file_list.append (f);
        }
    }

    // Список имеющихся log-файлов сортируется по "давности"
//  qSort (exist_log_file_list.begin(), exist_log_file_list.end()); заменено на std::qsort, так как qSort, оказывается, deprecated
    {
    int n = exist_log_file_list.size();
    exist_log_file *exist_log_file_array = new exist_log_file [n];
    for (int i=0; i<n; i++) exist_log_file_array[i] = exist_log_file_list.at(i);
    std::qsort (exist_log_file_array, n, sizeof(exist_log_file), log_file_compare);
    exist_log_file_list.clear ();
    for (int i=0; i<n; i++) exist_log_file_list.append (exist_log_file_array[i]);
    delete [] exist_log_file_array;
    }

    // Проход по списку имеющихся файлов и удаление ненужных
    // При этом некоторое минимальное количество наименее "давних" файлов не удаляется,
    // если остаются ещё файлы, то из них удаляются те, чья "давность" выше заданной

    qint64 delta_t_max = (quint64) DELETE_OLD_LOG_AFTER_INTERVAL_DAYS  * 24 * 60 * 60 +    // заданная максимальная "давность"
                         (quint64) DELETE_OLD_LOG_AFTER_INTERVAL_HOURS      * 60 * 60 ;

    for (int i = 0; i < exist_log_file_list.size(); ++i)    // цикл по всем имеющимся log-файлам
    {
        if (i<MIN_NUMBER_OF_NON_REMOVABLE_OLD_FILES) continue;

        exist_log_file f = exist_log_file_list.at(i);

        if (f.delta_t >= delta_t_max)
        {
            std::cout << " * * * * REMOVE  * * * * " << f.fileName.toStdString() << std::endl;
//          bool del_result = QFile::remove (f.fileName);
            dir.remove (f.fileName);
//          std::cout << "remove result of " << f.fileName.toStdString() << " is " << (del_result ? "true " : "false") << std::endl;
        }
    }
}
