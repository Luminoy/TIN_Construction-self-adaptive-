#pragma once
#ifndef TINHEADER
#define TINHEADER

//----------------宏定义
#include "math.h"
#define   ZOOMIN            1001
#define   ZOOMOUT           1002
#define   PAN               1003
#define   SELECT            1004
#define   ZoomFactor        2.0
#define _MAX_PNUM_anArc  100000
#define _MAX_ARCNUM_aMap  100000
#define _PI                  3.14159265358979323846
#define ThreadPoolSIZE 8
#define BlockGridSize 100 
#define BlockGridOffset 0.1
//HANDLE RequestExitEvent;
//定义数据结构
struct PNT        //三角点
{
	double x;
	double y;
};
struct PointSet    //点集   
{
	double x;
	double y;
	int ID;
	int nC;  //所在列号
	int nL;  //所在行号
	bool label;  //标记点在分块时是否已用
	int gNo;    //点所属的块号
};
struct ArcSet
{
	int pNum;
	PNT * pts;
};
struct GridField
{
	int pNum;
	int *ptId; //记录在原点集中的序号
	bool bvisited;
};
struct BinaryTree
{
	//记录起始、终止行列号
	int nStartLine, nStartCol;
	int nEndLine, nEndCol;
	int npNum;
	int nLevel;
};
struct Queue
{
	Queue() { treenode = NULL; next = NULL; }
	BinaryTree *treenode;
	Queue *next;
};
struct QueueForCenter
{
	int nC;  //所在列号
	int nL;  //所在行号
	QueueForCenter *next;
};

//-----------三角网结构--------------------------
typedef struct TRIANGLE
{
	int ID1, ID2, ID3; //记录坐标点在原点集中的序号
	TRIANGLE *next, *back;  //next和back指针在扫描法中窜起初始化的三角
	TRIANGLE *p1tin; //以下三个指针在扫描法中标示每个三角型对应的3个三角形
	TRIANGLE *p2tin; //对应关系与顶点y坐标有关
	TRIANGLE *p3tin; //p1tin对应y坐标最大的顶点对应的边的邻接三角形，p2tin次之，p3tin最小
	int g_SeqNum;  //三角型的序号
	int visited;   //在扫描法的非递归方式中要用
}TRIANGLENODE;

struct LineSet
{
	int ID1, ID2;          //ID1、ID2为线段的两端点在原始点集中的序号
};

struct BlockTin           //
{
	TRIANGLE *first, *last;
	int pNum;         //包含的点数
	PointSet *pts;    //记录点集
	TRIANGLE *Tin[4]; //数组的范围是1至3，保存外围的邻接三角形
	LineSet Edge[4];  //数组的范围是1至3，外围邻接三角形相应的边
	bool IsVisited[4];
};

typedef struct FB       //双向链表，表示壳底
{
	int no;
	TRIANGLE *tri;
	FB  *next, *back;
}FBNODE;

typedef struct treenode    //二叉排序树,存储前线结构
{
	int no;
	TRIANGLE *tri;
	treenode *lchild, *rchild, *parent;
	treenode *next, *back;
}TREENODE;

struct LIST //扫描线法中的广度优先算法用来压栈的链表
{
	TRIANGLE *tri;
	LIST *next;
};
//////////////////////////////////////////////////////////////////////////
struct GroupGrid
{
	int triangleNum;
	TRIANGLENODE* triFirst;
};
//////////////////////////////////////////////////////////////////////////
#endif // !TINDATASTRUCTURE