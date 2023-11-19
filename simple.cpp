#pragma comment(lib,"bdd_debug.lib")
#include "bdd.h"
#include <fstream>
#include <math.h>

const std::size_t N = 9;			// число объектов 
const std::size_t SIDE = sqrt(N);	// размерность таблицы
const std::size_t M = 4;			// число свойств
const std::size_t LOG_N = 4;		// максимальное изменение индекса, отвечающего за бит свойства
const std::size_t VAR_NUM = N * M * LOG_N;  // 144;	// количество булевых переменных
char var[VAR_NUM];					// массив булевых переменных
bdd p1[N][N];						// свойство 1 название
bdd p2[N][N];						// свойство 2 цена
bdd p3[N][N];						// свойство 3 цвет
bdd p4[N][N];						// свойство 4 принт

void print()
{
	for (std::size_t i = 0; i < N; i++)
	{
		std::cout << i << ": ";
		for (std::size_t j = 0; j < M; j++)
		{
			int J = i * M * LOG_N + j * LOG_N;
			int num = 0;
			for (std::size_t k = 0; k < LOG_N; k++)
				num += (std::size_t)(var[J + k] << k);
			std::cout << num << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

void build(const char* varset, const  std::size_t n, const  std::size_t I)
{
	if (I == n - 1)
	{
		if (varset[I] >= 0)
		{
			var[I] = varset[I];
			print();
			return;
		}
		var[I] = 0;
		print();
		var[I] = 1;
		print();
		return;
	}
	if (varset[I] >= 0)
	{
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}

void fun(char* varset, int size)
{
	build(varset, size, 0);
}

// Ограничения 1-ого типа
void makeConditionsType1(bdd& my_bdd)
{
	my_bdd &= p3[1][6];
	my_bdd &= p2[4][1];
	my_bdd &= p4[3][2];
	my_bdd &= p1[8][0];
	my_bdd &= p2[8][6];
	my_bdd &= p1[6][2];
	my_bdd &= p3[5][1];

	//Доп для простого уровня
	my_bdd &= p3[6][7];
	my_bdd &= p2[6][2];
	my_bdd &= p4[6][8];
	my_bdd &= p4[1][5];
	//my_bdd &= p2[7][7]; // продвинутый уровень
	//my_bdd &= p4[8][4]; // продвинутый уровень
}

// Ограничения 2-ого типа
void makeConditionsType2(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		my_bdd &= !(p1[i][1] ^ p3[i][5]);
		my_bdd &= !(p2[i][7] ^ p4[i][7]);
		my_bdd &= !(p3[i][3] ^ p4[i][3]);
		my_bdd &= !(p1[i][8] ^ p4[i][0]);

		//Доп для простого уровня
		my_bdd &= !(p1[i][6] ^ p2[i][4]);
		my_bdd &= !(p1[i][5] ^ p2[i][7]);
	}
}

// Ограничения 3-его типа
void makeConditionsType3(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		if (i == 4 || i == 5 || i == 7 || i == 8)
		{
			my_bdd &= !(p1[i - 4][4] ^ p3[i][4]);
			my_bdd &= !(p3[i - 4][0] ^ p1[i][5]);
			my_bdd &= !(p4[i - 4][1] ^ p3[i][2]);
		}
		if (i <= 5)
		{
			my_bdd &= !(p1[i + 3][7] ^ p2[i][3]);
			my_bdd &= !(p1[i + 3][8] ^ p4[i][6]);
		}
	}
}

// Ограничения 4-ого типа

void makeConditionsType4(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		if (i == 4 || i == 5)
		{
			my_bdd &= !(p2[i - 4][8] ^ p2[i][0]) | !(p2[i + 3][8] ^ p2[i][0]);
			my_bdd &= !(p3[i - 4][8] ^ p4[i][1]) | !(p3[i + 3][8] ^ p4[i][1]);
			my_bdd &= !(p1[i - 4][2] ^ p2[i][5]) | !(p1[i + 3][2] ^ p2[i][5]);
		}
		if (i <= 5)
		{
			my_bdd &= !(p2[i + 3][8] ^ p2[i][0]);
			my_bdd &= !(p3[i + 3][8] ^ p4[i][1]);
			my_bdd &= !(p1[i + 3][2] ^ p2[i][5]);
		}
		if (i >= 7)
		{
			my_bdd &= !(p2[i - 4][8] ^ p2[i][0]);
			my_bdd &= !(p3[i - 4][8] ^ p4[i][1]);
			my_bdd &= !(p1[i - 4][2] ^ p2[i][5]);
		}
	}
}

void limitValues(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		bdd bdd1 = bddfalse;
		bdd bdd2 = bddfalse;
		bdd bdd3 = bddfalse;
		bdd bdd4 = bddfalse;

		for (std::size_t j = 0; j < N; j++)
		{
			bdd1 |= p1[i][j];
			bdd2 |= p2[i][j];
			bdd3 |= p3[i][j];
			bdd4 |= p4[i][j];
		}
		my_bdd &= bdd1 & bdd2 & bdd3 & bdd4;
	}
}

// Ограничение количества разных свойств 8
void makeValuesUnique(bdd& my_bdd)
{
	for (std::size_t j = 0; j < N; j++)
	{
		for (std::size_t i = 0; i < N - 1; i++)
		{
			for (std::size_t k = i + 1; k < N; k++)
			{
				my_bdd &= p1[i][j] >> !p1[k][j];
				my_bdd &= p2[i][j] >> !p2[k][j];
				my_bdd &= p3[i][j] >> !p3[k][j];
				my_bdd &= p4[i][j] >> !p4[k][j];
			}
		}
	}
}

void createBDD(bdd& my_bdd)
{
	std::size_t I = 0;
	for (std::size_t i = 0; i < N; i++)
	{
		for (std::size_t j = 0; j < N; j++)
		{
			p1[i][j] = bddtrue;
			for (std::size_t k = 0; k < LOG_N; k++)
			{
				p1[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + k) : bdd_nithvar(I + k);
			}
			p2[i][j] = bddtrue;
			for (std::size_t k = 0; k < LOG_N; k++)
			{
				p2[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + LOG_N + k) : bdd_nithvar(I + LOG_N + k);
			}
			p3[i][j] = bddtrue;
			for (std::size_t k = 0; k < LOG_N; k++)
			{
				p3[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + LOG_N * 2 + k) : bdd_nithvar(I + LOG_N * 2 + k);
			}
			p4[i][j] = bddtrue;
			for (std::size_t k = 0; k < LOG_N; k++)
			{
				p4[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + LOG_N * 3 + k) : bdd_nithvar(I + LOG_N * 3 + k);
			}
		}
		I += LOG_N * M;
	}
}

int main()
{
	bdd_init(10000000, 100000);
	bdd_setvarnum(VAR_NUM);

	bdd my_bdd = bddtrue;
	createBDD(my_bdd);
	makeConditionsType1(my_bdd);
	makeConditionsType2(my_bdd);
	makeConditionsType3(my_bdd);
	makeConditionsType4(my_bdd);
	limitValues(my_bdd);
	makeValuesUnique(my_bdd);

	std::size_t satcount = bdd_satcount(my_bdd);
	std::cout << "Found " << satcount << " solution(s)\n\n";
	if (satcount)
	{
		bdd_allsat(my_bdd, fun);
	}
	bdd_done();
	return 0;
}
