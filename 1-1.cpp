#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <omp.h>
#include <ctime>
#include <cstdlib>

int main() {
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Различные размеры вектора
    std::vector<int> vector_sizes = {10000, 100000, 1000000, 10000000,100000000,1000000000};

    // Различные количество потоков
    std::vector<int> num_threads = {1, 2, 3, 4};

    // Открываем файл для записи результатов
    std::ofstream outfile("results1-1.csv");
    if (!outfile.is_open()) {
        std::cerr << "fail opening file." << std::endl;
        return 1;
    }

    // Заголовок CSV файла
    outfile << "VectorSize,NumThreads,MinValue,ExecutionTime" << std::endl;

    for (int size : vector_sizes) {
        std::vector<int> numbers(size);

        // Генерация случайных значений
        for (int i = 0; i < size; ++i) {
            numbers[i] = std::rand() % 1000;
        }

        for (int threads : num_threads) {
            // Установка количества потоков
            omp_set_num_threads(threads);

            // Инициализация переменной для хранения минимального значения
            int min_value = std::numeric_limits<int>::max();

            // Замер времени выполнения
            double start_time = omp_get_wtime();

            // Параллельное вычисление минимального значения вектора
            #pragma omp parallel for reduction(min:min_value)
            for (size_t i = 0; i < numbers.size(); ++i) {
                // Каждый поток находит минимальное значение в своей части вектора
                if (numbers[i] < min_value) {
                    min_value = numbers[i];
                }
            }

            // Замер времени выполнения
            double end_time = omp_get_wtime();

            // Вывод результата в консоль
            std::cout << "VectorSize: " << size << ", Threads: " << threads
                      << ", MinValue: " << min_value << ", ExecutionTime: " << end_time - start_time << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << size << "," << threads << "," << min_value << "," << end_time - start_time << std::endl;
        }
    }

    // Закрываем файл
    outfile.close();

    std::cout << "res here results.csv" << std::endl;

    return 0;
}
