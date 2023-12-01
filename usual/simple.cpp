#pragma comment(lib,"bdd_debug.lib")
#include "bdd.h"
#include <fstream>
#include <math.h>

const std::size_t N = 9;			// число объектов, значений любого свойства
const std::size_t SIDE = sqrt(N);	// размерность таблицы
const std::size_t LOG_N = 4;		// максимальное изменение индекса, отвечающего за бит свойства

const std::size_t M = 4;			// число свойств

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

void makeConditionsType1(bdd& my_bdd)
{
	my_bdd &= p3[1][6];
	my_bdd &= p2[4][1];
	my_bdd &= p4[3][2];
	my_bdd &= p1[8][0];
	my_bdd &= p2[8][6];
	my_bdd &= p1[6][2];
	my_bdd &= p3[5][1];

	// additional
	my_bdd &= p3[2][7];
	my_bdd &= p2[2][3];
	my_bdd &= p4[4][0];
	my_bdd &= p4[5][1];
	my_bdd &= p4[6][3];
	my_bdd &= p4[8][8];
	my_bdd &= p4[7][4];
	// my_bdd &= p1[7][3]; // 14
	my_bdd &= p1[0][3]; // 10
	// my_bdd &= p1[3][1]; // 36
	// my_bdd &= p1[7][4]; // 24
}

// соответствие между двумя свойствами какого-либо объекта
void makeConditionsType2(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		my_bdd &= !(p1[i][1] ^ p3[i][5]);
		my_bdd &= !(p2[i][7] ^ p4[i][7]);
		my_bdd &= !(p3[i][3] ^ p4[i][3]);
		my_bdd &= !(p1[i][8] ^ p4[i][0]);
		my_bdd &= !(p1[i][6] ^ p2[i][4]);

		// additional
		my_bdd &= !(p1[i][5] ^ p2[i][7]);
		my_bdd &= !(p1[i][3] ^ p2[i][5]);
	}
}

// Ограничение расположения объектов – расположение «слева» («справа»).
void makeConditionsType3(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		if (i % SIDE != SIDE - 1 && i >= SIDE)
		{
			std::size_t pos = i - SIDE + 1;
			my_bdd &= !(p1[pos][4] ^ p3[i][4]);
			my_bdd &= !(p3[pos][0] ^ p1[i][5]);
			my_bdd &= !(p4[pos][1] ^ p3[i][2]);
		}
		if (i % SIDE != SIDE - 1 && i < N - SIDE)
		{
			std::size_t pos = i + SIDE + 1;
			my_bdd &= !(p1[pos][7] ^ p2[i][3]);
			my_bdd &= !(p1[pos][8] ^ p4[i][6]);
		}
	}
}

// Ограничение расположения объектов – расположение «рядом» – дизъюнкция ограничений «слева» и «справа» типа 3.
void makeConditionsType4(bdd& my_bdd)
{
	for (std::size_t i = 0; i < N; i++)
	{
		if (i % SIDE != SIDE - 1 && i >= SIDE && i < N - SIDE)
		{
			std::cout << i;
			std::size_t left = i - SIDE + 1;
			std::size_t right = i + SIDE + 1;
			my_bdd &= !(p2[left][8] ^ p2[i][0]) | !(p2[right][8] ^ p2[i][0]);
			my_bdd &= !(p3[left][8] ^ p4[i][1]) | !(p3[right][8] ^ p4[i][1]);
			my_bdd &= !(p1[left][2] ^ p2[i][5]) | !(p1[right][2] ^ p2[i][5]);
		}
		if (i == 0 || i == 1)
		{
			my_bdd &= !(p2[i + 4][8] ^ p2[i][0]);
		}
	}
}

// Параметры принимают значения только из заданных множеств (значение свойств должно быть меньше N)
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

// У двух различных объектов значения любого параметра (свойства) не совпадают.
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
