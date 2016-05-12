// GISTinView.cpp : implementation of the CGISTinView class
//

#include "stdafx.h"
#include "GISTin.h"
#include <sstream>
#include "time.h"
using namespace std;

#include "GISTinDoc.h"
#include "GISTinView.h"
#include "GridDlg.h"
#include "math.h"

// #include "unistd.h"  
// #include "pthread.h" 
// #include "sys/syscall.h"  
// #define gettid() syscall(__NR_gettid) 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGISTinView

IMPLEMENT_DYNCREATE(CGISTinView, CView)

BEGIN_MESSAGE_MAP(CGISTinView, CView)
	//{{AFX_MSG_MAP(CGISTinView)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_COMMAND(ID_PAN, OnPan)
	ON_COMMAND(ID_GLOBE, OnGlobe)
	ON_COMMAND(ID_SELECT, OnSelect)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_LOAD_POINT_FILE, OnLoadPointFile)
	ON_COMMAND(ID_LOAD_LINE_FILE, OnLoadLineFile)
	ON_COMMAND(ID_BINARY_GROUP, OnBinaryGroup)
	ON_COMMAND(ID_DISPLAY_GRID, OnDisplayGrid)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_GRID, OnUpdateDisplayGrid)
	ON_COMMAND(ID_GENERATE_TIN, OnGenerateTin)
	ON_COMMAND(ID_TINMultiThread, OnTINMultiThread)
	ON_COMMAND(ID_TINNoGroup, OnTINNoGroup)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGISTinView construction/destruction

//���캯������ʼ�������
CGISTinView::CGISTinView()
{
	OperateID=0;  zoomratio=1; 	Captured=FALSE; 	
	m_hZoomIn=AfxGetApp()->LoadCursor(IDC_ZOOMIN);
	m_hZoomOut=AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
	m_hPan=AfxGetApp()->LoadCursor(IDC_PAN);
	m_hSelect=AfxGetApp()->LoadCursor(IDC_SELECT);    
	pointNumber=0;arcNumber=0;
    Arc= new ArcSet[_MAX_ARCNUM_aMap];
	xmin=ymin=1.E+256; xmax=ymax=-1.E+256;
	nFlagPoint=FALSE; nFlagArc=FALSE;
    
	PointData=NULL;
	CopyData=NULL;
	m_displayGrid = false;
	queuehead = NULL;
    PointCenter = NULL;
    CenterSize = 0;
	runTime[0]=0;runTime[1]=0;runTime[2]=0;runTime[3]=0;runTime[4]=0;runTime[5]=0;

	tinHead=tinEnd=tin=NULL;
	colors[0]=RGB(255,0,0);
	colors[1]=RGB(0,255,0);
	colors[2]=RGB(0,0,255);
	colors[3]=RGB(160,160,160);
	colors[4]=RGB(0,255,255);
    colors[5]=RGB(255,0,255);
    colors[6]=RGB(127,127,127);
    colors[7]=RGB(0,127,127);
    colors[8]=RGB(127,127,0);
	colors[9]=RGB(200,230,200);
    colors[10]=RGB(100,10,200);
    colors[11]=RGB(10,200,100);
	colors[12]=RGB(100,200,10);
    colors[13]=RGB(200,100,10);
	for (int i = 0;i<BlockGridSize;i++)
	{
		for (int j=0;j<BlockGridSize;j++)
		{
			blockGrid[i][j].triangleNum=0;
			blockGrid[i][j].triFirst=NULL;
		}
	}
}
CGISTinView::~CGISTinView()
{
	ClearQueueMemory(queuehead, PointCenter);
	for (int i=0;i<BlockGridSize;i++)
	{
		for (int j=0;j<BlockGridSize;j++)
		{
			TRIANGLENODE *del=NULL;
			for (TRIANGLENODE* t=blockGrid[i][j].triFirst;t!=NULL;)
			{
				del=t;
				t=t->next;
				delete del;
			}
		}
	}
 //	ClearTinMemory(&tinHead);
}

BOOL CGISTinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// ��ͼ����
void CGISTinView::OnDraw(CDC* pDC)
{
	CGISTinDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	DrawGraph(pDC);               //����ͼ��

	//һ��Ϊ��ʾ�����ֻ���ʱ�䣬��������
	CString sTime1,sTime2,sTime3,sTime4,sTime5,sTime6;
 	sTime1.Format(_T("���飺%f"),runTime[0]);
 	pDC->TextOut(0,0,sTime1);
	sTime2.Format(_T("���㣺%f"),runTime[1]);
 	pDC->TextOut(0,20,sTime2);
	sTime3.Format(_T("LOP��%f"),runTime[2]);
 	pDC->TextOut(0,40,sTime3);
	sTime4.Format(_T("ȥ���㣺%f"),runTime[3]);
 	pDC->TextOut(0,60,sTime4);
	sTime5.Format(_T("��ʱ�䣺%f"),runTime[4]);
 	pDC->TextOut(0,80,sTime5);
	sTime6.Format("�㶨λ��%f",runTime[5]);
 	pDC->TextOut(0,100,sTime6);
}

/////////////////////////////////////////////////////////////////////////////
// CGISTinView diagnostics

#ifdef _DEBUG
void CGISTinView::AssertValid() const
{
	CView::AssertValid();
}

void CGISTinView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGISTinDoc* CGISTinView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGISTinDoc)));
	return (CGISTinDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
//////////////////////���ڲ���//////////////////////////////////////////////
void CGISTinView::OnZoomIn() 
{
	OperateID=ZOOMIN;
}
void CGISTinView::OnZoomOut() 
{
	OperateID=ZOOMOUT;
}
void CGISTinView::OnPan() 
{  
	OperateID=PAN; 
}
void CGISTinView::OnGlobe() 
{
	ZoomFull();
    RefreshScreen();
}
void CGISTinView::OnSelect() 
{
	OperateID=SELECT;
}
void CGISTinView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    ptMouse.x=point.x;  ptMouse.y=point.y;  ptMouseTmp=ptMouse;
    PNT mpt=ptMouse;  GetMapPoint(&mpt);
	CRect rect;
	GetClientRect(&rect);
	//--------���������
    switch(OperateID)
	{
    case ZOOMOUT:  ::SetCursor(m_hZoomOut);
		            xcenter=mpt.x; ycenter=mpt.y; zoomratio*=ZoomFactor;
                   InvalidateRect(&rect);  break;
    case ZOOMIN:   ::SetCursor(m_hZoomIn);
		           Captured=true;  break;
	case SELECT:   ::SetCursor(m_hSelect);break; 
    
 	case PAN:      ::SetCursor(m_hPan);
    default: break;
	}
	CView::OnLButtonDown(nFlags, point);
}

void CGISTinView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int DX,DY;  PNT mpt1=ptMouse,mpt2={point.x,point.y};
    DX=(int)(mpt2.x-mpt1.x); DY=(int)(mpt2.y-mpt1.y);
    GetMapPoint(&mpt1); GetMapPoint(&mpt2);

	CRect   Rect;
	GetClientRect(&Rect);
	//-----���������
	switch(OperateID)
	{
    case ZOOMOUT:  ::SetCursor(m_hZoomOut);break;             
    case ZOOMIN:   ::SetCursor(m_hZoomIn);
		           Captured=false;
				   if(labs(DX)>=2&&labs(DY)>=2)
				   {
                       double xxmin,yymin,xxmax,yymax,dxx,dyy,fx,fy;
                       if(mpt1.x>mpt2.x){xxmin=mpt2.x; xxmax=mpt1.x;} 
					   else {xxmin=mpt1.x; xxmax=mpt2.x;}
                       if(mpt1.y>mpt2.y){yymin=mpt2.y; yymax=mpt1.y;} 
					   else {yymin=mpt1.y; yymax=mpt2.y;}
                       dxx=xxmax-xxmin;dyy=yymax-yymin;
					   xcenter=xxmin+dxx/2.;  ycenter=yymin+dyy/2.;
                        fx=dxx/ClientWidth;   fy=dyy/ClientHeight;
                       zoomratio=(fx>fy)?fx:fy;
				   }
                   else
				    {xcenter=mpt2.x; ycenter=mpt2.y; zoomratio/=ZoomFactor;}
				   InvalidateRect(&Rect); break;
	case PAN:      ::SetCursor(m_hPan);
		           xcenter-=mpt2.x-mpt1.x; ycenter-=mpt2.y-mpt1.y; 
		           InvalidateRect(&Rect); break;
	case SELECT:   ::SetCursor(m_hSelect);break;     
    default: break;
	}
	CView::OnLButtonUp(nFlags, point);
}

void CGISTinView::OnMouseMove(UINT nFlags, CPoint point) 
{
	PNT mpt={point.x,point.y},pt12=ptMouseTmp;
   
 	GetMapPoint(&mpt);
	
	//------���������
    switch(OperateID)
	{
 	case PAN:                     ::SetCursor(m_hPan);break; 
	case ZOOMOUT:                 ::SetCursor(m_hZoomOut);break;
	case ZOOMIN:                  ::SetCursor(m_hZoomIn);break;
	case SELECT:                  ::SetCursor(m_hSelect);break;
	default:                      ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); break;
	}

    if(Captured&&OperateID==ZOOMIN)
	{
        CClientDC dc(this);
		
        CPen  NewPen1(PS_DOT,1,RGB(255,255,255));
		CPen *OldPen=dc.SelectObject(&NewPen1);
		dc.SetROP2(R2_NOT);
		//-----����ԭ����
		dc.MoveTo((int)ptMouse.x,(int)ptMouse.y);
        dc.LineTo((int)ptMouse.x,(int)pt12.y);
		dc.LineTo((int)pt12.x,(int)pt12.y);
        dc.LineTo((int)pt12.x,(int)ptMouse.y);
		dc.LineTo((int)ptMouse.x,(int)ptMouse.y);		
		CPen NewPen2(PS_DOT,1,RGB(0,0,0));
		dc.SelectObject(&NewPen2)->DeleteObject();
        //���¾���
        dc.MoveTo((int)ptMouse.x,(int)ptMouse.y);
		dc.LineTo((int)ptMouse.x,(int)point.y);
		dc.LineTo((int)point.x,(int)point.y);
        dc.LineTo((int)point.x,(int)ptMouse.y);
		dc.LineTo((int)ptMouse.x,(int)ptMouse.y);
		dc.SelectObject(OldPen)->DeleteObject();
        
		
		ptMouseTmp.x=point.x;   ptMouseTmp.y=point.y;

	}
	CView::OnMouseMove(nFlags, point);
}
void CGISTinView::RefreshScreen()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidateRect(&rect);
}
void CGISTinView::GetScreenPoint(PNT *pt)
{
	CRect  Rect;
	GetClientRect(&Rect);
	ClientHeight=Rect.Height();
	ClientWidth=Rect.Width();
	//������Ļ����
    pt->x=(pt->x-xcenter)/zoomratio+ClientWidth/2;
    pt->y=(pt->y-ycenter)/zoomratio+ClientHeight/2;
    pt->y=ClientHeight-pt->y;
}
void CGISTinView::GetMapPoint(PNT * pt)
{
	//�����ͼ����
    pt->x=zoomratio*(pt->x-ClientWidth/2)+xcenter;
    pt->y=zoomratio*(ClientHeight/2-pt->y)+ycenter;
}
void CGISTinView::ZoomFull()
{
    //����ͻ����Ĵ�С	
	CRect  Rect;
	GetClientRect(&Rect);
	ClientHeight=Rect.Height();
	ClientWidth=Rect.Width();
	//ȫ����ʾ
    xcenter=xmin+dx/2.;  ycenter=ymin+dy/2.;
    double zx=dx/ClientWidth,
           zy=dy/ClientHeight;
    zoomratio=(zx>zy)?zx:zy;    
}
//--------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//////////////////////�ļ�����//////////////////////////////////////////////
void CGISTinView::LoadFile(int Type)
{
	CString  TheFileName;
    CFileDialog  FileDlg(TRUE);

    if(FileDlg.DoModal()==IDOK)
		TheFileName=FileDlg.GetPathName();
	else 
		return;

	FILE * fp=fopen(TheFileName,"r+");
    if(fp==NULL) return;
	CClientDC dc(this);
	if(Type==1)
	{
	   fscanf(fp,"%d",&pointNumber);
       PointData=new PointSet[pointNumber+4];
       for(int i=0;i<pointNumber;i++)
	   {
		   fscanf(fp,"%d%lf%lf",&PointData[i].ID,&PointData[i].x,&PointData[i].y);
		   PointData[i].ID = i;
		   PointData[i].label=false;  //��ʾ��δ����
	   }
	  
	   nFlagPoint=true;
	   CalcBoundPoint();
	   CalcBoundGraph();
	   ZoomFull();
	   AfxMessageBox("��ȡ��������ϣ�");
 	   RefreshScreen();  
	}
	if(Type==2)
	{
      char s1[50],s2[50];
      int ArcID,pNo,pNum=0,aNum=0;
      PNT * pts = new PNT[_MAX_PNUM_anArc];
      fscanf(fp,"%d",&ArcID);
      for(;;)
	  {
        if(feof(fp))break;
        fscanf(fp,"%s%s",&s1,&s2);
        if(s1[0]=='E')
		{
            Arc[aNum].pNum=pNum;
            Arc[aNum].pts=new PNT[pNum];
            for(pNo=0;pNo<pNum;pNo++)
			{
                Arc[aNum].pts[pNo].x=pts[pNo].x;
                Arc[aNum].pts[pNo].y=pts[pNo].y;
			}
            aNum++;  ArcID=atoi(s2);  pNum=0;
            if(s2[0]=='E')break;
		}
        else{pts[pNum].x=atof(s1); pts[pNum++].y=atof(s2);}
	  }
      arcNumber = aNum;
	  nFlagArc=true;
	  int k=Arc[0].pNum;
	  CalcBoundArc();
	  CalcBoundGraph();
	  ZoomFull();
	  RefreshScreen();
    }
	fclose(fp);
}
//���������ݵ����
void CGISTinView::CalcBoundArc()
{
	xminArc=yminArc= 1.E+256;  xmaxArc=ymaxArc=-1.E+256;
	for(int i=0;i<arcNumber;i++)
  	 for(int j=0;j<Arc[i].pNum;j++)
	 {
		if(Arc[i].pts[j].x>xmaxArc)
			xmaxArc=Arc[i].pts[j].x;  
		if(Arc[i].pts[j].x<xminArc)
			xminArc=Arc[i].pts[j].x;
		if(Arc[i].pts[j].y>ymaxArc)
			ymaxArc=Arc[i].pts[j].y;  
		if(Arc[i].pts[j].y<yminArc)
			yminArc=Arc[i].pts[j].y;  
	 }	 
}
//��������ݵ����
void CGISTinView::CalcBoundPoint()
{
  xminPoint=yminPoint=1.E+256; xmaxPoint=ymaxPoint=-1.E+256;
  for(int i=0;i<pointNumber;i++)
  {
    if(PointData[i].x<xminPoint) xminPoint=PointData[i].x;
    if(PointData[i].x>xmaxPoint) xmaxPoint=PointData[i].x;
    if(PointData[i].y<yminPoint) yminPoint=PointData[i].y;
    if(PointData[i].y>ymaxPoint) ymaxPoint=PointData[i].y;
  }
}
void CGISTinView::CalcBoundGraph()
{
    
    if(nFlagPoint==TRUE)
	{
	  if(xminPoint<xmin) xmin=xminPoint;
	  if(xmaxPoint>xmax) xmax=xmaxPoint;
      if(yminPoint<ymin) ymin=yminPoint;
	  if(ymaxPoint>ymax) ymax=ymaxPoint;
	}
	if(nFlagArc==TRUE)
	{
	  if(xminArc<xmin) xmin=xminArc;
	  if(xmaxArc>xmax) xmax=xmaxArc;
	  if(yminArc<ymin) ymin=yminArc;
	  if(ymaxArc>ymax) ymax=ymaxArc;
	}   
    dx=xmax-xmin;  dy=ymax-ymin;
}
void CGISTinView::OnLoadPointFile() 
{
 LoadFile(1);	
}

void CGISTinView::OnLoadLineFile() 
{
  LoadFile(2);
}
//-------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
//////////////////////ͼ����ʾ����//////////////////////////////////////////
void CGISTinView::DrawGraph(CDC*pDC)
{
// 	DrawPoint(pDC, PointData, pointNumber);
//	DrawPoint(pDC, PointCenter, CenterSize, false);
    DrawArc(pDC);	
 	DrawBinaryLeaf(pDC);	
	if (m_displayGrid)
	{
		DrawGrid(pDC);
	}	
	DrawTin(pDC,PointData);
}
void CGISTinView::DrawGrid(CDC* pDC)
{
	CPen  NewPen;
	NewPen.CreatePen(PS_DASH,1,RGB(0,255,0));
	CPen *OldPen=pDC->SelectObject(&NewPen);
	//��ʼ������
	double xStart=dXmin, yStart=dYmin;
	//��ֹ������
    double xEnd=dXmax, yEnd=dYmax;
	//ÿһС�����ĳ�����
//	double xcol_wide = dDx;
//	double yrow_wide = dDy;
	double xcol_wide = (xEnd-xStart)/100.0;
	double yrow_wide = (yEnd-yStart)/100.0;
	//�Ȼ���
	for(int row = 0; row <= 100; row ++)
	{
		PNT P1 ={xStart, yStart+yrow_wide*row};
		PNT P2 ={xEnd, yStart+yrow_wide*row };
		GetScreenPoint(&P1); GetScreenPoint(&P2); 
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P2.x,P2.y);       
	}
	//����
	for(int col = 0;col <= 100; col ++)
	{
		PNT P1 ={xStart+xcol_wide*col, yStart};
		PNT P2 ={xStart+xcol_wide*col, yEnd};
		GetScreenPoint(&P1); GetScreenPoint(&P2); 
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P2.x,P2.y);       
	}
	pDC->SelectObject(OldPen);
}

void CGISTinView::DrawBinaryLeaf(CDC* pDC)
{
	CPen  NewPen;
	NewPen.CreatePen(PS_SOLID,1,RGB(255,0,255));
	CPen *OldPen=pDC->SelectObject(&NewPen);
	for (Queue * pList = queuehead; pList != NULL; pList = pList->next)
	{
		BinaryTree *root = pList->treenode;
		if (root == NULL)
		{
			return;
		}
		//��ʼ������
	    double xStart = dXmin + root->nStartCol * dDx, yStart = dYmin + root->nStartLine * dDy;
	    //��ֹ������
        double xEnd =  dXmin + (root->nEndCol + 1) * dDx, yEnd = dYmin + (root->nEndLine + 1) * dDy;
	   
		PNT P1 ={xStart, yStart};
		PNT P2 ={xEnd, yEnd};
		GetScreenPoint(&P1); GetScreenPoint(&P2); 
     P1.x += 1, P1.y += 1;
		P2.x -= 1, P2.y -= 1;
		pDC->MoveTo(P1.x,P1.y);	pDC->LineTo(P2.x,P1.y);
		pDC->MoveTo(P2.x,P1.y);	pDC->LineTo(P2.x,P2.y); 
		pDC->MoveTo(P2.x,P2.y);	pDC->LineTo(P1.x,P2.y); 
		pDC->MoveTo(P1.x,P2.y);	pDC->LineTo(P1.x,P1.y); 
	}
	pDC->SelectObject(OldPen);
}

void CGISTinView::RefreshPoint(CDC *pDC,double x,double y, bool bFlag)
{
    PNT P = {x,y};
    GetScreenPoint(&P);
	if (bFlag)
	{
		pDC->SetPixel(P.x,P.y,RGB(255,0,0));
	}
    else
	{
	 pDC->Ellipse(P.x-2,P.y-2,P.x+2,P.y+2);
	}
}
void CGISTinView::DrawPoint(CDC* pDC, PointSet *Data, int size, bool bFlag)
{
	CPen  NewPen;
	CPen *OldPen = NULL;
	CBrush brush(RGB(0, 0, 255));
	CBrush *pOldbrush = NULL;
	if (bFlag)
	{	
 	   NewPen.CreatePen(PS_SOLID,8,RGB(255,0,0));
 	   OldPen=pDC->SelectObject(&NewPen);
	}
	else
	{		
 	   pOldbrush=pDC->SelectObject(&brush);
	}
 	for (int i=0;i<size;i++)
     	RefreshPoint(pDC,Data[i].x,Data[i].y,bFlag);
	if (bFlag)
	{	
 	  pDC->SelectObject(OldPen);
	}
    else
	{
		pDC->SelectObject(pOldbrush);
	}
}
void CGISTinView::DrawArc(CDC* pDC)
{
	CPen  NewPen;
	NewPen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	CPen *OldPen=pDC->SelectObject(&NewPen);
	for (int i=0;i<arcNumber;i++)
	   RefreshARC(pDC,Arc[i]);
	pDC->SelectObject(OldPen)->DeleteObject();
}
void CGISTinView::RefreshARC(CDC *pDC,ArcSet arc)
{
	for(int i=0;i<arc.pNum-1;i++)
		RefreshLine(pDC,arc.pts[i],arc.pts[i+1]);
}
void CGISTinView::RefreshLine(CDC *pDC,PNT pt1,PNT pt2)
{
	//����ԭ���ʻ����߶�pt1->pt2(��ͼ����)
    GetScreenPoint(&pt1);  GetScreenPoint(&pt2);
    double x1,x2,y1,y2,R=4000.,RxR=16.E+6,Dx,Dy,Dxy,Theta,kxy,x,y,Rxy;
    int IDA=0,IDB=0,ID_Draw=0;
    x1=pt1.x; y1=pt1.y;  x2=pt2.x; y2=pt2.y;
    if(x1*x1+y1*y1<=RxR)IDA=1;
    if(x2*x2+y2*y2<=RxR)IDB=1;
    if(IDA+IDB==2)ID_Draw=1;
    else if(IDA+IDB==1)
	{
		if(IDA==1)  {  Dx=x2-x1; Dy=y2-y1;  }
        if(IDB==1)  {  Dx=x1-x2; Dy=y1-y2;  }
        if(Dx==0.)    Theta=(Dy>=0.)? _PI/2.:-_PI/2.;
        else          Theta=atan(fabs(Dy/Dx));
        if(Dx>0&&Dy<0)Theta*=-1;
        if(Dx<0)      Theta=(Dy>=0.)?(_PI-Theta):(_PI+Theta);
        if(IDA==1)     {x2=x1+R*cos(Theta)*2;  y2=y1+R*sin(Theta)*2;}
        else if(IDB==1){x1=x2+R*cos(Theta)*2;  y1=y2+R*sin(Theta)*2;}
        ID_Draw=1;
	}
	else
	{ 
	  Dx=x2-x1;  Dy=y2-y1;  Dxy=x2*y1-x1*y2;
      if(Dxy*Dxy<R*R*(Dx*Dx+Dy*Dy))
	  {
		  Rxy=Dx*Dx+Dy*Dy;   x=-Dy*Dxy/Rxy;   y=Dx*Dxy/Rxy;
          if(isIn_Qujian(x,x1,x2)&&isIn_Qujian(y,y1,y2))
		  {
			  ID_Draw=1;
              if(fabs(Dy)>=fabs(Dx))
			  {
			  kxy=Dx/Dy; Dy=R*3.; Dx=kxy*Dy; x1=x+Dx; y1=y+Dy; x2=x-Dx; y2=y-Dy;
			  }
              else
			  {
				  kxy=Dy/Dx;  Dx=R*3.; Dy=kxy*Dx; x1=x+Dx; y1=y+Dy; x2=x-Dx; y2=y-Dy;
			  }
		  } 
		} 
	} 
	if(ID_Draw==1){pDC->MoveTo((int)x1,(int)y1); pDC->LineTo((int)x2,(int)y2);}
}
bool CGISTinView::isIn_Qujian(double x, double x1,double x2)
{
	if (x1>=x2)
	{
		if (x>=x2&&x<=x1)
		{
			return true;
		}
	}
	else
	{
		if (x>=x1&&x<=x2)
		{
			return true;
		}
	}
	return false;
}

void CGISTinView::OnDisplayGrid() 
{
	if (m_displayGrid == true)
	{
		m_displayGrid = false;
	}
	else if (m_displayGrid == false)
	{
		m_displayGrid = true;
	}
	RefreshScreen();
}

void CGISTinView::OnUpdateDisplayGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_displayGrid);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//1.��ʼ��դ��,��դ����������
void CGISTinView::InitialGrid(int &Line, int &Col, int &ndivision)
{
	int i = 0, j = 0;
	int C = 0, L = 0;
	double x = 0, y =0;
	CGridDlg dlgGrid;
  	int nsqrt = sqrt(pointNumber);
    dlgGrid.m_nLineSize = dlgGrid.m_nColumnSize = nsqrt * 4;
	dlgGrid.m_nDivision = 2;
    if(dlgGrid.DoModal()==IDOK)
	{
	  Line = dlgGrid.m_nLineSize; 
	  Col =dlgGrid.m_nColumnSize;
	  ndivision = dlgGrid.m_nDivision;
	}
	else
		return;
	dXdelt = dx, dYdelt = dy;
    dXmin = xmin -dXdelt*0.01;
    dXmax = xmax +dXdelt*0.01;
    dYmin = ymin -dYdelt*0.01;
    dYmax = ymax +dYdelt*0.01;
    dXdelt = dXmax-dXmin;
    dYdelt = dYmax-dYmin;
	if (dXdelt < dYdelt)
	{
		dDx = dXdelt/Col;
		Line = dYdelt/dDx;
		dDy = dYdelt/Line;
	}
	else if (dXdelt > dYdelt)
	{
		dDy = dYdelt/Line;
		Col = dXdelt/dDy;
		dDx = dXdelt/Col;
	}
	else
	{
		dDx = dXdelt/Col, dDy = dYdelt/Line;
	}
}

//2.���㼯���䵽դ�񳡸�����
GridField** CGISTinView::CraeteGridField(PointSet *Data, int num, int Line, int Col, double dxmin, double dymin, double ddx, double ddy)
{
	int i = 0, j = 0;
	int C = 0, L = 0;
	double x = 0, y = 0;
	//��ʼ����ά����
	GridField **GridArray = new GridField*[Line];
	for (i = 0; i < Line; i ++)
	{
		GridArray[i] = new GridField[Col];
	}
	for (i = 0; i < Line; i ++)
	  for (j = 0; j < Col; j++)
	  {
		  GridArray[i][j].pNum = 0;
		  GridArray[i][j].ptId = NULL;
		  GridArray[i][j].bvisited = false;
	  }
	//����ÿ�������ڵ����к�
    for(i = 0; i < num; i ++)
	{
      x=Data[i].x;    y = Data[i].y;
      C= (int)((x-dxmin) / ddx);
	  L= (int)((y-dymin) / ddy);
      Data[i].nC = C;
	  Data[i].nL = L;
      GridArray[L][C].pNum++;    //һ��դ���п�����һ���㡢����㣬Ҳ����û�е�
	}
	//���¿��������С
    for (i = 0; i < Line; i ++)
	 for (j = 0; j < Col; j++)
      if (GridArray[i][j].pNum != 0)    //��դ���еĵ�����Ϊ��ʱ������ռ�洢��
      {
		  GridArray[i][j].ptId = new int[GridArray[i][j].pNum];     //��ʱ�ͷ��ڴ棿������������
		  GridArray[i][j].pNum = 0;
      }
	//��ԭʼ�㼯���䵽��Ӧ�ĸ�����
    for(i = 0; i < num; i ++)
	{
      C = Data[i].nC;
	  L = Data[i].nL;
	  GridArray[L][C].ptId[GridArray[L][C].pNum] = Data[i].ID;
      GridArray[L][C].pNum++;
	}
	 return GridArray;
}

//3.����Ӧ����
Queue* CGISTinView::CreateQueue(GridField **GridArray, int Line, int Col, int ndivision, int num)
{
	BinaryTree *treeroot = GreateTreeNode(0, Line - 1, 0, Col - 1, num, 0);
	Queue *queuefirst = NULL, *queuelast = NULL;
    queuefirst = queuelast = GreateQueueElement(treeroot);
	Queue *List = new Queue;
	AdaptiveDivision(queuefirst, queuelast, ndivision, GridArray, List);
	queuelast->next = List->next;
	delete List;
	return queuefirst;
}

//�������еĽڵ�
Queue* CGISTinView::GreateQueueElement(BinaryTree *element)
{
	Queue *pCreateElement = new Queue;
	pCreateElement->treenode = element;
	pCreateElement->next = NULL;
	return pCreateElement;
} 

//�����������Ľڵ�
BinaryTree* CGISTinView::GreateTreeNode(int startLine, int endLine, int startCol, int endCol, int pNum, int level)
{
	BinaryTree *pCreateNode = new BinaryTree;
	pCreateNode->nStartLine = startLine;  pCreateNode->nStartCol = startCol;
	pCreateNode->nEndLine = endLine;      pCreateNode->nEndCol = endCol;
	pCreateNode->npNum = pNum;            pCreateNode->nLevel = level;
	return pCreateNode;
}

//����Ӧ����
void CGISTinView::AdaptiveDivision(Queue *&head, Queue *&rear, int ndivision, GridField **GridArray, Queue *List)
{
	while (head != NULL)
	{
		BinaryTree *pTreenode = head->treenode;
	    if (pTreenode->nLevel == ndivision)       //�ﵽ���Ѵ���ʱֹͣ
		{
		    return;
		}	
	    int pNumTotal = pTreenode->npNum; 
		if (pNumTotal < 2)       //����������ʱ
		{
		   if (head == rear)
		   {
			   return;
		   }
		   Queue *pTemp = head;
		   head = head->next;
		   pTemp->next = List->next;
		   List->next = pTemp;
		   continue;
		}
		int nCurrentTotal = 0;
	    double xdist = (pTreenode->nEndCol - pTreenode->nStartCol + 1) * dDx;
	    double ydist = (pTreenode->nEndLine - pTreenode->nStartLine + 1) * dDy;
	    if (xdist > ydist && pTreenode->nStartCol != pTreenode->nEndCol)   
		{
	       bool NotInColGroup = true;
		   int nCol = pTreenode->nStartCol;	
		   for (int i = pTreenode->nStartLine; i <= pTreenode->nEndLine; i ++)
		   {
			 nCurrentTotal += GridArray[i][nCol].pNum;
		   }
		   if (nCurrentTotal == pNumTotal)
		   {
			   NotInColGroup = false;
		   }
		   while (NotInColGroup)
		   {			
		 	  int nextColTotal = 0;
			  nCol++;
			  for (int i = pTreenode->nStartLine; i <= pTreenode->nEndLine; i ++)
				    nextColTotal += GridArray[i][nCol].pNum;
			  if (nCurrentTotal == pNumTotal)
			  {
			     NotInColGroup = false;
				 break;
			  }
			  if (fabs(pNumTotal - 2 * nCurrentTotal ) < fabs(pNumTotal - 2 * (nCurrentTotal + nextColTotal)))
			  {
				break;
			  }
			  nCurrentTotal += nextColTotal;				
		   }
		   if (NotInColGroup)
		   {
			   BinaryTree *LeftLeaf = GreateTreeNode(pTreenode->nStartLine, pTreenode->nEndLine, pTreenode->nStartCol,
			   nCol -1, nCurrentTotal, pTreenode->nLevel + 1);
		       BinaryTree *RightLeaf = GreateTreeNode(pTreenode->nStartLine, pTreenode->nEndLine, nCol, pTreenode->nEndCol,
			   pNumTotal - nCurrentTotal, pTreenode->nLevel + 1);
		       Queue *pElement = GreateQueueElement(LeftLeaf);
			   rear->next = pElement; rear = pElement;
			   pElement = GreateQueueElement(RightLeaf);
			   rear->next = pElement; rear = pElement;
		   }
		   else
		   {
			   if (head == rear)
			   {
			      return;
			   }
			   Queue *pTemp = head;
			   head = head->next;
			   pTemp->next = List->next;
			   List->next = pTemp;
			   continue;
		   }		   
		}
		else if (xdist > ydist && pTreenode->nStartCol == pTreenode->nEndCol)
		{
		   if (head == rear)
		   {
			 return;
		   }	
		   Queue *pTemp = head;
		   head = head->next;
		   pTemp->next = List->next;
		   List->next = pTemp;
		   continue;		  
		}
		else if (xdist <= ydist && pTreenode->nStartLine != pTreenode->nEndLine)
		{
			bool NotInLineGroup = true;
			int nLin = pTreenode->nStartLine;
			for (int i = pTreenode->nStartCol; i <= pTreenode->nEndCol; i ++)
			{
			    nCurrentTotal += GridArray[nLin][i].pNum;
			}
            if (nCurrentTotal == pNumTotal)
			{
			     NotInLineGroup = false;
			}
			while (NotInLineGroup)
			{			
				int nextLinTotal = 0;
				nLin ++;
				for (int i = pTreenode->nStartCol; i <= pTreenode->nEndCol; i ++)
				        nextLinTotal += GridArray[nLin][i].pNum;
				if (nCurrentTotal == pNumTotal)
				{
			        NotInLineGroup = false;
					break;
				}
				if (fabs(pNumTotal - 2 * nCurrentTotal ) < fabs(pNumTotal - 2 * (nCurrentTotal + nextLinTotal)))
				{
					break;
				}
				nCurrentTotal += nextLinTotal;				
			}
			if (NotInLineGroup)
			{
				BinaryTree *LeftLeaf = GreateTreeNode(pTreenode->nStartLine, nLin - 1, pTreenode->nStartCol,
			                                      pTreenode->nEndCol, nCurrentTotal, pTreenode->nLevel + 1);
		        BinaryTree *RightLeaf = GreateTreeNode(nLin, pTreenode->nEndLine, pTreenode->nStartCol, pTreenode->nEndCol,
			                                      pNumTotal - nCurrentTotal, pTreenode->nLevel + 1);
			   Queue *pElement = GreateQueueElement(LeftLeaf);
			   rear->next = pElement; rear = pElement;
			   pElement = GreateQueueElement(RightLeaf);
			   rear->next = pElement; rear = pElement;
			}
			else
			{
			   if (head == rear)
			   {
			      return;
			   }
			   Queue *pTemp = head;
			   head = head->next;
			   pTemp->next = List->next;
			   List->next = pTemp;
			   continue;
			}			
		}
		else if (xdist <= ydist && pTreenode->nStartLine == pTreenode->nEndLine)
		{
		   if (head == rear)
		   {
			  return;
		   }	
		   Queue *pTemp = head;
		   head = head->next;
		   pTemp->next = List->next;
		   List->next = pTemp;
		   continue;		
		}
		Queue *pTemp = head;
		head = head->next;
		delete pTemp;
		delete pTreenode;
	}
}

//4.��ȡÿ��Ľ������ĵ�
void CGISTinView::AbstractCenterPoint(GridField **GridArray, Queue *head, PointSet *&Data, int &num, PointSet *OriginalData)
{
	int L = 0, C = 0;
	num = 0;
	for (Queue *q = head; q != NULL; q = q->next)
	{
		num ++;
	}
	Data = new PointSet[num+4];
	num = 0;
    for (Queue *q = head; q != NULL; q = q->next)
	{
		BinaryTree *pTreeLeaf = q->treenode;
		L = (pTreeLeaf->nStartLine + pTreeLeaf->nEndLine) / 2;
		C = (pTreeLeaf->nStartCol + pTreeLeaf->nEndCol) / 2;
		if (GridArray[L][C].pNum != 0)
		{
			Data[num].ID = GridArray[L][C].ptId[0];
			Data[num].x = OriginalData[Data[num].ID].x;
			Data[num].y = OriginalData[Data[num].ID].y;	
			OriginalData[Data[num].ID].label=true;
			num ++;
			continue;
		}
		GridArray[L][C].bvisited = true;
		QueueForCenter *abstractfirst = NULL, *abstractlast = NULL;			
		abstractlast = abstractfirst = GreateElementForCenter(L, C);        
		while (abstractfirst != NULL)
        {
			if (GridArray[abstractfirst->nL][abstractfirst->nC].pNum > 0)
			{
				Data[num].ID = GridArray[abstractfirst->nL][abstractfirst->nC].ptId[0];
			    Data[num].x = OriginalData[Data[num].ID].x;
			    Data[num].y = OriginalData[Data[num].ID].y;
				OriginalData[Data[num].ID].label=true;
			    num ++;
				while (abstractfirst != NULL)
				{
					QueueForCenter *pdelete = abstractfirst;
					abstractfirst = abstractfirst->next;
					delete pdelete;
				}
				break;
			}
			if (abstractfirst->nC -1 >= pTreeLeaf->nStartCol && GridArray[abstractfirst->nL][abstractfirst->nC -1].bvisited == false)
			{
				QueueForCenter *pElement =GreateElementForCenter(abstractfirst->nL, abstractfirst->nC -1);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			if (abstractfirst->nC +1 <= pTreeLeaf->nEndCol && GridArray[abstractfirst->nL][abstractfirst->nC +1].bvisited == false)
			{
				QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL, abstractfirst->nC +1);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			if (abstractfirst->nL -1 >= pTreeLeaf->nStartLine && GridArray[abstractfirst->nL -1][abstractfirst->nC].bvisited == false)
			{
                QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL - 1, abstractfirst->nC);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			if (abstractfirst->nL +1 <= pTreeLeaf->nEndLine && GridArray[abstractfirst->nL +1][abstractfirst->nC].bvisited == false)
			{
				QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL + 1, abstractfirst->nC);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			//�ĸ��ǵ�
			if (abstractfirst->nC -1 >= pTreeLeaf->nStartCol && abstractfirst->nL - 1 >= pTreeLeaf->nStartLine
				&& GridArray[abstractfirst->nL-1][abstractfirst->nC -1].bvisited == false)
			{
				QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL - 1, abstractfirst->nC -1);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			if (abstractfirst->nC -1 >= pTreeLeaf->nStartCol && abstractfirst->nL + 1 <= pTreeLeaf->nEndLine
				&& GridArray[abstractfirst->nL+1][abstractfirst->nC -1].bvisited == false)
			{
				QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL + 1, abstractfirst->nC -1);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			if (abstractfirst->nC +1 <= pTreeLeaf->nEndCol && abstractfirst->nL + 1 <= pTreeLeaf->nEndLine
				&& GridArray[abstractfirst->nL+1][abstractfirst->nC +1].bvisited == false)
			{
				QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL + 1, abstractfirst->nC +1);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
			if (abstractfirst->nC +1 <= pTreeLeaf->nEndCol && abstractfirst->nL - 1 >= pTreeLeaf->nStartLine
				&& GridArray[abstractfirst->nL-1][abstractfirst->nC +1].bvisited == false)
			{
				QueueForCenter *pElement = GreateElementForCenter(abstractfirst->nL - 1, abstractfirst->nC +1);
                InsertElementForCenter(abstractlast, pElement, GridArray);
			}
            QueueForCenter *pdelete = abstractfirst;
			abstractfirst = abstractfirst->next;
			delete pdelete;
        }
	}
}
void CGISTinView::InsertElementForCenter(QueueForCenter *&rear, QueueForCenter *insertelement, GridField **GridArray)
{
	rear->next = insertelement; 
	rear = insertelement;
	GridArray[insertelement->nL][insertelement->nC].bvisited = true;
}
QueueForCenter* CGISTinView::GreateElementForCenter(int L, int C)
{
	QueueForCenter *pElement = new QueueForCenter;
    pElement->nC = C;
	pElement->nL = L;
	pElement->next = NULL;
	return pElement;
}
//5.ɾ���ڴ�
void CGISTinView::ClearGridFieldMemory(GridField **GridArray, int Line, int Col)
{
	for (int i = 0; i < nLineSize; i ++)
	for (int j = 0; j < nColSize; j++)
	if (GridArray[i][j].pNum != 0)
	{
		delete GridArray[i][j].ptId;
	}
	for (int i = 0; i < nLineSize; i ++)
	{
		delete[]GridArray[i];
	}
	delete GridArray;
}
void CGISTinView::ClearQueueMemory(Queue *head, PointSet *Data)
{
	while (head != NULL)
	{
		Queue *pdeleteQueue = head;
		BinaryTree *pdeleteTree = pdeleteQueue->treenode;
		head = head->next;
		delete pdeleteTree;
		delete pdeleteQueue;
	}
	if (Data != NULL)
	{
		delete []Data;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////����Ӧ����///////////////////////////////////////////////////////////////
void CGISTinView::OnBinaryGroup() 
{ 
   int ndivision = 0;  
   InitialGrid(nLineSize, nColSize, ndivision);  //��ʼ������դ����
   GridField **GridArray = CraeteGridField(PointData,pointNumber,nLineSize,nColSize,dXmin,dYmin,dDx,dDy);
   queuehead = CreateQueue(GridArray, nLineSize, nColSize, ndivision, pointNumber);  
   AbstractCenterPoint(GridArray, queuehead, PointCenter, CenterSize,PointData);
   ClearGridFieldMemory(GridArray, nLineSize, nColSize);
   RefreshScreen();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////




//****************************************************
//-------------Tinɨ�����㷨--------------------------
void CGISTinView::swap(PointSet r[],int i,int j)
{
	double x, y, xuhao;
	x=r[i].x; y=r[i].y; xuhao=r[i].ID;
	r[i].x=r[j].x; r[i].y=r[j].y; r[i].ID=r[j].ID;	
	r[j].x=x; r[j].y=y; r[j].ID=xuhao;
}

//����������x����
//������point���������ݼ�
void CGISTinView::qs_x(PointSet *point,int left,int right)
{
  int i,j,x,y,z;
  i=left;j=right;
  x=point[(left+right)/2].x;
  int ID;
  do{
	   while((point[i].x>x)&&(i<right)) i++;
	   while((x>point[j].x)&&(j>left))  j--;
	   if(i<=j)
	   { y=point[i].x;	z=point[i].y; ID=point[i].ID;
	     point[i].x=point[j].x; point[i].ID=point[j].ID;
		point[i].y=point[j].y; 	point[j].x=y;  point[j].y=z;point[j].ID=ID;
		i++;j--;
	   }
  }while(i<=j);
  if(left<j) 
	  qs_x(point,left,j);     
  if(i<right) 
	  qs_x(point,i,right);
}
int CGISTinView::Partition(PointSet r[],int first,int end)
{
  	int i=first,j=end;     //��ʼ��
	while(i<j)
	{
		while(i<j&&r[i].y<=r[j].y) j--;  //�Ҳ�ɨ��
		if(i<j)
		{
			swap(r,i,j);       //����С��¼������ǰ��
			i++;
		}
		
		while(i<j&&r[i].y<=r[j].y) i++;    //���ɨ��
		if(i<j)
		{
			swap(r,i,j);   //���ϴ��¼����������
			j--;
		}
	}
	return i;   // iΪ��ֵ��¼������λ��
}
void CGISTinView::QuickSort(PointSet r[],int first,int end)
{
	if(first<end)
	{
	 int pivot=Partition(r,first,end);
	 QuickSort(r,first,pivot-1);
	 QuickSort(r,pivot+1,end);
	}
}
//
void CGISTinView::AddNewPointer(TRIANGLE *tinNode1,TRIANGLE *tinNode2)
{
	int q1, q2, no;
	if(HaveSameEdgeScan(tinNode1->ID1,tinNode1->ID2,tinNode1->ID3,tinNode2->ID1,tinNode2->ID2,tinNode2->ID3,&q1,&q2))
	{
		no=TheNumber(q1,q2,tinNode1->ID1,tinNode1->ID2,tinNode1->ID3);
		if(no==1)  tinNode1->p1tin=tinNode2;
		else if(no==2) tinNode1->p2tin=tinNode2;
		else if(no==3) tinNode1->p3tin=tinNode2;
		no=TheNumber(q1,q2,tinNode2->ID1,tinNode2->ID2,tinNode2->ID3);
		if(no==1)  tinNode2->p1tin=tinNode1;
		else if(no==2) tinNode2->p2tin=tinNode1;
		else if(no==3) tinNode2->p3tin=tinNode1;
	}
	else
		AfxMessageBox("AddNewPointer����"); 
}
//�����������ε���������
//������a��b��c���������ζ�����
//		p�������������
//		tinlast������������βָ��
void CGISTinView::AddNewTin(int a,int b,int c,TRIANGLE *p,TRIANGLE **tinlast)
{
  TRIANGLE *tinNew=new TRIANGLE;
  tinNew->back=NULL;  tinNew->next=NULL;
  tinNew->p1tin=NULL;tinNew->p2tin=NULL;tinNew->p3tin=NULL;
  tinNew->ID1=a;   tinNew->ID2=b;   tinNew->ID3=c; 
  int SeqNum=(*tinlast)->g_SeqNum;
  tinNew->g_SeqNum=++SeqNum;
  tinNew->p1tin=NULL;  tinNew->p2tin=NULL;   tinNew->p3tin=NULL;
  (*tinlast)->next=tinNew;  tinNew->back=*tinlast;
  *tinlast=tinNew;
  int q1, q2, no;
  if(HaveSameEdgeScan(tinNew->ID1,tinNew->ID2,tinNew->ID3,p->ID1,p->ID2,p->ID3,&q1,&q2))
  {
	no=TheNumber(q1,q2,tinNew->ID1,tinNew->ID2,tinNew->ID3);
	if(no==1)  tinNew->p1tin=p;
	else if(no==2) tinNew->p2tin=p;
	else if(no==3) tinNew->p3tin=p;
	no=TheNumber(q1,q2,p->ID1,p->ID2,p->ID3);
	if(no==1)  p->p1tin=tinNew;
	else if(no==2) p->p2tin=tinNew;
	else if(no==3) p->p3tin=tinNew;
  }
  else
	AfxMessageBox("AddNewTin����"); 
}
int CGISTinView::TheOtherPoint(int p1,int p2,int a,int b,int c)
{
	if ((p1==a&&p2==b)||(p1==b&&p2==a))
	{
		return c;
	}
	else if ((p1==a&&p2==c)||(p1==c&&p2==a))
	{
		return b;
	}
	else if ((p1==b&&p2==c)||(p1==c&&p2==b))
	{
		return a;
	}
	else
		return -1;
}
//�����������ε���������
//������a��b��c���������ζ�����
//		p�������������
//		tinlast������������βָ��
void CGISTinView::AddNewTin(int a,int b,int c,TRIANGLE *p,TRIANGLE **tinlast,PointSet *original)
{
	TRIANGLE *tinNew=new TRIANGLE;
	tinNew->back=NULL;  
	tinNew->next=NULL;
	tinNew->ID1=a;   
	tinNew->ID2=b;   
	tinNew->ID3=c; 
	int SeqNum=(*tinlast)->g_SeqNum;
	tinNew->g_SeqNum=++SeqNum;
	tinNew->p1tin=NULL;  
	tinNew->p2tin=NULL;  
	tinNew->p3tin=NULL;
	(*tinlast)->next=tinNew;  
	tinNew->back=*tinlast;
	*tinlast=tinNew;
	int q1, q2, no;			//q1��q2��ͬ�ߵ���������
	int q3,q4;	
	if(HaveSameEdgeScan(tinNew->ID1,tinNew->ID2,tinNew->ID3,p->ID1,p->ID2,p->ID3,&q1,&q2))
	{
		no=TheNumber(q1,q2,tinNew->ID1,tinNew->ID2,tinNew->ID3);
		if(no==1)  tinNew->p1tin=p;
		else if(no==2) tinNew->p2tin=p;
		else if(no==3) tinNew->p3tin=p;
		no=TheNumber(q1,q2,p->ID1,p->ID2,p->ID3);
		if(no==1)  p->p1tin=tinNew;
		else if(no==2) p->p2tin=tinNew;
		else if(no==3) p->p3tin=tinNew;
		q3=TheOtherPoint(q1,q2,p->ID1,p->ID2,p->ID3);
		q4=TheOtherPoint(q1,q2,tinNew->ID1,tinNew->ID2,tinNew->ID3);
		if (PointInCircle(original,q4,p->ID1,p->ID2,p->ID3))
		{
			EdgeChange(q3,q4,q1,q2,&p,&tinNew);
		}
		
	}
	else
		AfxMessageBox("AddNewTin����"); 
}
//�����һ���������д��ڹ��ߵ����
//������a��b��c���������������ζ�������
//		p���������ε�ͷָ�룬ͨ��p�����ߵ�������μ������������
//		tinlast������������βָ��
void CGISTinView::FirAddTin(int a,int b,int c,TRIANGLE **p,TRIANGLE **tinlast)
{
  TRIANGLE *tinNew=new TRIANGLE;
  tinNew->back=NULL;  tinNew->next=NULL;
  tinNew->ID1=a;   tinNew->ID2=b;   tinNew->ID3=c;
  int SeqNum=(*tinlast)->g_SeqNum;
  tinNew->g_SeqNum=++SeqNum;
  tinNew->p1tin=NULL;  tinNew->p2tin=NULL;   tinNew->p3tin=NULL;
  (*tinlast)->next=tinNew;  tinNew->back=*tinlast;
  *tinlast=tinNew;
  if(*p!=NULL)
  {
	int n=TheNumber(a,b,a,b,c);   
	if(n==1) tinNew->p1tin=*p;    //
	else if(n==2) tinNew->p2tin=*p;
	else if(n==3) tinNew->p3tin=*p;
	else
		AfxMessageBox("FirAddTin����");
	n=TheNumber(a,b,(*p)->ID1,(*p)->ID2,(*p)->ID3);
	if(n==1) (*p)->p1tin=tinNew;
    else if(n==2) (*p)->p2tin=tinNew;
    else if(n==3) (*p)->p3tin=tinNew;
	else
		AfxMessageBox("FirAddTin����");
  }
  *p=tinNew;	
}
//���ǰ�Ǻ͵׿ǵ��ڴ�
void CGISTinView::ClearPartionMemory(TREENODE *treefirst, FB *fbfirst)
{
	while(treefirst!=NULL)
	{
		TREENODE	*treeNode=treefirst;
		treefirst=treefirst->next;
		delete treeNode;
	}
	while(fbfirst!=NULL)
	{
		FB *fbNode=fbfirst;
		fbfirst=fbfirst->next;
		delete fbNode;
	}
}
void CGISTinView::ClearTinMemory(TRIANGLE **tin)
{
	TRIANGLE *t;
	while((*tin)!=NULL)
	{
      t=*tin;
	  *tin=(*tin)->next;
	  delete t;
	}
}
//�����һ��������
//������tinfirst������������ͷָ�룻tinlast����βָ��
//		a,b,c������һ�������ε������������
void CGISTinView::AddFirstTin(TRIANGLE **tinfirst, TRIANGLE **tinlast,int a,int b,int c)
{
   TRIANGLE *tinNew=new TRIANGLE;
   tinNew->ID1=a;    tinNew->ID2=b;    tinNew->ID3=c;
   tinNew->p1tin=NULL;    tinNew->p2tin=NULL;  tinNew->p3tin=NULL;
   tinNew->g_SeqNum=0;
   tinNew->next=NULL;   tinNew->back=NULL;
   *tinfirst=*tinlast=tinNew;
}
void CGISTinView::TreeDelete(TREENODE *p,int no,TREENODE **Root)
{
	TREENODE *f,*a,*b,*s,*h;
	TREENODE *root=*Root;
	f=p->parent;
	if(p->lchild==NULL&&p->rchild==NULL)  h=NULL;
	else if(p->lchild==NULL) h=p->rchild;
	else if(p->rchild==NULL) h=p->lchild;
	else 
	{
		a=p->lchild;b=p->rchild;
		if(b->lchild==NULL) {h=b;b->lchild=a;a->parent=b;}
		else 
		{
			h=b;
			while(h->lchild)
			{s=h;h=h->lchild;}
			s->lchild=h->rchild;
			if(h->rchild!=NULL) h->rchild->parent=s;
			h->rchild=b;
			if(b!=NULL) b->parent=h;
			h->lchild=a;
			if(a!=NULL) a->parent=h;
		}
	}
	if(f==NULL) {root=h;if(h!=NULL) h->parent=NULL;*Root=root;}
	else if(f->lchild==p)  {f->lchild=h;if(h!=NULL) h->parent=f;}
	else   {f->rchild=h;if(h!=NULL) h->parent=f;}
}
//���¼���ĵ����ɵ������εı߼����ǰ����
//������Point���������ݼ�
//		p������ǰ�ڵ�
//		tin������������βָ��
//		no�����ڵ�ı��
//		flag���������־
void CGISTinView::TreeInsert(PointSet *Point,TREENODE *p,TRIANGLENODE *tin,int no,int flag)    //������root
{
   	TREENODE *r;
	if(p!=NULL)
	{  
		r=new TREENODE;
		r->no=no;
		r->lchild=r->rchild=r->parent=NULL;	
		r->tri=tin;
		if(Point[p->no].x>Point[no].x)   
		{
			p->lchild=r;r->parent=p;
		}
		else
		{
			p->rchild=r;r->parent=p;
		}
		if(flag==0)
		{
			r->next=p->next;
			r->back=p;
			p->next->back=r;
			p->next=r;
		}
		else 
		{
			r->back=p->back;
			r->next=p;
			p->back->next=r;
			p->back=r;
		}
    }
}
void CGISTinView::TreeSearch(PointSet *Point, TREENODE **root,TREENODE **f,int no)
{
  	TREENODE *p,*q;
	p=*root;
	while(p)
	{
		q=p;
		if(Point[no].x<Point[p->no].x) 
			p=p->lchild;
		else 
			p=p->rchild;
	}
	*f=q;
}
int CGISTinView::SameEdge(int P1,int P2,int PA,int PB)
{
 int Same=-1;  if(P1==PA&&P2==PB)Same=1;
  else if(P1==PB&&P2==PA)Same=1;
  return Same;
}

//��c��ֱ��ab�Ϸ��ͷ���
int CGISTinView::PointUpLine(PointSet *Point,int a,int b,int c)  
{
	int t=0;
	if(Point[a].x==Point[b].x) 
		t=1;
	else if((Point[a].y-Point[b].y)/(Point[a].x-Point[b].x)>=0)
	{
		if(Point[a].x==Point[c].x)
		{ 
			if(Point[c].y>Point[a].y)
				t=1; 
		}
		else if((Point[a].y-Point[c].y)/(Point[a].x-Point[c].x)>=(Point[a].y-Point[b].y)/(Point[a].x-Point[b].x))
			t=1;
	}
	else if((Point[a].y-Point[b].y)/(Point[a].x-Point[b].x)<0)
	{
		if(Point[a].x==Point[c].x)
		{
			if(Point[c].y>Point[a].y)
				t=1;
		}
		else if((Point[a].y-Point[c].y)/(Point[a].x-Point[c].x)>=(Point[a].y-Point[b].y)/(Point[a].x-Point[b].x))
			t=1;
	}
	return t;
}

int CGISTinView::HaveSameEdgeScan(int P1,int P2,int P3,int PA,int PB,int PC,int *Q1,int *Q2)
{
	int Q=0;
	if(SameEdge(P1,P2,PA,PB)==1){*Q2=P1;*Q1=P2;Q=1;}
	else if(SameEdge(P1,P2,PB,PC)==1){*Q2=P1;*Q1=P2;Q=1;}
	else if(SameEdge(P1,P2,PC,PA)==1){*Q2=P1;*Q1=P2;Q=1;}
	else if(SameEdge(P2,P3,PA,PB)==1){*Q2=P2;*Q1=P3;Q=1;}
	else if(SameEdge(P2,P3,PB,PC)==1){*Q2=P2;*Q1=P3;Q=1;}
	else if(SameEdge(P2,P3,PC,PA)==1){*Q2=P2;*Q1=P3;Q=1;}
	else if(SameEdge(P1,P3,PA,PB)==1){*Q2=P1;*Q1=P3;Q=1;}
	else if(SameEdge(P1,P3,PB,PC)==1){*Q2=P1;*Q1=P3;Q=1;}
	else if(SameEdge(P1,P3,PC,PA)==1){*Q2=P1;*Q1=P3;Q=1;} 
	return Q;
}
//�ж�m,n
//ThuNumber�������Ƚ�a,b,c����������С��������
//Ȼ��b,c������õ��������������
//������������ǰ������ȣ��򷵻�1����b,c��ֵΪa,b,c��������С��ֵ
//���������ȣ��򷵻�2����b,cΪa,b,c�нϴ������ֵ
//��ǰ���������,�򷵻�3,��b,cΪa,b,c��һ��һ������ֵ
//��������Ҫ�������򣨰�װyֵ��С�������򣩣����Ե��������Сֱ�����
int CGISTinView::TheNumber(int m,int n,int a,int b,int c)
{
    int d[3]={a, b, c};	
	int t;
	if(d[1]<d[0]) {t=d[0];d[0]=d[1];d[1]=t;}
	if(d[2]<d[0]) {t=d[0];d[0]=d[2];d[2]=t;}
	if(d[2]<d[1]) {t=d[1];d[1]=d[2];d[2]=t;}   //��С��������
	if((m==d[0]&&n==d[1])||(n==d[0]&&m==d[1]))  //m��n����ǰ����ֵ
		return 1; 
	else if((m==d[1]&&n==d[2])||(n==d[1]&&m==d[2]))  //m��n���ں�����ֵ
		return 2;
	else if((m==d[0]&&n==d[2])||(n==d[0]&&m==d[2]))  //m��n����ǰ������ֵ
		return 3;
	else
	{   
		AfxMessageBox("TheNumber��������!");
		return -1;
	}
}
//���2�����¼�����ͶӰδ�䵽ǰ����
//������i�����ڵ���
//		Data���������ݼ���pNum�������ݼ���С
//		tinfirst������������ͷָ�룻tinlast������������βָ��
//		root��frontf��frontl�����������������洢ǰ��
//		basef��basel����˫�������洢�׿�
void CGISTinView::Circs2(int i, PointSet *Point, TRIANGLE **tinlast, TREENODE **Root,TREENODE **Frontf, TREENODE **Frontl, FB **Basef, FB **Basel)
{
  
	TREENODE *frontf=*Frontf, *frontl=*Frontl;
    FB *basef=*Basef, *basel=*Basel;

	TREENODE *treeNew = NULL;
    FB *fbNew= NULL, *tfNode1= NULL, *tfNode2= NULL;
	int a=0,b=0,c=0;
	int tusanjiao=0;    //��ʾ��������ͻ������,���ڶ���ʱ�Ͳ����ٹ�����ȥ��
	float a1=0,b1=0,c1=0;
	TRIANGLE *tinNode= NULL, *tinSave= NULL, *t= NULL;
    //��������
	if(Point[i].x<=Point[frontf->no].x)   //i��x����С��ǰ�ǵ�first�ڵ��x����
	{
		if(((Point[i].y-Point[frontf->no].y)*(Point[frontf->next->no].x-Point[frontf->no].x)<=                         //         /
			(Point[i].x-Point[frontf->no].x)*(Point[frontf->next->no].y-Point[frontf->no].y))&&						   //	  \  /
		   ((Point[frontf->no].y-Point[frontf->next->no].y)/(Point[frontf->no].x-Point[frontf->next->no].x)<=0))       //      \/
		{
			treeNew=new TREENODE;
			treeNew->no=i;  
			treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
			treeNew->back=treeNew->next=NULL;
			treeNew->parent=frontf;  frontf->lchild=treeNew;
			treeNew->next=frontf;  frontf->back=treeNew;
			AddNewTin(Point[i].ID, Point[basef->no].ID, Point[basef->next->no].ID, basef->tri, tinlast);
//			AddNewTin(Point[i].ID, Point[basef->no].ID, Point[basef->next->no].ID, basef->tri, tinlast,Point);
			treeNew->tri=*tinlast;
			frontf=treeNew;
	  
			fbNew=new FB; fbNew->no=i; //�ǵ׵ĳ�������
			fbNew->tri=*tinlast;
			fbNew->back=NULL;		fbNew->next=NULL;
			fbNew->next=basef->next;
			basef->next->back=fbNew;
			basef=fbNew;           //ע�⣺�Ͽ���ָ��Ӧ��ɾ�����ͷ��ڴ�
			tfNode1=basef->next;  
			tfNode2=tfNode1->next; //�ǵ������εĴ���
			if(tfNode2!=NULL)
			{
				a=i;  b=tfNode1->no;   c=tfNode2->no;  
				tinNode=tfNode1->tri;
				//�����c��ֱ��ab���Ҳ�,ѭ������
				while((Point[b].y-Point[a].y)*(Point[c].x-Point[a].x)>(Point[c].y-Point[a].y)*(Point[b].x-Point[a].x))
				{
					AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, basef->tri, tinlast);
					//AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, basef->tri, tinlast,Point);
					AddNewPointer(tinNode,*tinlast);		
					basef->next=tfNode2;   basef->tri=*tinlast;
					tfNode2->back=basef;
					if(tfNode2->next==NULL) 
					{
					  *Frontf=frontf;*Frontl=frontl;
					  *Basef=basef;    *Basel=basel;
					   return;		  
					}
					else
					{ 
						tfNode1=tfNode2;  tfNode2=tfNode2->next;
						b=tfNode1->no;  c=tfNode2->no;
						tinNode=tfNode1->tri;
					}
				}
			}
		}
		else
		{
			tinNode=frontf->tri;
			AddNewTin(Point[i].ID, Point[frontf->no].ID, Point[frontf->next->no].ID, tinNode, tinlast);
			//AddNewTin(Point[i].ID, Point[frontf->no].ID, Point[frontf->next->no].ID, tinNode, tinlast,Point);
			tinSave=*tinlast;
			treeNew=new TREENODE;
			treeNew->no=i;  treeNew->tri=*tinlast;
			treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
			treeNew->back=treeNew->next=NULL;
			treeNew->next=frontf->next; frontf->next->back=treeNew;
			frontf->lchild=treeNew;  treeNew->parent=frontf;
			TreeDelete(frontf,frontf->no,Root);
			frontf=treeNew;

			for(TREENODE *r=frontf->next;((r->next!=NULL)&&(tusanjiao==0));r=r->next)
			{
				a1=(Point[i].x-Point[r->no].x)*(Point[i].x-Point[r->no].x)+(Point[i].y-Point[r->no].y)*(Point[i].y-Point[r->no].y);
				b1=(Point[r->next->no].x-Point[r->no].x)*(Point[r->next->no].x-Point[r->no].x)+(Point[r->next->no].y-Point[r->no].y)*(Point[r->next->no].y-Point[r->no].y);
				c1=(Point[i].x-Point[r->next->no].x)*(Point[i].x-Point[r->next->no].x)+(Point[i].y-Point[r->next->no].y)*(Point[i].y-Point[r->next->no].y);
				if((a1+b1-c1)/(2.0*(sqrt(a1*b1)))>0)
				{
					tinNode=*tinlast;
					t=r->tri;
					if(r->next->next!=NULL)
						if((Point[r->next->next->no].ID==t->ID1)||(Point[r->next->next->no].ID==t->ID2)||(Point[r->next->next->no].ID==t->ID3))
							tusanjiao=1;
					AddNewTin(Point[i].ID, Point[r->no].ID,Point[r->next->no].ID, t, tinlast);
					//AddNewTin(Point[i].ID, Point[r->no].ID,Point[r->next->no].ID, t, tinlast,Point);
					AddNewPointer(tinNode, *tinlast);
					frontf->tri=*tinlast;
					frontf->next=r->next;
					r->next->back=frontf;
					TreeDelete(r,r->no,Root);
				}
				else 
					break;
			}
			//�ǵ׵ĳ�������
			//*****************************************************************
			if((Point[basef->next->no].x-Point[i].x)*(Point[basef->no].y-Point[i].y)<(Point[basef->no].x-Point[i].x)*(Point[basef->next->no].y-Point[i].y)) 
			{
				fbNew=new FB; fbNew->no=i; 
				fbNew->tri=tinSave;
				fbNew->back=NULL; fbNew->next=NULL;
				fbNew->next=basef;
				basef->back=fbNew;
				basef=fbNew;  
				*Frontf=frontf,   *Frontl=frontl;
				*Basef=basef,      *Basel=basel;
				return;		 
			}
			AddNewTin(Point[i].ID, Point[basef->no].ID, Point[basef->next->no].ID, tinSave, tinlast);
			//AddNewTin(Point[i].ID, Point[basef->no].ID, Point[basef->next->no].ID, tinSave, tinlast,Point);
			AddNewPointer(basef->tri,*tinlast);

			fbNew=new FB;  fbNew->no=i; 
			fbNew->tri=*tinlast;
			fbNew->back=NULL;fbNew->next=NULL;
			fbNew->next=basef->next;
			basef->next->back=fbNew;
			basef=fbNew;           //ע�⣺�Ͽ���ָ��Ӧ��ɾ�����ͷ��ڴ�
			tfNode1=basef->next; tfNode2=tfNode1->next; //�ǵ������εĴ���
			if(tfNode2!=NULL)
			{
				a=i;  b=tfNode1->no;  c=tfNode2->no;  t=*tinlast;
				tinNode=tfNode1->tri;
				while((Point[b].y-Point[a].y)*(Point[c].x-Point[a].x)>(Point[c].y-Point[a].y)*(Point[b].x-Point[a].x))
				{
					AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, t, tinlast);
					//AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, t, tinlast,Point);
					AddNewPointer(tinNode,*tinlast); 

					t=*tinlast;
					basef->next=tfNode2; basef->tri=*tinlast;
					tfNode2->back=basef;
					if(tfNode2->next==NULL)
					{
						*Frontf=frontf,   *Frontl=frontl;
						*Basef=basef,      *Basel=basel;
						return;
					}
					else
					{ 
						tfNode1=tfNode2;   tfNode2=tfNode2->next; 
						b=tfNode1->no;  c=tfNode2->no;
						tinNode=tfNode1->tri;
					}
				}
			}
		}
	}
	//���Ҳ�,���
	else if(Point[i].x>=Point[frontl->no].x)   
	{
		if(((Point[i].y-Point[frontl->no].y)*(Point[frontl->back->no].x-Point[frontl->no].x)>=
		(Point[i].x-Point[frontl->no].x)*(Point[frontl->back->no].y-Point[frontl->no].y))&&
		((Point[frontl->no].y-Point[frontl->back->no].y)/(Point[frontl->no].x-Point[frontl->back->no].x)>=0))
		{
			treeNew=new TREENODE;
			treeNew->no=i;  treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
			treeNew->back=treeNew->next=NULL;
			treeNew->parent=frontl;   frontl->rchild=treeNew;
			treeNew->back=frontl;   frontl->next=treeNew;
			treeNew->tri=NULL;
			AddNewTin(Point[i].ID, Point[basel->no].ID, Point[basel->back->no].ID, basel->back->tri, tinlast);
			//AddNewTin(Point[i].ID, Point[basel->no].ID, Point[basel->back->no].ID, basel->back->tri, tinlast,Point);
			frontl->tri=*tinlast;
			frontl=treeNew;

			fbNew=new FB; fbNew->no=i; //�ǵ׵ĳ�������
			fbNew->tri=NULL;
			fbNew->back=NULL; fbNew->next=NULL;
			fbNew->back=basel->back;
			basel->back->next=fbNew;
			basel->back->tri=*tinlast;
			basel=fbNew;
			tfNode1=basel->back;tfNode2=tfNode1->back; //�ǵ������εĴ���
			if(tfNode2!=NULL)
			{
				a=i;   b=tfNode1->no;   c=tfNode2->no;  
				t=*tinlast; 
				tinNode=tfNode2->tri;
				//��c��ֱ��ab��ߣ�ѭ������		
				while((Point[b].y-Point[a].y)*(Point[c].x-Point[a].x)<(Point[c].y-Point[a].y)*(Point[b].x-Point[a].x))
				{
					AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, t, tinlast);
					//AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, t, tinlast,Point);
					AddNewPointer(*tinlast,tinNode);		
					t=*tinlast;
					basel->back=tfNode2; tfNode2->tri=*tinlast;
					tfNode2->next=basel;
					if(tfNode2->back==NULL)
					{
						*Frontf=frontf,   *Frontl=frontl;
						*Basef=basef,      *Basel=basel;
						return;
					}
					else
					{ 
						tfNode1=tfNode2;  tfNode2=tfNode2->back;  b=tfNode1->no;
						c=tfNode2->no; tinNode=tfNode2->tri;
					}
				}
			}
		}
		else
		{
			tinNode=frontl->back->tri;
			AddNewTin(Point[i].ID, Point[frontl->no].ID, Point[frontl->back->no].ID, tinNode,tinlast);
			//AddNewTin(Point[i].ID, Point[frontl->no].ID, Point[frontl->back->no].ID, tinNode,tinlast,Point);
			//AddTinScan(i,frontl->no,frontl->back->no,&t);
			tinSave=*tinlast;
			treeNew=new TREENODE;
			treeNew->no=i;treeNew->tri=NULL;
			treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
			treeNew->back=NULL;treeNew->next=NULL;
			frontl->back->tri=*tinlast;
			treeNew->back=frontl->back;frontl->back->next=treeNew;
			frontl->rchild=treeNew;treeNew->parent=frontl;
			TreeDelete(frontl,frontl->no,Root);
			frontl=treeNew;
			for(TREENODE *r=frontl->back;((r->back!=NULL)&&(tusanjiao==0));r=r->back)
			{
				a1=(Point[i].x-Point[r->no].x)*(Point[i].x-Point[r->no].x)+(Point[i].y-Point[r->no].y)*(Point[i].y-Point[r->no].y);
				b1=(Point[r->back->no].x-Point[r->no].x)*(Point[r->back->no].x-Point[r->no].x)+(Point[r->back->no].y-Point[r->no].y)*(Point[r->back->no].y-Point[r->no].y);
				c1=(Point[i].x-Point[r->back->no].x)*(Point[i].x-Point[r->back->no].x)+(Point[i].y-Point[r->back->no].y)*(Point[i].y-Point[r->back->no].y);
				if((a1+b1-c1)/(2.0*(sqrt(a1*b1)))>0)
				{
					tinNode=r->tri;  t=r->back->tri;
					if(r->back->back!=NULL)
					if((Point[r->back->back->no].ID==t->ID1)||(Point[r->back->back->no].ID==t->ID2)||(Point[r->back->back->no].ID==t->ID3))
						tusanjiao=1;
					 AddNewTin(Point[i].ID, Point[r->no].ID, Point[r->back->no].ID, t,tinlast);
					 //AddNewTin(Point[i].ID, Point[r->no].ID, Point[r->back->no].ID, t,tinlast,Point);
					 t=*tinlast;
					 AddNewPointer(tinNode,*tinlast);
					 frontl->back=r->back;
					 r->back->next=frontl;
					 TreeDelete(r,r->no,Root);
					 frontl->back->tri=*tinlast;
				}
				else 
					break;
			}
			//�����Ǽ���ǵ׵�
			if((Point[basel->back->no].x-Point[i].x)*(Point[basel->no].y-Point[i].y)>(Point[basel->no].x-Point[i].x)*(Point[basel->back->no].y-Point[i].y)) 
			{
				fbNew=new FB; fbNew->no=i; 
				fbNew->tri=NULL;
				fbNew->back=NULL;fbNew->next=NULL;
				basel->tri=tinSave;
				basel->next=fbNew; fbNew->back=basel;
				basel=fbNew;
				*Frontf=frontf,   *Frontl=frontl;
				*Basef=basef,      *Basel=basel;
				return;		 
			}
			if(basel->back==NULL)
			{
				*Frontf=frontf,   *Frontl=frontl;
				*Basef=basef,      *Basel=basel;
				return;
			}
			AddNewTin(Point[i].ID, Point[basel->no].ID, Point[basel->back->no].ID, tinSave, tinlast);
			//AddNewTin(Point[i].ID, Point[basel->no].ID, Point[basel->back->no].ID, tinSave, tinlast,Point);
			AddNewPointer(*tinlast,basel->back->tri);	

			fbNew=new FB;   fbNew->no=i; 
			fbNew->tri=NULL;
			fbNew->back=NULL;fbNew->next=NULL;
			fbNew->back=basel->back;
			basel->back->next=fbNew;
			basel->back->tri=*tinlast;
			basel=fbNew;
			tfNode1=basel->back;tfNode2=tfNode1->back;
			if(tfNode2!=NULL)
			{
				a=i;b=tfNode1->no;c=tfNode2->no;   
				tinNode=*tinlast;
				t=tfNode2->tri;
				while((Point[b].y-Point[a].y)*(Point[c].x-Point[a].x)<(Point[c].y-Point[a].y)*(Point[b].x-Point[a].x))
				{
					AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, tinNode, tinlast);
					//AddNewTin(Point[a].ID, Point[b].ID, Point[c].ID, tinNode, tinlast,Point);
					AddNewPointer(*tinlast,t);	
					tinNode=*tinlast;
					basel->back=tfNode2;
					tfNode2->tri=*tinlast;
					tfNode2->next=basel;
					if(tfNode2->back==NULL) 
					{
						*Frontf=frontf,   *Frontl=frontl;
						*Basef=basef,      *Basel=basel;
						return;
					}
					else
					{ 
						tfNode1=tfNode2;  tfNode2=tfNode2->back;  b=tfNode1->no;c=tfNode2->no;
						t=tfNode2->tri; 
					}
				}
			} 
		}
	}
	*Frontf=frontf,   *Frontl=frontl;
	*Basef=basef,      *Basel=basel;
}

//���1�����¼�����ͶӰ�����䵽ǰ����
//������i�����ڵ���
//		Point���������ݼ���
//		p�����������������������Ľڵ�
//		tinlast������������βָ��
//		Root���������������ĸ��ڵ�
void CGISTinView::Circs1(int i,PointSet *Point,TREENODE *p,TRIANGLE **tinlast,TREENODE **Root)
{
	int flag=0;       //flag=1��ʾ����p��i���
	TRIANGLE *pp=NULL;
	TREENODE *lp=NULL,*rp=NULL,*t=NULL,*tfb=NULL,*r1=NULL,*np=NULL;
	float a=0,b=0,c=0;
	int tl=0,tr=0;
	if(Point[p->no].x>Point[i].x)//p��i���
    {
		np=p;p=np->back;flag=1;
	}
	else //p��i��ǰ
	{
		np=p->next;flag=0;
	}
	pp=p->tri;
	
	AddNewTin(Point[i].ID, Point[p->no].ID, Point[np->no].ID, pp,tinlast); 
	//AddNewTin(Point[i].ID, Point[p->no].ID, Point[np->no].ID, pp,tinlast,Point);
	pp=*tinlast;
	lp=p;		rp=np;
	//
	if(flag==0) 
		TreeInsert(Point,p,pp,i,flag);
	else 
		TreeInsert(Point,np,pp,i,flag);
	p->tri=pp;

	tfb=p->next;
	//-------------------����ǰ��-------------------------------//
	//��ǰ�ǵĺ�˸���
	for(t=lp;lp->back!=NULL;lp=r1)
	{
		a=(Point[i].x-Point[lp->no].x)*(Point[i].x-Point[lp->no].x)+(Point[i].y-Point[lp->no].y)*(Point[i].y-Point[lp->no].y);
		b=(Point[lp->back->no].x-Point[lp->no].x)*(Point[lp->back->no].x-Point[lp->no].x)+(Point[lp->back->no].y-Point[lp->no].y)*(Point[lp->back->no].y-Point[lp->no].y);
		c=(Point[i].x-Point[lp->back->no].x)*(Point[i].x-Point[lp->back->no].x)+(Point[i].y-Point[lp->back->no].y)*(Point[i].y-Point[lp->back->no].y);
		if((a+b-c)/(2.0*(sqrt(a*b)))>0)
		{
			if(PointUpLine(Point,lp->back->no,i,lp->no)==1) 
				break; 
			t=t->back;
			AddNewTin(Point[i].ID, Point[lp->no].ID, Point[lp->back->no].ID, (t->tri),tinlast);
			//AddNewTin(Point[i].ID, Point[lp->no].ID, Point[lp->back->no].ID, (t->tri),tinlast,Point);
			lp->back->next=tfb;
			tfb->back=lp->back;
			r1=lp->back;
			r1->tri=*tinlast;
			TreeDelete(lp,lp->no,Root);
			AddNewPointer(r1->tri,lp->tri);		
		}
		else 
			break;
	}
	tfb=p->next;
	//��ǰ�ǵ�ǰ�˸���
	for(t=rp;rp->next!=NULL;t=rp=r1)
	{
		a=(Point[i].x-Point[rp->no].x)*(Point[i].x-Point[rp->no].x)+(Point[i].y-Point[rp->no].y)*(Point[i].y-Point[rp->no].y);
		b=(Point[rp->next->no].x-Point[rp->no].x)*(Point[rp->next->no].x-Point[rp->no].x)+(Point[rp->next->no].y-Point[rp->no].y)*(Point[rp->next->no].y-Point[rp->no].y);
		c=(Point[i].x-Point[rp->next->no].x)*(Point[i].x-Point[rp->next->no].x)+(Point[i].y-Point[rp->next->no].y)*(Point[i].y-Point[rp->next->no].y);
		if((a+b-c)/(2.0*(sqrt(a*b)))>0)
		{
			if(PointUpLine(Point,i,rp->next->no,rp->no)==1) 
				break; 
			AddNewTin(Point[i].ID, Point[rp->no].ID, Point[rp->next->no].ID, t->tri,tinlast);
			//AddNewTin(Point[i].ID, Point[rp->no].ID, Point[rp->next->no].ID, t->tri,tinlast,Point);
			rp->next->back=tfb;
			tfb->next=rp->next;
			r1=rp->next;
			TreeDelete(rp,rp->no,Root);
			AddNewPointer(tfb->tri, *tinlast);		
			tfb->tri=*tinlast;	 
		}
		else 
			break;
	}
}
void CGISTinView::Circs1(int i,PointSet *Point,TREENODE *p,TRIANGLE **tinlast,TREENODE **Root,FB **basel,FB **baser)
{
	int flag=0;       //flag=1��ʾ����p��i���
	TRIANGLE *pp=NULL;
	TREENODE *lp=NULL,*rp=NULL,*t=NULL,*tfb=NULL,*r1=NULL,*np=NULL;
	float a=0,b=0,c=0;
	int tl=0,tr=0;
	if(Point[p->no].x>Point[i].x)//p��i���
    {
		np=p;p=np->back;flag=1;
	}
	else //p��i��ǰ
	{
		np=p->next;flag=0;
	}
	pp=p->tri;
	
	if (PointInCircle(PointData,Point[i].ID,pp->ID1,pp->ID2,pp->ID3)) //i����pp�����Բ�ڣ�������
	{
		//����һ���µ������β����뵽tin�У��ߵ��ڽӹ�ϵδȷ��
		TRIANGLENODE *newTri = new TRIANGLENODE;
		newTri->back=NULL;  newTri->next=NULL;
		newTri->p1tin=NULL;newTri->p2tin=NULL;newTri->p3tin=NULL;
		newTri->ID1=Point[i].ID;		
		newTri->ID2=Point[np->no].ID;
		if (Point[p->no].ID!=pp->ID1&&Point[np->no].ID!=pp->ID1)
		{
			newTri->ID3=pp->ID1;
		}
		if (Point[p->no].ID!=pp->ID2&&Point[np->no].ID!=pp->ID2)
		{
			newTri->ID3=pp->ID2;
		}
		if (Point[p->no].ID!=pp->ID3&&Point[np->no].ID!=pp->ID3)
		{
			newTri->ID3=pp->ID3;
		}
		int SeqNum=(*tinlast)->g_SeqNum;
		newTri->g_SeqNum=++SeqNum;
		(*tinlast)->next=newTri;
		newTri->back=(*tinlast);
		*tinlast=newTri;
		//�޸�pp�Ķ���
		pp->ID1=Point[p->no].ID;
		pp->ID2=Point[i].ID;
		pp->ID3=newTri->ID3;
		//����ppԭ�е��ڽӹ�ϵ
		TRIANGLENODE *p1tin=pp->p1tin,*p2tin=pp->p2tin,*p3tin=pp->p3tin;
		if(p1tin!=NULL)
		{
			int q1,q2,no;
			if(HaveSameEdgeScan(pp->ID1,pp->ID2,pp->ID3,p1tin->ID1,p1tin->ID2,p1tin->ID3,&q1,&q2))
			{
				no=TheNumber(q1,q2,p1tin->ID1,p1tin->ID2,p1tin->ID3);
				if(no==1)  p1tin->p1tin=pp;
				else if(no==2) p1tin->p2tin=pp;
				else if(no==3) p1tin->p3tin=pp;
				no=TheNumber(q1,q2,pp->ID1,pp->ID2,pp->ID3);
				if(no==1)  pp->p1tin=p1tin;
				else if(no==2) pp->p2tin=p1tin;
				else if(no==3) pp->p3tin=p1tin;
			}
			if(HaveSameEdgeScan(newTri->ID1,newTri->ID2,newTri->ID3,p1tin->ID1,p1tin->ID2,p1tin->ID3,&q1,&q2))
			{
				no=TheNumber(q1,q2,p1tin->ID1,p1tin->ID2,p1tin->ID3);
				if(no==1)  p1tin->p1tin=newTri;
				else if(no==2) p1tin->p2tin=newTri;
				else if(no==3) p1tin->p3tin=newTri;
				no=TheNumber(q1,q2,newTri->ID1,newTri->ID2,newTri->ID3);
				if(no==1)  newTri->p1tin=p1tin;
				else if(no==2) newTri->p2tin=p1tin;
				else if(no==3) newTri->p3tin=p1tin;
			}
		}
		if(p2tin!=NULL)
		{
			int q1,q2,no;
			if(HaveSameEdgeScan(pp->ID1,pp->ID2,pp->ID3,p2tin->ID1,p2tin->ID2,p2tin->ID3,&q1,&q2))
			{
				no=TheNumber(q1,q2,p2tin->ID1,p2tin->ID2,p2tin->ID3);
				if(no==1)  p2tin->p1tin=pp;
				else if(no==2) p2tin->p2tin=pp;
				else if(no==3) p2tin->p3tin=pp;
				no=TheNumber(q1,q2,pp->ID1,pp->ID2,pp->ID3);
				if(no==1)  pp->p1tin=p2tin;
				else if(no==2) pp->p2tin=p2tin;
				else if(no==3) pp->p3tin=p2tin;
			}
			if(HaveSameEdgeScan(newTri->ID1,newTri->ID2,newTri->ID3,p2tin->ID1,p2tin->ID2,p2tin->ID3,&q1,&q2))
			{
				no=TheNumber(q1,q2,p2tin->ID1,p2tin->ID2,p2tin->ID3);
				if(no==1)  p2tin->p1tin=newTri;
				else if(no==2) p2tin->p2tin=newTri;
				else if(no==3) p2tin->p3tin=newTri;
				no=TheNumber(q1,q2,newTri->ID1,newTri->ID2,newTri->ID3);
				if(no==1)  newTri->p1tin=p2tin;
				else if(no==2) newTri->p2tin=p2tin;
				else if(no==3) newTri->p3tin=p2tin;
			}
		}
		if(p3tin!=NULL)
		{
			int q1,q2,no;
			if(HaveSameEdgeScan(pp->ID1,pp->ID2,pp->ID3,p3tin->ID1,p3tin->ID2,p3tin->ID3,&q1,&q2))
			{
				no=TheNumber(q1,q2,p3tin->ID1,p3tin->ID2,p3tin->ID3);
				if(no==1)  p3tin->p1tin=pp;
				else if(no==2) p3tin->p2tin=pp;
				else if(no==3) p3tin->p3tin=pp;
				no=TheNumber(q1,q2,pp->ID1,pp->ID2,pp->ID3);
				if(no==1)  pp->p1tin=p3tin;
				else if(no==2) pp->p2tin=p3tin;
				else if(no==3) pp->p3tin=p3tin;
			}
			if(HaveSameEdgeScan(newTri->ID1,newTri->ID2,newTri->ID3,p3tin->ID1,p3tin->ID2,p3tin->ID3,&q1,&q2))
			{
				no=TheNumber(q1,q2,p3tin->ID1,p3tin->ID2,p3tin->ID3);
				if(no==1)  p3tin->p1tin=newTri;
				else if(no==2) p3tin->p2tin=newTri;
				else if(no==3) p3tin->p3tin=newTri;
				no=TheNumber(q1,q2,newTri->ID1,newTri->ID2,newTri->ID3);
				if(no==1)  newTri->p1tin=p3tin;
				else if(no==2) newTri->p2tin=p3tin;
				else if(no==3) newTri->p3tin=p3tin;
			}
		}
		//����pp��newtri���ڽӹ�ϵ
		int q1,q2,no;
		if(HaveSameEdgeScan(pp->ID1,pp->ID2,pp->ID3,newTri->ID1,newTri->ID2,newTri->ID3,&q1,&q2))
		{
			no=TheNumber(q1,q2,newTri->ID1,newTri->ID2,newTri->ID3);
			if(no==1)  newTri->p1tin=pp;
			else if(no==2) newTri->p2tin=pp;
			else if(no==3) newTri->p3tin=pp;
			no=TheNumber(q1,q2,pp->ID1,pp->ID2,pp->ID3);
			if(no==1)  pp->p1tin=newTri;
			else if(no==2) pp->p2tin=newTri;
			else if(no==3) pp->p3tin=newTri;
		}
		if(flag==0) 
			TreeInsert(Point,p,newTri,i,flag);
		else 
			TreeInsert(Point,np,newTri,i,flag);
		//����׿�
		FB* fbl=*basel,*fbr=*baser;
// 		while (fb!=NULL)
// 		{
// 			if (fb->no==p->no&&fb->next->no==p->)
// 			{
// 				fb->tri=pp;
// 				fb->next->tri=newTri;
// 			}
// 			fb=fb->next;
// 		}
		if (fbl->no==p->no)
		{
			fbl->tri=pp;
		}
		if (fbr->no==np->no)
		{
			fbr->back->tri=newTri;
		}
		lp=p;		rp=np;
//		return;
	}
	else     //ֱ�ӽ�i���i���䵽�ıߵ������������������
	{
		Circs1(i,Point,p,tinlast,Root);
	}
	
	tfb=p->next;
	//-------------------����ǰ��-------------------------------//
	//��ǰ�ǵĺ�˸���
	for(t=lp;lp->back!=NULL;lp=r1)
	{
		a=(Point[i].x-Point[lp->no].x)*(Point[i].x-Point[lp->no].x)+(Point[i].y-Point[lp->no].y)*(Point[i].y-Point[lp->no].y);
		b=(Point[lp->back->no].x-Point[lp->no].x)*(Point[lp->back->no].x-Point[lp->no].x)+(Point[lp->back->no].y-Point[lp->no].y)*(Point[lp->back->no].y-Point[lp->no].y);
		c=(Point[i].x-Point[lp->back->no].x)*(Point[i].x-Point[lp->back->no].x)+(Point[i].y-Point[lp->back->no].y)*(Point[i].y-Point[lp->back->no].y);
		if((a+b-c)/(2.0*(sqrt(a*b)))>0)
		{
			if(PointUpLine(Point,lp->back->no,i,lp->no)==1) 
				break; 
			t=t->back;
			AddNewTin(Point[i].ID, Point[lp->no].ID, Point[lp->back->no].ID, (t->tri),tinlast);
			//AddNewTin(Point[i].ID, Point[lp->no].ID, Point[lp->back->no].ID, (t->tri),tinlast,Point);
			lp->back->next=tfb;
			tfb->back=lp->back;
			r1=lp->back;
			r1->tri=*tinlast;
			TreeDelete(lp,lp->no,Root);
			AddNewPointer(r1->tri,lp->tri);		
		}
		else 
			break;
	}
	tfb=p->next;
	//��ǰ�ǵ�ǰ�˸���
	for(t=rp;rp->next!=NULL;t=rp=r1)
	{
		a=(Point[i].x-Point[rp->no].x)*(Point[i].x-Point[rp->no].x)+(Point[i].y-Point[rp->no].y)*(Point[i].y-Point[rp->no].y);
		b=(Point[rp->next->no].x-Point[rp->no].x)*(Point[rp->next->no].x-Point[rp->no].x)+(Point[rp->next->no].y-Point[rp->no].y)*(Point[rp->next->no].y-Point[rp->no].y);
		c=(Point[i].x-Point[rp->next->no].x)*(Point[i].x-Point[rp->next->no].x)+(Point[i].y-Point[rp->next->no].y)*(Point[i].y-Point[rp->next->no].y);
		if((a+b-c)/(2.0*(sqrt(a*b)))>0)
		{
			if(PointUpLine(Point,i,rp->next->no,rp->no)==1) 
				break; 
			AddNewTin(Point[i].ID, Point[rp->no].ID, Point[rp->next->no].ID, t->tri,tinlast);
			//AddNewTin(Point[i].ID, Point[rp->no].ID, Point[rp->next->no].ID, t->tri,tinlast,Point);
			rp->next->back=tfb;
			tfb->next=rp->next;
			r1=rp->next;
			TreeDelete(rp,rp->no,Root);
			AddNewPointer(tfb->tri, *tinlast);		
			tfb->tri=*tinlast;	 
		}
		else 
			break;
	}
}

//����ǰ�ǣ���ɹ���
void CGISTinView::EndFrontLine(PointSet *Point, TRIANGLE **tinlast, TREENODE **Frontf, TREENODE **Root)
{
	int finish=0;		TREENODE *p=NULL;	TRIANGLE *t=NULL;
	TREENODE *frontf=*Frontf;
	do{
		finish=1;
		for(p=frontf;p->next->next!=NULL;)
		{
			//���м��λ�����˵���·�ʱ�������������Σ�����ǰ��
			//�޸�p���̵�ָ��
			//������⣬ֱ��ǰ�ǵ�β��
			if(PointUpLine(Point,p->no,p->next->next->no,p->next->no)==0)
			{
				t=p->tri;
				AddNewTin(Point[p->next->next->no].ID, Point[p->next->no].ID, Point[p->no].ID, t,tinlast);
				//AddNewTin(Point[p->next->next->no].ID, Point[p->next->no].ID, Point[p->no].ID, t,tinlast,Point);
				AddNewPointer(*tinlast,p->next->tri);
				p->tri=*tinlast;
				finish=0; 
				TreeDelete(p->next,p->next->no,Root);
				p->next=p->next->next;
			}
			else
				p=p->next;		
		}
     }while(finish==0); //����⵽ǰ���и���ʱ��������ͷ�ٴμ�⣬ֱ����ⲻ�����м��λ�����˵��·������
}
//LOP�Ż��������ߵȲ���
//tԭʼ�����Σ�rear1�ڽ�����������rear2Ϊԭʼ����������ڵ㣬OriginalDataΪԭʼ���ݵ㼯
void CGISTinView::Lop_all(TRIANGLE **t, LIST **rear1, LIST *rear2,PointSet *OriginalData)
{
 	TRIANGLE *t4=NULL;	int d;
    if(CorrespondingTriangle((*t)->ID1,(*t)->ID2,(*t)->ID3,&d,(*t),&t4))//ȡ����������θ�t4����
	{
		if(PointInCircle(OriginalData,d,(*t)->ID1,(*t)->ID2,(*t)->ID3)>0) //�жϹ��������ԵĶ����Ƿ������Բ��
		{ 
			SetTinScan_all((*t)->ID1,d,(*t)->ID2,(*t)->ID3,&(*t),&t4, rear1, rear2); //������
			Lop_all(&(*t), rear1, rear2,OriginalData);	//�ݹ���ã�����*t��rear1��rear2�Եõ�����
			return;
		}
	}
    if(CorrespondingTriangle((*t)->ID2,(*t)->ID3,(*t)->ID1,&d,(*t),&t4))
	{
		if(PointInCircle(OriginalData,d,(*t)->ID1,(*t)->ID2,(*t)->ID3)>0)
		{ 
			SetTinScan_all((*t)->ID2,d,(*t)->ID3,(*t)->ID1,&(*t),&t4, rear1,  rear2);  
			Lop_all(&(*t), rear1, rear2,OriginalData);	
			return;
		}
	}
    if(CorrespondingTriangle((*t)->ID3,(*t)->ID1,(*t)->ID2,&d,(*t),&t4))
	{
		if(PointInCircle(OriginalData,d,(*t)->ID1,(*t)->ID2,(*t)->ID3)>0)
		{ 
			SetTinScan_all((*t)->ID3,d,(*t)->ID1,(*t)->ID2,&(*t),&t4, rear1,  rear2); 
			Lop_all(&(*t), rear1, rear2,OriginalData);	
			return;	
		}
	}
}
//����������ж�
//a,b,c�ֱ�Ϊĳ�����ε���������ı��
//tΪԭ������
//t2Ϊ�ҵ������������
int CGISTinView::CorrespondingTriangle(int a,int b,int c,int *d,TRIANGLE *t,TRIANGLE **t2)
{
  	TRIANGLE *tp=NULL;	
	int no=TheNumber(b,c,a,b,c);   //ThuNumber�������Ƚ�a,b,c����������С��������
								   //Ȼ��b,c������õ��������������
								   //������������ǰ������ȣ��򷵻�1����b,c��ֵΪa,b,c��������С��ֵ
								   //���������ȣ��򷵻�2����b,cΪa,b,c�нϴ������ֵ
								   //��ǰ���������,�򷵻�3,��b,cΪa,b,c��һ��һ������ֵ
								   //��������Ҫ�������򣨰�װyֵ��С�������򣩣����Ե��������Сֱ�����

	if(no==1)     //��ʱa��yֵ��󣬼�a�����Ϸ�
	{
		tp=t->p1tin;  //ȡ���������εĵ�һ���ڽ�������
		if(tp==NULL) 
			return 0;
		*t2=tp;
		//ȡ����һ���ڽ��������빫������ԵĶ��㣬��ֵ��d
		if(tp->ID1!=b&&tp->ID1!=c) 
		{
			*d=tp->ID1;
		} 
		else if(tp->ID2!=b&&tp->ID2!=c) 
		{
			*d=tp->ID2;
		}
		else if(tp->ID3!=b&&tp->ID3!=c)
		{
			*d=tp->ID3;
		}
		else
			AfxMessageBox("CorrespondingTriangle����");
		return 1;
	}
	else if(no==2) 
	{
      tp=t->p2tin;
	  if(tp==NULL) 
		  return 0;
	  *t2=tp;
	  if(tp->ID1!=b&&tp->ID1!=c) 
	  {
		  *d=tp->ID1;
	  }
	  else if(tp->ID2!=b&&tp->ID2!=c) 
	  {
		  *d=tp->ID2;
	  }
	  else if(tp->ID3!=b&&tp->ID3!=c)  
	  {
		  *d=tp->ID3;
	  }
	  else
		  AfxMessageBox("CorrespondingTriangle����");
      return 1;
	}
	else if(no==3)
	{
       tp=t->p3tin;
	   if(tp==NULL) 
		   return 0;
	   *t2=tp;
	   if(tp->ID1!=b&&tp->ID1!=c) 
	   {
		   *d=tp->ID1;
	   }
	   else if(tp->ID2!=b&&tp->ID2!=c) 
	   {
		   *d=tp->ID2;
	   }
	   else if(tp->ID3!=b&&tp->ID3!=c)  
	   {
		   *d=tp->ID3;
	   }
	   else
		  AfxMessageBox("CorrespondingTriangle����");
       return 1;
	}
	else 
	{ 
		AfxMessageBox("CorrespondingTriangle����");
		return 0;
	}
}
//�����Բ�ж�
//P0��P1��P2��P3Ϊԭʼ�㼯PointData�е����
//P0Ϊ�жϵ�
//P1��P2��P3Ϊ�����ε���������
int CGISTinView::PointInCircle(PointSet *OriginalData,int P0,int P1,int P2,int P3)
{
	int isIn=-1;  
	double x0,y0,R;   //x0��y0ΪԲ�����꣬RΪ�뾶
	CircleBy3Points(OriginalData[P1],OriginalData[P2],OriginalData[P3],&x0,&y0,&R); //����ȷ��һ��Բ
	if(R-((x0-OriginalData[P0].x)*(x0-OriginalData[P0].x)+(y0-OriginalData[P0].y)*(y0-OriginalData[P0].y))>0.0001)
		 isIn=1;
	return isIn;
}
//�����ߵȲ���
//a,b,cΪ�����εĶ���
//dΪ��Ҫ�ڽ������εĶ���
//tΪ������
//t1Ϊ�ڽ�������
//rear1��rear2
void CGISTinView::SetTinScan_all(int a,int d,int b,int c,TRIANGLE **t,TRIANGLE **t1, LIST **rear1, LIST *rear2)
{
	int no;
	TRIANGLE *abTin=NULL, *acTin=NULL, *dbTin=NULL, *dcTin=NULL;//�൱�ڱ������������������ɵ��ı��εı�
	//�߽���֮ǰ�����ȱ�������������Ӧ���ڽ���������Ϣ
	no=TheNumber(a, b, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  abTin=(*t)->p1tin;
    else if(no==2) abTin=(*t)->p2tin;
	else if(no==3)  abTin=(*t)->p3tin;

	no=TheNumber(a, c, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  acTin=(*t)->p1tin;
    else if(no==2) acTin=(*t)->p2tin;
	else if(no==3)  acTin=(*t)->p3tin;

	no=TheNumber(d, b, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  dbTin=(*t1)->p1tin;
    else if(no==2) dbTin=(*t1)->p2tin;
	else if(no==3)  dbTin=(*t1)->p3tin;

	no=TheNumber(d, c, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  dcTin=(*t1)->p1tin;
    else if(no==2) dcTin=(*t1)->p2tin;
	else if(no==3)  dcTin=(*t1)->p3tin;

	//������
	(*t)->ID1=a, (*t)->ID2=b, (*t)->ID3=d;
	(*t1)->ID1=a, (*t1)->ID2=c, (*t1)->ID3=d;
	
	//�߽�����ά���ڽ������ε���Ϣ
	no=TheNumber(a, b, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  (*t)->p1tin=abTin;
    else if(no==2) (*t)->p2tin=abTin;
	else if(no==3)  (*t)->p3tin=abTin;
	if(abTin!=NULL)
    {
		no=TheNumber(a, b, abTin->ID1, abTin->ID2, abTin->ID3);
	    if(no==1)  abTin->p1tin=*t;
        else if(no==2) abTin->p2tin=*t;
	    else if(no==3)  abTin->p3tin=*t;
	}

	no=TheNumber(d, b, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  (*t)->p1tin=dbTin;
    else if(no==2) (*t)->p2tin=dbTin;
	else if(no==3)  (*t)->p3tin=dbTin;
	if(dbTin!=NULL)
	{
		no=TheNumber(d, b, dbTin->ID1, dbTin->ID2, dbTin->ID3);
	    if(no==1)  dbTin->p1tin=*t;
        else if(no==2) dbTin->p2tin=*t;
	    else if(no==3)  dbTin->p3tin=*t;
	}

	no=TheNumber(a, c, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  (*t1)->p1tin=acTin;
    else if(no==2) (*t1)->p2tin=acTin;
	else if(no==3)  (*t1)->p3tin=acTin;
	if(acTin!=NULL)
	{
       no=TheNumber(a, c, acTin->ID1, acTin->ID2, acTin->ID3);
	   if(no==1)  acTin->p1tin=*t1;
       else if(no==2) acTin->p2tin=*t1;
	   else if(no==3)  acTin->p3tin=*t1;
	}

	no=TheNumber(d, c, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  (*t1)->p1tin=dcTin;
    else if(no==2) (*t1)->p2tin=dcTin;
	else if(no==3)  (*t1)->p3tin=dcTin;
	if(dcTin!=NULL)
	{
       no=TheNumber(d, c, dcTin->ID1, dcTin->ID2, dcTin->ID3);
	   if(no==1)  dcTin->p1tin=*t1;
       else if(no==2) dcTin->p2tin=*t1;
	   else if(no==3)  dcTin->p3tin=*t1;
	}
	
	no=TheNumber(a, d, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  (*t)->p1tin=*t1;
    else if(no==2) (*t)->p2tin=*t1;
	else if(no==3)  (*t)->p3tin=*t1;
	no=TheNumber(a, d, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  (*t1)->p1tin=*t;
    else if(no==2) (*t1)->p2tin=*t;
	else if(no==3)  (*t1)->p3tin=*t;

	if((*rear1)!=NULL)   //���ڷǵݹ��ɨ�跨
	{
	    LIST *tp=new LIST;       //���������������μ��뵽�ڽ�������������
	    tp->tri=*t1;
	    tp->tri->visited=-1;
	    (*rear1)->next=tp;
		tp->next=rear2;
	    *rear1=(*rear1)->next;  
	}
}
void CGISTinView::EdgeChange(int a,int d,int b,int c,TRIANGLE **t,TRIANGLE **t1)
{
	int no;
	TRIANGLE *abTin=NULL, *acTin=NULL, *dbTin=NULL, *dcTin=NULL;//�൱�ڱ������������������ɵ��ı��εı�
	//�߽���֮ǰ�����ȱ�������������Ӧ���ڽ���������Ϣ
	no=TheNumber(a, b, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  abTin=(*t)->p1tin;
    else if(no==2) abTin=(*t)->p2tin;
	else if(no==3)  abTin=(*t)->p3tin;

	no=TheNumber(a, c, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  acTin=(*t)->p1tin;
    else if(no==2) acTin=(*t)->p2tin;
	else if(no==3)  acTin=(*t)->p3tin;

	no=TheNumber(d, b, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  dbTin=(*t1)->p1tin;
    else if(no==2) dbTin=(*t1)->p2tin;
	else if(no==3)  dbTin=(*t1)->p3tin;

	no=TheNumber(d, c, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  dcTin=(*t1)->p1tin;
    else if(no==2) dcTin=(*t1)->p2tin;
	else if(no==3)  dcTin=(*t1)->p3tin;

	//������
	(*t)->ID1=a, (*t)->ID2=b, (*t)->ID3=d;
	(*t1)->ID1=a, (*t1)->ID2=c, (*t1)->ID3=d;
	
	//�߽�����ά���ڽ������ε���Ϣ
	no=TheNumber(a, b, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  (*t)->p1tin=abTin;
    else if(no==2) (*t)->p2tin=abTin;
	else if(no==3)  (*t)->p3tin=abTin;
	if(abTin!=NULL)
    {
		no=TheNumber(a, b, abTin->ID1, abTin->ID2, abTin->ID3);
	    if(no==1)  abTin->p1tin=*t;
        else if(no==2) abTin->p2tin=*t;
	    else if(no==3)  abTin->p3tin=*t;
	}

	no=TheNumber(d, b, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  (*t)->p1tin=dbTin;
    else if(no==2) (*t)->p2tin=dbTin;
	else if(no==3)  (*t)->p3tin=dbTin;
	if(dbTin!=NULL)
	{
		no=TheNumber(d, b, dbTin->ID1, dbTin->ID2, dbTin->ID3);
	    if(no==1)  dbTin->p1tin=*t;
        else if(no==2) dbTin->p2tin=*t;
	    else if(no==3)  dbTin->p3tin=*t;
	}

	no=TheNumber(a, c, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  (*t1)->p1tin=acTin;
    else if(no==2) (*t1)->p2tin=acTin;
	else if(no==3)  (*t1)->p3tin=acTin;
	if(acTin!=NULL)
	{
       no=TheNumber(a, c, acTin->ID1, acTin->ID2, acTin->ID3);
	   if(no==1)  acTin->p1tin=*t1;
       else if(no==2) acTin->p2tin=*t1;
	   else if(no==3)  acTin->p3tin=*t1;
	}

	no=TheNumber(d, c, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  (*t1)->p1tin=dcTin;
    else if(no==2) (*t1)->p2tin=dcTin;
	else if(no==3)  (*t1)->p3tin=dcTin;
	if(dcTin!=NULL)
	{
       no=TheNumber(d, c, dcTin->ID1, dcTin->ID2, dcTin->ID3);
	   if(no==1)  dcTin->p1tin=*t1;
       else if(no==2) dcTin->p2tin=*t1;
	   else if(no==3)  dcTin->p3tin=*t1;
	}
	
	no=TheNumber(a, d, (*t)->ID1, (*t)->ID2, (*t)->ID3);
	if(no==1)  (*t)->p1tin=*t1;
    else if(no==2) (*t)->p2tin=*t1;
	else if(no==3)  (*t)->p3tin=*t1;
	no=TheNumber(a, d, (*t1)->ID1, (*t1)->ID2, (*t1)->ID3);
	if(no==1)  (*t1)->p1tin=*t;
    else if(no==2) (*t1)->p2tin=*t;
	else if(no==3)  (*t1)->p3tin=*t;
}
//����ȷ��һ��Բ
void CGISTinView::CircleBy3Points(PointSet P1,PointSet P2,PointSet P3,double * ox,double * oy,double * RR)
{
	double a1,b1,c1,a2,b2,c2;//xc,yc;
	//�ȴ������㹲��
	a1=(P1.y-P2.y);a2=(P1.x-P2.x);
	b1=(P3.y-P2.y);b2=(P3.x-P2.x);
	if(a2*b1==a1*b2)
	{
		*oy=0.0;*ox=0.0;*RR=0.0;	 
	}
	else
	{
		a1=2*(P1.x-P2.x); b1=2*(P1.y-P2.y); c1=(P1.x*P1.x+P1.y*P1.y)-(P2.x*P2.x+P2.y*P2.y);
		a2=2*(P2.x-P3.x); b2=2*(P2.y-P3.y); c2=(P2.x*P2.x+P2.y*P2.y)-(P3.x*P3.x+P3.y*P3.y);
		*oy=(a2*c1-a1*c2)/(a2*b1-a1*b2+1.E-8);
		*ox=(b1*c2-b2*c1)/(a2*b1-a1*b2+1.E-8);
		*RR=(P1.x-*ox)*(P1.x-*ox)+(P1.y-*oy)*(P1.y-*oy);
	}
}

///////ɨ���߷�����TIN///////////////////////////
//������Data���������ݼ���pNum�������ݼ���С
//		tinfirst������������ͷָ�룻tinlast������������βָ��
//		root��frontf��frontl�����������������洢ǰ��
//		basef��basel����˫�������洢�׿�
//		g_pointmark����ǰ����ߵ�ı��
void CGISTinView::CreateTinBy_Scan(PointSet *Point,int pNum, TRIANGLE **tinlast, int &g_pointmark, TREENODE **root, TREENODE **frontf, TREENODE **frontl, FB **basef, FB **basel)
{
	TREENODE *r1=NULL,*r2=NULL;  //r2����r1��X����ĺ�һ���ڵ�
	for(int i=g_pointmark+1;i<pNum;i++)
	{
		if(((Point[i].x==Point[(*frontf)->no].x)&&(Point[i].y==Point[(*frontf)->no].y))||
		((Point[i].x==Point[(*frontl)->no].x)&&(Point[i].y==Point[(*frontl)->no].y)))
			;                        //���ǰ��ͷ���β���غϣ���ʲôҲ������
		else if((Point[i].x<=Point[(*frontf)->no].x)||(Point[i].x>=Point[(*frontl)->no].x))  //����ǰ���ཻ
			//���2����ͶӰδ�䵽ǰ����
		{
			Circs2(i, Point, tinlast, root, frontf, frontl, basef, basel);  
			//AfxMessageBox("circs2"); 
		}
		else 
		{
			TreeSearch(Point,root,&r1,i);
			if(Point[r1->no].x>Point[i].x) 
				r2=r1->back;
			else 
				r2=r1->next;
			if(((Point[i].x==Point[r1->no].x)&&(Point[i].y==Point[r1->no].y))||
				(Point[i].x==Point[r2->no].x)&&(Point[i].y==Point[r2->no].y)) 
				;                //��˵��غϣ���������
			else if(((Point[i].x>=Point[r1->no].x)&&(Point[i].x<Point[r2->no].x))||
				   ((Point[i].x>=Point[r2->no].x)&&(Point[i].x<Point[r1->no].x)))
			{	
				//���1
				Circs1(i, Point, r1, tinlast,root);
				//Circs1(i, Point, r1, tinlast,root,basef,basel);//��ǰ���ཻ
				//AfxMessageBox("circs1");
			}
		}	
	}  
}
int cmp( const void *a , const void *b ) 
{ 
  struct PointSet *c = (PointSet *)a; 
  struct PointSet *d = (PointSet *)b; 
  if(c->y != d->y) 
  {  
	  if(c->y-d->y>0)
          return 1;
      else
          return -1; 
  }
  else 
  {  
	 if(c->x-d->x>0)
	    return 1;
     else if(c->x-d->x<0)
		 return -1;
	 else
		 return 0;
  }
} 
//LOP�Ż��㷨
void CGISTinView::LOPScan_Nonrecursion(TRIANGLE *p,TRIANGLE **tinfirst,PointSet *OriginalData)
{
	LIST  *tp1=NULL,*tp2=NULL;	//�����¼�����ε�����ڵ�
								//tp1��������ͷ
								//tp2���涯̬������ڽ�������

	LIST *front=NULL,*rear1=NULL,*rear2=NULL;	//front�������ԭ����������
												//rear1��������ڽ������ε�����
												//rear2�����ʵ����һ���ڵ�
												//�ýڵ㱣���˵�һ�������Σ�tinfirst����nextָ��null
												//�ڽ������������ĩβ��ָ��ýڵ㣬���ô���ǣ��ݹ�ĳ��ڣ���nextָ��null

	rear2=new LIST;	rear2->tri=*tinfirst;  rear2->next=NULL;
	front=new LIST;	front->tri=p; p->visited=1;	front->next=rear2;
	rear1=front;    
	while(front->next!=NULL)
	{
		tp1=front;    //tp1Ϊ����ͷ
		if(tp1->tri->p1tin!=NULL)  //�ڽ������δ���
		{
			if(tp1->tri->p1tin->visited!=1) //��δ������
			{
				tp2=new LIST;
				tp2->tri=tp1->tri->p1tin;
				tp2->tri->visited=1;
				rear1->next=tp2;      //���ڽ������μ��뵽rear1��
				tp2->next=rear2;
				rear1=rear1->next;	
			}
		}
		if(tp1->tri->p2tin!=NULL)
		{
			if(tp1->tri->p2tin->visited!=1)
			{
				tp2=new LIST;
				tp2->tri=tp1->tri->p2tin;
				tp2->tri->visited=1;
				rear1->next=tp2;tp2->next=rear2;
				rear1=rear1->next;				
			}
		}
		if(tp1->tri->p3tin!=NULL)
		{
			if(tp1->tri->p3tin->visited!=1)
			{
				tp2=new LIST;
				tp2->tri=tp1->tri->p3tin;
				tp2->tri->visited=1;
				rear1->next=tp2;tp2->next=rear2;
				rear1=rear1->next;				
			}
		}
		front=front->next;
		//����һ�������ε��ڽ�������ȫ���ҵ��������뵽����rear1��
		//Ȼ���ڴӵ�һ�������ο�ʼ�����ڽ������α�����һ�������ο�ʼ�����Ż�
		Lop_all(&(tp1->tri),&rear1, rear2,OriginalData);
		delete tp1;
	}
	delete rear2;
//	delete front;
}

//��ʼ��ɨ����
//������Data���������ݼ���pNum�������ݼ���С
//		tinfirst������������ͷָ�룻tinlast������������βָ��
//		root��frontf��frontl�����������������洢ǰ��
//		basef��basel����˫�������洢�׿�
//		g_pointmark������¼��һ����ǰ�����㲻���ߵĵ�
void CGISTinView::SetInitialScan(PointSet *Point, int pNum,TRIANGLE **tinfirst, TRIANGLE **tinlast, int &g_pointmark, TREENODE **Root, TREENODE **frontf, TREENODE **frontl, FB **basef, FB **basel)
{
  int i;
  TRIANGLE *tinNode;
  TREENODE *treeNew, *root;
  FB *fbNew;    //�׿�
  //�����һ�������Σ����ҳ���ǰ�����㲻���ߵĵ�һ����
  for(i=2;i<pNum;i++)
	if(((Point[i].y-Point[i-1].y)*(Point[i].x-Point[0].x))!=((Point[i].x-Point[i-1].x)*(Point[i].y-Point[0].y)))
		break;                   //������ǰ�����㲻���ߵĵ㣬ֹͣ
  g_pointmark=i; 
  //Point���鰴yֵ��С�������򣬳�ʼ���ߵļ��㰴xֵ�Ӵ�С���� 
  if(i>2)
    qs_x(Point,0,i-1); //��x��������
  //---------------����ǰ�Ǻ͵׿ǣ�������������--------------
  root=new TREENODE;
  //7-d
  if((Point[i-1].x<Point[0].x)&&(Point[i].x<=Point[i-1].x))
  {
	if((Point[i-1].x-Point[0].x)*(Point[i].y-Point[0].y)<(Point[i].x-Point[0].x)*(Point[i-1].y-Point[0].y))
    //��i-1��ֱ��0��i�����
	{
       AddFirstTin(tinfirst, tinlast, Point[0].ID, Point[i].ID, Point[1].ID); //������һ��������
	   tinNode=*tinfirst;
	   for(int t=2;t<i;t++)
		 FirAddTin(Point[i].ID, Point[t-1].ID, Point[t].ID, &tinNode, tinlast); //���빲�ߵĵ�

	   //-------------------------��ʼ��ǰ��------------------------------//
	   //�����������ĸ��ڵ��ʼ��
	   root->no=i;             //��¼ǰ����ߵ����� 
	   root->tri=*tinfirst;    //��ǰ�ǹ��ߵ�������ָ����������һ��������
	   root->lchild = root->rchild= root->parent=NULL;  //���Һ��Ӽ����ڵ�ָ��NULL

	   treeNew=new TREENODE;	  
	   treeNew->no=0;    treeNew->tri=NULL;
	   treeNew->lchild = treeNew->rchild = treeNew->parent=NULL;

	   root->rchild = treeNew;  treeNew->parent = root;

	   *frontf=root;  *frontl=treeNew;
	   root->next = treeNew;  treeNew->back = root;
	   root->back=NULL; treeNew->next=NULL;
	   //-------------------------��ʼ���׿�------------------------------//
	   fbNew=new FB;  fbNew->no=i;  fbNew->next=NULL;  fbNew->back=NULL;
	   fbNew->tri = *tinlast;

	   *basef=*basel=fbNew;  
	   tinNode = *tinlast;  //��¼��׿ǹ��ߵ�������
	   for(int m=i-1; m>0; m--)
	   {
         fbNew=new FB;  fbNew->no=m;  fbNew->next=NULL;  fbNew->back=NULL;
		 fbNew->tri = tinNode;
		 (*basel)->next=fbNew; fbNew->back=*basel;  *basel=fbNew;
		 tinNode=tinNode->back;
	   } 
	   fbNew=new FB;  fbNew->no=0;  fbNew->next=NULL;  fbNew->back=NULL;
	   fbNew->tri=NULL;
	   (*basel)->next=fbNew;  fbNew->back=*basel; 
	   *basel=fbNew;
	}
	else //��i-1��ֱ��0��i���Ҳ�
	{
        AddFirstTin(tinfirst, tinlast,Point[0].ID, Point[i].ID, Point[1].ID);
        tinNode=*tinfirst;
		for(int t=2;t<i;t++)
		   FirAddTin(Point[i].ID, Point[t-1].ID, Point[t].ID, &tinNode, tinlast);	
		
		root->no=i; root->tri=*tinlast;
		root->lchild=root->rchild=root->parent=NULL;
		root->back=NULL;

		*frontf= *frontl= root;  tinNode=*tinlast;
		for(int m=i-1;m>0;m--)
		{
           treeNew=new TREENODE;
		   treeNew->no=m;  treeNew->tri=tinNode;
		   treeNew->lchild= treeNew->rchild= treeNew->parent=NULL;			     
		   
		   (*frontl)->rchild=treeNew; treeNew->parent=*frontl;
		   (*frontl)->next=treeNew; treeNew->back=*frontl;
		   
		   *frontl=treeNew;   tinNode=tinNode->back;
		} 
        treeNew=new TREENODE;
		treeNew->no=0;  treeNew->tri=NULL;
		treeNew->lchild= treeNew->rchild= treeNew->parent=NULL;			     
		(*frontl)->rchild=treeNew;  treeNew->parent=*frontl;
		(*frontl)->next=treeNew;  treeNew->back=*frontl;  treeNew->next=NULL;
		*frontl=treeNew;

		fbNew=new FB;  fbNew->no=i; fbNew->next=NULL;  fbNew->back=NULL;
		fbNew->tri=*tinfirst;
		*basef=*basel=fbNew; 
		fbNew=new FB;  fbNew->no=0;	fbNew->next=NULL;fbNew->back=NULL;
		fbNew->tri=NULL;			 
		 (*basel)->next=fbNew; fbNew->back=*basel; *basel=fbNew;
	}
  }
  //7-c
  else if((Point[i-1].x<Point[0].x)&&(Point[i].x>Point[i-1].x)&&(Point[i].x<Point[0].x))
  {
	  AddFirstTin(tinfirst, tinlast, Point[0].ID, Point[i].ID, Point[1].ID);
	  tinNode=*tinfirst;
	  for(int t=2;t<i;t++)
	    FirAddTin(Point[i].ID, Point[t-1].ID, Point[t].ID, &tinNode, tinlast);

      root->no=i; root->tri=*tinfirst;
	  root->lchild=root->rchild=root->parent=NULL;

	  treeNew=new TREENODE;
	  treeNew->no=0;  treeNew->tri=NULL;
	  treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;

	  root->rchild=treeNew;  treeNew->parent=root;
	  treeNew->next=NULL;  treeNew->back=root;  root->next=treeNew;
	  
	  *frontl=treeNew; 
	  treeNew=new TREENODE;
	  treeNew->no=i-1; treeNew->tri=*tinlast;
	  treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
	  root->lchild=treeNew;  treeNew->parent=root;
	  treeNew->back=NULL;treeNew->next=root; root->back=treeNew;
	  *frontf=treeNew;
	 
	  //�ǵ״���
      fbNew=new FB; fbNew->no=i-1; fbNew->next=NULL; fbNew->back=NULL;
	  fbNew->tri=*tinlast;
      *basef=*basel=fbNew;   tinNode=(*tinlast)->back;
      for(int m=i-2;m>=0;m--)
	  {
       fbNew=new FB;  fbNew->no=m;  fbNew->next=NULL;fbNew->back=NULL;
	   fbNew->tri=tinNode;

	   (*basel)->next=fbNew;  fbNew->back=*basel;  *basel=fbNew;
	   if(tinNode==NULL) break;
	   tinNode=tinNode->back;	   
	  }
  }
  //7-a
  else if((Point[i-1].x<=Point[0].x)&&(Point[i].x>=Point[0].x))
  {
	if((Point[0].x-Point[i-1].x)*(Point[i].y-Point[i-1].y)>(Point[i].x-Point[i-1].x)*(Point[0].y-Point[i-1].y))
	{
	  AddFirstTin(tinfirst, tinlast,Point[0].ID, Point[i].ID, Point[1].ID);
	  tinNode=*tinfirst;
	  for(int t=2;t<i;t++)
	     FirAddTin(Point[i].ID, Point[t-1].ID, Point[t].ID, &tinNode, tinlast);

      root->no=i-1; root->tri=*tinlast;
	  root->lchild=root->rchild=root->parent=NULL;

	  treeNew=new TREENODE;
	  treeNew->no=i; treeNew->tri=NULL;
	  treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;

	  root->rchild=treeNew;  treeNew->parent=root;
	  root->back=NULL; root->next=treeNew;
	  treeNew->back=root; treeNew->next=NULL;
	 *frontf=root; *frontl=treeNew;

	 fbNew=new FB; fbNew->no=i-1; fbNew->next=NULL;fbNew->back=NULL;
	 fbNew->tri=*tinlast;
	 *basef=*basel=fbNew; tinNode=(*tinlast)->back;
	 for(int m=i-2;m>0;m--)
	 {
         fbNew=new FB;  fbNew->no=m;  fbNew->next=NULL; fbNew->back=NULL;
		 fbNew->tri=tinNode;
		 tinNode=tinNode->back;
		 (*basel)->next=fbNew; fbNew->back=*basel; *basel=fbNew;
	 }
     fbNew=new FB;  fbNew->no=0;  fbNew->next=NULL;fbNew->back=NULL;
	 fbNew->tri=*tinfirst;
	 (*basel)->next=fbNew; fbNew->back=*basel; *basel=fbNew;

	 fbNew=new FB; fbNew->no=i; fbNew->next=NULL;fbNew->back=NULL;	
	 fbNew->tri=NULL;	
	 (*basel)->next=fbNew; fbNew->back=*basel; *basel=fbNew;
	}
	else if((g_pointmark>2)&&((Point[0].x-Point[i-1].x)*(Point[i].y-Point[i-1].y)<(Point[i].x-Point[i-1].x)*(Point[0].y-Point[i-1].y)))
	{
	  AddFirstTin(tinfirst, tinlast,Point[0].ID, Point[i].ID, Point[1].ID);
	  tinNode=*tinfirst;
	  for(int t=2;t<i;t++)
	    FirAddTin(Point[i].ID, Point[t-1].ID, Point[t].ID, &tinNode, tinlast);

      root->no=0; root->tri=*tinfirst;
	  root->lchild=root->rchild=root->parent=NULL;
	  root->back=root->next=NULL;

	  treeNew=new TREENODE;
	  treeNew->no=i;  treeNew->tri=NULL;
	  treeNew->lchild= treeNew->rchild= treeNew->parent=NULL;
	  treeNew->back=treeNew->next=NULL;
	 
	  root->rchild=treeNew;  treeNew->parent=root;
	  root->next=treeNew;  treeNew->back=root;
	  *frontl=treeNew;
	  *frontf=root; tinNode=*tinfirst;
	  for(int m=1;m<=i-1;m++)
	  {
	    treeNew=new TREENODE;
	    treeNew->no=m; treeNew->tri=tinNode;
	    treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
	    treeNew->back=treeNew->next=NULL;

		(*frontf)->lchild=treeNew; treeNew->parent=*frontf;
		treeNew->next=*frontf; treeNew->back=treeNew;
		*frontf=treeNew; tinNode=tinNode->next;
	  }
	  //����ǵ�
	  fbNew=new FB;
	  fbNew->back=fbNew->next=NULL;
	  fbNew->no=i-1;  fbNew->tri=*tinlast;
	  *basef=*basel=fbNew;
	  fbNew=new FB;
	  fbNew->no=i; fbNew->back=fbNew->next=NULL;
	  fbNew->tri=NULL;
	  (*basef)->next=fbNew; fbNew->back=*basel; 
	  *basel=fbNew;
	}
  }
  //7-e
   else if((Point[i-1].x>Point[0].x)&&(Point[i].x>=Point[i-1].x))
   {
     //��1��ֱ��0��2���Ҳ�
	 if((Point[i].y-Point[0].y)*(Point[i-1].x-Point[0].x)>(Point[i].x-Point[0].x)*(Point[i-1].y-Point[0].y))
	 {
	    AddFirstTin(tinfirst, tinlast,Point[0].ID, Point[i].ID, Point[1].ID);
	    root->no=0; root->tri=*tinfirst;
		root->lchild=root->rchild=root->parent=NULL;

	    treeNew=new TREENODE;
	    treeNew->no=i;  treeNew->tri=NULL;
	    treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;

		root->rchild=treeNew;  treeNew->parent=root;
		root->back=NULL;  root->next=treeNew;
		treeNew->back=root;  treeNew->next=NULL;

	    *frontf=root;  *frontl=treeNew;

        fbNew=new FB;  fbNew->no=0; fbNew->next=NULL;fbNew->back=NULL; 	fbNew->tri=*tinfirst; 
		*basef=*basel=fbNew;
        fbNew=new FB;  fbNew->no=1; fbNew->next=NULL;fbNew->back=NULL; fbNew->tri=*tinfirst;
	    (*basel)->next=fbNew;  fbNew->back=*basel; 
		*basel=fbNew;
	    fbNew=new FB;  fbNew->no=2; fbNew->next=NULL;fbNew->back=NULL; fbNew->tri=NULL; 
		(*basel)->next=fbNew;  fbNew->back=*basel; 
		*basel=fbNew;
	 }
     else //��1��ֱ��0��2�����
	 {
       AddFirstTin(tinfirst, tinlast, Point[0].ID, Point[i].ID, Point[1].ID);
	   root->no=0;  root->tri=*tinfirst;
	   root->lchild= root->rchild= root->parent=NULL;
	   root->back=NULL;	
	   *frontf=*frontl=root;

       treeNew=new TREENODE;
	   treeNew->no=1;   treeNew->tri=*tinfirst;
	   treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
	   treeNew->next=treeNew->back=NULL;
	   root->rchild=treeNew; treeNew->parent=root;
	   root->next=treeNew; treeNew->back=root;
       *frontl=treeNew;

	   treeNew=new TREENODE;
	   treeNew->no=2; treeNew->tri=NULL;
	   treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
	   treeNew->next=treeNew->back=NULL;

	   (*frontl)->rchild=treeNew; treeNew->parent=*frontl;
	   (*frontl)->next=treeNew; treeNew->back=*frontl;
	   *frontl=treeNew;

       fbNew=new FB;  fbNew->no=0;   fbNew->next=NULL;fbNew->back=NULL;fbNew->tri=*tinfirst; 
	   *basef=*basel=fbNew;
	   fbNew=new FB; fbNew->no=2;  fbNew->next=NULL;fbNew->back=NULL; fbNew->tri=NULL;
	   (*basel)->next=fbNew; fbNew->back=*basel;  *basel=fbNew;
	 }
   }
   //7-f
   else if((Point[i-1].x>Point[0].x)&&(Point[i].x<Point[i-1].x)&&(Point[i].x>Point[0].x))
   {
	  AddFirstTin(tinfirst, tinlast, Point[0].ID, Point[i].ID, Point[1].ID);
      root->no=i; root->tri=*tinfirst;
      root->lchild=root->rchild=root->parent=NULL;
	 
	  treeNew=new TREENODE;
	  treeNew->no=i-1;  treeNew->tri=NULL;
	  treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
	  root->rchild=treeNew;  treeNew->parent=root;
	  *frontl=treeNew;
	  root->next=treeNew;  treeNew->back=root;  treeNew->next=NULL;

	  treeNew=new TREENODE;  treeNew->no=0;  treeNew->tri=*tinfirst;
	  treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;
	  root->lchild=treeNew;  treeNew->parent=root;
	  root->back=treeNew;  treeNew->next=root;  treeNew->back=NULL;
	  *frontf=treeNew;	

	  fbNew=new FB; fbNew->no=0; fbNew->next=NULL;fbNew->back=NULL;	fbNew->tri=*tinfirst; 
	  *basef=*basel=fbNew;
	  fbNew=new FB; fbNew->no=1; fbNew->next=NULL;fbNew->back=NULL;	 fbNew->tri=NULL;
	  (*basel)->next=fbNew;  fbNew->back=*basel;  *basel=fbNew;
   }
   //6-b
   else if((Point[i-1].x>=Point[0].x)&&(Point[i].x<=Point[0].x))
   {
	 AddFirstTin(tinfirst, tinlast, Point[0].ID, Point[i].ID, Point[1].ID);
	 root->no=i; root->tri=*tinfirst;
	 root->lchild=root->rchild=root->parent=NULL;

	 treeNew=new TREENODE;
	 treeNew->no=i-1;treeNew->tri=NULL;
	 treeNew->lchild=treeNew->rchild=treeNew->parent=NULL;

	 root->rchild=treeNew;  treeNew->parent=root;
	 root->back=NULL;  root->next=treeNew;
	 treeNew->back=root;  treeNew->next=NULL;
	 *frontf=root;  *frontl=treeNew;

     fbNew=new FB; fbNew->no=2; fbNew->next=NULL;fbNew->back=NULL; fbNew->tri=*tinfirst;
	 *basef=*basel=fbNew; 
	 
	 fbNew=new FB; fbNew->no=0; fbNew->next=NULL;fbNew->back=NULL; fbNew->tri=*tinfirst;
     (*basel)->next=fbNew; fbNew->back=*basel; *basel=fbNew;

     fbNew=new FB; fbNew->no=1; fbNew->next=NULL;fbNew->back=NULL; fbNew->tri=NULL; 
     (*basel)->next=fbNew; fbNew->back=*basel; *basel=fbNew;
   }   
   *Root=root;
}
//�жϵ�P ���߶�P1-P2����һ��? (-1:���; 1: �Ҳ�)
int CGISTinView::OnLeft(PointSet P, PointSet P1, PointSet P2)
{
	if((P2.x-P1.x)*(P.y-P1.y)-(P2.y-P1.y)*(P.x-P1.x)>0)
		return -1;
	else if((P2.x-P1.x)*(P.y-P1.y)-(P2.y-P1.y)*(P.x-P1.x)<0)
		return 1;
	else 
		return 0;
}
void CGISTinView::SetTriInfor(TRIANGLE* tri,TRIANGLE *blockTri,int e1,int e2)
{
	if (tri!=NULL)
	{
		int no=TheNumber(e1,e2,tri->ID1,tri->ID2,tri->ID3);
		if (no==-1)
		{
			int no1;
			if (tri->p1tin!=NULL)
			{
				no1=TheNumber(e1,e2,tri->p1tin->ID1,tri->p1tin->ID2,tri->p1tin->ID3);
				if (no1!=-1)
				{
					blockTri=tri->p1tin;
					return;
				}
			}
			if (tri->p2tin!=NULL)
			{
				no1=TheNumber(e1,e2,tri->p2tin->ID1,tri->p2tin->ID2,tri->p2tin->ID3);
				if (no1!=-1)
				{
					blockTri=tri->p2tin;
					return;
				}
			}
			if (tri->p3tin!=NULL)
			{
				no1=TheNumber(e1,e2,tri->p3tin->ID1,tri->p3tin->ID2,tri->p3tin->ID3);
				if (no1!=-1)
				{
					blockTri=tri->p3tin;
					return;
				}
			}
			return;
		}
		else
		{
			blockTri=tri;
			return;
		}
	}
}
//BlockTIN���ɺ�ά��BlockTIN��������Ϣ
void CGISTinView::GetCurrentBlockInfor(BlockTin &blocktin, TRIANGLE *TinSave, PointSet *Point, TREENODE *frontf,FB *basef)
{
	int no;
	//̽��ǰ�Ƕ�Ӧ�������εı�
	for(TREENODE *f=frontf; f->next!=NULL&& f!=NULL; f=f->next)
	{
		no=TheNumber(Point[f->no].ID, Point[f->next->no].ID, TinSave->ID1, TinSave->ID2, TinSave->ID3);
		if(no==1) 
		{
			blocktin.Tin[no]=f->tri;
			//SetTriInfor(f->tri,blocktin.Tin[no],Point[f->no].ID, Point[f->next->no].ID);
			blocktin.IsVisited[no]=false;
			blocktin.Edge[no].ID1=Point[f->no].ID; blocktin.Edge[no].ID2=Point[f->next->no].ID;
		}
		else if(no==2) 
		{
			blocktin.Tin[no]=f->tri;
			//SetTriInfor(f->tri,blocktin.Tin[no],Point[f->no].ID, Point[f->next->no].ID);
			blocktin.IsVisited[no]=false;
			blocktin.Edge[no].ID1=Point[f->no].ID; blocktin.Edge[no].ID2=Point[f->next->no].ID;
		}
		else if(no==3) 
		{
			blocktin.Tin[no]=f->tri;
			//SetTriInfor(f->tri,blocktin.Tin[no],Point[f->no].ID, Point[f->next->no].ID);
			blocktin.IsVisited[no]=false;
			blocktin.Edge[no].ID1=Point[f->no].ID; blocktin.Edge[no].ID2=Point[f->next->no].ID;
		}
	}
	//̽��׿Ƕ�Ӧ�������εı�
	for(FB *b=basef; b->next!=NULL&& b!=NULL; b=b->next)
	{
		no=TheNumber(Point[b->no].ID, Point[b->next->no].ID, TinSave->ID1, TinSave->ID2, TinSave->ID3);
		if(no==1) 
		{
			blocktin.Tin[no]=b->tri; 
			//SetTriInfor(b->tri,blocktin.Tin[no],Point[b->no].ID, Point[b->next->no].ID);
			blocktin.IsVisited[no]=false;
			blocktin.Edge[no].ID1=Point[b->no].ID; blocktin.Edge[no].ID2=Point[b->next->no].ID;
		}
		else if(no==2) 
		{
			blocktin.Tin[no]=b->tri;  
			//SetTriInfor(b->tri,blocktin.Tin[no],Point[b->no].ID, Point[b->next->no].ID);
			blocktin.IsVisited[no]=false;
			blocktin.Edge[no].ID1=Point[b->no].ID; blocktin.Edge[no].ID2=Point[b->next->no].ID;
		}
		else if(no==3) 
		{
			blocktin.Tin[no]=b->tri;  
			//SetTriInfor(b->tri,blocktin.Tin[no],Point[b->no].ID, Point[b->next->no].ID);
			blocktin.IsVisited[no]=false;
			blocktin.Edge[no].ID1=Point[b->no].ID; blocktin.Edge[no].ID2=Point[b->next->no].ID;
		}
	}
}
void CGISTinView::SetBlockPointer(TRIANGLE *Tbig1, BlockTin *block)
{
   TRIANGLE *Tbig2; //������ǿ�֮����ڽ�������
   TRIANGLE *Tsmall1, *Tsmall2; //������ǿ��ڲ���С������
   int no ,e1, e2;
   if(Tbig1==NULL) return;
   //����Ϊ�ݹ�ĳ��ڣ�
   //ĳһ����Ϊ�գ������߶������ʹ�
   //���������߶���Ϊ�գ����������ʹ�
   else if(Tbig1->p1tin==NULL && block[Tbig1->g_SeqNum].IsVisited[2]==true && block[Tbig1->g_SeqNum].IsVisited[3]==true)
	   return;
   else if(Tbig1->p2tin==NULL && block[Tbig1->g_SeqNum].IsVisited[1]==true && block[Tbig1->g_SeqNum].IsVisited[3]==true)
	   return;
   else if(Tbig1->p3tin==NULL && block[Tbig1->g_SeqNum].IsVisited[1]==true && block[Tbig1->g_SeqNum].IsVisited[2]==true)
	   return;
   else if(Tbig1->p1tin!=NULL && Tbig1->p2tin!=NULL && Tbig1->p3tin!=NULL && block[Tbig1->g_SeqNum].IsVisited[1]==true && block[Tbig1->g_SeqNum].IsVisited[2]==true && block[Tbig1->g_SeqNum].IsVisited[3]==true)
	   return;
   //��һ����
   if(Tbig1->p1tin!=NULL && block[Tbig1->g_SeqNum].IsVisited[1]!=true)
   {
	   block[Tbig1->g_SeqNum].IsVisited[1]=true; //��Ǹ������ο��е���Χ�����εĵ�һ���߱����ʹ�
       e1=block[Tbig1->g_SeqNum].Edge[1].ID1;
	   e2=block[Tbig1->g_SeqNum].Edge[1].ID2;    //ȡ�������ߵ���������
       Tsmall1=block[Tbig1->g_SeqNum].Tin[1];    //ȡ���������ο������һ�������Ӧ��������
	   
	   Tbig2=Tbig1->p1tin;     //ȡ���������ο���ڽ������Σ����һ�������Ӧ��
	   no=TheNumber(e1,e2,Tbig2->ID1, Tbig2->ID2, Tbig2->ID3);//�����ڽӹ�ϵ

	   if(no==1)
	   {
	    	 Tsmall2=block[Tbig2->g_SeqNum].Tin[1];
             block[Tbig2->g_SeqNum].IsVisited[1]=true;
	   }
	   else if(no==2) 
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[2];
		   block[Tbig2->g_SeqNum].IsVisited[2]=true;
	   }
	   else if(no==3) 
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[3];
		   block[Tbig2->g_SeqNum].IsVisited[3]=true;
	   }
	   //����Tsmall1��Tsmall2֮����ڽ�ָ��
	   no=TheNumber(e1,e2, Tsmall1->ID1, Tsmall1->ID2, Tsmall1->ID3);
	   if(no==1) Tsmall1->p1tin=Tsmall2;
	   else if(no==2) Tsmall1->p2tin=Tsmall2;
	   else if(no==3) Tsmall1->p3tin=Tsmall2;
	   no=TheNumber(e1,e2, Tsmall2->ID1, Tsmall2->ID2, Tsmall2->ID3);
	   if(no==1) Tsmall2->p1tin=Tsmall1;
	   else if(no==2) Tsmall2->p2tin=Tsmall1;
	   else if(no==3) Tsmall2->p3tin=Tsmall1;
   }
   
   //�ڶ�����
   else if(Tbig1->p2tin!=NULL && block[Tbig1->g_SeqNum].IsVisited[2]!=true)
   {
	   block[Tbig1->g_SeqNum].IsVisited[2]=true;
       e1=block[Tbig1->g_SeqNum].Edge[2].ID1;
	   e2=block[Tbig1->g_SeqNum].Edge[2].ID2;
       Tsmall1=block[Tbig1->g_SeqNum].Tin[2];
	   
	   Tbig2=Tbig1->p2tin; 
	   no=TheNumber(e1,e2, Tbig2->ID1, Tbig2->ID2, Tbig2->ID3);
	   if(no==1)
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[1];
           block[Tbig2->g_SeqNum].IsVisited[1]=true;
	   }
	   else if(no==2) 
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[2];
		   block[Tbig2->g_SeqNum].IsVisited[2]=true;
	   }
	   else if(no==3) 
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[3];
		   block[Tbig2->g_SeqNum].IsVisited[3]=true;
	   }
	   //����Tsmall1��Tsmall2֮����ڽ�ָ��
	   no=TheNumber(e1,e2, Tsmall1->ID1, Tsmall1->ID2, Tsmall1->ID3);
	   if(no==1) Tsmall1->p1tin=Tsmall2;
	   else if(no==2) Tsmall1->p2tin=Tsmall2;
	   else if(no==3) Tsmall1->p3tin=Tsmall2;
	   no=TheNumber(e1,e2, Tsmall2->ID1, Tsmall2->ID2, Tsmall2->ID3);
	   if(no==1) Tsmall2->p1tin=Tsmall1;
	   else if(no==2) Tsmall2->p2tin=Tsmall1;
	   else if(no==3) Tsmall2->p3tin=Tsmall1;
   }
   //��������
   else if(Tbig1->p3tin!=NULL && block[Tbig1->g_SeqNum].IsVisited[3]!=true)
   {
	   block[Tbig1->g_SeqNum].IsVisited[3]=true;
       e1=block[Tbig1->g_SeqNum].Edge[3].ID1;
	   e2=block[Tbig1->g_SeqNum].Edge[3].ID2;
       Tsmall1=block[Tbig1->g_SeqNum].Tin[3];
	   
	   Tbig2=Tbig1->p3tin; 
	   no=TheNumber(e1,e2, Tbig2->ID1, Tbig2->ID2, Tbig2->ID3);
	   if(no==1)
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[1];
           block[Tbig2->g_SeqNum].IsVisited[1]=true;
	   }
	   else if(no==2) 
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[2];
		   block[Tbig2->g_SeqNum].IsVisited[2]=true;
	   }
	   else if(no==3) 
	   {
		   Tsmall2=block[Tbig2->g_SeqNum].Tin[3];
		   block[Tbig2->g_SeqNum].IsVisited[3]=true;
	   }
	   //����Tsmall1��Tsmall2֮����ڽ�ָ��
	   no=TheNumber(e1,e2, Tsmall1->ID1, Tsmall1->ID2, Tsmall1->ID3);
	   if(no==1) Tsmall1->p1tin=Tsmall2;
	   else if(no==2) Tsmall1->p2tin=Tsmall2;
	   else if(no==3) Tsmall1->p3tin=Tsmall2;
	   no=TheNumber(e1,e2, Tsmall2->ID1, Tsmall2->ID2, Tsmall2->ID3);
	   if(no==1) Tsmall2->p1tin=Tsmall1;
	   else if(no==2) Tsmall2->p2tin=Tsmall1;
	   else if(no==3) Tsmall2->p3tin=Tsmall1;
   }
   //���ζ��ڽӵ����������ν��еݹ���ã���ɶ��������������������
   SetBlockPointer(Tbig1->p1tin, block);   
   SetBlockPointer(Tbig1->p2tin, block);
   SetBlockPointer(Tbig1->p3tin, block);
}
//����������
//������Data���������ݼ���pNum�������ݼ���С
//		tinfirst������������ͷָ�룻tinlast������������βָ��
//		root��frontf��frontl�����������������洢ǰ��
//		basef��basel����˫�������洢�׿�
void CGISTinView::GeneDelaunay(PointSet *Data,int pNum,TRIANGLE **tinfirst, TRIANGLE **tinlast, TREENODE **root, TREENODE **frontf, TREENODE **frontl, FB **basef, FB **basel)
{
   qsort(Data, pNum,sizeof(Data[0]),cmp);   //��������
   int g_pointmark=0;
   //��ʼ��ɨ����
   SetInitialScan(Data, pNum, tinfirst, tinlast, g_pointmark, root, frontf, frontl, basef, basel);
   //����TIN
   CreateTinBy_Scan(Data, pNum, tinlast, g_pointmark, root, frontf, frontl, basef, basel);
   //����ǰ��
   EndFrontLine(Data, tinlast, frontf, root);
}
void CGISTinView::DrawBlockTin(TRIANGLE *tin,PointSet *OriginalData)
{
    CDC *pDC=GetDC();
	static int flag=0;
	CPen  NewPen;
	if(flag!=0)
	{  
		srand((unsigned)time( NULL));	
	    int i=rand()%14;  //[0, 13]
	    NewPen.CreatePen(PS_SOLID,1,colors[i]);
	}
	else
	   NewPen.CreatePen(PS_SOLID,2,colors[0]);	
	CPen *OldPen=pDC->SelectObject(&NewPen);
	for(TRIANGLE *T=tin;T!=NULL;T=T->next)
	{
		PNT P1 ={OriginalData[T->ID1].x,OriginalData[T->ID1].y};
		PNT P2 ={OriginalData[T->ID2].x,OriginalData[T->ID2].y};
		PNT P3 ={OriginalData[T->ID3].x,OriginalData[T->ID3].y};
		GetScreenPoint(&P1); GetScreenPoint(&P2); GetScreenPoint(&P3);
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P2.x,P2.y);

		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P3.x,P3.y);

		pDC->MoveTo(P3.x,P3.y);
		pDC->LineTo(P2.x,P2.y);	  
	}

	pDC->SelectObject(OldPen);
	flag++;
	ReleaseDC(pDC);
}
void CGISTinView::DrawTin(CDC *pDC,PointSet *OriginalData)
{
	CPen  NewPen;
	NewPen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	CPen *OldPen=pDC->SelectObject(&NewPen);
	for(TRIANGLE *T=tinHead;T!=NULL;T=T->next)
	{
		PNT P1 ={OriginalData[T->ID1].x,OriginalData[T->ID1].y};
		PNT P2 ={OriginalData[T->ID2].x,OriginalData[T->ID2].y};
		PNT P3 ={OriginalData[T->ID3].x,OriginalData[T->ID3].y};
		GetScreenPoint(&P1); GetScreenPoint(&P2); GetScreenPoint(&P3);
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P2.x,P2.y);
// 		CString ss1;
// 		ss1.Format("%d",T->ID1);
// 		pDC->TextOut(P1.x,P1.y,ss1);
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P3.x,P3.y);
// 		CString ss2;
// 		ss2.Format("%d",T->ID2);
// 		pDC->TextOut(P2.x,P2.y,ss2);
		pDC->MoveTo(P3.x,P3.y);
		pDC->LineTo(P2.x,P2.y);	  
// 		CString ss3;
// 		ss3.Format("%d",T->ID3);
// 		pDC->TextOut(P3.x,P3.y,ss3);
	 }
	pDC->SelectObject(OldPen);
}

///////��ʼ������Ӧ�����������ĵ㼯///////////////
void CGISTinView::InitialCenterPointset(PointSet *Data,int &DataSize, PointSet *OriginalData, long &OriginalSize)
{
	//ԭʼ�㼯��ʼ��������Χ�ĸ��������㼯
	//��Χ�ĸ�����������չ��20������Ŀ����Ϊ��ȥ������ʱ����Ӱ���ڲ�������
	OriginalData[OriginalSize].x=dXmin-20*dXdelt; OriginalData[OriginalSize].y=dYmin-20*dYdelt; 
	OriginalData[OriginalSize].ID=OriginalSize;  OriginalData[OriginalSize].label=true;
	OriginalSize++; 
	OriginalData[OriginalSize].x=dXmax+20*dXdelt; OriginalData[OriginalSize].y=dYmin-20*dYdelt; 
	OriginalData[OriginalSize].ID=OriginalSize;  OriginalData[OriginalSize].label=true;
	OriginalSize++;
	OriginalData[OriginalSize].x=dXmax+20*dXdelt; OriginalData[OriginalSize].y=dYmax+20*dYdelt;
	OriginalData[OriginalSize].ID=OriginalSize;  OriginalData[OriginalSize].label=true;
	OriginalSize++;
	OriginalData[OriginalSize].x=dXmin-20*dXdelt; OriginalData[OriginalSize].y=dYmax+20*dYdelt;
	OriginalData[OriginalSize].ID=OriginalSize;  OriginalData[OriginalSize].label=true;
	OriginalSize++;
	//����Χ�ĸ�����������ĵ㼯	
	int Index=OriginalSize-4;  
	Data[DataSize].x=dXmin-dXdelt; Data[DataSize].y=dYmin-dYdelt;  Data[DataSize].ID=Index++; 
	DataSize++;
	Data[DataSize].x=dXmax+dXdelt; Data[DataSize].y=dYmin-dYdelt;  Data[DataSize].ID=Index++;
	DataSize++;
	Data[DataSize].x=dXmax+dXdelt; Data[DataSize].y=dYmax+dYdelt;  Data[DataSize].ID=Index++; 
	DataSize++;
	Data[DataSize].x=dXmin-dXdelt; Data[DataSize].y=dYmax+dYdelt;  Data[DataSize].ID=Index++; 
	DataSize++;
}

//////////////////////////////////////////////////////////////////////////
int CGISTinView::OnLeft(double x,double y,PointSet P1,PointSet P2)
{
	if((P2.x-P1.x)*(y-P1.y)-(P2.y-P1.y)*(x-P1.x)>0)
		return -1;
	else if((P2.x-P1.x)*(y-P1.y)-(P2.y-P1.y)*(x-P1.x)<0)
		return 1;
	else 
		return 0;
}
//�õ�����������block��������blockgrid��Ϣ
void CGISTinView::GetBlockGridInfo()
{
	//�������
	double xStart=xmin-BlockGridOffset, yStart=ymin-BlockGridOffset;
	//�յ�����
    double xEnd=xmax+BlockGridOffset, yEnd=ymax+BlockGridOffset;
	//ÿһС�����ĳ�����
	double xcol_wide = (xEnd-xStart)/(BlockGridSize);
	double yrow_wide = (yEnd-yStart)/(BlockGridSize);
	double x1,y1,x2,y2,x3,y3,x4,y4;


	for(int i=0;i<BlockGridSize;i++)
	{
		for (int j=0;j<BlockGridSize;j++)
		{
			x1=xStart+j*xcol_wide;y1=yStart+i*yrow_wide;
			x2=xStart+(j+1)*xcol_wide;y2=yStart+i*yrow_wide;
			x3=xStart+j*xcol_wide;y3=yStart+(i+1)*yrow_wide;
			x4=xStart+(j+1)*xcol_wide;y4=yStart+(i+1)*yrow_wide;
			blockGrid[i][j].triangleNum=0;
			blockGrid[i][j].triFirst=NULL;
			for(TRIANGLE *t=tinHead; t!=NULL; t=t->next)
			{
				//��ͬһ�������εĵ�ĸ���
				int sameTriNum=0;
				if(OnLeft(x1,y1,PointData[t->ID1],PointData[t->ID2])+OnLeft(x1,y1,PointData[t->ID2],PointData[t->ID3])+OnLeft(x1,y1,PointData[t->ID3],PointData[t->ID1])==3
					||OnLeft(x1,y1,PointData[t->ID1],PointData[t->ID2])+OnLeft(x1,y1,PointData[t->ID2],PointData[t->ID3])+OnLeft(x1,y1,PointData[t->ID3],PointData[t->ID1])==-3
				)	//�жϵ��Ƿ�����������
					sameTriNum++;
				if(OnLeft(x2,y2,PointData[t->ID1],PointData[t->ID2])+OnLeft(x2,y2,PointData[t->ID2],PointData[t->ID3])+OnLeft(x2,y2,PointData[t->ID3],PointData[t->ID1])==3
					||OnLeft(x2,y2,PointData[t->ID1],PointData[t->ID2])+OnLeft(x2,y2,PointData[t->ID2],PointData[t->ID3])+OnLeft(x2,y2,PointData[t->ID3],PointData[t->ID1])==-3
				)	//�жϵ��Ƿ�����������
					sameTriNum++;
				if(OnLeft(x3,y3,PointData[t->ID1],PointData[t->ID2])+OnLeft(x3,y3,PointData[t->ID2],PointData[t->ID3])+OnLeft(x3,y3,PointData[t->ID3],PointData[t->ID1])==3
					||OnLeft(x3,y3,PointData[t->ID1],PointData[t->ID2])+OnLeft(x3,y3,PointData[t->ID2],PointData[t->ID3])+OnLeft(x3,y3,PointData[t->ID3],PointData[t->ID1])==-3
				)	//�жϵ��Ƿ�����������
					sameTriNum++;
				if(OnLeft(x4,y4,PointData[t->ID1],PointData[t->ID2])+OnLeft(x4,y4,PointData[t->ID2],PointData[t->ID3])+OnLeft(x4,y4,PointData[t->ID3],PointData[t->ID1])==3
					||OnLeft(x4,y4,PointData[t->ID1],PointData[t->ID2])+OnLeft(x4,y4,PointData[t->ID2],PointData[t->ID3])+OnLeft(x4,y4,PointData[t->ID3],PointData[t->ID1])==-3
				)	//�жϵ��Ƿ�����������
					sameTriNum++;
				if (sameTriNum==0)
				{
					continue;
				}
				else if (sameTriNum==4)
				{
					blockGrid[i][j].triangleNum++;
					TRIANGLENODE *tri =new TRIANGLENODE;
					tri->next=NULL;tri->g_SeqNum=t->g_SeqNum;tri->ID1=t->ID1;tri->ID2=t->ID2;tri->ID3=t->ID3;
					blockGrid[i][j].triFirst=tri;
					break;
				}
				else
				{
					blockGrid[i][j].triangleNum++;
					TRIANGLENODE *tri =new TRIANGLENODE;
					switch(blockGrid[i][j].triangleNum)
					{
						case 1: 
								
							tri->next=NULL;tri->g_SeqNum=t->g_SeqNum;tri->ID1=t->ID1;tri->ID2=t->ID2;tri->ID3=t->ID3;
							blockGrid[i][j].triFirst=tri;
							break;
						case 2: 
							
							tri->next=NULL;tri->g_SeqNum=t->g_SeqNum;tri->ID1=t->ID1;tri->ID2=t->ID2;tri->ID3=t->ID3;
							blockGrid[i][j].triFirst->next=tri;
							break;
						case 3: 
							
							tri->next=NULL;tri->g_SeqNum=t->g_SeqNum;tri->ID1=t->ID1;tri->ID2=t->ID2;tri->ID3=t->ID3;
							blockGrid[i][j].triFirst->next->next=tri;
							break;
						case 4: 
							
							tri->next=NULL;tri->g_SeqNum=t->g_SeqNum;tri->ID1=t->ID1;tri->ID2=t->ID2;tri->ID3=t->ID3;
							blockGrid[i][j].triFirst->next->next->next=tri;
							break;
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////


///////////////����TIN��������////////////////////
/*************************************************/
void CGISTinView::OnGenerateTin() 
{
	clock_t start,finish,startTotal,finishTotal;
	
	if (PointCenter == NULL)
	{
		AfxMessageBox(_T("���Ƚ�������Ӧ���飡"));
		return;
	}
	startTotal = start = clock();
	FBNODE *basef=NULL,*basel=NULL; //˫�������洢�׿�
    TREENODE *root=NULL,*frontf=NULL,*frontl=NULL; //�������������洢ǰ��

	int i =0;
	//��ʼ�����ĵ����ݣ�����Χ�ĸ��������ݼ��뵽���ݼ���
	//����PointCenter�����ĵ����飬CenterSize �������С
	//    PointData  ��ԭʼ�����ݣ�pointNumber��ԭʼ�����ݴ�С
	InitialCenterPointset(PointCenter,CenterSize, PointData, pointNumber);
	//�������ĵ�����������
    GeneDelaunay(PointCenter, CenterSize, &tinHead, &tinEnd,&root,&frontf, &frontl, &basef, &basel);
 	//����ڴ�
	ClearPartionMemory(frontf, basef);
	//LOP�Ż�
 	LOPScan_Nonrecursion(tinHead, &tinHead,PointData); 
	
    BlockTotal=tinEnd->g_SeqNum+1;
	int *gpNum = new int[BlockTotal]; 
    for(i=0; i<BlockTotal; i++) 
		gpNum[i]=0;  //����ÿһ���ڵĵ���
    GetBlockGridInfo();
	//////////////////////////////////////////////////////////////////////////
	//�������
	double xStart=xmin-BlockGridOffset, yStart=ymin-BlockGridOffset;
	//�յ�����
    double xEnd=xmax+BlockGridOffset, yEnd=ymax+BlockGridOffset;
	//ÿһС�����ĳ�����
	double xcol_wide = (xEnd-xStart)/(BlockGridSize);
	double yrow_wide = (yEnd-yStart)/(BlockGridSize);
	for(i=0;i<pointNumber-4;i++)      //����ÿ���㣬�ж������ڵ�������
	{
		if(PointData[i].label==true)
		{
			PointData[i].gNo=-1;  
			continue;
		}
		int col=(int)((PointData[i].x-xStart)/xcol_wide);
		int row=(int)((PointData[i].y-yStart)/yrow_wide);
		if (blockGrid[row][col].triangleNum==1)
		{
			PointData[i].gNo =blockGrid[row][col].triFirst->g_SeqNum;
			gpNum[PointData[i].gNo]++;
		}
		else if (blockGrid[row][col].triangleNum==0)
		{
			PointData[i].gNo=-1;
		}
		else
		{
			bool find = false;
			//������block��ص��������в���
			for (TRIANGLE *t=blockGrid[row][col].triFirst;t!=NULL; t=t->next)
			{
				if(OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==3
				||OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==-3
				)	//�жϵ��Ƿ�����������
				{
					PointData[i].gNo =t->g_SeqNum;
					gpNum[PointData[i].gNo]++;
					find = true;
					break;
				}
			}
			if(find==false)
			{
				TRIANGLE *t = tinHead;
				for(; t!=NULL; t=t->next)
				{
					if(OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==3
						||OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==-3
					)	//�жϵ��Ƿ�����������
					{
						PointData[i].gNo =t->g_SeqNum;
						gpNum[PointData[i].gNo]++;
						break;
					}
				}
				if(t==NULL) 
				{
					PointData[i].gNo=-1;
				}
			}			
		}		
	}
	blockTin=new BlockTin[BlockTotal];
	//��ָ���������洢�����еĶ�Ӧ�����Σ����ڼ���

	TinSave=(TRIANGLE**)malloc(sizeof(*TinSave) * BlockTotal);
	i=0;
	for(TRIANGLE *t=tinHead; t!=NULL; t=t->next,i++)
       TinSave[i]=t;
	for(i=0; i<BlockTotal; i++)
	{
		blockTin[i].pNum=gpNum[i]+3;   //�������ڵ���+��������������
		blockTin[i].pts=new PointSet[blockTin[i].pNum];  //�洢�㼯������
		int pT=blockTin[i].pNum;
		////////���½������ε���������ŵ���������////////////
		blockTin[i].pts[pT-3].x=PointData[TinSave[i]->ID1].x; blockTin[i].pts[pT-3].y=PointData[TinSave[i]->ID1].y; blockTin[i].pts[pT-3].ID=PointData[TinSave[i]->ID1].ID;
		blockTin[i].pts[pT-2].x=PointData[TinSave[i]->ID2].x; blockTin[i].pts[pT-2].y=PointData[TinSave[i]->ID2].y; blockTin[i].pts[pT-2].ID=PointData[TinSave[i]->ID2].ID;
		blockTin[i].pts[pT-1].x=PointData[TinSave[i]->ID3].x; blockTin[i].pts[pT-1].y=PointData[TinSave[i]->ID3].y; blockTin[i].pts[pT-1].ID=PointData[TinSave[i]->ID3].ID;
	}
	for(i=0; i<BlockTotal; i++) 
		gpNum[i]=0;  //���ÿһ���ڵĵ��������
    for( i=0; i<pointNumber-4; i++)
	{
      int gNo =PointData[i].gNo;    //�����ڵĿ��
      if(gNo>=0 && gNo<BlockTotal)
	  {
		blockTin[gNo].pts[gpNum[gNo]].x=PointData[i].x;
		blockTin[gNo].pts[gpNum[gNo]].y=PointData[i].y;
		blockTin[gNo].pts[gpNum[gNo]].ID=PointData[i].ID;
		gpNum[gNo]++;  //���ڵ�����һ
	  }
	}
	delete[]gpNum;
	finish = clock();    
	runTime[0]=(double) (finish - start)/CLOCKS_PER_SEC;   // ����Ӧ�����ʱ��   
	start = clock();
	for(i=0; i<BlockTotal; i++)
	{
	    int	pTotal=blockTin[i].pNum;     //���ڵĵ���

		root=NULL;	frontf=NULL;  frontl=NULL;	basef=NULL;	 basel=NULL;
		GeneDelaunay(blockTin[i].pts, pTotal, &(blockTin[i].first), &(blockTin[i].last),&root,&frontf, &frontl, &basef, &basel);
		
		GetCurrentBlockInfor(blockTin[i], TinSave[i], blockTin[i].pts, frontf, basef);
		LOPScan_Nonrecursion(blockTin[i].first,&(blockTin[i].first),PointData);
		RefreshBlockInfor(&blockTin[i], *TinSave[i]);
		ClearPartionMemory(frontf, basef);
//		DrawBlockTin(blockTin[i].first,PointData);
	}
	SetBlockPointer(tinHead, blockTin);		
	ClearTinMemory(&tinHead);
	for(i=0; i<BlockTotal; i++)
	{
		if(i==0) 
		{
			tinHead= blockTin[i].first;
			tinEnd=blockTin[i].last;
		} 
		else
		{
          tinEnd->next=blockTin[i].first;
		  tinEnd=blockTin[i].last;
		}
	}
	delete[]blockTin;
	finish = clock();
	runTime[1] = (double) (finish - start)/CLOCKS_PER_SEC;      //�ֿ����TIN����ʱ��
	start = clock();
	TRIANGLENODE *pTin=tinHead;
	while (pTin!=NULL)
	{
		pTin->visited = 0;
		pTin = pTin->next;
	}
    LOPScan_Nonrecursion(tinHead, &tinHead,PointData);
//	LOPScan_Nonrecursion(tinHead, &tinHead,PointData);
	finish = clock();
	runTime[2] = (double) (finish - start)/CLOCKS_PER_SEC;      //LOP����ʱ��
	start = clock();
	DeleteVertex();
	finish = clock();
	runTime[3] = (double) (finish - start)/CLOCKS_PER_SEC;      //ɾ��������ζ����ʱ��
	RefreshScreen();
	finishTotal = clock();
	runTime[4] = (double) (finishTotal - startTotal)/CLOCKS_PER_SEC;
}
void CGISTinView::SetBlockTri(BlockTin* block,TRIANGLE* blockTri,TRIANGLE tri,int no,int* found)
{

	TRIANGLE* tp=NULL;
	TRIANGLE* tp1=NULL;
	int p=0,q=0;

	tp=tp1=blockTri;
	while (tp!=NULL&&tp->next!=NULL)
	{
		tp=tp->next;
		if (HaveSameEdgeScan(tp->ID1,tp->ID2,tp->ID3,tri.ID1,tri.ID2,tri.ID3,&p,&q)==1&&SameEdge(block->Edge[no].ID1,block->Edge[no].ID2,p,q)==1)
		{
			block->Tin[no]=tp;
			*found=1;
			return;
		}
	}
	while (tp1!=NULL&&tp1->back!=NULL)
	{
		tp1=tp1->back;
		if (HaveSameEdgeScan(tp1->ID1,tp1->ID2,tp1->ID3,tri.ID1,tri.ID2,tri.ID3,&p,&q)==1&&SameEdge(block->Edge[no].ID1,block->Edge[no].ID2,p,q)==1)
		{
			block->Tin[no]=tp1;
			*found=1;
			return;
		}
	}
	if (*found!=1)
	{
		AfxMessageBox("SetBlockTri����");
	}
}
void CGISTinView::RefreshBlockInfor(BlockTin* block,TRIANGLE tri)
{
	int p=0,q=0,find=0;
	if (!HaveSameEdgeScan(block->Tin[1]->ID1,block->Tin[1]->ID2,block->Tin[1]->ID3,tri.ID1,tri.ID2,tri.ID3,&p,&q))
	{

		find=0;
		SetBlockTri(block,block->Tin[1],tri,1,&find);
	}
	if (!HaveSameEdgeScan(block->Tin[2]->ID1,block->Tin[2]->ID2,block->Tin[2]->ID3,tri.ID1,tri.ID2,tri.ID3,&p,&q))
	{

		find=0;
		SetBlockTri(block,block->Tin[2],tri,2,&find);
	}
	if (!HaveSameEdgeScan(block->Tin[3]->ID1,block->Tin[3]->ID2,block->Tin[3]->ID3,tri.ID1,tri.ID2,tri.ID3,&p,&q))
	{

		find=0;
		SetBlockTri(block,block->Tin[3],tri,3,&find);
	}
}
//ɾ��������ζ�����ĸ�����
void CGISTinView::DeleteVertex()
{
 	TRIANGLENODE *p=tinHead;
	for(TRIANGLENODE *t=p->next;t!=NULL;)
	{
		//pointNumber��¼�˵㼯�е�ĸ���������ĸ�Ϊ������ε��ĸ�����
		if(pointNumber-5<t->ID1||pointNumber-5<t->ID2||pointNumber-5<t->ID3)
		{
			p->next=t->next;
			TRIANGLENODE *temp=t;
			t=t->next;
			delete temp;
			continue;
		}
		t=t->next;
		p=p->next;
	}
	if(tinHead->ID1>pointNumber-5||tinHead->ID2>pointNumber-5||tinHead->ID3>pointNumber-5)
		tinHead=tinHead->next;
}

void CGISTinView::LOPScan_BlockTIN(TRIANGLE *p, TRIANGLE **tinfirst, PointSet *OriginalData)
{
	LIST  *tp1=NULL,*tp2=NULL;	//�����¼�����ε�����ڵ�
								//tp1��������ͷ
								//tp2���涯̬������ڽ�������

	LIST *front=NULL,*rear1=NULL,*rear2=NULL;	//front�������ԭ����������
												//rear1��������ڽ������ε�����
												//rear2�����ʵ����һ���ڵ㣬
												//�ýڵ㱣���˵�һ�������Σ�tinfirst����nextָ��null
												//�ڽ������������ĩβ��ָ��ýڵ㣬���ô���ǣ��ݹ�ĳ��ڣ���nextָ��null

	rear2=new LIST;	rear2->tri=*tinfirst;  rear2->next=NULL;
	front=new LIST;	front->tri=p; p->visited=1;	front->next=rear2;
	rear1=front;    
	while(front->next!=NULL)
	{
		tp1=front;    //tp1Ϊ����ͷ
		if(tp1->tri->p1tin!=NULL)  //�ڽ������δ���
		{
			if(tp1->tri->p1tin->visited!=1) //��δ������
			{
				tp2=new LIST;
				tp2->tri=tp1->tri->p1tin;
				tp2->tri->visited=1;
				rear1->next=tp2;      //���ڽ������μ��뵽rear1��
				tp2->next=rear2;
				rear1=rear1->next;	
			}
		}
		if(tp1->tri->p2tin!=NULL)
		{
			if(tp1->tri->p2tin->visited!=1)
			{
				tp2=new LIST;
				tp2->tri=tp1->tri->p2tin;
				tp2->tri->visited=1;
				rear1->next=tp2;tp2->next=rear2;
				rear1=rear1->next;				
			}
		}
		if(tp1->tri->p3tin!=NULL)
		{
			if(tp1->tri->p3tin->visited!=1)
			{
				tp2=new LIST;
				tp2->tri=tp1->tri->p3tin;
				tp2->tri->visited=1;
				rear1->next=tp2;tp2->next=rear2;
				rear1=rear1->next;				
			}
		}
		front=front->next;
		//����һ�������ε��ڽ�������ȫ���ҵ��������뵽����rear1��
		//Ȼ���ڴӵ�һ�������ο�ʼ�����ڽ������α�����һ�������ο�ʼ�����Ż�
		Lop_all(&(tp1->tri),&rear1, rear2,OriginalData);
		delete tp1;
	}
	delete rear2;
	TRIANGLENODE *pTin=p;
	while (pTin!=NULL)
	{
		pTin->visited = 0;
		pTin = pTin->next;
	}
//	delete front;
}

DWORD WINAPI CreateTin(LPVOID pThreadParam)
{
 	CThreadParam * p=(CThreadParam*)pThreadParam;

	int i=p->iBlock;
	FBNODE *basef=NULL,*basel=NULL; //�׿�
	TREENODE *root=NULL,*frontf=NULL,*frontl=NULL; //ǰ��
	int	pTotal=p->pTin->blockTin[i].pNum;     //���ڵĵ���
	root=NULL;	frontf=NULL;  frontl=NULL;	basef=NULL;	 basel=NULL;
	p->pTin->GeneDelaunay(p->pTin->blockTin[i].pts, pTotal, &(p->pTin->blockTin[i].first), &(p->pTin->blockTin[i].last),&root,&frontf, &frontl, &basef, &basel);
	p->pTin->GetCurrentBlockInfor(p->pTin->blockTin[i], p->pTin->TinSave[i], p->pTin->blockTin[i].pts, frontf, basef);		
	p->pTin->ClearPartionMemory(frontf, basef);
	p->pTin->LOPScan_BlockTIN(p->pTin->blockTin[i].first,&(p->pTin->blockTin[i].first),p->pTin->PointData);
	p->pTin->RefreshBlockInfor(&p->pTin->blockTin[i],*p->pTin->TinSave[i]);
	return 0;
}

void CGISTinView::OnTINMultiThread() 
{
// TODO: Add your command handler code here
	clock_t startMulti,finishMulti,start,finish,groupStart,groupEnd;

	if (PointCenter == NULL)
	{
		AfxMessageBox("���Ƚ�������Ӧ���飡");
		return;
	}
	startMulti = start = clock();
	FBNODE *basef=NULL,*basel=NULL; //�׿�
    TREENODE *root=NULL,*frontf=NULL,*frontl=NULL; //ǰ��

	int i =0;
	InitialCenterPointset(PointCenter,CenterSize, PointData, pointNumber);
    GeneDelaunay(PointCenter, CenterSize, &tinHead, &tinEnd,&root,&frontf, &frontl, &basef, &basel);
 	ClearPartionMemory(frontf, basef);
 	LOPScan_Nonrecursion(tinHead, &tinHead,PointData); 	
	GetBlockGridInfo();
    BlockTotal=tinEnd->g_SeqNum+1;
	int *gpNum = new int[BlockTotal]; 
    for(i=0; i<BlockTotal; i++) 
		gpNum[i]=0;  //����ÿһ���ڵĵ���
    groupStart = clock();
	//�������
	double xStart=xmin-BlockGridOffset, yStart=ymin-BlockGridOffset;
	//�յ�����
    double xEnd=xmax+BlockGridOffset, yEnd=ymax+BlockGridOffset;
	//ÿһС�����ĳ�����
	double xcol_wide = (xEnd-xStart)/(BlockGridSize);
	double yrow_wide = (yEnd-yStart)/(BlockGridSize);
	for(i=0;i<pointNumber-4;i++)      //����ÿ���㣬�ж������ڵ�������
	{
		if(PointData[i].label==true)
		{
			PointData[i].gNo=-1;  
			continue;
		}
		int col=(int)((PointData[i].x-xStart)/xcol_wide);
		int row=(int)((PointData[i].y-yStart)/yrow_wide);
		if (blockGrid[row][col].triangleNum==1)
		{
			PointData[i].gNo =blockGrid[row][col].triFirst->g_SeqNum;
			gpNum[PointData[i].gNo]++;
		}
		else if (blockGrid[row][col].triangleNum==0)
		{
			PointData[i].gNo=-1;
		}
		else
		{
			bool find = false;
			//������block��ص��������в���
			TRIANGLE *t = blockGrid[row][col].triFirst;
			for (;t!=NULL; t=t->next)
			{
				if(OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==3
				||OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==-3
				)	//�жϵ��Ƿ�����������
				{
					PointData[i].gNo =t->g_SeqNum;
					gpNum[PointData[i].gNo]++;
					find = true;
					break;
				}
			}
			if(find==false)
			{
				for(t=tinHead; t!=NULL; t=t->next)
				{
					if(OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==3
						||OnLeft(PointData[i],PointData[t->ID1],PointData[t->ID2])+OnLeft(PointData[i],PointData[t->ID2],PointData[t->ID3])+OnLeft(PointData[i],PointData[t->ID3],PointData[t->ID1])==-3
					)	//�жϵ��Ƿ�����������
					{
						PointData[i].gNo =t->g_SeqNum;
						gpNum[PointData[i].gNo]++;
						break;
					}
				}
				if(t==NULL) 
				{
					PointData[i].gNo=-1;
				}
			}			
		}		
	}

	groupEnd = clock();
	blockTin=new BlockTin[BlockTotal];
	//��ָ���������洢�����еĶ�Ӧ�����Σ����ڼ���

	TinSave=(TRIANGLE**)malloc(sizeof(*TinSave) * BlockTotal);
	i=0;
	for(TRIANGLE *t=tinHead; t!=NULL; t=t->next,i++)
       TinSave[i]=t;
	for(i=0; i<BlockTotal; i++)
	{
		blockTin[i].pNum=gpNum[i]+3;   //�������ڵ���+��������������
		blockTin[i].pts=new PointSet[blockTin[i].pNum];  //�洢�㼯������
		int pT=blockTin[i].pNum;
		////////���½������ε���������ŵ���������////////////
		blockTin[i].pts[pT-3].x=PointData[TinSave[i]->ID1].x; blockTin[i].pts[pT-3].y=PointData[TinSave[i]->ID1].y; blockTin[i].pts[pT-3].ID=PointData[TinSave[i]->ID1].ID;
		blockTin[i].pts[pT-2].x=PointData[TinSave[i]->ID2].x; blockTin[i].pts[pT-2].y=PointData[TinSave[i]->ID2].y; blockTin[i].pts[pT-2].ID=PointData[TinSave[i]->ID2].ID;
		blockTin[i].pts[pT-1].x=PointData[TinSave[i]->ID3].x; blockTin[i].pts[pT-1].y=PointData[TinSave[i]->ID3].y; blockTin[i].pts[pT-1].ID=PointData[TinSave[i]->ID3].ID;
	}
	for(i=0; i<BlockTotal; i++) 
		gpNum[i]=0;  //���ÿһ���ڵĵ��������
    for( i=0; i<pointNumber-4; i++)
	{
      int gNo =PointData[i].gNo;    //�����ڵĿ��
      if(gNo>=0 && gNo<BlockTotal)
	  {
		blockTin[gNo].pts[gpNum[gNo]].x=PointData[i].x;
		blockTin[gNo].pts[gpNum[gNo]].y=PointData[i].y;
		blockTin[gNo].pts[gpNum[gNo]].ID=PointData[i].ID;
		gpNum[gNo]++;  //���ڵ�����һ
	  }
	}
	delete[]gpNum;
	finish = clock();   
	
	runTime[5]=(double) (groupEnd - groupStart)/CLOCKS_PER_SEC;
	runTime[0]=(double) (finish - start)/CLOCKS_PER_SEC;   // ����Ӧ�����ʱ��   
 
	start = clock();
// 	CopyData=new PointSet[pointNumber];
// 	memcpy(CopyData, PointData, pointNumber*sizeof(PointSet));
	DWORD *idHTin = new DWORD[BlockTotal];
	param = new CThreadParam[BlockTotal];
	HANDLE hThrds[ThreadPoolSIZE];
	DWORD rc;
	int slot=0;
	for(iBlock=0;iBlock<BlockTotal;iBlock++)
	{ 
		if(iBlock >= ThreadPoolSIZE)
		{
			rc=WaitForMultipleObjects(ThreadPoolSIZE,hThrds,FALSE,INFINITE);
			slot = rc - WAIT_OBJECT_0;
			if(slot >=0 && slot <ThreadPoolSIZE)
				CloseHandle(hThrds[slot]);
		}
		param[iBlock].pTin = this;
		param[iBlock].iBlock = iBlock;
		hThrds[slot++] = CreateThread(NULL,0,CreateTin,&param[iBlock],0,&idHTin[iBlock]);
	}
	if(iBlock<ThreadPoolSIZE)
	{
		WaitForMultipleObjects(iBlock,hThrds,TRUE,INFINITE);
		for(i=0;i<iBlock;i++)
			CloseHandle(hThrds[i]);
	}
	else
	{
		rc=WaitForMultipleObjects(ThreadPoolSIZE,hThrds,TRUE,INFINITE);
		for(i=0;i<ThreadPoolSIZE;i++)
			CloseHandle(hThrds[i]);
	}

	delete[] param;
	finish = clock();    
	runTime[1]=(double) (finish - start)/CLOCKS_PER_SEC;   // ����Ӧ�����ʱ��   
	start = clock();
    SetBlockPointer(tinHead, blockTin);		
	ClearTinMemory(&tinHead);
	for(i=0; i<BlockTotal; i++)
	{
		if(i==0) 
		{
			tinHead= blockTin[i].first;
			tinEnd=blockTin[i].last;
		} 
		else
		{
          tinEnd->next=blockTin[i].first;
		  tinEnd=blockTin[i].last;
		}
	}
	delete[]blockTin;
	TRIANGLENODE* pTin = tinHead;
	while (pTin!=NULL)
	{
		pTin->visited = 0;
		pTin = pTin->next;
	}
    LOPScan_Nonrecursion(tinHead, &tinHead,PointData);   
	finish = clock();    
	runTime[2]=(double) (finish - start)/CLOCKS_PER_SEC;   //    
	start = clock();
	DeleteVertex();	
	finish=clock();
	runTime[3] = (double) (finish-start)/CLOCKS_PER_SEC;
	RefreshScreen();
	finishMulti = clock();
	runTime[4] = (double) (finishMulti-startMulti)/CLOCKS_PER_SEC;
	delete[] idHTin;
//	delete[] CopyData;
}

void CGISTinView::OnTINNoGroup() 
{
	clock_t start,finish,startTotal,finishTotal;
	start = clock();
	startTotal = clock();
	FBNODE *basef=NULL,*basel=NULL; //�׿�
    TREENODE *root=NULL,*frontf=NULL,*frontl=NULL; //ǰ��
	PointSet *Data=new PointSet[pointNumber];
	memcpy(Data, PointData, pointNumber*sizeof(PointSet));
	GeneDelaunay(Data, pointNumber, &tinHead, &tinEnd,&root,&frontf, &frontl, &basef, &basel);
	ClearPartionMemory(frontf, basef);
	finish=clock();
	runTime[1] = (double) (finish-start)/CLOCKS_PER_SEC;
	start = clock();
 	LOPScan_Nonrecursion(tinHead, &tinHead,PointData);
	LOPScan_Nonrecursion(tinHead, &tinHead,PointData);
	delete Data;
	finish=clock();
	runTime[2] = (double) (finish-start)/CLOCKS_PER_SEC;
	finishTotal = clock();
	runTime[4] = (double) (finishTotal-startTotal)/CLOCKS_PER_SEC;
	RefreshScreen();	
}
