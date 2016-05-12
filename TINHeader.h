#pragma once
#ifndef TINHEADER
#define TINHEADER

//----------------�궨��
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
//�������ݽṹ
struct PNT        //���ǵ�
{
	double x;
	double y;
};
struct PointSet    //�㼯   
{
	double x;
	double y;
	int ID;
	int nC;  //�����к�
	int nL;  //�����к�
	bool label;  //��ǵ��ڷֿ�ʱ�Ƿ�����
	int gNo;    //�������Ŀ��
};
struct ArcSet
{
	int pNum;
	PNT * pts;
};
struct GridField
{
	int pNum;
	int *ptId; //��¼��ԭ�㼯�е����
	bool bvisited;
};
struct BinaryTree
{
	//��¼��ʼ����ֹ���к�
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
	int nC;  //�����к�
	int nL;  //�����к�
	QueueForCenter *next;
};

//-----------�������ṹ--------------------------
typedef struct TRIANGLE
{
	int ID1, ID2, ID3; //��¼�������ԭ�㼯�е����
	TRIANGLE *next, *back;  //next��backָ����ɨ�跨�д����ʼ��������
	TRIANGLE *p1tin; //��������ָ����ɨ�跨�б�ʾÿ�������Ͷ�Ӧ��3��������
	TRIANGLE *p2tin; //��Ӧ��ϵ�붥��y�����й�
	TRIANGLE *p3tin; //p1tin��Ӧy�������Ķ����Ӧ�ıߵ��ڽ������Σ�p2tin��֮��p3tin��С
	int g_SeqNum;  //�����͵����
	int visited;   //��ɨ�跨�ķǵݹ鷽ʽ��Ҫ��
}TRIANGLENODE;

struct LineSet
{
	int ID1, ID2;          //ID1��ID2Ϊ�߶ε����˵���ԭʼ�㼯�е����
};

struct BlockTin           //
{
	TRIANGLE *first, *last;
	int pNum;         //�����ĵ���
	PointSet *pts;    //��¼�㼯
	TRIANGLE *Tin[4]; //����ķ�Χ��1��3��������Χ���ڽ�������
	LineSet Edge[4];  //����ķ�Χ��1��3����Χ�ڽ���������Ӧ�ı�
	bool IsVisited[4];
};

typedef struct FB       //˫��������ʾ�ǵ�
{
	int no;
	TRIANGLE *tri;
	FB  *next, *back;
}FBNODE;

typedef struct treenode    //����������,�洢ǰ�߽ṹ
{
	int no;
	TRIANGLE *tri;
	treenode *lchild, *rchild, *parent;
	treenode *next, *back;
}TREENODE;

struct LIST //ɨ���߷��еĹ�������㷨����ѹջ������
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