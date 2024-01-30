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

// Функция для вычисления максимального значения среди минимальных элементов строк треугольной матрицы
int calculateMaxAmongMins(const std::vector<std::vector<int>>& matrix) {
    int max_among_mins = std::numeric_limits<int>::min();

    #pragma omp parallel for reduction(max:max_among_mins)
    for (size_t i = 0; i < matrix.size(); ++i) {
        int min_value = matrix[i][0];  // Инициализация минимального значения текущей строки

        // Для треугольной матрицы, начиная со второго элемента строки
        #pragma omp parallel for reduction(min:min_value)
        for (size_t j = i+1; j < matrix[i].size(); ++j) {
            if (matrix[i][j] < min_value) {
                min_value = matrix[i][j];
            }
        }

        // Обновление максимального значения среди минимальных элементов
        if (min_value > max_among_mins) {
            max_among_mins = min_value;
        }
    }

    return max_among_mins;
}

int main() {
    // Размеры треугольной матрицы
    const int size = 10000;
    // Размеры матрицы
    std::vector<int> RowCols = {10, 100, 1000, 10000,20000,40000};
    
        // Различные количество потоков
    std::vector<int> num_threads = {1, 2, 3, 4};

    // Открываем файл для записи результатов
    std::ofstream outfile("results1-5.csv");
    if (!outfile.is_open()) {
        std::cerr << "fail file." << std::endl;
        return 1;
    }

    // Заголовок CSV файла
    outfile << "SizeMatrix,NumThreads,MaxAmongMins,ExecutionTime" << std::endl;

    for (int size : RowCols) {
        // Инициализация треугольной матрицы с случайными значениями
        std::vector<std::vector<int>> matrix(size, std::vector<int>(size, 0));
        for (int i = 0; i < size; ++i) {
            for (int j = i; j <size; ++j) {  // Инициализация только нижнего треугольника матрицы
                matrix[i][j] = generateRandomNumber(-10000, 10000);
            }
        }


        for (int threads : num_threads) {
            // Установка количества потоков
            omp_set_num_threads(threads);

            // Замер времени выполнения
            double start_time = omp_get_wtime();

            // Вычисление максимального значения среди минимальных элементов строк треугольной матрицы
            int max_among_mins = calculateMaxAmongMins(matrix);

            // Замер времени выполнения
            double end_time = omp_get_wtime();

            // Вывод результата в консоль
            std::cout << "Size: " << size << ", Threads: " << threads << ", MaxAmongMins: " << max_among_mins << ", ExecutionTime: " << end_time - start_time << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << size << "," << threads << "," << max_among_mins << "," << end_time - start_time << std::endl;
        }
    }
    // Закрываем файл
    outfile.close();

    std::cout << "1-5.csv" << std::endl;

    return 0;
}
