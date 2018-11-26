#include "UCT.h"

UCT::UCT(int M, int N, int noX, int noY) : row(M), column(N), nox(noX), noy(noY), startTime(clock())
{
}


UCT::~UCT()
{
}

Point *UCT::search(int **boardState, int *topState)
{
	root = new node(boardState, topState, row, column, nox, noy); //以当前状态创建根节点 
	while (clock() - startTime <= TIME_LIMITATION) { //尚未耗尽计算时长 
		node *selectedNode = treepolicy(root); //运用搜索树策略节点 
		double deltaProfit = defaultpolicy(selectedNode); //运用模拟策略对选中节点进行一次随机模拟 
		backup(selectedNode, deltaProfit); //将模拟结果回溯反馈给各祖先 
	}
	node *bestpoint = bestchild(root);
	Point *point = new Point(bestpoint->x(), bestpoint->y());
	root->clear();
	delete root;
	root = NULL;
	return point;
}

int UCT::profit(int **board, int *top, int chesschance, int x, int y) const
{
	if (chesschance == USER_CHANCE && userWin(x, y, row, column, board))
		return USER_WIN_PROFIT;
	if (chesschance == MACHINE_CHANCE && machineWin(x, y, row, column, board))
		return MACHINE_WIN_PROFIT;
	if (isTie(column, top))
		return TIE_PROFIT;
	return UNTERMINAL_STATE; //未进入终止状态
}

void UCT::placechessman(int **board, int *top, int chessman, int &x, int &y)
{
	y = rand() % column; //随机选择一列 
	while (top[y] == 0) //若此列已下满 
		y = rand() % column; //再随机选择一列 
	x = --top[y]; //确定落子高度 
	board[x][y] = chessman; //落子 
	if (x - 1 == nox && y == noy) //若落子位置正上方紧邻不可落子点
		top[y] --;
}

int UCT::rightchange(int chessman) const
{
	if (chessman == USER_CHANCE)
		return MACHINE_CHANCE;
	else if (chessman == MACHINE_CHANCE)
		return USER_CHANCE;
	else
		return -1;
}

node *UCT::treepolicy(node *presentNode)
{
	while (!presentNode->isTerminal()) { //节点不是终止节点 
		if (presentNode->isExpandable()) //且拥有未被访问的子状态 
			return expand(presentNode); //扩展该节点 
		else
			presentNode = bestchild(presentNode); //选择最优子节点 
	}
	return presentNode;
}

node *UCT::expand(node *presentNode)
{
	return presentNode->expand(rightchange(presentNode->whochess()));
}
node *UCT::bestchild(node *father)
{
	return father->bestChild();
}

double UCT::defaultpolicy(node *selectedNode)
{
	int **boardState = selectedNode->BoardState(), *top = selectedNode->TopState();
	int chessman = selectedNode->whochess(), depth = selectedNode->_depth;
	int x = selectedNode->x(), y = selectedNode->y();
	int nowprofit = profit(boardState, top, rightchange(chessman), x, y); //计算收益 
	while (nowprofit == UNTERMINAL_STATE) { //若当前状态未达终止状态 
		depth++;
		placechessman(boardState, top, chessman, x, y); //随机落子 
		nowprofit = profit(boardState, top, chessman, x, y); //计算收益 
		chessman = rightchange(chessman); //棋权变换 
	}
	for (int i = 0; i != row; i++)
		delete[] boardState[i];
	delete[] boardState;
	delete[] top;
	return double(nowprofit);// / logl(depth + 1); //非线性加速
}

void UCT::backup(node *selectedNode, double deltaProfit)
{
	selectedNode->backup(deltaProfit);
}