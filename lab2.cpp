#include <stdio.h>
#include <iostream> 
#include <stdlib.h>  
#include <time.h>
#include <cmath>
#include <omp.h>  
#include <windows.h>
#include <string> 
#include <locale.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define SIZE_STR 8
#define SIZE_STR_DOUBLE 23

/* Функция получения целочисленного значения из входного потока */
int getIntValue(bool zero, const char* text = "Введите размер массива : ") {
	printf(text);

	size_t length = 0;
	char* end = NULL;
	char buf[SIZE_STR] = "";
	int value = 0;
	bool t = true;

	while ((fgets(buf, sizeof(buf), stdin)) && (t)) {
		length = strlen(buf);
		t = false;

		if (buf[length - 1] == '\n') {
			buf[--length] = '\0';

			errno = 0;
			value = strtol(buf, &end, 10);

			if (length == 0) {
				fprintf(stderr, "\nНеобходимо ввести данные.\n");
				t = true;
			}
			if (errno != 0 || *end != '\0') {
				fprintf(stderr, "\nПри вводе данных возникли ошибки\n");
				fprintf(stderr, "\t%s\n", buf);
				fprintf(stderr, "\t%*c\n", (int)(end - buf) + 1, '^');
				t = true;
			}
		}
		else {
			scanf_s("%*[^\n]");
			scanf_s("%*c");
			fprintf(stderr, "Возникло %d ошибок: \n", (SIZE_STR - 2));

			t = true;
		}

		if ((!t) && (value <= 0) && (!zero)) {
			printf("\nОшибка: число не может быть меньше 0\n");
			t = true;
		}

		if (t) {
			printf(text);
		}
	}

	return value;
}

int main() {
	// Инициализация генератора случайных чисел
	srand(time(0));
	// Установка локализации для вывода
	setlocale(LC_ALL, "Rus");

	int N;
	int n;

	N = getIntValue(false);
	n = getIntValue(false, "Введите кол-во потоков = ");

	int* arr = new int[N];

	// Наполнение чисел рандомно
	for (int i = 0; i < N; i++) {
		arr[i] = 100 + rand() % 1000000;
	}

	// - Секция вычисления с потоками -
	double time = omp_get_wtime();
	omp_set_dynamic(false);
	omp_set_num_threads(n);

	long unsigned int sum1 = 0;
	long unsigned int sum2 = 0;

#pragma omp parallel firstprivate(sum2) shared(sum1) 
	{
		sum2 = 0;

#pragma omp for
		for (int i = 0; i < N; i++)
		{
			if (i % 2 == 0)
			{
				sum2 += arr[i];
			}
		}
#pragma omp critical
		{
			sum1 += sum2;
		}
	}

	time = omp_get_wtime() - time;
	std::cout << "Сумма элементов массива (с потоками) = " << sum1 << std::endl;
	std::cout << "Промежуточное время вычислений = " << time << std::endl << std::endl;

	// - Секция вычислений без потоков -
	time = omp_get_wtime();

	sum1 = 0;

	for (int i = 0; i < N; i++)
	{
		if (i % 2 == 0)
		{
			sum1 += arr[i];
		}
	}

	time = omp_get_wtime() - time;
	std::cout << "Сумма элементов массива (без потоков) = " << sum1 << std::endl;
	std::cout << "Промежуточное время вычислений = " << time << std::endl << std::endl;

	// - Секция вычисления с редукцией -
	time = omp_get_wtime();
	sum1 = 0;

#pragma omp parallel for reduction (+: sum1)  

	for (int i = 0; i < N; i++)
	{
		if (i % 2 == 0)
		{
			sum1 += arr[i];
		}
	}

	time = omp_get_wtime() - time;
	std::cout << "Сумма элементов массива (редукция) = " << sum1 << std::endl;
	std::cout << "Промежуточное время вычислений = " << time << std::endl << std::endl;

	system("pause");
	return 0;
}