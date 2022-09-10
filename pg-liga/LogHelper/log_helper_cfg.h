/**
 *  Файл настройки программного лога log_helper
 *  Это НЕ интерфейсный файл, его НЕ требуется
 *  включать в пользовательские файлы, где
 *  вызывается log_helper
 **/

// Название директории хранения программного лога
#define PRGLOG_FOLDER        "ProgramLog"

// Настройка имён log-файлов
// имя файла состоит из префикса, даты и времени,
// разделённых знаками подчеркивания, и расширения
#define FILENAME_PREFIX      "log"
#define FILENAME_DATE_FORMAT "yyyy_MM_dd" // "hh:mm:ss.zzz"
#define FILENAME_TIME_FORMAT "hh:mm:ss"
#define FILENAME_EXTENSION   "txt"

// параметры очистки директории от ненужных log-файлов
// файлы будут удаляться если они созданы раньше
// последнего (текущего активного) log-файла на определённый
// интервал времени. Интервал задаётся как число
// дней + число часов.
// Но несколько последних файлов в любом случае сохраняется.
// Это число также задаётся здесь
#define DELETE_OLD_LOG_AFTER_INTERVAL_DAYS   30
#define DELETE_OLD_LOG_AFTER_INTERVAL_HOURS   0
#define MIN_NUMBER_OF_NON_REMOVABLE_OLD_FILES 5

// Ограничения размеров одного log-файла
#define MAX_RECORDS_IN_ONE_FILE 1000

