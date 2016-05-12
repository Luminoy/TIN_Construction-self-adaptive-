// GISTinDoc.cpp : implementation of the CGISTinDoc class
//

#include "stdafx.h"
#include "GISTin.h"

#include "GISTinDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGISTinDoc

IMPLEMENT_DYNCREATE(CGISTinDoc, CDocument)

BEGIN_MESSAGE_MAP(CGISTinDoc, CDocument)
	//{{AFX_MSG_MAP(CGISTinDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGISTinDoc construction/destruction

CGISTinDoc::CGISTinDoc()
{
	// TODO: add one-time construction code here

}

CGISTinDoc::~CGISTinDoc()
{
}

BOOL CGISTinDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CGISTinDoc serialization

void CGISTinDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGISTinDoc diagnostics

#ifdef _DEBUG
void CGISTinDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGISTinDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGISTinDoc commands
