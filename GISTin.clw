; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGISTinView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "GISTin.h"
LastPage=0

ClassCount=7
Class1=CGISTinApp
Class2=CGISTinDoc
Class3=CGISTinView
Class4=CMainFrame

ResourceCount=3
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Resource2=IDR_MAINFRAME
Class6=CGridDlg
Class7=CThresholdDlg
Resource3=IDD_GRID_DLG

[CLS:CGISTinApp]
Type=0
HeaderFile=GISTin.h
ImplementationFile=GISTin.cpp
Filter=N

[CLS:CGISTinDoc]
Type=0
HeaderFile=GISTinDoc.h
ImplementationFile=GISTinDoc.cpp
Filter=N

[CLS:CGISTinView]
Type=0
HeaderFile=GISTinView.h
ImplementationFile=GISTinView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CGISTinView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_DISPLAY_GRID
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=GISTin.cpp
ImplementationFile=GISTin.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_LOAD_POINT_FILE
Command2=ID_LOAD_LINE_FILE
Command3=ID_TINNoGroup
Command4=ID_BINARY_GROUP
Command5=ID_GENERATE_TIN
Command6=ID_TINMultiThread
Command7=ID_DISPLAY_GRID
Command8=ID_ZOOM_IN
Command9=ID_ZOOM_OUT
Command10=ID_PAN
Command11=ID_GLOBE
Command12=ID_SELECT
Command13=ID_TwoThread
Command14=ID_MultiThread_CS
CommandCount=14

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_GLOBE
Command2=ID_ZOOM_IN
Command3=ID_ZOOM_OUT
Command4=ID_PAN
Command5=ID_SELECT
CommandCount=5

[DLG:IDD_GRID_DLG]
Type=1
Class=CGridDlg
ControlCount=8
Control1=IDC_EDIT1,edit,1350641793
Control2=IDC_EDIT2,edit,1350641793
Control3=IDOK,button,1342242817
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT3,edit,1350639744

[CLS:CGridDlg]
Type=0
HeaderFile=GridDlg.h
ImplementationFile=GridDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CGridDlg

[CLS:CThresholdDlg]
Type=0
HeaderFile=ThresholdDlg.h
ImplementationFile=ThresholdDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

