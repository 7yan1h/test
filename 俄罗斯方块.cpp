// ����˹����.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include <iostream>
using namespace std;
#include   <stdlib.h>      
#include   <time.h>  
#include<conio.h>
#include <windows.h>
#include <WinBase.h>
#include <ctime>
#include <string>
/* �궨�� */
#define HEIGHT	20 
#define LENGTH	13//��ͼ��Χ 
//
#define MIN 0 
#define MAX 18//�������Χ 
/* //////////// */
int Map[HEIGHT][LENGTH];     /* ��ͼ */
int score;//�÷� 
string name;//������� 
int dividend = 4;//������ 
int multiple = 10000;
int difficulty = 300;//�Ѷ� 
					 /* ��������*/
typedef struct {
	int x, y;
}Point;
//ͼ�� 
typedef struct {
	int	graphId;        /* 19��ͼ�εı�ʶ��*/
	Point point[4];//����ͼ��4���� 
	Point delta[4];//ת������һ��ͼ����Ҫ������ 
	int NextGraphId;//��һ��ͼ�εı�ʶ�� 
}graph;
/////////////////////////�������� 
void Start();
int Rand(int *a, int *b);
void Conversion(graph *Graph);
void InitGraph(graph *Graph);
void KeyPress(graph *Graph);
void AddToMap(graph *A_Graph);
void MapPrint(int NextGraphId);
void Clear(graph *Graph);
void AutoDown(graph *Graph);
void IsEnd(graph *Graph);
int NeedCreate(graph *Graph);
int CanConversion(graph *Graph);
void IsBeyondLR(graph *Graph);
void CountScoreAndVanish(graph *Graph);
///////////////����ʵ�� 
//........................................
void Start()
{
	score = 0;
	cout << "������һ���������ʼ�Ѷ�(�Ƽ�300),�����س�����ʼ��Ϸ��(�Ѷ������ֳɷ����)" << endl;
	cin >> difficulty;
	while (difficulty > 2000 || difficulty < 0)
	{
		cout << "���������Ϊ�������Ѷȹ��ͣ���������" << endl;
		cin >> difficulty;
	}
	graph Graph;//���嵱ǰͼ��  
	Graph.graphId = Graph.NextGraphId = 0;
	/* ��ʼ����ͼ */
	for (int i = 0; i < HEIGHT - 1; i++)
	{
		for (int j = 0; j < LENGTH; j++)
		{
			Map[i][j] = 0;//����� 
		}
	}
	for (int j = 0; j < LENGTH; j++)
	{
		Map[HEIGHT - 1][j] = 1;//����߽�,2����ͼ�� 
	}
	Rand(&Graph.graphId, &Graph.NextGraphId);
	InitGraph(&Graph);
	AddToMap(&Graph);
	MapPrint(Graph.NextGraphId);
	while (1)
	{
		if (kbhit())
		{
			KeyPress(&Graph);
		}
		else
		{
			AutoDown(&Graph);
		}
	}
}
////ͳ�Ƶ÷ֲ����� 
void CountScoreAndVanish(graph *Graph)
{
	for (int i = HEIGHT - 2; i > -1; i--)
	{
		int label = 1;//����i�п����� 
		for (int j = 0; j < LENGTH - 1; j++)
		{
			if (Map[i][j] != 1)
				label = 0;
		}
		if (label)
		{
			for (int m = i; m > -1; m--)
			{
				if (m > 0)
					for (int n = 0; n < LENGTH - 1; n++)
					{
						Map[m][n] = Map[m - 1][n];
					}
				else if (m == 0)
					for (int n = 0; n < LENGTH - 1; n++)
					{
						Map[m][n] = 0;
					}
			}
			score += 1 * multiple;
			if (score != 0 && score%dividend == 0 && difficulty > 20)
			{
				dividend += 4;
				if (difficulty > 300)
					difficulty = 300;
				else
					difficulty -= 20;
			}
			i++;
		}
	}
}
///�Ƿ񳬳����ұ߽� 
void IsBeyondLR(graph *Graph)
{
	int flag = 0;//����û�г������ұ߽�
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].x > LENGTH - 2)
		{
			for (int j = 0; j < 4; j++)
			{
				Graph->point[j].x -= 1;
			}
			flag = 1;
			continue;
		}
		if (Graph->point[i].x < 0)
		{
			for (int j = 0; j < 4; j++)
			{
				Graph->point[j].x += 1;
			}
			flag = 1;
			continue;
		}
	}
	if (flag == 1)
		IsBeyondLR(Graph);
}
//�ܷ���κ��� 
int CanConversion(graph *Graph)
{
	int flag = 0;//��ʾ���Ա��� 
				 //�ж��ܷ���� 
	for (int i = 0; i < 4; i++)
	{
		if (((Graph->point[i].y + Graph->delta[i].y) > -1))
		{
			if (Map[Graph->point[i].y + Graph->delta[i].y][Graph->point[i].x + Graph->delta[i].x] == 1)
			{
				flag = 1;
			}
		}
	}
	if (!flag)
	{
		//�ܱ��� 
		Clear(Graph);
		switch (Graph->graphId)
		{
		case 1:
		case 11:
		case 13:
			Graph->graphId -= 1;
			break;
		case 5:
		case 9:
		case 18:
			Graph->graphId -= 3;
			break;
		default:
			Graph->graphId += 1;
		}
		//////
		graph TGraph = *Graph;//������һ��ͼ�ε���Ϣ 
		InitGraph(Graph);//�任���ͼ�Ρ� 
						 //
		for (int i = 0; i < 4; i++)
		{
			Graph->point[i].x = TGraph.point[i].x + TGraph.delta[i].x;	Graph->point[i].y = TGraph.point[i].y + TGraph.delta[i].y;
		}
	}
	if (!flag)
		return 0;//�ܱ��� 
	else
		return 1;//���ܱ��� 
}

////ͼ�ε���ײ�����һ���Ƿ��� 
int NeedCreate(graph *Graph)
{
	int label = 0;
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > (HEIGHT - 3))//ͼ�ε���ײ� 
		{
			for (int k = 0; k < 4; k++)
			{
				if (Graph->point[k].y > -1)
					Map[Graph->point[k].y][Graph->point[k].x] = 1;
			}
			label = 1;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y + 1 > -1)
			if (Map[Graph->point[i].y + 1][Graph->point[i].x] == 1)//ͼ��������һ�������һ���Ǳ߽� 
			{
				for (int k = 0; k < 4; k++)
				{
					if (Graph->point[k].y > -1)
						Map[Graph->point[k].y][Graph->point[k].x] = 1;
				}
				label = 1;
			}
	}
	if (label)
		return 1;//ͼ��������һ���㵽��ײ�
	else
		return 0;//ͼ��û��һ���㵽��ײ� 
}
///////ͼ���Զ����亯�� 
void AutoDown(graph *Graph)
{
	if (!NeedCreate(Graph))//û����ײ��ͱ߽� 
	{
		Clear(Graph);
		for (int i = 0; i < 4; i++)
			Graph->point[i].y += 1;
		AddToMap(Graph);
	}
	else//����ײ���߽� 
	{
		//������ͼ�� 
		CountScoreAndVanish(Graph);
		Graph->graphId = Graph->NextGraphId;
		InitGraph(Graph);
		AddToMap(Graph);
		Graph->NextGraphId = Rand(&Graph->NextGraphId, &Graph->NextGraphId);
	}

	MapPrint(Graph->NextGraphId);
	//�ж����޼����� 
	for (int i = 0; i < difficulty; i++)
	{
		if (kbhit())
		{
			KeyPress(Graph);
		}
		else
			Sleep(1);
	}
}

//����ͼ�κ��� 
void InitGraph(graph *Graph)
{
	switch (Graph->graphId)
	{
	case 0:
		Graph->point[0].x = 5;	Graph->point[0].y = -3;	Graph->delta[0].x = 3;	Graph->delta[0].y = 3;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = -2;	Graph->delta[1].x = 2;	Graph->delta[1].y = 2;
		//
		Graph->point[2].x = 5;	Graph->point[2].y = -1;	Graph->delta[2].x = 1;	Graph->delta[2].y = 1;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 0;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	case 1:
		Graph->point[0].x = 8;	Graph->point[0].y = 0;	Graph->delta[0].x = -3;	Graph->delta[0].y = -3;
		//
		Graph->point[1].x = 7;	Graph->point[1].y = 0;	Graph->delta[1].x = -2;	Graph->delta[1].y = -2;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = 0;	Graph->delta[2].x = -1;	Graph->delta[2].y = -1;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 0;	Graph->delta[3].y = -0;
		//
		IsEnd(Graph);
		break;
	case 2:
		Graph->point[0].x = 6;	Graph->point[0].y = 0;	Graph->delta[0].x = -1;	Graph->delta[0].y = 0;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = 0;	Graph->delta[1].x = 0;	Graph->delta[1].y = -1;
		//
		Graph->point[2].x = 5;	Graph->point[2].y = -1;	Graph->delta[2].x = 1;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = -2;	Graph->delta[3].x = 2;	Graph->delta[3].y = 1;
		//
		IsEnd(Graph);
		break;
	case 3:
		Graph->point[0].x = 5;	Graph->point[0].y = 0;	Graph->delta[0].x = 0;	Graph->delta[0].y = -2;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = -1;	Graph->delta[1].x = 1;	Graph->delta[1].y = -1;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 7;	Graph->point[3].y = -1;	Graph->delta[3].x = -1;	Graph->delta[3].y = 1;
		//
		IsEnd(Graph);
		break;
	case 4:
		Graph->point[0].x = 5;	Graph->point[0].y = -2;	Graph->delta[0].x = 2;	Graph->delta[0].y = 1;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = -2;	Graph->delta[1].x = 1;	Graph->delta[1].y = 2;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 1;
		//
		Graph->point[3].x = 6;	Graph->point[3].y = 0;	Graph->delta[3].x = -1;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	case 5:
		Graph->point[0].x = 7;	Graph->point[0].y = -1;	Graph->delta[0].x = 0;	Graph->delta[0].y = 1;
		//
		Graph->point[1].x = 7;	Graph->point[1].y = 0;	Graph->delta[1].x = -1;	Graph->delta[1].y = 0;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = 0;	Graph->delta[2].x = 0;	Graph->delta[2].y = -1;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 1;	Graph->delta[3].y = -2;
		//
		IsEnd(Graph);
		break;
	case 6:
		Graph->point[0].x = 5;	Graph->point[0].y = 0;	Graph->delta[0].x = 0;	Graph->delta[0].y = -1;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = 0;	Graph->delta[1].x = -1;	Graph->delta[1].y = 0;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 1;
		//
		Graph->point[3].x = 6;	Graph->point[3].y = -2;	Graph->delta[3].x = 1;	Graph->delta[3].y = 2;
		//
		IsEnd(Graph);
		break;
	case 7:
		Graph->point[0].x = 5;	Graph->point[0].y = -1;	Graph->delta[0].x = 1;	Graph->delta[0].y = -1;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = 0;	Graph->delta[1].x = 0;	Graph->delta[1].y = -2;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = 0;	Graph->delta[2].x = -1;	Graph->delta[2].y = -1;
		//
		Graph->point[3].x = 7;	Graph->point[3].y = 0;	Graph->delta[3].x = -2;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	case 8:
		Graph->point[0].x = 6;	Graph->point[0].y = -2;	Graph->delta[0].x = 1;	Graph->delta[0].y = 2;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = -2;	Graph->delta[1].x = 2;	Graph->delta[1].y = 1;
		//
		Graph->point[2].x = 5;	Graph->point[2].y = -1;	Graph->delta[2].x = 1;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 0;	Graph->delta[3].y = -1;
		//
		IsEnd(Graph);
		break;
	case 9:
		Graph->point[0].x = 7;	Graph->point[0].y = 0;	Graph->delta[0].x = -1;	Graph->delta[0].y = 0;
		//
		Graph->point[1].x = 7;	Graph->point[1].y = -1;	Graph->delta[1].x = 0;	Graph->delta[1].y = 1;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 1;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = -1;	Graph->delta[3].x = 2;	Graph->delta[3].y = -1;
		//
		IsEnd(Graph);
		break;
	case 10:
		Graph->point[0].x = 5;	Graph->point[0].y = -2;	Graph->delta[0].x = 2;	Graph->delta[0].y = 1;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = -1;	Graph->delta[1].x = 1;	Graph->delta[1].y = 0;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 1;
		//
		Graph->point[3].x = 6;	Graph->point[3].y = 0;	Graph->delta[3].x = -1;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	case 11:
		Graph->point[0].x = 7;	Graph->point[0].y = -1;	Graph->delta[0].x = -2;	Graph->delta[0].y = -1;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = -1;	Graph->delta[1].x = -1;	Graph->delta[1].y = 0;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = 0;	Graph->delta[2].x = 0;	Graph->delta[2].y = -1;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 1;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	case 12:
		Graph->point[0].x = 6;	Graph->point[0].y = -2;	Graph->delta[0].x = 1;	Graph->delta[0].y = 2;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = -1;	Graph->delta[1].x = 0;	Graph->delta[1].y = 1;
		//
		Graph->point[2].x = 5;	Graph->point[2].y = -1;	Graph->delta[2].x = 1;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 0;	Graph->delta[3].y = -1;
		//
		IsEnd(Graph);
		break;
	case 13:
		Graph->point[0].x = 7;	Graph->point[0].y = 0;	Graph->delta[0].x = 0;	Graph->delta[0].y = -2;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = 0;	Graph->delta[1].x = 1;	Graph->delta[1].y = -1;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = -1;	Graph->delta[3].x = 1;	Graph->delta[3].y = 1;
		//
		IsEnd(Graph);
		break;
	case 14:
		Graph->point[0].x = 5;	Graph->point[0].y = -1;	Graph->delta[0].x = 0;	Graph->delta[0].y = 0;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = -1;	Graph->delta[1].x = 0;	Graph->delta[1].y = 0;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = 0;	Graph->delta[2].x = 0;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 0;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	case 15:
		Graph->point[0].x = 6;	Graph->point[0].y = -1;	Graph->delta[0].x = 0;	Graph->delta[0].y = 1;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = -2;	Graph->delta[1].x = 2;	Graph->delta[1].y = 1;
		//
		Graph->point[2].x = 5;	Graph->point[2].y = -1;	Graph->delta[2].x = 1;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = 0;	Graph->delta[3].x = 0;	Graph->delta[3].y = -1;
		//
		IsEnd(Graph);
		break;
	case 16:
		Graph->point[0].x = 6;	Graph->point[0].y = 0;	Graph->delta[0].x = -1;	Graph->delta[0].y = -1;
		//
		Graph->point[1].x = 7;	Graph->point[1].y = -1;	Graph->delta[1].x = -1;	Graph->delta[1].y = 1;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 0;
		//
		Graph->point[3].x = 5;	Graph->point[3].y = -1;	Graph->delta[3].x = 1;	Graph->delta[3].y = -1;
		//
		IsEnd(Graph);
		break;
	case 17:
		Graph->point[0].x = 5;	Graph->point[0].y = -1;	Graph->delta[0].x = 1;	Graph->delta[0].y = 0;
		//
		Graph->point[1].x = 6;	Graph->point[1].y = 0;	Graph->delta[1].x = -1;	Graph->delta[1].y = 0;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = -1;	Graph->delta[2].x = 0;	Graph->delta[2].y = 1;
		//
		Graph->point[3].x = 6;	Graph->point[3].y = -2;	Graph->delta[3].x = 1;	Graph->delta[3].y = 2;
		//
		IsEnd(Graph);
		break;
	case 18:
		Graph->point[0].x = 6;	Graph->point[0].y = -1;	Graph->delta[0].x = 1;	Graph->delta[0].y = 0;
		//
		Graph->point[1].x = 5;	Graph->point[1].y = 0;	Graph->delta[1].x = 1;	Graph->delta[1].y = -2;
		//
		Graph->point[2].x = 6;	Graph->point[2].y = 0;	Graph->delta[2].x = 0;	Graph->delta[2].y = -1;
		//
		Graph->point[3].x = 7;	Graph->point[3].y = 0;	Graph->delta[3].x = -1;	Graph->delta[3].y = 0;
		//
		IsEnd(Graph);
		break;
	}
}

/*�������������*/
int Rand(int *a, int *b)
{	
	srand(time(NULL));
	*a = rand() % 18;
	*b = rand() % 18;
	return *a;
}

/* ��Ļˢ�º��� */


void MapPrint(int NextGraphId)
{
	//	��ʾ��ͼ 
	system("cls");//����Ļ 
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[0][j] == 0)//�� 
			cout << "  ";
		if (Map[0][j] == 1 || Map[0][j] == 2)//�߽��ͼ�� 
			cout << "��";
	}
	cout << "||";//�ұ߽� 
	cout << "                            ";
	if (NextGraphId == 0)
		cout << "��";
	cout << "      ��ǰ�Ѷ���" << difficulty;
	cout << endl;
	////first line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[1][j] == 0)//�� 
			cout << "  ";
		if (Map[1][j] == 1 || Map[1][j] == 2)//�߽��ͼ�� 
			cout << "��";
	}
	cout << "||";//�ұ߽� 
	cout << "      ��һ��ͼ����          ";
	switch (NextGraphId)
	{
	case 0:
	case 2:
	case 10:
	case 15:
		cout << "��" << endl;
		break;
	case 4:
	case 8:
		cout << "����" << endl;
		break;
	case 6:
	case 12:
	case 17:
		cout << "  ��" << endl;
		break;
	default:
		cout << endl;
	}
	////second line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[2][j] == 0)//�� 
			cout << "  ";
		if (Map[2][j] == 1 || Map[2][j] == 2)//�߽��ͼ�� 
			cout << "��";
	}
	cout << "||";//�ұ߽� 
	cout << "                            ";
	switch (NextGraphId)
	{
	case 0:
	case 2:
	case 7:
	case 8:
		cout << "��" << endl;
		break;
	case 1:
		cout << endl;
		break;
	case 3:
	case 9:
	case 16:
		cout << "������" << endl;
		break;
	case 4:
	case 6:
	case 18:
		cout << "  ��" << endl;
		break;
	case 5:
		cout << "    ��" << endl;
		break;
	case 10:
	case 12:
	case 13:
	case 14:
	case 15:
	case 17:
		cout << "����" << endl;
		break;
	case 11:
		cout << "  ����" << endl;
		break;
	}
	////third line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[3][j] == 0)//�� 
			cout << "  ";
		if (Map[3][j] == 1 || Map[3][j] == 2)//�߽��ͼ�� 
			cout << "��";
	}
	cout << "||";//�ұ߽� 
	cout << "                            ";
	switch (NextGraphId)
	{
	case 0:
	case 3:
	case 8:
	case 12:
	case 15:
		cout << "��" << endl;
		break;
	case 1:
		cout << "��������" << endl;
		break;
	case 2:
	case 6:
	case 11:
	case 14:
		cout << "����" << endl;
		break;
	case 4:
	case 10:
	case 16:
	case 17:
		cout << "  ��" << endl;
		break;
	case 5:
	case 7:
	case 18:
		cout << "������" << endl;
		break;
	case 9:
		cout << "    ��" << endl;
		break;
	case 13:
		cout << "  ����" << endl;
		break;
	}
	////fourth line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[4][j] == 0)//�� 
			cout << "  ";
		if (Map[4][j] == 1 || Map[4][j] == 2)//�߽��ͼ�� 
			cout << "��";
	}
	cout << "||";//�ұ߽� 
	cout << "      " << name << ",��ĵ�ǰ�÷�Ϊ:  " << score << endl;
	for (int i = 5; i < HEIGHT - 1; i++)
	{
		for (int j = 0; j < LENGTH - 1; j++)
		{
			if (Map[i][j] == 0)//�� 
				cout << "  ";
			if (Map[i][j] == 1 || Map[i][j] == 2)//�߽��ͼ�� 
				cout << "��";
		}
		cout << "||" << endl;//�ұ߽� 
	}
	for (int i = 0; i < LENGTH; i++)
		cout << "һ";
}
//ͼ�α任�������ܱ任���ܵ����������,KeyPress�������ж��ܷ���ã������������ǽҲ������ƽ�ơ� 
void	Conversion(graph *C_Graph)
{
	if (!CanConversion(C_Graph))
		IsBeyondLR(C_Graph);
}
//��ǰͼ���������е�λ����� �� 
void AddToMap(graph *Graph)
{
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > -1)
			Map[Graph->point[i].y][Graph->point[i].x] = 2;//2����ͼ�� 
	}
}
//�ж���Ϸ�Ƿ����
void IsEnd(graph *Graph)
{
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > -1)
		{
			if (Map[Graph->point[i].y][Graph->point[i].x] == 1)
			{
				system("cls");//����Ļ 
				if (score < 15 * multiple)
					cout << "��̨��û˯����!��ĵ÷�Ϊ��" << score << endl;
				else if (score > 14 * multiple && score < 30 * multiple)
					cout << "�����̶�������棡��ĵ÷�Ϊ��" << score << endl;
				else if (score > 29 * multiple && score < 45 * multiple)
					cout << "���ͦ��Ŷ(*^_^*)������Ŭ������ĵ÷�Ϊ��" << score << endl;
				else if (score > 44 * multiple)
					cout << "�Ѳ�����Ҳ�Ǵ�������λ������һ��������ŷ�ɥ���Ĵ��������Ļ��ε�ͽ������ĵ÷�Ϊ��" << score << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "��g��G������һ����Ϸ����q��Q�˳���Ϸ" << endl;
				char c;
				c = getch();
				if (c == 'g' ||c == 'G')
				{
					Start();
				}
				else if (c == 'q' || c == 'Q')
				{
					exit(0);
				}
			}
		}
	}
}
///////����һ��ͼ�ε�λ����䡰 �� 
void Clear(graph *Graph)
{
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > -1)
			Map[Graph->point[i].y][Graph->point[i].x] = 0;
	}
}
/*���������� */


void KeyPress(graph *K_Graph)
{
	int c = _getch();
	if ((c == 'w' || c == 'W') && K_Graph->graphId != 14)
	{
		Conversion(K_Graph);
		AddToMap(K_Graph);
		MapPrint(K_Graph->NextGraphId);
	}
	else if (c == 's' || c == 'S')
	{
		if (!NeedCreate(K_Graph))//û����ײ��ͱ߽� 
		{
			Clear(K_Graph);
			for (int i = 0; i < 4; i++)
				K_Graph->point[i].y += 1;
			AddToMap(K_Graph);
		}
		else//����ײ���߽� 
		{
			//������ͼ�� 
			CountScoreAndVanish(K_Graph);
			K_Graph->graphId = K_Graph->NextGraphId;
			InitGraph(K_Graph);
			AddToMap(K_Graph);
			K_Graph->NextGraphId = Rand(&K_Graph->NextGraphId, &K_Graph->NextGraphId);
		}
		MapPrint(K_Graph->NextGraphId);
	}
	else if (c == 'a' || c == 'A')
	{
		int	label = 0;
		for (int i = 0; i < 4; i++)
		{
			if (K_Graph->point[i].y > -1)
				if (K_Graph->point[i].x == 0 || Map[K_Graph->point[i].y][K_Graph->point[i].x - 1] == 1)//ͼ��û��һ���㵽������߻����һ���Ǳ߽� 
				{
					label = 1;
				}
		}
		if (!label)
		{
			Clear(K_Graph);
			for (int i = 0; i < 4; i++)
				K_Graph->point[i].x -= 1;
			AddToMap(K_Graph);
		}
		MapPrint(K_Graph->NextGraphId);
	}
	else if (c == 'd' || c == 'D')
	{
		int label = 0;
		for (int i = 0; i < 4; i++)
		{
			if (K_Graph->point[i].y > -1)
				if (K_Graph->point[i].x == (LENGTH - 2) || Map[K_Graph->point[i].y][K_Graph->point[i].x + 1] == 1)//ͼ��û��һ���㵽�����ұ߻��ұ�һ���Ǳ߽� 
				{
					label = 1;
				}
		}
		if (!label)
		{
			Clear(K_Graph);
			for (int i = 0; i < 4; i++)
				K_Graph->point[i].x += 1;
			AddToMap(K_Graph);
		}
		MapPrint(K_Graph->NextGraphId);
	}
	else if (c == 27)//ESC�� 
	{
		exit(0);
	}
}
//////////////������ 
int main(void)
{
	cout << "......................����˹������Ϸ����................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "1. �������Ӣ�����뷨�½�����Ϸ������...................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "2. W(w)�����Σ�A(a)�������ƶ���S(s)�������ƶ���D(d)�������ƶ�;Esc�˳���Ϸ" << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "��������´���,���س���ȷ��" << endl;
	cin >> name;
	while (name == "����")
	{
		cout << "�����������θ�����֣�����������" << endl;
		cin >> name;
	}
	Start();
	return(0);
}


