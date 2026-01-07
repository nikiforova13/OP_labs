#include <iostream>
#include <crtdbg.h>

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

// Функция для вычисления длины строки (без использования strlen)
int stringLength(const char* str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

// Функция копирования строки (без использования strcpy)
void stringCopy(char* destination, const char* source) {
    int i = 0;
    while (source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

// Вариант 8: Функция удаляет из строки заданное количество символов, начиная с заданной позиции
// Параметры:
//   str - исходная строка
//   position - позиция, с которой начинать удаление (индексация с 0)
//   count - количество символов для удаления
// Возвращает: новая строка с удалёнными символами (динамически выделенная память)
//             или nullptr в случае ошибки
char* removeCharacters(const char* str, int position, int count) {
    // Проверка входных данных
    if (str == nullptr) {
        return nullptr;
    }
    
    int length = stringLength(str);
    
    // Проверка корректности позиции
    if (position < 0 || position >= length) {
        return nullptr;
    }
    
    // Проверка корректности количества символов
    if (count < 0) {
        return nullptr;
    }
    
    // Если удаляем больше символов, чем есть после позиции, удаляем всё до конца
    if (position + count > length) {
        count = length - position;
    }
    
    // Вычисляем длину новой строки
    int newLength = length - count;
    
    // Выделяем память для новой строки (+1 для нулевого символа)
    char* result = new char[newLength + 1];
    
    // Копируем символы до позиции удаления
    int i = 0;
    for (i; i < position; i++) {
        result[i] = str[i];
    }
    
    // Пропускаем удаляемые символы и копируем оставшиеся
    for (int j = position + count; j < length; j++) {
        result[i] = str[j];
        i++;
    }
    
    // Добавляем нулевой символ в конец
    result[newLength] = '\0';
    
    return result;
}

// Функция для вывода строки
void printString(const char* str) {
    if (str == nullptr) {
        std::cout << "(null)";
        return;
    }
    int i = 0;
    while (str[i] != '\0') {
        std::cout << str[i];
        i++;
    }
}

// Структура для хранения введённых данных
struct InputData {
    static const int MAX_INPUT = 1000;
    char input[MAX_INPUT];
    int position;
    int count;
};

// Функция для ввода данных от пользователя
// Возвращает структуру InputData с введёнными данными
InputData inputData() {
    InputData data;
    
    std::cout << "Введите строку (максимум " << InputData::MAX_INPUT - 1 << " символов): ";
    std::cin.getline(data.input, InputData::MAX_INPUT);
    
    std::cout << "Введите позицию начала удаления (индексация с 0): ";
    std::cin >> data.position;
    
    std::cout << "Введите количество символов для удаления: ";
    std::cin >> data.count;
    
    // Очистка буфера ввода после чтения числа
    std::cin.ignore(1000, '\n');
    
    return data;
}

int main() {
    // Установка кодировки
    setConsoleEncoding();
    
    // Включение проверки утечек памяти
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    std::cout << "=== Лабораторная работа 7. Вариант 8 ===\n\n";
    std::cout << "Функция удаляет из строки заданное количество символов,\n";
    std::cout << "начиная с заданной позиции.\n\n";
    
    // Ввод данных
    InputData data = inputData();
    
    std::cout << "\n=== Результаты ===\n";
    std::cout << "Исходная строка: \"";
    printString(data.input);
    std::cout << "\"\n";
    std::cout << "Длина исходной строки: " << stringLength(data.input) << "\n";
    std::cout << "Позиция начала удаления: " << data.position << "\n";
    std::cout << "Количество символов для удаления: " << data.count << "\n";
    
    // Выполняем удаление
    char* result = removeCharacters(data.input, data.position, data.count);
    
    if (result != nullptr) {
        std::cout << "Результирующая строка: \"";
        printString(result);
        std::cout << "\"\n";
        std::cout << "Длина результирующей строки: " << stringLength(result) << "\n";
        
        // Освобождаем память
        delete[] result;
    } else {
        std::cout << "Ошибка: некорректные параметры!\n";
    }

    std::cout << "\nПрограмма завершена.\n";
    return 0;
}

