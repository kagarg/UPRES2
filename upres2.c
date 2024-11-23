#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define A 0.0
#define B 1.0
#define K 100 // Количество разбиений отрезка [A;B]

double calculateExp(double x) { //вычисляем exp(x) путем разложения в ряд тейлора до 51 члена
    double result = 1;
    double term = 1;
    double numerator = 1;
    double denominator = 1; 

    for (int i = 1; i <= 51; i++) {
        numerator *= x;
        denominator *= i;
        term = numerator / denominator;
        result += term;
    }

    return result;
}

double f(double x) // вычисляем заданную функцию
{
    int pid1 = fork();
    if (pid1 == 0) 
    {
        // Дочерний процесс 1 для exp(x)
        double result1 = calculateExp(x);
        FILE* file = fopen("tempfile.txt", "a");
        fprintf(file, "1 %.10f\n", result1); // Записываем результат в файл
        fclose(file);
        exit(0);
    } 
    else 
    {
        int pid2 = fork();
        if (pid2 == 0)
         {
            // Дочерний процесс 2 для exp(-x)
            double result2 = calculateExp(-x); 
            FILE* file = fopen("tempfile.txt", "a");
            fprintf(file, "2 %.10f\n", result2); // Дописываем результат в файл
            fclose(file);
            exit(0);
        } 
        else 
        {
            // Родительский процесс
            wait(NULL); // Ждем, пока оба дочерних процесса завершатся
            wait(NULL);
        }

    FILE* file = fopen("tempfile.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    char buffer[255];
    double results[2];
    while (fscanf(file, "%s %lf", buffer, &results[atoi(buffer) - 1]) != EOF);
    fclose(file);
    return (results[1]-results[0]) / 2;
    }
}

int main() 
{
    
    double h = (B - A) / K;
    double sum = 0.5 * (f(A) + f(B)); // Начальное значение суммы

    for (int i = 1; i < K; i++) 
    {
    double x = A + i * h;
    sum += f(x);
    }
    double integral = h * sum;

    printf("Calculated integral: %lf\n", integral);
    FILE* file = fopen("output.txt", "w");
    fprintf(file, "Calculated integral: %lf\n", integral);
    return 0;

}
