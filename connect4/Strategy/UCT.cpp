#include "UCT.h"

UCT::UCT(int M, int N, int noX, int noY)
{
	row = M;
	column = N;
	nox = noX;
	noy = noY;
	starttime = clock();
}


UCT::~UCT()
{
}

Point *UCT::search(int **board, int *top)
{
	root = new node(board, top, row, column, nox, noy);
	while (clock() - starttime <= TIMELIMITE)
	{
		node *tempnode = treepolicy(root);
		double deltaprofit = defaultpolicy(tempnode);
		backup(tempnode, deltaprofit);
	}
	node *bestpoint = bestchild(root);
	Point *point = new Point(bestpoint->pointx, bestpoint->pointy);
	root->clear();
	delete root;
	root = NULL;
	return point;
}

node *UCT::treepolicy(node *rootnode)
{
	while (!rootnode->isleaf())
	{
		if (rootnode->expandablenum > 0)
			return expand(rootnode);
		else
			rootnode = bestchild(rootnode);
	}
	return rootnode;
}

double UCT::defaultpolicy(node *tempnode)
{
	int **board = tempnode->getboard(), *top = tempnode->gettop();
	int chesschance = tempnode->chesschance, depth = tempnode->depth;
	int x = tempnode->pointx, y = tempnode->pointy;
	int tempprofit = profit(board, top, chancechange(chesschance), x, y); //计算收益 
	while (tempprofit == NOTEND)
	{
		depth++;
		placechess(board, top, chesschance, x, y);
		tempprofit = profit(board, top, chesschance, x, y);
		chesschance = chancechange(chesschance);
	}
	for (int i = 0; i < row; i++)
		delete[] board[i];
	delete[] board;
	delete[] top;
	return double(tempprofit);
}

void UCT::backup(node *leaf, double deltaprofit)
{
	node *temp = leaf;
	while (temp)
	{
		temp->visitednum++;
		temp->profit += deltaprofit;
		temp = temp->father;
	}
}

node *UCT::expand(node *expandnode)
{
	//int expandablenum = expandnode->expandablenum;
	//int index = rand() % expandablenum; //随机确定一个索引值 
	//int **tempboardarray = new int*[row];
	//for (int i = 0; i < row; i++)
	//{
	//	tempboardarray[i] = new int[column];
	//	for (int j = 0; j < column; j++)
	//		tempboardarray[i][j] = expandnode->boardarray[i][j];
	//}
	//int *temptoparray = new int[column];
	//for (int i = 0; i < column; i++)
	//	temptoparray[i] = expandnode->toparray[i];
	///*int **tempboardarray = expandnode->getboard();
	//int *temptoparray = expandnode->gettop();*/
	//int newy = expandnode->expandablenode[index], newx = --temptoparray[newy];
	//tempboardarray[newx][newy] = expandnode->chesschance; 
	//if (newx - 1 == nox && newy == noy)
	//	temptoparray[newy] --; 
	////为当前节点创建扩展子节点 
	//expandnode->children[newy] = new node(tempboardarray, temptoparray, row, column, nox, noy, expandnode->depth + 1, chancechange(expandnode->chesschance), newx, newy, expandnode);
	//for (int i = 0; i < row; i++)
	//	delete[] tempboardarray[i];
	//delete[] tempboardarray;
	//delete[] temptoparray;
	//swap(expandnode->expandablenode[index], expandnode->expandablenode[--expandablenum]); 
	//return expandnode->children[newy];
	return expandnode->expand(chancechange(expandnode->chesschance));
}

node *UCT::bestchild(node *rootnode)
{
	node* bestpoint;
	double maxprofit = -(numeric_limits<double>::max)();
	for (int i = 0; i < column; i++)
	{
		if (rootnode->children[i] == NULL)
			continue;
		double newprofit = (rootnode->chesschance == USERCHANCE ? -1 : 1) * rootnode->children[i]->profit;
		int childvisitednum = rootnode->children[i]->visitednum;
		double tempprofit = newprofit / childvisitednum + C * sqrtl(2 * logl(rootnode->visitednum) / childvisitednum); //计算综合收益率 
		//if (tempProfitRatio > maxProfitRatio || (tempProfitRatio == maxProfitRatio && rand() % 2 == 0)) 
		if (tempprofit > maxprofit)
		{
			maxprofit = tempprofit;
			bestpoint = rootnode->children[i];
		}
	}
	return bestpoint;
}

int UCT::profit(int **board, int *top, int chesschance, int x, int y) const
{
	if (chesschance == USERCHANCE && userWin(x, y, row, column, board))
		return USERWIN;
	if (chesschance == MACHINECHANCE && machineWin(x, y, row, column, board))
		return MACHINEWIN;
	if (isTie(column, top))
		return TIE;
	return NOTEND;
}

void UCT::placechess(int **board, int *top, int chesschance, int &x, int &y)
{
	y = eg() % column;
	while (top[y] == 0)
		y = eg() % column;
	x = --top[y];
	board[x][y] = chesschance;
	if (x - 1 == nox && y == noy)
		top[y] --;
}

int UCT::chancechange(int chesschance) const
{
	if (chesschance == USERCHANCE)
		return MACHINECHANCE;
	else
	{
		if (chesschance == MACHINECHANCE)
			return USERCHANCE;
	}
}