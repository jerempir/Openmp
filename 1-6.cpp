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

// Глобальные ключи блокировки
omp_lock_t min_value_lock;
omp_lock_t max_among_mins_lock;

// Инициализация блокировок
void initializeLocks() {
    omp_init_lock(&min_value_lock);
    omp_init_lock(&max_among_mins_lock);
}

// Освобождение блокировок
void destroyLocks() {
    omp_destroy_lock(&min_value_lock);
    omp_destroy_lock(&max_among_mins_lock);
}

// Функция для вычисления максимального значения среди минимальных элементов строк матрицы
int calculateMaxAmongMinsReduction(const std::vector<std::vector<int>>& matrix) {
    int max_among_mins = std::numeric_limits<int>::min();

    #pragma omp parallel for reduction(max:max_among_mins)
    for (size_t i = 0; i < matrix.size(); ++i) {

        int min_value = std::numeric_limits<int>::max();

        #pragma omp parallel for reduction(min:min_value)
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            if (matrix[i][j] < min_value) {
                min_value = matrix[i][j];
            }
        }

        if (min_value > max_among_mins) {
            max_among_mins = min_value;
        }
    }

    return max_among_mins;
}

// Функция для вычисления максимального значения среди минимальных элементов строк матрицы
int calculateMaxAmongMinsCritical(const std::vector<std::vector<int>>& matrix) {
    int max_among_mins = std::numeric_limits<int>::min();

    #pragma omp parallel for
    for (size_t i = 0; i < matrix.size(); ++i) {

        int min_value = std::numeric_limits<int>::max();

        #pragma omp parallel for
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            #pragma omp critical
            {    
                if (matrix[i][j] < min_value) {
                    min_value = matrix[i][j];
                }
            }
        }

        #pragma omp critical
        {
            if (min_value > max_among_mins) {
                max_among_mins = min_value;
            }
        }
    }

    return max_among_mins;
}

// Функция для вычисления максимального значения среди минимальных элементов строк матрицы
int calculateMaxAmongMinsLock(const std::vector<std::vector<int>>& matrix) {
    int max_among_mins = std::numeric_limits<int>::min();

    #pragma omp parallel for
    for (size_t i = 0; i < matrix.size(); ++i) {

        int min_value = std::numeric_limits<int>::max();

        #pragma omp parallel for
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            omp_set_lock(&min_value_lock); 
            if (matrix[i][j] < min_value) {
                min_value = matrix[i][j];
            }
            omp_unset_lock(&min_value_lock);
        }

        omp_set_lock(&max_among_mins_lock);
        if (min_value > max_among_mins) {
            max_among_mins = min_value;
        }
        omp_unset_lock(&max_among_mins_lock);
        
    }

    return max_among_mins;
}


int main() {
    // Инициализация блокировок
    initializeLocks();
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    // Размеры матрицы
    std::vector<int> RowCols = {10000,20000};

    // Различные количество потоков
    std::vector<int> num_threads = {1, 2, 3, 4};

    // Открываем файл для записи результатов
    std::ofstream outfile("results1-6.csv");
    if (!outfile.is_open()) {
        std::cerr << "file fail." << std::endl;
        return 1;
    }

    // Заголовок CSV файла
    outfile << "Type,SizeMatrix,NumThreads,MaxAmongMins,ExecutionTime" << std::endl;


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
            double start_timer = omp_get_wtime();
            // Вычисление максимального значения среди минимальных элементов строк матрицы
            int max_among_minsr = calculateMaxAmongMinsReduction(matrix);
            // Замер времени выполнения
            double end_timer = omp_get_wtime();

            // Вывод результата в консоль
            std::cout <<"Reduction"<< ", Size: " << rc << ", Threads: " << threads << ", MaxAmongMins: " << max_among_minsr << ", ExecutionTime: " << end_timer - start_timer << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << "Reduction" << "," << rc << "," << threads << "," << max_among_minsr << "," << end_timer - start_timer << std::endl;

            /////////////////////////////////////////
             // Замер времени выполнения
            double start_timec = omp_get_wtime();
            // Вычисление максимального значения среди минимальных элементов строк матрицы
            int max_among_minsc = calculateMaxAmongMinsCritical(matrix);
            // Замер времени выполнения
            double end_timec = omp_get_wtime();

            // Вывод результата в консоль
            std::cout <<"Critical"<< ", Size: " << rc << ", Threads: " << threads << ", MaxAmongMins: " << max_among_minsc << ", ExecutionTime: " << end_timec - start_timec << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << "Critical" << "," << rc << "," << threads << "," << max_among_minsc << "," << end_timec - start_timec << std::endl;
        
            /////////////////////////////////////////
            
            // Замер времени выполнения
            double start_timel = omp_get_wtime();
            // Вычисление максимального значения среди минимальных элементов строк матрицы
            int max_among_minsl = calculateMaxAmongMinsLock(matrix);
            // Замер времени выполнения
            double end_timel = omp_get_wtime();

            // Вывод результата в консоль
            std::cout <<"Lock"<< ", Size: " << rc << ", Threads: " << threads << ", MaxAmongMins: " << max_among_minsl << ", ExecutionTime: " << end_timel - start_timel << " seconds" << std::endl;

            // Запись результата в CSV файл
            outfile << "Lock" << "," << rc << "," << threads << "," << max_among_minsl << "," << end_timel - start_timel << std::endl;
        }
    }
    // Освобождение блокировок
    destroyLocks();
    // Закрываем файл
    outfile.close();

    std::cout << "results1-6.csv" << std::endl;

    return 0;
}
