#ifndef __UCT_H__
#define __UCT_H__

#include <iostream>
#include <time.h>
#include "Judge.h" //包含判断胜负的函数userWin，machineWin和isTie 

//#define EMPTY 0 //未落子 
#define USER_CHANCE 1 //对方棋权 
#define MACHINE_CHANCE 2 //本方棋权 
#define TIME_LIMITATION 2500 //时长限制
#define USER_WIN_PROFIT -1 //对方获胜收益 
#define MACHINE_WIN_PROFIT 1 //本方获胜收益 
#define TIE_PROFIT 0 //平局收益
#define UNTERMINAL_STATE 2 //非终止状态 
#define VITALITY_COEFFICIENT 1.38 //比例系数c，选取待定 

using namespace std;

class UCT;

class node
{
private:
	int **boardState; //棋局状态
	int *topState; //顶端状态
	int row, column; //棋盘大小（M, N）
	int _noX, _noY; //不可落子点位置 
	int _chessman; //我方持子属性 
	int lastx, lasty; //前一上落子位置
	int visitednum; //被访问次数 
	double profit; //当前状态我方收益
	int _depth; //节点深度 
	node *father; //父节点
	node **children; //子节点
	int expandablenum; //可扩展节点数量 
	int *expandablenode; //可扩展节点编号 
	friend class UCT;

	int *TopState() const { //复制棋盘顶端状态数组topState 
		int *presentTop = new int[column];
		for (int i = 0; i != column; i++)
			presentTop[i] = topState[i];
		return presentTop;
	}
	int **BoardState() const { //复制棋盘状态数组boardState 
		int **presentBoardState = new int*[row];
		for (int i = 0; i < row; i++) {
			presentBoardState[i] = new int[column];
			for (int j = 0; j < column; j++)
				presentBoardState[i][j] = boardState[i][j];
		}
		return presentBoardState;
	}
	void clear() { //空间释放
		for (int i = 0; i != row; i++)
			delete[] boardState[i];
		delete[] boardState;
		delete[] topState;
		delete[] expandablenode;
		for (int i = 0; i != column; i++)
			if (children[i]) {
				children[i]->clear();
				delete children[i];
			}
		delete[] children;
	}

public:
	//构造函数 
	node(int **board, int *top, int r, int c, int noX, int noY, int depth = 0, int x = -1, int y = -1, int playingRight = MACHINE_CHANCE, node* _father = NULL) :
		boardState(board), topState(top), row(r), column(c), _noX(noX), _noY(noY), _depth(depth), lastx(x), lasty(y), _chessman(playingRight), visitednum(0), profit(0), father(_father) {
		expandablenum = 0;
		children = new node*[column]; //大小等于行数的子节点数组 
		expandablenode = new int[column]; //可到达子节点编号的数组 
		for (int i = 0; i != column; i++) {
			if (topState[i] != 0) //若第i列可落子 
				expandablenode[expandablenum++] = i;
			children[i] = NULL;
		}
	}
	int x() const { return lastx; }
	int y() const { return lasty; }
	int chessman() const { return _chessman; }
	bool isExpandable() const { return expandablenum > 0; }//是否可扩展
	//是否为终止节点 
	bool isTerminal() {
		if (lastx == -1 && lasty == -1) //若为根节点 
			return false;
		if ((_chessman == USER_CHANCE && machineWin(lastx, lasty, row, column, boardState)) || //计算机胜利 
			(_chessman == MACHINE_CHANCE && userWin(lastx, lasty, row, column, boardState)) || //玩家胜利 
			(isTie(column, topState))) //平局 
			return true;
		return false;
	}
	//扩展节点 
	node *expand(int playingRight) {
		int index = rand() % expandablenum; //随机确定一个索引值 
		int **newBoardState = BoardState(); //复制棋盘状态数组 
		int *newTopState = TopState(); //复制棋盘顶端状态数组 
		int newY = expandablenode[index], newX = --newTopState[newY]; //确定落子坐标 
		newBoardState[newX][newY] = chessman(); //落子 
		if (newX - 1 == _noX && newY == _noY) //若落子位置的正上方位置是不可落子点 
			newTopState[newY] --; //更新棋盘顶端状态数组
		//为当前节点创建扩展子节点 
		children[newY] = new node(newBoardState, newTopState, row, column, _noX, _noY, _depth + 1, newX, newY, playingRight, this);
		swap(expandablenode[index], expandablenode[--expandablenum]); //将被选中子节点编号置换到目录末尾
		return children[newY];
	}
	//最优子节点
	node *bestChild() {
		node* best;
		double maxProfitRatio = -RAND_MAX;
		for (int i = 0; i != column; i++) {
			if (children[i] == NULL) continue;
			double modifiedProfit = (_chessman == USER_CHANCE ? -1 : 1) * children[i]->profit; //修正收益值
			int childVisitedNum = children[i]->visitednum; //子节点访问数 
			double tempProfitRatio = modifiedProfit / childVisitedNum +
				sqrtl(2 * logl(visitednum) / childVisitedNum) * VITALITY_COEFFICIENT; //计算综合收益率 
			if (tempProfitRatio > maxProfitRatio || (tempProfitRatio == maxProfitRatio && rand() % 2 == 0)) { //选择综合收益率最大的子节点 
				maxProfitRatio = tempProfitRatio;
				best = children[i];
			}
		}
		return best;
	}
	//回溯更新
	void backup(double deltaProfit) {
		node *temp = this;
		while (temp) {
			temp->visitednum++; //访问次数+1 
			temp->profit += deltaProfit; //收益增加delta 
			temp = temp->father;
		}
	}
};

class UCT
{
private:
	node *_root; //根节点
	int _row, _column; //行数、列数
	int _noX, _noY; //不可落子点的位置 
	int startTime; //计算开始时间

	//计算当前状态收益
	int Profit(int **board, int *top, int chessman, int x, int y) const {
		if (chessman == USER_CHANCE && userWin(x, y, _row, _column, board))
			return USER_WIN_PROFIT;
		if (chessman == MACHINE_CHANCE && machineWin(x, y, _row, _column, board))
			return MACHINE_WIN_PROFIT;
		if (isTie(_column, top))
			return TIE_PROFIT;
		return UNTERMINAL_STATE; //未进入终止状态 
	}
	//随机落子 
	void placeChessman(int **board, int *top, int chessman, int &x, int &y) {
		y = rand() % _column; //随机选择一列 
		while (top[y] == 0) //若此列已下满 
			y = rand() % _column; //再随机选择一列 
		x = --top[y]; //确定落子高度 
		board[x][y] = chessman; //落子 
		if (x - 1 == _noX && y == _noY) //若落子位置正上方紧邻不可落子点 
			top[y] --;
	}
	//棋权变换 
	int rightChange(int chessman) const {
		if (chessman == USER_CHANCE)
			return MACHINE_CHANCE;
		else if (chessman == MACHINE_CHANCE)
			return USER_CHANCE;
		else
			return -1;
	}

	//搜索树策略 
	node *TreePolicy(node *presentNode) {
		while (!presentNode->isTerminal()) { //节点不是终止节点 
			if (presentNode->isExpandable()) //且拥有未被访问的子状态 
				return Expand(presentNode); //扩展该节点 
			else
				presentNode = BestChild(presentNode); //选择最优子节点 
		}
		return presentNode;
	}
	//对节点进行扩展
	node *Expand(node *presentNode) { return presentNode->expand(rightChange(presentNode->chessman())); }
	//最优子节点 
	node *BestChild(node *father) { return father->bestChild(); }
	//模拟策略 
	double DefaultPolicy(node *selectedNode) {
		int **boardState = selectedNode->BoardState(), *top = selectedNode->TopState();
		int chessman = selectedNode->chessman(), depth = selectedNode->_depth;
		int x = selectedNode->x(), y = selectedNode->y();
		int profit = Profit(boardState, top, rightChange(chessman), x, y); //计算收益 
		while (profit == UNTERMINAL_STATE) { //若当前状态未达终止状态 
			depth++;
			placeChessman(boardState, top, chessman, x, y); //随机落子 
			profit = Profit(boardState, top, chessman, x, y); //计算收益 
			chessman = rightChange(chessman); //棋权变换 
		}
		for (int i = 0; i != _row; i++)
			delete[] boardState[i];
		delete[] boardState;
		delete[] top;
		return double(profit);// / logl(depth + 1); //非线性加速
	}
	//回溯更新收益(深度越深收益越小)
	void Backup(node *selectedNode, double deltaProfit) { selectedNode->backup(deltaProfit); }

public:
	//构造函数 
	UCT(int row, int column, int noX, int noY) : _row(row), _column(column), _noX(noX), _noY(noY), startTime(clock()) {}
	//信心上限树搜索 
	node *search(int **boardState, int *topState) {
		_root = new node(boardState, topState, _row, _column, _noX, _noY); //以当前状态创建根节点 
		while (clock() - startTime <= TIME_LIMITATION) { //尚未耗尽计算时长 
			node *selectedNode = TreePolicy(_root); //运用搜索树策略节点 
			double deltaProfit = DefaultPolicy(selectedNode); //运用模拟策略对选中节点进行一次随机模拟 
			Backup(selectedNode, deltaProfit); //将模拟结果回溯反馈给各祖先 
		}
		return BestChild(_root);
	}
	//析构函数 
	~UCT() { _root->clear(); delete _root; }
};

#endif //__UCT_H__