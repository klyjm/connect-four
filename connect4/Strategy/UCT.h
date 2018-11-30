#ifndef __UCT_H__
#define __UCT_H__

#include <iostream>
#include <random>
#include <Windows.h>
#include <time.h>
#include "Point.h"
#include "Judge.h"

#define USERCHANCE 1
#define MACHINECHANCE 2
#define TIMELIMITE 2600
#define USERWIN -1
#define MACHINEWIN 1
#define TIE 0
#define NOTEND 2
#define C 0.67 //比例系数c，选取待定 

using namespace std;
static default_random_engine eg(GetTickCount64());


class UCT;

class node
{
private:
//	int **boardarray, *toparray, row, column, nox, noy, depth, chesschance, lastx, lasty, visitednum, expandablenum, *expandablenode;
//	double profit;
//	node *father, **children;
//	friend class UCT;
//
//	int *gettop() const
//	{
//		int *temp = new int[column];
//		for (int i = 0; i < column; i++)
//			temp[i] = toparray[i];
//		return temp;
//	}
//	int **getboard() const
//	{
//		int **temp = new int*[row];
//		for (int i = 0; i < row; i++) 
//		{
//			temp[i] = new int[column];
//			for (int j = 0; j < column; j++)
//				temp[i][j] = boardarray[i][j];
//		}
//		return temp;
//	}
//	void clear()
//	{
//		for (int i = 0; i < row; i++)
//			delete[] boardarray[i];
//		delete[] boardarray;
//		delete[] toparray;
//		delete[] expandablenode;
//		for (int i = 0; i < column; i++)
//			if (children[i])
//			{
//				children[i]->clear();
//				delete[] children[i];
//			}
//		delete[] children;
//	}

public:
	int **boardarray, *toparray, row, column, nox, noy, depth, chesschance, pointx, pointy, visitednum, expandablenum, *expandablenode;
	double profit;
	node *father, **children;

	node(int **board, int *top, int M, int N, int noX, int noY, int nodedepth = 0, int chessright = MACHINECHANCE, int x = -1, int y = -1, node* fathernode = NULL)
	{
		boardarray = new int*[M];
		for (int i = 0; i < M; i++)
		{
			boardarray[i] = new int[N];
			for (int j = 0; j < N; j++)
				boardarray[i][j] = board[i][j];
		}
		toparray = new int[N];
		for (int i = 0; i < N; i++)
			toparray[i] = top[i];
		row = M;
		column = N;
		nox = noX;
		noy = noY;
		depth = nodedepth;
		chesschance = chessright;
		pointx = x;
		pointy = y;
		visitednum = 0;
		expandablenum = 0;
		expandablenode = new int[column];
		profit = 0;
		father = fathernode;
		children = new node*[column];
		for (int i = 0; i < column; i++)
		{
			if (toparray[i] != 0)
				expandablenode[expandablenum++] = i;
			children[i] = NULL;
		}
	}
	int *gettop() const
	{
		int *temp = new int[column];
		for (int i = 0; i < column; i++)
			temp[i] = toparray[i];
		return temp;
	}
	int **getboard() const
	{
		int **temp = new int*[row];
		for (int i = 0; i < row; i++)
		{
			temp[i] = new int[column];
			for (int j = 0; j < column; j++)
				temp[i][j] = boardarray[i][j];
		}
		return temp;
	}
	bool isleaf()
	{
		if (pointx == -1 && pointy == -1)
			return false;
		if ((chesschance == USERCHANCE && machineWin(pointx, pointy, row, column, boardarray)) || (chesschance == MACHINECHANCE && userWin(pointx, pointy, row, column, boardarray)) || (isTie(column, toparray)))
			return true;
		return false;
	}
	node *expand(int chessright)
	{
		int index = eg() % expandablenum;
		int **tempboardarray = new int*[row];
		for (int i = 0; i < row; i++)
		{
			tempboardarray[i] = new int[column];
			for (int j = 0; j < column; j++)
				tempboardarray[i][j] = boardarray[i][j];
		}
		int *temptoparray = new int[column];
		for (int i = 0; i < column; i++)
			temptoparray[i] = toparray[i];
		int newy = expandablenode[index], newx = --temptoparray[newy];
		tempboardarray[newx][newy] = chesschance;
		if (newx - 1 == nox && newy == noy)
			temptoparray[newy] --;
		children[newy] = new node(tempboardarray, temptoparray, row, column, nox, noy, depth + 1, chessright, newx, newy, this);
		for (int i = 0; i < row; i++)
			delete[] tempboardarray[i];
		delete[] tempboardarray;
		delete[] temptoparray;
		swap(expandablenode[index], expandablenode[--expandablenum]);
		return children[newy];
	}
	void clear()
	{
		for (int i = 0; i < row; i++)
			delete[] boardarray[i];
		delete[] boardarray;
		delete[] toparray;
		delete[] expandablenode;
		for (int i = 0; i < column; i++)
			if (children[i])
			{
				children[i]->clear();
				delete[] children[i];
			}
		delete[] children;
	}
};

class UCT
{
private:
	node *root;
	int row, column;
	int nox, noy;
	int startTime;
	int profit(int **board, int *top, int chesschance, int x, int y) const;
	void placechess(int **board, int *top, int chesschance, int &x, int &y);
	int chancechange(int chesschance) const;
	node *treepolicy(node *rootnode);
	node *expand(node *expandnode);
	node *bestchild(node *rootnode);
	double defaultpolicy(node *tempnode);
	void backup(node *leaf, double deltaprofit);

public:
	UCT(int M, int N, int noX, int noY);
	Point *search(int **board, int *top);
	virtual ~UCT();
};

#endif //__UCT_H__