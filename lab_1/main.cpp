#include <iostream>
#include <cstdlib>
#include <crtdbg.h>
#include <locale>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif



// Функция для выделения памяти под матрицу
int** allocateMatrix(int rows, int columns) {
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[columns];
    }
    return matrix;
}

// Функция для освобождения памяти матрицы
void deallocateMatrix(int** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

// Функция для ввода матрицы
void inputMatrix(int** matrix, int rows, int columns) {
    std::cout << "Введите элементы матрицы " << rows << "x" << columns << ":\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            std::cout << "matrix[" << i << "][" << j << "] = ";
            std::cin >> matrix[i][j];
        }
    }
}

// Функция для вывода матрицы
void printMatrix(int** matrix, int rows, int columns) {
    std::cout << "\nМатрица:\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << "\n";
    }
}

// Задание 1: Вычислить характеристику столбца (сумма модулей отрицательных нечётных элементов)
int calculateColumnCharacteristic(int** matrix, int column, int rows) {
    int sum = 0;
    for (int i = 0; i < rows; i++) {
        // Проверяем: отрицательный И нечётный
        if (matrix[i][column] < 0 && matrix[i][column] % 2 != 0) {
            sum += abs(matrix[i][column]); // Модуль отрицательного числа
        }
    }
    return sum;
}

// Функция для перестановки столбцов матрицы
void swapColumns(int** matrix, int column1, int column2, int rows) {
    for (int i = 0; i < rows; i++) {
        int temp = matrix[i][column1];
        matrix[i][column1] = matrix[i][column2];
        matrix[i][column2] = temp;
    }
}

// Задание 1: Сортировка столбцов по возрастанию характеристик
void sortColumnsByCharacteristics(int** matrix, int rows, int columns) {
    // Создаём массив характеристик
    int* characteristics = new int[columns];
    for (int j = 0; j < columns; j++) {
        characteristics[j] = calculateColumnCharacteristic(matrix, j, rows);
    }
    
    // Сортировка пузырьком по возрастанию характеристик
    for (int i = 0; i < columns - 1; i++) {
        for (int j = 0; j < columns - i - 1; j++) {
            if (characteristics[j] > characteristics[j + 1]) {
                // Перестановка характеристик
                int tempChar = characteristics[j];
                characteristics[j] = characteristics[j + 1];
                characteristics[j + 1] = tempChar;
                // Перестановка столбцов
                swapColumns(matrix, j, j + 1, rows);
            }
        }
    }
    
    delete[] characteristics;
}

// Проверка, содержит ли столбец хотя бы один отрицательный элемент
bool hasNegativeElement(int** matrix, int column, int rows) {
    for (int i = 0; i < rows; i++) {
        if (matrix[i][column] < 0) {
            return true;
        }
    }
    return false;
}

// Задание 2: Найти сумму элементов в столбцах, которые содержат хотя бы один отрицательный элемент
int sumColumnsWithNegatives(int** matrix, int rows, int columns) {
    int totalSum = 0;
    for (int j = 0; j < columns; j++) {
        if (hasNegativeElement(matrix, j, rows)) {
            // Суммируем все элементы этого столбца
            for (int i = 0; i < rows; i++) {
                totalSum += matrix[i][j];
            }
        }
    }
    return totalSum;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Включение проверки утечек памяти
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    int rows, columns;
    
    std::cout << "=== Лабораторная работа. Вариант 8 ===\n\n";
    std::cout << "Введите количество строк: ";
    std::cin >> rows;
    std::cout << "Введите количество столбцов: ";
    std::cin >> columns;
    
    if (rows <= 0 || columns <= 0) {
        std::cout << "Ошибка: размеры матрицы должны быть положительными!\n";
        return 1;
    }
    
    // Выделение памяти под матрицу
    int** matrix = allocateMatrix(rows, columns);
    
    // Ввод матрицы
    inputMatrix(matrix, rows, columns);
    
    // Вывод исходной матрицы
    std::cout << "\n=== Исходная матрица ===";
    printMatrix(matrix, rows, columns);
    
    // Задание 1: Вычисление характеристик столбцов
    std::cout << "\n=== Задание 1 ===";
    std::cout << "\nХарактеристики столбцов (сумма модулей отрицательных нечётных элементов):\n";
    for (int j = 0; j < columns; j++) {
        int charValue = calculateColumnCharacteristic(matrix, j, rows);
        std::cout << "Столбец " << j << ": характеристика = " << charValue << "\n";
    }
    
    // Сортировка столбцов по возрастанию характеристик
    std::cout << "\nСортировка столбцов по возрастанию характеристик...\n";
    sortColumnsByCharacteristics(matrix, rows, columns);
    
    // Вывод отсортированной матрицы
    std::cout << "\n=== Матрица после сортировки столбцов ===";
    printMatrix(matrix, rows, columns);
    
    std::cout << "\nХарактеристики столбцов после сортировки:\n";
    for (int j = 0; j < columns; j++) {
        int charValue = calculateColumnCharacteristic(matrix, j, rows);
        std::cout << "Столбец " << j << ": характеристика = " << charValue << "\n";
    }
    
    // Задание 2: Сумма элементов в столбцах с отрицательными элементами
    std::cout << "\n=== Задание 2 ===";
    std::cout << "\nПоиск суммы элементов в столбцах, содержащих хотя бы один отрицательный элемент...\n";
    
    // Выводим информацию о каждом столбце
    for (int j = 0; j < columns; j++) {
        if (hasNegativeElement(matrix, j, rows)) {
            int columnSum = 0;
            for (int i = 0; i < rows; i++) {
                columnSum += matrix[i][j];
            }
            std::cout << "Столбец " << j << " содержит отрицательные элементы. Сумма элементов = " << columnSum << "\n";
        }
    }
    
    int totalSum = sumColumnsWithNegatives(matrix, rows, columns);
    std::cout << "\nОбщая сумма элементов во всех столбцах с отрицательными элементами: " << totalSum << "\n";
    
    // Освобождение памяти
    deallocateMatrix(matrix, rows);
    
    std::cout << "\nПрограмма завершена.\n";
    
    return 0;
}

