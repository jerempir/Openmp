#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <ctime>
#include <fstream>

// Функция, для которой вычисляется определённый интеграл

double function(double x) {
    return std::exp(2*x);
}

// Функция для вычисления определённого интеграла методом прямоугольников
double calculateIntegral(double a, double b, int numRectangles) {
    double dx = (b - a) / numRectangles;
    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < numRectangles; ++i) {
        double x = a + i * dx;
        sum += function(x) * dx;
    }

    return sum;
}

int main() {
    // Различные количество прямоугольников
    std::vector<int> num_rectangles = {10000, 100000, 1000000, 10000000};

    // Различные количество потоков
    std::vector<int> num_threads = {1, 2, 3, 4};

    // Открываем файл для записи результатов
    std::ofstream outfile("results1-3.csv");
    if (!outfile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
        return 1;
    }

    // Заголовок CSV файла
    outfile << "NumRectangles,NumThreads,Integral,ExecutionTime" << std::endl;

    for (int rectangles : num_rectangles) {
        for (int threads : num_threads) {
            // Установка количества потоков
            omp_set_num_threads(threads);

            // Замер времени выполнения
            double start_time = omp_get_wtime();

            // Вычисление определённого интеграла методом прямоугольников
            double integral = calculateIntegral(1, 2, rectangles);

            // Замер времени выполнения
            double end_time = omp_get_wtime();

            // Вывод результата в консоль
            std::cout << "NumRectangles: " << rectangles << ", Threads: " << threads
                      << ", Integral: " << integral << ", ExecutionTime: " << end_time - start_time << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << rectangles << "," << threads << "," << integral << "," << end_time - start_time << std::endl;
        }
    }

    // Закрываем файл
    outfile.close();

    std::cout << "res integral_results.csv" << std::endl;

    return 0;
}
