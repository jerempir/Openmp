#include <iostream>
#include <vector>
#include <omp.h>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <functional>


void loopStatic(std::vector<int>& data,int N) {
    // Генерация случайных чисел на части итераций
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; ++i) {
        // Некоторые итерации выполняют дополнительную работу
        if (i % 100 == 0) {
            data[i] = std::rand() % 100;
        }
    }
}


void loopDynamic(std::vector<int>& data,int N) {
    // Генерация случайных чисел на части итераций
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; ++i) {
        // Некоторые итерации выполняют дополнительную работу
        if (i % 100 == 0) {
            data[i] = std::rand() % 100;
        }
    }
}


void loopGuided(std::vector<int>& data,int N) {
    // Генерация случайных чисел на части итераций
    #pragma omp parallel for schedule(guided)
    for (int i = 0; i < N; ++i) {
        // Некоторые итерации выполняют дополнительную работу
        if (i % 10 == 0) {
            data[i] = std::rand() % 100;
        }
    }
}

void measureTimeAndWriteToCSV(std::ofstream& outfile, int N, const char* schedule_type, void (*func)(std::vector<int>&,int N)) {
    std::vector<int> data(N, 0);
    double start_time = omp_get_wtime();
    func(data,N);
    double end_time = omp_get_wtime();
    double execution_time = end_time - start_time;

    std::cout << schedule_type << " N: " << N << " Schedule Time: " << execution_time << " seconds" << std::endl;

    // Запись результатов в CSV файл
    outfile << schedule_type << "," << N << ","<< execution_time << std::endl;
}


int main() {
    std::vector<int> N = {100000,10000000,1000000000};
    // Открываем файл для записи результатов
    std::ofstream outfile("results1-11.csv");
    if (!outfile.is_open()) {
        std::cerr << "file fail." << std::endl;
        return 1;
    }
    // Заголовок CSV файла
    outfile << "ScheduleType,N,ExecutionTime" << std::endl;
    for (int n : N) {
        std::cout<<n<<std::endl;
        // Замер времени выполнения и запись в CSV для статического распределения
        measureTimeAndWriteToCSV(outfile,n, "Static", loopStatic);

        // Замер времени выполнения и запись в CSV для динамического распределения
        measureTimeAndWriteToCSV(outfile,n, "Dynamic", loopDynamic);

        // Замер времени выполнения и запись в CSV для guided распределения
        measureTimeAndWriteToCSV(outfile,n, "Guided", loopGuided);
    }

    // Закрываем файл
    outfile.close();

    std::cout << "Res 1-11.csv" << std::endl;

    return 0;
}
