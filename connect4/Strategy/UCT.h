//#ifndef __UCT_H__
//#define __UCT_H__
//
//#include <iostream>
//#include <time.h>
//#include "Point.h"
//#include "Judge.h"
//
//#define USER_CHANCE 1 //对方下棋
//#define MACHINE_CHANCE 2 //己方下棋 
//#define TIME_LIMITATION 2500 //时长限制
//#define USER_WIN_PROFIT -1 //对方获胜收益 
//#define MACHINE_WIN_PROFIT 1 //己方获胜收益 
//#define TIE_PROFIT 0 //平局收益
//#define UNTERMINAL_STATE 2 //非终止状态 
//#define C 0.8 //比例系数c，选取待定 
//
//using namespace std;
//
//class UCT;
//
//class node
//{
//private:
//	int **boardState; //棋局数组
//	int *topState; //顶端数组
//	int row, column; //棋盘行列数
//	int nox, noy; //不可落子位置 
//	int chesschance; //棋权 
//	int lastx, lasty; //上一步落子位置
//	int visitednum; //被访问次数 
//	double profit; //当前状态我方收益
//	int _depth; //节点深度 
//	node *father; //父节点
//	node **children; //子节点
//	int expandablenum; //可扩展节点数量 
//	int *expandablenode; //可扩展节点编号 
//	friend class UCT;
//
//	int *TopState() const { //复制棋盘顶端状态数组topState 
//		int *presentTop = new int[column];
//		for (int i = 0; i < column; i++)
//			presentTop[i] = topState[i];
//		return presentTop;
//	}
//	int **BoardState() const { //复制棋盘状态数组boardState 
//		int **presentBoardState = new int*[row];
//		for (int i = 0; i < row; i++) {
//			presentBoardState[i] = new int[column];
//			for (int j = 0; j < column; j++)
//				presentBoardState[i][j] = boardState[i][j];
//		}
//		return presentBoardState;
//	}
//	void clear() { //空间释放
//		for (int i = 0; i < row; i++)
//			delete[] boardState[i];
//		delete[] boardState;
//		delete[] topState;
//		delete[] expandablenode;
//		for (int i = 0; i < column; i++)
//			if (children[i])
//			{
//				children[i]->clear();
//				//cout << "OK" << endl;
//				delete[] children[i];
//			}
//		delete[] children;
//	}
//
//public:
//	//构造函数 
//	node(int **board, int *top, int M, int N, int noX, int noY, int depth = 0, int x = -1, int y = -1, int chessright = MACHINE_CHANCE, node* _father = NULL)
//	{
//		boardState = new int*[M];
//		for (int i = 0; i < M; i++)
//		{
//			boardState[i] = new int[N];
//			for (int j = 0; j < N; j++)
//				boardState[i][j] = board[i][j];
//		}
//		topState = new int[N];
//		for (int i = 0; i < N; i++)
//			topState[i] = top[i];
//		row = M;
//		column = N;
//		nox = noX;
//		noy = noY;
//		_depth = depth;
//		lastx = x;
//		lasty = y;
//		chesschance = chessright;
//		visitednum = 0;
//		profit = 0;
//		father = _father;
//		expandablenum = 0;
//		children = new node*[column]; //大小等于行数的子节点数组 
//		expandablenode = new int[column]; //可到达子节点编号的数组 
//		for (int i = 0; i < column; i++) {
//			if (topState[i] != 0) //若第i列可落子 
//				expandablenode[expandablenum++] = i;
//			children[i] = NULL;
//		}
//	}
//	int x() const { return lastx; }
//	int y() const { return lasty; }
//	int whochess() const { return chesschance; }
//	bool isExpandable() const { return expandablenum > 0; }//是否可扩展
//	//是否为终止节点 
//	bool isTerminal() 
//	{
//		if (lastx == -1 && lasty == -1) //若为根节点 
//			return false;
//		if ((chesschance == USER_CHANCE && machineWin(lastx, lasty, row, column, boardState)) || //计算机胜利 
//			(chesschance == MACHINE_CHANCE && userWin(lastx, lasty, row, column, boardState)) || //玩家胜利 
//			(isTie(column, topState))) //平局 
//			return true;
//		return false;
//	}
//	//扩展节点 
//	node *expand(int chessright)
//	{
//		int index = rand() % expandablenum; //随机确定一个索引值 
//		//int **newBoardState = BoardState(); //复制棋盘状态数组 
//		int **newBoardState = new int*[row];
//		for (int i = 0; i < row; i++) {
//			newBoardState[i] = new int[column];
//			for (int j = 0; j < column; j++)
//				newBoardState[i][j] = boardState[i][j];
//		}
//		int *newTopState = TopState(); //复制棋盘顶端状态数组 
//		int newy = expandablenode[index], newx = --newTopState[newy]; //确定落子坐标 
//		newBoardState[newx][newy] = chesschance; //落子 
//		if (newx - 1 == nox && newy == noy) //若落子位置的正上方位置是不可落子点 
//			newTopState[newy] --; //更新棋盘顶端状态数组
//		//为当前节点创建扩展子节点 
//		children[newy] = new node(newBoardState, newTopState, row, column, nox, noy, _depth + 1, newx, newy, chessright, this);
//		for (int i = 0; i < row; i++)
//			delete[] newBoardState[i];
//		delete[] newBoardState;
//		delete[] newTopState;
//		swap(expandablenode[index], expandablenode[--expandablenum]); //将被选中子节点编号置换到目录末尾
//		return children[newy];
//	}
//	//最优子节点
//	//node *bestChild() {
//	//	node* best;
//	//	double maxProfitRatio = -RAND_MAX;
//	//	for (int i = 0; i != column; i++) {
//	//		if (children[i] == NULL) continue;
//	//		double modifiedProfit = (chesschance == USER_CHANCE ? -1 : 1) * children[i]->profit; //修正收益值
//	//		int childVisitedNum = children[i]->visitednum; //子节点访问数 
//	//		double tempProfitRatio = modifiedProfit / childVisitedNum +
//	//			sqrtl(2 * logl(visitednum) / childVisitedNum) * C; //计算综合收益率 
//	//		//if (tempProfitRatio > maxProfitRatio || (tempProfitRatio == maxProfitRatio && rand() % 2 == 0)) { //选择综合收益率最大的子节点 
//	//		if (tempProfitRatio > maxProfitRatio)
//	//		{
//	//			maxProfitRatio = tempProfitRatio;
//	//			best = children[i];
//	//		}
//	//	}
//	//	return best;
//	//}
//	//回溯更新
//	void backup(double deltaProfit) {
//		node *temp = this;
//		while (temp) {
//			temp->visitednum++; //访问次数+1 
//			temp->profit += deltaProfit; //收益增加delta 
//			temp = temp->father;
//		}
//	}
//};
//
//class UCT
//{
//private:
//	node *root; //根节点
//	int row, column; //行数、列数
//	int nox, noy; //不可落子点的位置 
//	int startTime; //计算开始时间
//
//	//计算当前状态收益
//	int profit(int **board, int *top, int chessman, int x, int y) const;
//	//随机落子 
//	void placechessman(int **board, int *top, int chessman, int &x, int &y);
//	//棋权变换 
//	int rightchange(int chessman) const;
//	//搜索树策略 
//	node *treepolicy(node *presentNode);
//	//对节点进行扩展
//	node *expand(node *presentNode);
//	//最优子节点 
//	node *bestchild(node *father);
//	//模拟策略 
//	double defaultpolicy(node *selectedNode);
//	//回溯更新收益(深度越深收益越小)
//	void backup(node *selectedNode, double deltaProfit);
//
//public:
//	//构造函数 
//	UCT(int M, int N, int noX, int noY);
//	//信心上限树搜索 
//	Point *search(int **boardState, int *topState);
//	//析构函数 
//	virtual ~UCT();
//};
//
//#endif //__UCT_H__

#ifndef __UCT_H__
#define __UCT_H__

#include <iostream>
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
#define C 0.7 //比例系数c，选取待定 

using namespace std;

class UCT;

class node
{
private:
	int **boardarray, *toparray, row, column, nox, noy, depth, chesschance, lastx, lasty, visitednum, expandablenum, *expandablenode;
	double profit;
	node *father, **children;
	friend class UCT;

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
		for (int i = 0; i < row; i++) {
			temp[i] = new int[column];
			for (int j = 0; j < column; j++)
				temp[i][j] = boardarray[i][j];
		}
		return temp;
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

public:
	node(int **board, int *top, int M, int N, int noX, int noY, int nodedepth = 0, int x = -1, int y = -1, int chessright = MACHINECHANCE, node* fathernode = NULL)
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
		lastx = x;
		lasty = y;
		chesschance = chessright;
		visitednum = 0;
		profit = 0;
		father = fathernode;
		expandablenum = 0;
		children = new node*[column];
		expandablenode = new int[column];
		for (int i = 0; i < column; i++)
		{
			if (toparray[i] != 0)
				expandablenode[expandablenum++] = i;
			children[i] = NULL;
		}
	}
	int x() const 
	{ 
		return lastx; 
	}
	int y() const 
	{ 
		return lasty; 
	}
	int whochess() const 
	{ 
		return chesschance; 
	}
	bool isExpandable() const 
	{ 
		return expandablenum > 0; 
	}
	bool isleaf()
	{
		if (lastx == -1 && lasty == -1)
			return false;
		if ((chesschance == USERCHANCE && machineWin(lastx, lasty, row, column, boardarray)) || (chesschance == MACHINECHANCE && userWin(lastx, lasty, row, column, boardarray)) || (isTie(column, toparray)))
			return true;
		return false;
	}
	node *expand(int chessright)
	{
		int index = rand() % expandablenum;
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
		children[newy] = new node(tempboardarray, temptoparray, row, column, nox, noy, depth + 1, newx, newy, chessright, this);
		for (int i = 0; i < row; i++)
			delete[] tempboardarray[i];
		delete[] tempboardarray;
		delete[] temptoparray;
		swap(expandablenode[index], expandablenode[--expandablenum]);
		return children[newy];
	}
	//最优子节点
	//node *bestChild() 
	//{
	//	node* best;
	//	double maxProfitRatio = -RAND_MAX;
	//	for (int i = 0; i < column; i++) {
	//		if (children[i] == NULL) continue;
	//		double modifiedProfit = (chesschance == USERCHANCE ? -1 : 1) * children[i]->profit; //修正收益值
	//		int childVisitedNum = children[i]->visitednum; //子节点访问数 
	//		double tempProfitRatio = modifiedProfit / childVisitedNum + sqrtl(2 * logl(visitednum) / childVisitedNum) * C; //计算综合收益率 
	//		//if (tempProfitRatio > maxProfitRatio || (tempProfitRatio == maxProfitRatio && rand() % 2 == 0)) 
	//		if (tempProfitRatio > maxProfitRatio)
	//		{ //选择综合收益率最大的子节点 
	//			maxProfitRatio = tempProfitRatio;
	//			best = children[i];
	//		}
	//	}
	//	return best;
	//}
	//回溯更新
	//void backup(double deltaProfit) 
	//{
	//	node *temp = this;
	//	while (temp) 
	//	{
	//		temp->visitednum++; //访问次数+1 
	//		temp->profit += deltaProfit; //收益增加delta 
	//		temp = temp->father;
	//	}
	//}
};

class UCT
{
private:
	node *root; //根节点
	int row, column; //行数、列数
	int nox, noy; //不可落子点的位置 
	int startTime; //计算开始时间
	int profit(int **board, int *top, int chessman, int x, int y) const;
	void placechess(int **board, int *top, int chesschance, int &x, int &y);
	int rightchange(int chessman) const;
	node *treepolicy(node *presentNode);
	node *expand(node *expandnode);
	node *bestchild(node *father);
	double defaultpolicy(node *selectedNode);
	void backup(node *selectedNode, double deltaProfit);

public:
	UCT(int M, int N, int noX, int noY);
	Point *search(int **board, int *top);
	virtual ~UCT();
};

#endif //__UCT_H__