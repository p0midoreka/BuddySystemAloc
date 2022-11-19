
#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//Структура
struct Buddy
{
	int size;
	int address;
	Buddy* parent;
	Buddy* right;
	Buddy* left;
	Buddy* next;
	Buddy* previous;
	bool allocated;
	int freeSpace;
	int level;
	bool deleteThis;
};


//Функции

void buddySystem();
void allocateMemoryToProcess();
void deallocateMemoryOfProcess();
void printStatistics();
Buddy* splitMemory(Buddy* tree);
void randomSearchAndDelete(Buddy* root);
void recursiveCheck(Buddy* root);
void computeInternalFragmentation(Buddy* root);


//Объявление
Buddy* root, * newnodeLeft, * newnodeRight;
Buddy* freelist[7], * allocateThisNode;
Buddy* rootCopy;

int allocations = 0, internalFragmentation;
int processSize = 0, level = 0;
int value = 1, allocationDenied = 0;
int luck = 0, processValue = 0;
bool deleteSuccess = false, nodeFound = false;

using namespace std;

int main()
{
int n = 1;
	
	while (n > 0)
	{
		cout << "\n Press 1 to Simulate Buddy System."
			<< "\n Press 0 to Exit."
			<< "\n\n Enter Your Choice = ";
		cin >> n;

		if (n == 1)
			buddySystem();
		else
			break;

	}



	return 0;
}



void buddySystem()
{
	//для поддержки нескольких прогонов в одном исполнении - очищаем все указатели
	root = NULL;
	rootCopy = NULL;
	newnodeLeft = NULL;
	newnodeRight = NULL;
	allocations = 0;
	allocationDenied = 0;
	internalFragmentation = 0;


	for (int clear = 0; clear < 7; clear++)
	{
		freelist[clear] = NULL;
	}
	//создадим первый узел в дереве - 64К памяти, которая на данный момент свободна
	root = new Buddy;
	root->size = 64;
	root->address = 0;
	root->freeSpace = 64;
	root->left = NULL;
	root->right = NULL;
	root->parent = NULL;
	root->allocated = false;
	root->deleteThis = false;
	root->next = NULL;
	root->previous = NULL;

	rootCopy = root;
	freelist[6] = root;

	for (int i = 0; i < 10100; i++)
	{
		if (allocations > 0)
		{
			srand(time(NULL));
			if (rand() % 3 != 0) 
			{
			

				allocateMemoryToProcess();
			} 
			else	
			{
				deallocateMemoryOfProcess();
			}

		}
		else
		{	
			// процесс случайного распределения и освобождения
			allocateMemoryToProcess();
		}

	}


	printStatistics();


}




//Выделение памяти путем разделения дерева 

void allocateMemoryToProcess()
{
	srand(time(NULL));
	processSize = 1 + rand() % 17;
	processValue = 1;
	level = 0;
	while (processValue < processSize)
	{
		processValue = processValue * 2;
		level++;
	}

	allocateThisNode = NULL;
	// поиск свободного узла в списке
	if (freelist[level] != NULL)
	{
		if (freelist[level]->size == value)
		{
			allocateThisNode = freelist[level];
			if (freelist[level]->next != NULL)
			{
				freelist[level] = freelist[level]->next;
				freelist[level]->previous = NULL;
			}
			else
				freelist[level] = NULL;

			allocateThisNode->next = NULL;
			allocateThisNode->previous = NULL;
		}

	}
	else
	{
		nodeFound = false;
		root = rootCopy;
		splitMemory(root);
		if (allocateThisNode != NULL)
			allocateThisNode->deleteThis = true;
		else
		{
			allocationDenied++;
			return;
		}
	}

	
	if (allocateThisNode == NULL)
		return;

	if (allocateThisNode->deleteThis == true)
	{
		level = 0;
		value = 1;
		while (allocateThisNode->size > value)
		{
			value = value * 2;
			level++;
		}

		while (freelist[level]->deleteThis != true)
		{
			freelist[level] = freelist[level]->next;
		}

		if (freelist[level]->previous != NULL)
		{
			freelist[level]->previous->next = freelist[level]->next;
			if (freelist[level]->next != NULL)
				freelist[level]->next->previous = freelist[level]->previous;
		}
		else
		{
			if (freelist[level]->next != NULL)
			{
				freelist[level] = freelist[level]->next;
				freelist[level]->previous = NULL;
			}
			else
				freelist[level] = NULL;
		}

	}

	allocateThisNode->allocated = true;
	allocateThisNode->freeSpace = allocateThisNode->size - processSize;
	allocations++;
	cout << "\n Allocated Size= " << allocateThisNode->size << "--- Process Size = " << processSize;

}


// Разделим дерево  

Buddy* splitMemory(Buddy* tree)
{
	if (tree == NULL)
		return NULL;

	if (tree->right == NULL && tree->left == NULL && tree->size > processValue && tree->allocated == false && nodeFound == false)
	{
		newnodeRight = new Buddy;
		newnodeLeft = new Buddy;

		newnodeRight->parent = tree;
		newnodeLeft->parent = tree;
		newnodeRight->size = tree->size / 2;
		newnodeLeft->size = tree->size / 2;
		newnodeRight->freeSpace = tree->size / 2;
		newnodeLeft->freeSpace = tree->size / 2;
		newnodeRight->allocated = false;
		newnodeLeft->allocated = false;
		newnodeRight->deleteThis = false;
		newnodeLeft->deleteThis = false;
		newnodeLeft->address = tree->address;
		newnodeRight->address = (tree->address + tree->size / 2);
		newnodeRight->left = NULL;
		newnodeRight->right = NULL;
		newnodeLeft->left = NULL;
		newnodeLeft->right = NULL;

	
		tree->left = newnodeLeft;
		tree->right = newnodeRight;
		tree->deleteThis = true;
		value = 1;
		level = 0;
		while (value < tree->size)
		{
			value = value * 2;
			level++;
		}

		while (freelist[level]->deleteThis != true)
		{
			freelist[level] = freelist[level]->next;
		}

		
		freelist[level]->deleteThis = false;
		if (freelist[level]->previous != NULL)
		{
			freelist[level]->previous->next = freelist[level]->next;
			if (freelist[level]->next != NULL)
				freelist[level]->next->previous = freelist[level]->previous;
		}
		else
		{
			if (freelist[level]->next != NULL)
			{
				freelist[level] = freelist[level]->next;
				freelist[level]->previous = NULL;
			}
			else
				freelist[level] = NULL;
		}
		

		// вставим два узла в свободный список
		value = 1;
		level = 0;
		while (value < newnodeLeft->size)
		{
			value = value * 2;
			level++;
		}

		if (freelist[level] != NULL)
		{
			while (freelist[level]->next != NULL)
			{
				freelist[level] = freelist[level]->next;
			}

			freelist[level]->next = newnodeLeft;
			newnodeLeft->previous = freelist[level];
			newnodeLeft->next = newnodeRight;
			newnodeRight->previous = newnodeLeft;
			newnodeRight->next = NULL;
		}

		else
		{
			freelist[level] = newnodeLeft;
			freelist[level]->previous = NULL;
			freelist[level]->next = newnodeRight;
			newnodeRight->previous = newnodeLeft;
			newnodeRight->next = NULL;
		}

		if (newnodeLeft->size == processValue)
		{
			nodeFound = true;
			allocateThisNode = tree->left;
		}

	}

	if (nodeFound == false)
	{
		splitMemory(tree->left);
		splitMemory(tree->right);
	}

}


//Освобождение память и объединение блоков 
void deallocateMemoryOfProcess()
{
	root = rootCopy;
	deleteSuccess = false;
	if (root->left != NULL && root->right != NULL)
	{
		randomSearchAndDelete(root);
	}

	//if(deleteSuccess==false)
	//	cout<<"\n Delete Failed.";

}


//Случайным образом определим удаление 
void randomSearchAndDelete(Buddy* root)
{
	if (root == NULL)
		return;

	if (root->left == NULL && root->right == NULL)
	{
		srand(time(NULL));
		luck = rand() % 10;
		if (luck != 0)
		{
			root->allocated = false;
			root->freeSpace = root->size;
			allocations--;
			cout << "\n Deallocated.";
			deleteSuccess = true;
			if (root->parent != NULL)
			{
				if (root->parent->left->allocated == false && root->parent->right->allocated == false && root->parent->right->right == NULL && root->parent->left->left == NULL)
				{ // объединим два узла

					if (root->parent->left == root)
					{
						
						root->parent->right->deleteThis = true;
						value = 1;
						level = 0;
						while (value < root->parent->right->size)
						{
							value = value * 2;
							level++;
						}

						while (freelist[level]->deleteThis != true)
						{
							freelist[level] = freelist[level]->next;
						}

					
						freelist[level]->deleteThis = false;

						if (freelist[level]->previous != NULL)
						{
							freelist[level]->previous->next = freelist[level]->next;
							if (freelist[level]->next != NULL)
								freelist[level]->next->previous = freelist[level]->previous;
						}
						else
						{
							if (freelist[level]->next != NULL)
							{
								freelist[level] = freelist[level]->next;
								freelist[level]->previous = NULL;
							}
							else
								freelist[level] = NULL;
						}

					}

					else
					{
						// удалим левую часть из списка свободных
						root->parent->left->deleteThis = true;
						value = 1;
						level = 0;
						while (value < root->parent->left->size)
						{
							value = value * 2;
							level++;
						}

						while (freelist[level]->deleteThis != true)
						{
							freelist[level] = freelist[level]->next;
						}

					
						freelist[level]->deleteThis = false;

						if (freelist[level]->previous != NULL)
						{
							freelist[level]->previous->next = freelist[level]->next;
							if (freelist[level]->next != NULL)
								freelist[level]->next->previous = freelist[level]->previous;
						}
						else
						{
							if (freelist[level]->next != NULL)
							{
								freelist[level] = freelist[level]->next;
								freelist[level]->previous = NULL;
							}
							else
								freelist[level] = NULL;
						}


					}

					root->parent->left = NULL;
					root->parent->right = NULL;

					// рекурсивно проверяем до корня, возможно ли слияние всех блоков близнецов или нет
					if (root->parent != NULL)
						recursiveCheck(root->parent);
				}
				else
				{	
					value = 1;
					level = 0;
					while (value < root->size)
					{
						value = value * 2;
						level++;
					}

					if (freelist[level] != NULL)
					{
						while (freelist[level]->next != NULL)
						{
							freelist[level] = freelist[level]->next;
						}
						freelist[level]->next = root;
						root->previous = freelist[level];

					}
					else
					{
						freelist[level] = root;
						freelist[level]->previous = NULL;
						freelist[level]->next = NULL;
					}

				}

			}
		}
	}
	if (deleteSuccess == false)
	{
		randomSearchAndDelete(root->left);
		randomSearchAndDelete(root->right);
	}


}


//Рекурсивно проверим до корня, можно ли объединить близнецов
void recursiveCheck(Buddy* root)
{
	if (root->parent != NULL && root->parent->left->left == NULL && root->parent->left->right == NULL && root->parent->right->left == NULL && root->parent->right->right == NULL)
	{
		if (root->parent->left->allocated == false && root->parent->right->allocated == false)
		{ 

			if (root->parent->left == root)
			{
				// удалим правую часть из свободного списка
				root->parent->right->deleteThis = true;
				value = 1;
				level = 0;
				while (value < root->parent->right->size)
				{
					value = value * 2;
					level++;
				}

				while (freelist[level]->deleteThis != true)
				{
					freelist[level] = freelist[level]->next;
				}

				
				freelist[level]->deleteThis = false;

				if (freelist[level]->previous != NULL)
				{
					freelist[level]->previous->next = freelist[level]->next;
					if (freelist[level]->next != NULL)
						freelist[level]->next->previous = freelist[level]->previous;
				}
				else
				{
					if (freelist[level]->next != NULL)
					{
						freelist[level] = freelist[level]->next;
						freelist[level]->previous = NULL;
					}
					else
						freelist[level] = NULL;
				}

			}

			else
			{
				// удаляем левую часть из списка свободных
				root->parent->left->deleteThis = true;
				value = 1;
				level = 0;
				while (value < root->parent->left->size)
				{
					value = value * 2;
					level++;
				}

				while (freelist[level]->deleteThis != true)
				{
					freelist[level] = freelist[level]->next;
				}

				
				freelist[level]->deleteThis = false;

				if (freelist[level]->previous != NULL)
				{
					freelist[level]->previous->next = freelist[level]->next;
					if (freelist[level]->next != NULL)
						freelist[level]->next->previous = freelist[level]->previous;
				}
				else
				{
					if (freelist[level]->next != NULL)
					{
						freelist[level] = freelist[level]->next;
						freelist[level]->previous = NULL;
					}
					else
						freelist[level] = NULL;
				}


			}

			root->parent->left = NULL;
			root->parent->right = NULL;

			// рекурсивно проверяем до корня, возможно ли слияние всех блоков близнецов или нет
			recursiveCheck(root->parent);
		}
		else
		{
			
			if (root->parent->left->allocated == true)
			{ 

				value = 1;
				level = 0;
				while (value < root->parent->right->size)
				{
					value = value * 2;
					level++;
				}

				if (freelist[level] != NULL)
				{
					while (freelist[level]->next != NULL)
					{
						freelist[level] = freelist[level]->next;
					}
					freelist[level]->next = root->parent->right;
					root->parent->right->previous = freelist[level];

				}
				else
				{
					freelist[level] = root->parent->right;
					freelist[level]->previous = NULL;
					freelist[level]->next = NULL;
				}


			}
			else
			{
				value = 1;
				level = 0;
				while (value < root->parent->left->size)
				{
					value = value * 2;
					level++;
				}

				if (freelist[level] != NULL)
				{
					while (freelist[level]->next != NULL)
					{
						freelist[level] = freelist[level]->next;
					}
					freelist[level]->next = root->parent->left;
					root->parent->left->previous = freelist[level];

				}
				else
				{
					freelist[level] = root->parent->left;
					freelist[level]->previous = NULL;
					freelist[level]->next = NULL;
				}


			}


		}


	}
	else
	{ // мы находимся в корневом узле, поэтому вставляем фрагмент размером 64 КБ в свободном списке.

		if (root->parent == NULL)
		{
			value = 1;
			level = 0;
			while (value < root->size)
			{
				value = value * 2;
				level++;
			}

			if (freelist[level] != NULL)
			{
				while (freelist[level]->next != NULL)
				{
					freelist[level] = freelist[level]->next;
				}
				freelist[level]->next = root;
				root->previous = freelist[level];

			}
			else
			{
				
				freelist[level] = root;
				freelist[level]->previous = NULL;
				freelist[level]->next = NULL;
			}


		}


	}



}


// Вывод статистики
void printStatistics()
{
	root = rootCopy;
	computeInternalFragmentation(root);
	cout << "\n Internal Fragmentation (KB)= " << internalFragmentation << " -----% " << ((float)internalFragmentation / 64); //Внутренняя фрагментация (КБ)
	cout << "\n # of Allocation Requests Denied = " << allocationDenied; // Количество отклоненных запросов на выделение
	cout << "\n External Fragmentation = 0";// Внешняя фрагментация = 0

}

//Рассчёт внутренней фрагментации 
void computeInternalFragmentation(Buddy* root)
{
	if (root != NULL && root->freeSpace < root->size)
		internalFragmentation += root->freeSpace;
	else if (root == NULL)
		return;

	computeInternalFragmentation(root->left);
	computeInternalFragmentation(root->right);


}