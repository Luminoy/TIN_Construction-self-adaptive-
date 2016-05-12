# Microsoft Developer Studio Project File - Name="GISTin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GISTin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GISTin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GISTin.mak" CFG="GISTin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GISTin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GISTin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GISTin - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /stack:0x989680 /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "GISTin - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /stack:0x989680 /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "GISTin - Win32 Release"
# Name "GISTin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GISTin.cpp
# End Source File
# Begin Source File

SOURCE=.\GISTin.rc
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\GISTin.rc"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GISTinDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\GISTinView.cpp
# End Source File
# Begin Source File

SOURCE=.\GridDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\GISTin.h
# End Source File
# Begin Source File

SOURCE=.\GISTinDoc.h
# End Source File
# Begin Source File

SOURCE=.\GISTinView.h
# End Source File
# Begin Source File

SOURCE=.\GridDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\GISTin.ico
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\GISTin.ico"
# End Source File
# Begin Source File

SOURCE=.\res\GISTin.rc2
# End Source File
# Begin Source File

SOURCE=.\res\GISTinDoc.ico
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\GISTinDoc.ico"
# End Source File
# Begin Source File

SOURCE=.\res\NewToolbar.bmp
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\NewToolbar.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\pan.cur
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\pan.cur"
# End Source File
# Begin Source File

SOURCE=.\res\select.cur
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\select.cur"
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\zoomin.cur
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\zoomin.cur"
# End Source File
# Begin Source File

SOURCE=.\res\zoomout.cur
# End Source File
# Begin Source File

SOURCE="E:\GIS学习篇\台式机资料\Voronoi程序备份\V图程序\分治法(有待继续完成)\res\zoomout.cur"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
