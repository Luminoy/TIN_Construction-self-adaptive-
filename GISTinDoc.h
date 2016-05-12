// GISTinDoc.h : interface of the CGISTinDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GISTINDOC_H__21A20CBA_41B0_425E_A3F3_8BF5BBB6A9EF__INCLUDED_)
#define AFX_GISTINDOC_H__21A20CBA_41B0_425E_A3F3_8BF5BBB6A9EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGISTinDoc : public CDocument
{
protected: // create from serialization only
	CGISTinDoc();
	DECLARE_DYNCREATE(CGISTinDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGISTinDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGISTinDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGISTinDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GISTINDOC_H__21A20CBA_41B0_425E_A3F3_8BF5BBB6A9EF__INCLUDED_)
