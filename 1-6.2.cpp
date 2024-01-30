#include <stdio.h>
#include <omp.h>
#include <vector>
#include <fstream>
#include <iostream>

int main() {
    int global_result_atomic = 0;
    int global_result_critical = 0;
    int global_result_locks = 0;

    std::vector<int> N_VALUES = {10000,20000,500000};
    std::vector<int> NUM_THREADS = {1,2,3,4};

    char* methods[] = {"atomic", "critical", "locks"};

 // Открываем файл для записи результатов
    std::ofstream outfile("results1-6.2.csv");


    // Заголовок CSV файла
    outfile << "N, Method, NumThreads, Time (s), Result" << std::endl;

    for (int N : N_VALUES) {

        // Инициализация массива данных (заполнение значениями)
        int data[N];
        for (int k = 0; k < N; ++k) {
             data[k] = k + 1;
        }

        for (int threads : NUM_THREADS) {
            // Установка количества потоков
            omp_set_num_threads(threads);

            // Атомарные операции
            // Измерение времени для атомарных операций
            double start_time_atomic = omp_get_wtime();
            #pragma omp parallel for
            for (int k = 0; k < N; ++k) {
                #pragma omp atomic
                global_result_atomic += data[k];
            }
            double end_time_atomic = omp_get_wtime();
            // Вывод результата в консоль
            std::cout <<N<< threads<<methods[0] << end_time_atomic - start_time_atomic <<  std::endl;

            // Запись результата в CSV файл
            outfile << N << "," << methods[0] << "," << threads << "," << end_time_atomic - start_time_atomic << "," << global_result_atomic << std::endl;

            // Сброс результатов
            global_result_atomic = 0;

            // Критические секции
            // Измерение времени для критических секций
            double start_time_critical = omp_get_wtime();
            #pragma omp parallel for
            for (int k = 0; k < N; ++k) {
                #pragma omp critical
                global_result_critical += data[k];
            }
            double end_time_critical = omp_get_wtime();

            // Вывод результата в консоль
            std::cout <<N<< threads <<methods[1] << end_time_critical - start_time_critical <<  std::endl;
            // Запись результата в CSV файл
            outfile << N << "," << methods[1] << "," << threads << "," << end_time_critical - start_time_critical << "," << global_result_critical << std::endl;

            // Сброс результатов
            global_result_critical = 0;

            // Замки
            // Измерение времени для замков
            omp_lock_t lock;
            omp_init_lock(&lock);

            double start_time_locks = omp_get_wtime();
            #pragma omp parallel for
            for (int k = 0; k < N; ++k) {
                omp_set_lock(&lock);
                global_result_locks += data[k];
                omp_unset_lock(&lock);
            }
            double end_time_locks = omp_get_wtime();

            // Вывод результата в консоль
            std::cout <<N<<threads<< methods[2] << end_time_locks - start_time_locks <<  std::endl;
            // Запись результата в CSV файл
            outfile << N << "," << methods[2] << "," << threads << "," << end_time_locks - start_time_locks << "," << global_result_locks << std::endl;

            // Сброс результатов
            global_result_locks = 0;

            // Освобождение памяти
            omp_destroy_lock(&lock);
        }
    }

    // Закрываем файл
    outfile.close();

    return 0;
}
