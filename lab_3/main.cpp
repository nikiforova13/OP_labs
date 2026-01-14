#include <iostream>
#include <fstream>
#include <crtdbg.h>
#include <cerrno>
#include <cstring>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#ifdef _WIN32
#include <windows.h>
#endif

// Установка кодировки для корректного отображения русского текста
void setConsoleEncoding() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 > nul 2>&1");
#endif
}

// Коды результата проверки файла
enum FileCheckResult {
    FILE_CHECK_ERROR,      // Ошибка открытия файла
    FILE_CHECK_EMPTY,      // Файл пустой
    FILE_CHECK_NO_QUOTES,  // Файл не пустой, но нет цитат
    FILE_CHECK_OK          // Файл готов к обработке
};

// Функция проверки входного файла: открывает файл и проверяет его содержимое
// Параметры:
//   inputFileName - имя входного файла
//   inputFile - ссылка на поток, который будет открыт для чтения (если проверка успешна)
// Возвращает: код результата проверки (FileCheckResult)
FileCheckResult checkInputFile(const char* inputFileName, std::ifstream& inputFile) {
    // Проверка входных данных
    if (inputFileName == nullptr) {
        std::cerr << "Ошибка: неверное имя файла!\n";
        return FILE_CHECK_ERROR;
    }
    
    // Открываем входной файл для чтения (в текстовом режиме)
    errno = 0;  // Сбрасываем errno перед операцией
    inputFile.open(inputFileName, std::ios::in);
    if (!inputFile.is_open()) {
        // Определяем конкретную причину ошибки
        if (errno == ENOENT) {
            std::cerr << "Ошибка: входной файл \"" << inputFileName << "\" не существует!\n";
        } else if (errno == EACCES) {
            std::cerr << "Ошибка: нет доступа к файлу \"" << inputFileName << "\" (недостаточно прав)!\n";
        } else if (errno != 0) {
            std::cerr << "Ошибка: не удалось открыть входной файл \"" << inputFileName << "\": " 
                      << std::strerror(errno) << "\n";
        } else {
            std::cerr << "Ошибка: не удалось открыть входной файл \"" << inputFileName << "\" (файл не существует или недоступен)!\n";
        }
        return FILE_CHECK_ERROR;
    }
    
    // Проверка на пустой файл
    char firstChar;
    if (!inputFile.get(firstChar)) {
        // Файл пустой
        std::cout << "Входной файл пуст. Цитаты не найдены.\n";
        inputFile.close();
        return FILE_CHECK_EMPTY;
    }
    // Возвращаем прочитанный символ обратно в поток для дальнейшей обработки
    inputFile.putback(firstChar);
    
    // Проверяем наличие цитат в файле
    bool hasQuotes = false;
    char checkChar;
    
    // Сохраняем текущую позицию в файле
    std::streampos startPos = inputFile.tellg();
    
    // Проверяем наличие цитат
    while (inputFile.get(checkChar)) {
        if (checkChar == '"') {
            hasQuotes = true;
            break;  // Нашли хотя бы одну кавычку - достаточно
        }
    }
    
    // Если цитат не найдено
    if (!hasQuotes) {
        std::cout << "В файле нет цитат. Выходной файл не создан.\n";
        inputFile.close();
        return FILE_CHECK_NO_QUOTES;
    }
    
    // Возвращаемся к началу файла для чтения цитат
    inputFile.clear();  // Сбрасываем флаги ошибок
    inputFile.seekg(startPos);
    
    return FILE_CHECK_OK;
}

// Функция извлечения цитат из входного файла и записи их в выходной файл
// Использует потоковый подход: читает посимвольно и сразу записывает в выходной файл
// Параметры:
//   inputFileName - имя входного файла
//   outputFileName - имя выходного файла
// Возвращает: true при успехе, false при ошибке
bool extractQuotes(const char* inputFileName, const char* outputFileName) {
    // Проверка входных данных
    if (inputFileName == nullptr || outputFileName == nullptr) {
        std::cerr << "Ошибка: неверные имена файлов!\n";
        return false;
    }
    
    // Выполняем все проверки входного файла
    std::ifstream inputFile;
    FileCheckResult checkResult = checkInputFile(inputFileName, inputFile);
    
    // Обрабатываем результаты проверки
    if (checkResult == FILE_CHECK_ERROR) {
        return false;  // Ошибка открытия файла
    } else if (checkResult == FILE_CHECK_EMPTY || checkResult == FILE_CHECK_NO_QUOTES) {
        return true;  // Файл пустой или нет цитат - это не ошибка, но выходной файл не создаётся
    }
    // Если checkResult == FILE_CHECK_OK, продолжаем обработку
    
    
    // Открываем выходной файл для записи (в текстовом режиме)
    std::ofstream outputFile(outputFileName, std::ios::out);
    if (!outputFile.is_open()) {
        std::cerr << "Ошибка: не удалось открыть выходной файл \"" << outputFileName << "\"\n";
        inputFile.close();
        return false;
    }

    bool insideQuote = false;  // Флаг: находимся ли мы внутри цитаты
    char ch;                    // Текущий символ
    
    // Читаем файл посимвольно
    while (inputFile.get(ch)) {
        if (ch == '"') {
            // Найдена кавычка
            if (insideQuote) {
                // Закрывающая кавычка - заканчиваем цитату
                outputFile.put(ch);
                outputFile.put('\n');  // Добавляем перевод строки после каждой цитаты
                insideQuote = false;
            } else {
                // Открывающая кавычка - начинаем цитату
                outputFile.put(ch);
                insideQuote = true;
            }
        } else if (insideQuote) {
            // Мы внутри цитаты - записываем символ в выходной файл
            outputFile.put(ch);
        }
        // Если мы не внутри цитаты и символ не кавычка - пропускаем его
    }
    
    // Проверяем, не осталась ли незакрытая цитата
    if (insideQuote) {
        std::cerr << "Предупреждение: найдена незакрытая цитата в конце файла!\n";
    }
    
    // Закрываем файлы
    inputFile.close();
    outputFile.close();
    
    return true;
}

// Функция обработки файлов: выводит информацию и вызывает извлечение цитат
// Параметры:
//   inputFileName - имя входного файла
//   outputFileName - имя выходного файла
// Возвращает: код возврата (0 при успехе, 1 при ошибке)
int processFiles(const char* inputFileName, const char* outputFileName) {
    std::cout << "Входной файл: " << inputFileName << "\n";
    std::cout << "Выходной файл: " << outputFileName << "\n\n";
    
    // Извлекаем цитаты
    bool result = extractQuotes(inputFileName, outputFileName);
    
    // Проверяем, был ли создан выходной файл (если файл был пуст, выходной файл не создаётся)
    std::ifstream checkOutput(outputFileName);
    bool outputFileExists = checkOutput.good();
    checkOutput.close();
    
    if (result) {
        // Если выходной файл был создан, значит были найдены цитаты
        if (outputFileExists) {
            std::cout << "Цитаты успешно извлечены и записаны в файл \"" << outputFileName << "\"\n";
        }
        // Если выходной файл не был создан, значит файл был пуст (сообщение уже выведено в extractQuotes)
        return 0;
    }
    std::cerr << "Ошибка при обработке файлов!\n";
    return 1;
}

int main() {
    // Установка кодировки
    setConsoleEncoding();
    
    // Включение проверки утечек памяти
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    std::cout << "=== Лабораторная работа 8. Вариант 8 ===\n\n";
    std::cout << "Программа извлекает цитаты (текст в кавычках) из файла\n";
    std::cout << "и записывает их в другой файл.\n\n";
    
    // Имена файлов
    const char* inputFileName = "input.txt";
    const char* outputFileName = "output.txt";
    
    // Обрабатываем файлы
    int result = processFiles(inputFileName, outputFileName);
    
    std::cout << "\nПрограмма завершена.\n";
    
    return result;
}

