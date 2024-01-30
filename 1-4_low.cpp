#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>
#include <ctime>
#include <fstream>

#include <random>

int generateRandomNumber(int min, int max) {
    auto a = rand() % (max - min + 1) + min;
    return a;
}

// Функция для вычисления минимального элемента в строке матрицы
int calculateMin(const std::vector<std::vector<int>>& matrix, int row) {
    int min_value = std::numeric_limits<int>::max();

    #pragma omp parallel for reduction(min:min_value)
    for (size_t i = 0; i < matrix[row].size(); ++i) {
        if (matrix[row][i] < min_value) {
            min_value = matrix[row][i];
        }
    }

    return min_value;
}

// Функция для вычисления максимального значения среди минимальных элементов строк матрицы
int calculateMaxAmongMins(const std::vector<std::vector<int>>& matrix) {
    int max_among_mins = std::numeric_limits<int>::min();
    for (size_t i = 0; i < matrix.size(); ++i) {
        int min_value = calculateMin(matrix, i);
        if (min_value > max_among_mins) {
            max_among_mins = min_value;
        }
    }

    return max_among_mins;
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // Размеры матрицы
    std::vector<int> RowCols = {10, 100, 1000, 10000,20000,40000,100000};

    // Различные количество потоков
    std::vector<int> num_threads = {1, 2, 3, 4};

    // Открываем файл для записи результатов
    std::ofstream outfile("results1-4_low.csv");
    if (!outfile.is_open()) {
        std::cerr << "file fail." << std::endl;
        return 1;
    }

    // Заголовок CSV файла
    outfile << "SizeMatrix,NumThreads,MaxAmongMins,ExecutionTime" << std::endl;


    for (int rc : RowCols) {
        // Инициализация матрицы с случайными значениями
        std::vector<std::vector<int>> matrix(rc, std::vector<int>(rc));

        for (int i = 0; i < rc; ++i) {
            for (int j = 0; j < rc; ++j) {
                matrix[i][j] = generateRandomNumber(-10000, 10000);
            }
        }

        for (int threads : num_threads) {
            // Установка количества потоков
            omp_set_num_threads(threads);

            // Замер времени выполнения
            double start_time = omp_get_wtime();

            // Вычисление максимального значения среди минимальных элементов строк матрицы
            int max_among_mins = calculateMaxAmongMins(matrix);

            // Замер времени выполнения
            double end_time = omp_get_wtime();

            // Вывод результата в консоль
            std::cout << "Size: " << rc << ", Threads: " << threads << ", MaxAmongMins: " << max_among_mins << ", ExecutionTime: " << end_time - start_time << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << rc << "," << threads << "," << max_among_mins << "," << end_time - start_time << std::endl;
        }
    }

    // Закрываем файл
    outfile.close();

    std::cout << "results1-4_low.csv" << std::endl;

    return 0;
}
