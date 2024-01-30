#include <iostream>
#include <vector>
#include <omp.h>
#include <ctime>
#include <cstdlib>
#include <fstream>

int main() {
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Различные размеры векторов
    std::vector<int> vector_sizes = {10000, 100000, 1000000, 10000000,100000000,1000000000};

    // Различные количество потоков
    std::vector<int> num_threads = {1, 2, 3, 4};

    // Открываем файл для записи результатов
    std::ofstream outfile("results1-2.csv");
    if (!outfile.is_open()) {
        std::cerr << "fail opening file." << std::endl;
        return 1;
    }

    // Заголовок CSV файла
    outfile << "VectorSize,NumThreads,ExecutionTime" << std::endl;

    for (int size : vector_sizes) {
        std::vector<int> vector1(size);
        std::vector<int> vector2(size);

        // Генерация случайных значений
        for (int i = 0; i < size; ++i) {
            vector1[i] = std::rand() % 100;
            vector2[i] = std::rand() % 100;
        }

        for (int threads : num_threads) {
            // Установка количества потоков
            omp_set_num_threads(threads);

            // Инициализация переменной для хранения скалярного произведения
            int dot_product = 0;

            // Замер времени выполнения
            double start_time = omp_get_wtime();

            // Параллельное вычисление скалярного произведения векторов
            #pragma omp parallel for reduction(+:dot_product)
            for (size_t i = 0; i < vector1.size(); ++i) {
                dot_product += vector1[i] * vector2[i];
            }

            // Замер времени выполнения
            double end_time = omp_get_wtime();

            // Вывод результата в консоль
            std::cout << "VectorSize: " << size << ", Threads: " << threads
                      << ", DotProduct: " << dot_product << ", ExecutionTime: " << end_time - start_time << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << size << "," << threads << "," << end_time - start_time << std::endl;
        }
    }

    // Закрываем файл
    outfile.close();

    std::cout << "res in dot_product_results.csv" << std::endl;

    return 0;
}
