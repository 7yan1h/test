// 俄罗斯方块.cpp : 定义控制台应用程序的入口点。
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
/* 宏定义 */
#define HEIGHT	20 
#define LENGTH	13//地图范围 
//
#define MIN 0 
#define MAX 18//随机数范围 
/* //////////// */
int Map[HEIGHT][LENGTH];     /* 地图 */
int score;//得分 
string name;//玩家姓名 
int dividend = 4;//被除数 
int multiple = 10000;
int difficulty = 300;//难度 
					 /* 定义坐标*/
typedef struct {
	int x, y;
}Point;
//图形 
typedef struct {
	int	graphId;        /* 19种图形的标识符*/
	Point point[4];//定义图形4个点 
	Point delta[4];//转换到下一个图形需要的增量 
	int NextGraphId;//下一个图形的标识符 
}graph;
/////////////////////////函数声明 
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
///////////////函数实现 
//........................................
void Start()
{
	score = 0;
	cout << "请输入一个数代表初始难度(推荐300),并按回车键开始游戏！(难度与数字成反相关)" << endl;
	cin >> difficulty;
	while (difficulty > 2000 || difficulty < 0)
	{
		cout << "输入的数字为负数或难度过低！，请重输" << endl;
		cin >> difficulty;
	}
	graph Graph;//定义当前图形  
	Graph.graphId = Graph.NextGraphId = 0;
	/* 初始化地图 */
	for (int i = 0; i < HEIGHT - 1; i++)
	{
		for (int j = 0; j < LENGTH; j++)
		{
			Map[i][j] = 0;//代表空 
		}
	}
	for (int j = 0; j < LENGTH; j++)
	{
		Map[HEIGHT - 1][j] = 1;//代表边界,2代表图形 
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
////统计得分并消除 
void CountScoreAndVanish(graph *Graph)
{
	for (int i = HEIGHT - 2; i > -1; i--)
	{
		int label = 1;//假设i行可消除 
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
///是否超出左右边界 
void IsBeyondLR(graph *Graph)
{
	int flag = 0;//代表没有超出左右边界
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
//能否变形函数 
int CanConversion(graph *Graph)
{
	int flag = 0;//表示可以变形 
				 //判断能否变形 
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
		//能变形 
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
		graph TGraph = *Graph;//保存上一个图形的信息 
		InitGraph(Graph);//变换后的图形。 
						 //
		for (int i = 0; i < 4; i++)
		{
			Graph->point[i].x = TGraph.point[i].x + TGraph.delta[i].x;	Graph->point[i].y = TGraph.point[i].y + TGraph.delta[i].y;
		}
	}
	if (!flag)
		return 0;//能变形 
	else
		return 1;//不能变形 
}

////图形到达底部或下一格是方块 
int NeedCreate(graph *Graph)
{
	int label = 0;
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > (HEIGHT - 3))//图形到达底部 
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
			if (Map[Graph->point[i].y + 1][Graph->point[i].x] == 1)//图形至少有一个点的下一格是边界 
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
		return 1;//图形至少有一个点到达底部
	else
		return 0;//图形没有一个点到达底部 
}
///////图形自动下落函数 
void AutoDown(graph *Graph)
{
	if (!NeedCreate(Graph))//没到达底部和边界 
	{
		Clear(Graph);
		for (int i = 0; i < 4; i++)
			Graph->point[i].y += 1;
		AddToMap(Graph);
	}
	else//到达底部或边界 
	{
		//产生新图形 
		CountScoreAndVanish(Graph);
		Graph->graphId = Graph->NextGraphId;
		InitGraph(Graph);
		AddToMap(Graph);
		Graph->NextGraphId = Rand(&Graph->NextGraphId, &Graph->NextGraphId);
	}

	MapPrint(Graph->NextGraphId);
	//判断有无键按下 
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

//产生图形函数 
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

/*随机数产生函数*/
int Rand(int *a, int *b)
{	
	srand(time(NULL));
	*a = rand() % 18;
	*b = rand() % 18;
	return *a;
}

/* 屏幕刷新函数 */


void MapPrint(int NextGraphId)
{
	//	显示地图 
	system("cls");//清屏幕 
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[0][j] == 0)//空 
			cout << "  ";
		if (Map[0][j] == 1 || Map[0][j] == 2)//边界或图形 
			cout << "■";
	}
	cout << "||";//右边界 
	cout << "                            ";
	if (NextGraphId == 0)
		cout << "■";
	cout << "      当前难度是" << difficulty;
	cout << endl;
	////first line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[1][j] == 0)//空 
			cout << "  ";
		if (Map[1][j] == 1 || Map[1][j] == 2)//边界或图形 
			cout << "■";
	}
	cout << "||";//右边界 
	cout << "      下一个图形是          ";
	switch (NextGraphId)
	{
	case 0:
	case 2:
	case 10:
	case 15:
		cout << "■" << endl;
		break;
	case 4:
	case 8:
		cout << "■■" << endl;
		break;
	case 6:
	case 12:
	case 17:
		cout << "  ■" << endl;
		break;
	default:
		cout << endl;
	}
	////second line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[2][j] == 0)//空 
			cout << "  ";
		if (Map[2][j] == 1 || Map[2][j] == 2)//边界或图形 
			cout << "■";
	}
	cout << "||";//右边界 
	cout << "                            ";
	switch (NextGraphId)
	{
	case 0:
	case 2:
	case 7:
	case 8:
		cout << "■" << endl;
		break;
	case 1:
		cout << endl;
		break;
	case 3:
	case 9:
	case 16:
		cout << "■■■" << endl;
		break;
	case 4:
	case 6:
	case 18:
		cout << "  ■" << endl;
		break;
	case 5:
		cout << "    ■" << endl;
		break;
	case 10:
	case 12:
	case 13:
	case 14:
	case 15:
	case 17:
		cout << "■■" << endl;
		break;
	case 11:
		cout << "  ■■" << endl;
		break;
	}
	////third line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[3][j] == 0)//空 
			cout << "  ";
		if (Map[3][j] == 1 || Map[3][j] == 2)//边界或图形 
			cout << "■";
	}
	cout << "||";//右边界 
	cout << "                            ";
	switch (NextGraphId)
	{
	case 0:
	case 3:
	case 8:
	case 12:
	case 15:
		cout << "■" << endl;
		break;
	case 1:
		cout << "■■■■" << endl;
		break;
	case 2:
	case 6:
	case 11:
	case 14:
		cout << "■■" << endl;
		break;
	case 4:
	case 10:
	case 16:
	case 17:
		cout << "  ■" << endl;
		break;
	case 5:
	case 7:
	case 18:
		cout << "■■■" << endl;
		break;
	case 9:
		cout << "    ■" << endl;
		break;
	case 13:
		cout << "  ■■" << endl;
		break;
	}
	////fourth line is over
	for (int j = 0; j < LENGTH - 1; j++)
	{
		if (Map[4][j] == 0)//空 
			cout << "  ";
		if (Map[4][j] == 1 || Map[4][j] == 2)//边界或图形 
			cout << "■";
	}
	cout << "||";//右边界 
	cout << "      " << name << ",你的当前得分为:  " << score << endl;
	for (int i = 5; i < HEIGHT - 1; i++)
	{
		for (int j = 0; j < LENGTH - 1; j++)
		{
			if (Map[i][j] == 0)//空 
				cout << "  ";
			if (Map[i][j] == 1 || Map[i][j] == 2)//边界或图形 
				cout << "■";
		}
		cout << "||" << endl;//右边界 
	}
	for (int i = 0; i < LENGTH; i++)
		cout << "一";
}
//图形变换函数，能变换才能调用这个函数,KeyPress（）中判断能否调用，并且左或右是墙也由它先平移。 
void	Conversion(graph *C_Graph)
{
	if (!CanConversion(C_Graph))
		IsBeyondLR(C_Graph);
}
//当前图形在数组中的位置添加 ■ 
void AddToMap(graph *Graph)
{
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > -1)
			Map[Graph->point[i].y][Graph->point[i].x] = 2;//2代表图形 
	}
}
//判断游戏是否结束
void IsEnd(graph *Graph)
{
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > -1)
		{
			if (Map[Graph->point[i].y][Graph->point[i].x] == 1)
			{
				system("cls");//清屏幕 
				if (score < 15 * multiple)
					cout << "兄台，没睡醒吗!你的得分为：" << score << endl;
				else if (score > 14 * multiple && score < 30 * multiple)
					cout << "我奶奶都比你会玩！你的得分为：" << score << endl;
				else if (score > 29 * multiple && score < 45 * multiple)
					cout << "玩得挺溜哦(*^_^*)，继续努力！你的得分为：" << score << endl;
				else if (score > 44 * multiple)
					cout << "难不成你也是传送中那位万中无一，令江湖人闻风丧胆的大名鼎鼎的黄鑫的徒弟吗？你的得分为：" << score << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "............................." << endl;
				cout << "按g或G进行新一局游戏；按q或Q退出游戏" << endl;
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
///////把上一个图形的位置填充“ ” 
void Clear(graph *Graph)
{
	for (int i = 0; i < 4; i++)
	{
		if (Graph->point[i].y > -1)
			Map[Graph->point[i].y][Graph->point[i].x] = 0;
	}
}
/*按键处理函数 */


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
		if (!NeedCreate(K_Graph))//没到达底部和边界 
		{
			Clear(K_Graph);
			for (int i = 0; i < 4; i++)
				K_Graph->point[i].y += 1;
			AddToMap(K_Graph);
		}
		else//到达底部或边界 
		{
			//产生新图形 
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
				if (K_Graph->point[i].x == 0 || Map[K_Graph->point[i].y][K_Graph->point[i].x - 1] == 1)//图形没有一个点到达最左边或左边一格是边界 
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
				if (K_Graph->point[i].x == (LENGTH - 2) || Map[K_Graph->point[i].y][K_Graph->point[i].x + 1] == 1)//图形没有一个点到达最右边或右边一格是边界 
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
	else if (c == 27)//ESC键 
	{
		exit(0);
	}
}
//////////////主函数 
int main(void)
{
	cout << "......................俄罗斯方块游戏规则................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "1. 请务必在英文输入法下进行游戏操作！...................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "2. W(w)键变形；A(a)键向左移动；S(s)键向下移动；D(d)键向右移动;Esc退出游戏" << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "........................................................................." << endl;
	cout << "请输入阁下大名,并回车键确定" << endl;
	cin >> name;
	while (name == "黄鑫")
	{
		cout << "不能输入你鑫哥的名字，请重新输入" << endl;
		cin >> name;
	}
	Start();
	return(0);
}


