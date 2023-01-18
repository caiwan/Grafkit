#include "stdafx.h"

#include "shadersrceditor.h"
#include "EditorApp.h"

#include <SciLexer.h>
#include <Scintilla.h>

#include "exceptions.h"

using namespace FWdebugExceptions;
using namespace Scintilla;

// keywordok
namespace {
	#include "ShaderEditorKeywords.cpp.inc"
}

namespace ShaderSource {
	#include "defaultShader.cpp.inc"
}

//////////////////////////////////////////////////////////////////////
// Menu gombok
//////////////////////////////////////////////////////////////////////

class CShaderEditorMenuButton : public CMFCToolBarMenuButton
{
	friend class CShaderEditor;

	DECLARE_SERIAL(CShaderEditorMenuButton)

public:
	CShaderEditorMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CShaderEditorMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction, Registerclass
//////////////////////////////////////////////////////////////////////
#define SHADER_EDITOR_CLASNAME _T("MFC_SHADEREDITOR_CTRL")

IMPLEMENT_DYNAMIC(CShaderEditor, CWnd);

CShaderEditor::CShaderEditor()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
	m_hWnd = NULL;
}

CShaderEditor::~CShaderEditor()
{
}

BOOL CShaderEditor::RegisterWindowClass(){
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, SHADER_EDITOR_CLASNAME , &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = SHADER_EDITOR_CLASNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

//BEGIN_MESSAGE_MAP(CShaderEditor, CDockablePane)
BEGIN_MESSAGE_MAP(CShaderEditor, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Create, onCreate, setupStyle, adjustLayout
//////////////////////////////////////////////////////////////////////
BOOL CShaderEditor::Create(int n, CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle){
	//this->RegisterWindowClass();
	
	// CString nname; nname.Format(SHADER_EDITOR_CLASNAME"_%d",n);	//remelem jau'1
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, nullptr, (HBRUSH)(GetStockObject(GRAY_BRUSH)), nullptr);

	if (!CWnd::Create(className, SHADER_EDITOR_CLASNAME, dwStyle, rect, pParentWnd, nID))
		return false;

	this->hWnd = pParentWnd;

	return true;
}

int CShaderEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	
	if (!m_wndTextEditor.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rectDummy, this, 0)) {
		TRACE0("Cannot create scintilla editor");
		return -1;
	}

#ifndef TOOLBAR_KITEP
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, MenuButton_t(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	MenuButton_t* pButton =  DYNAMIC_DOWNCAST(MenuButton_t, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

#endif

	AdjustLayout(); 
	SetupStyle();

	return 0;
}


void CShaderEditor::SetupStyle(){
	CScintillaCtrl& rCtrl = GetCtrl();

	// --- font
	///@todo FEATURE REQ: menubol allithato legyen legalabb a betumeret
	/*CString*/ std::string sFaceName; int nFaceSize = 0;
	if (0){
		sFaceName = "Courier New";		//rCtrl.StyleSetFont(0, "Courier New");	
		nFaceSize = 8;					//rCtrl.StyleSetSize(0, 8);
	}else{
		sFaceName = "Consolas";	//rCtrl.StyleSetFont(0, "Lucida Console");	
		nFaceSize = 10;					//rCtrl.StyleSetSize(0, 8);
	}

	rCtrl.SetBufferedDraw(false, false);
	rCtrl.SetCodePage(SC_CP_UTF8);


	rCtrl.SetLexer(SCLEX_CPP);
#if 1
	rCtrl.SetKeyWords(0, shaderKeyword);
	rCtrl.SetKeyWords(1, shaderType);
	rCtrl.SetKeyWords(2, shaderBuiltin);
#else
	rCtrl.SetKeyWords(0, cppKeyWords);
#endif

	//Setup styles
	SetAStyle(STYLE_DEFAULT, RGB(0, 0, 0), RGB(0xff, 0xff, 0xff), nFaceSize, sFaceName.c_str());
	rCtrl.StyleClearAll();
	SetAStyle(SCE_C_DEFAULT,                   RGB(0x00, 0x00, 0x00));
	SetAStyle(SCE_C_COMMENT,                   RGB(0x00, 0x80, 0x00));
	SetAStyle(SCE_C_COMMENTLINE,               RGB(0x00, 0x80, 0x00));
	SetAStyle(SCE_C_COMMENTDOC,                RGB(0x00, 0x80, 0x00));
	SetAStyle(SCE_C_COMMENTLINEDOC,            RGB(0x00, 0x80, 0x00));
	SetAStyle(SCE_C_COMMENTDOCKEYWORD,         RGB(0x00, 0x80, 0x00));
	SetAStyle(SCE_C_COMMENTDOCKEYWORDERROR,    RGB(0x00, 0x80, 0x00));
	SetAStyle(SCE_C_NUMBER,                    RGB(0x00, 0x80, 0x80));
	SetAStyle(SCE_C_WORD,                      RGB(0x00, 0x00, 0x80));
	SetAStyle(SCE_C_WORD2,                     RGB(0x00, 0x80, 0x80));

	rCtrl.StyleSetBold(SCE_C_WORD, 1);
	SetAStyle(SCE_C_STRING,                    RGB(0x80, 0, 0x80));
	SetAStyle(SCE_C_IDENTIFIER,                RGB(0, 0, 0));
	SetAStyle(SCE_C_PREPROCESSOR,              RGB(0x80, 0, 0));
	SetAStyle(SCE_C_OPERATOR,                  RGB(0x80, 0x80, 0));

#if 0
  //Setup folding
	rCtrl.SetMarginWidthN(2, 16);
	rCtrl.SetMarginSensitiveN(2, TRUE);
	rCtrl.SetMarginTypeN(2, SC_MARGIN_SYMBOL);
	rCtrl.SetMarginMaskN(2, SC_MASK_FOLDERS);
	rCtrl.SetProperty(_T("fold"), _T("1"));

	//Setup markers

	DefineMarker(SC_MARKNUM_FOLDEROPEN,		SC_MARK_MINUS, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0xFF));
	DefineMarker(SC_MARKNUM_FOLDER,			SC_MARK_PLUS,  RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDERSUB,		SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDERTAIL,		SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDEREND,		SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDEROPENMID,	SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
	DefineMarker(SC_MARKNUM_FOLDERMIDTAIL,	SC_MARK_EMPTY, RGB(0xff, 0xff, 0xff), RGB(0, 0, 0));
#endif 

#if 0
	//Setup auto completion
	rCtrl.AutoCSetSeparator(10); //Use a separator of line feed

	//Setup call tips
	rCtrl.SetMouseDwellTime(1000);
#endif

#if 0
	//Enable Multiple selection
	rCtrl.SetMultipleSelection(TRUE);
#endif

	// --- sorszamok megjelenitese
	///@todo FEATURE REQ: menubol kapcsolhato legyen a sorszamozas
	int nMarginWidth = rCtrl.GetMarginWidthN(0);
	if (0){
		rCtrl.SetMarginWidthN(0, 0);
	}else{
		rCtrl.SetMarginWidthN(0, 32);
	}

	// --- wrapping
	///@todo FEATURE REQ: menubol allithato legyen az automatikus sortores 
	/* http://www.scintilla.org/ScintillaDoc.html#SCI_SETWRAPMODE */

	rCtrl.SetWrapMode(SC_WRAP_WORD);
	rCtrl.SetWrapVisualFlags(SC_WRAPVISUALFLAG_END);
	// --- 

	rCtrl.SetTabWidth(4);
}

void CShaderEditor::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char* face){
	CScintillaCtrl& rCtrl = GetCtrl();

	rCtrl.StyleSetFore(style, fore);
	rCtrl.StyleSetBack(style, back);
	if (size >= 1)
		rCtrl.StyleSetSize(style, size);
	if (face) 
		rCtrl.StyleSetFont(style, face);
}

void CShaderEditor::DefineMarker(int marker, int markerType, COLORREF fore, COLORREF back) 
{
  CScintillaCtrl& rCtrl = GetCtrl();

  rCtrl.MarkerDefine(marker, markerType);
  rCtrl.MarkerSetFore(marker, fore);
  rCtrl.MarkerSetBack(marker, back);
}

void CShaderEditor::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CScintillaCtrl& rCtrl = GetCtrl();

	CRect rectClient;
	GetClientRect(rectClient);

#ifdef TOOLBAR_KITEP
	m_wndTextEditor.MoveWindow(&rectClient);
#else
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	rCtrl.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
#endif

}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

/*int m = this->m_wndEditor.GetCtrl().GetTextLength();
	TCHAR *b = new  TCHAR [m+1];
	this->m_wndEditor.GetCtrl().GetText(m, b);
	this->m_sh
	delete[] b;*/

void CShaderEditor::docFromEditor(CString &dst){
	int m = this->GetCtrl().GetTextLength();
	TCHAR *b = new  TCHAR [m+1];
	this->GetCtrl().GetText(m, b);
	//dst.assign(b);
	dst = b;
	delete[] b;
}

void CShaderEditor::docToEditor(CString &src){
	this->GetCtrl().SetText(src);
}

//////////////////////////////////////////////////////////////////////
// Create, setup style, adjust layout
//////////////////////////////////////////////////////////////////////

void CShaderEditor::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CShaderEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
}


BOOL CShaderEditor::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg);
}

void CShaderEditor::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CShaderEditor::OnSetFocus(CWnd* pOldWnd)
{
	CScintillaCtrl& rCtrl = GetCtrl();

	CWnd::OnSetFocus(pOldWnd);
	rCtrl.SetFocus();
}

void CShaderEditor::OnChangeVisualStyle()
{
#ifndef TOOLBAR_KITEP
	m_ClassViewImages.DeleteImageList();

	int hirez = theApp.m_bHiColorIcons; // BaseApplication::getApplication().Template_IsUseHiResIcons();

	UINT uiBmpId = hirez ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (hirez) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	// ... 
#endif 
}





//////////////////////////////////////////////////////////////////////
// Single shader editor instance 
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CShaderEditorSingle, CDockablePane)

BEGIN_MESSAGE_MAP(CShaderEditorSingle, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()

	ON_COMMAND(ID_EDIT_CUT, &CShaderEditorSingle::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CShaderEditorSingle::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CShaderEditorSingle::OnEditPaste)
//	ON_COMMAND(ID_EDIT_CLEAR, &CShaderEditorSingle::OnEditClear)
	ON_COMMAND(ID_EDIT_UNDO, &CShaderEditorSingle::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CShaderEditorSingle::OnEditRedo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CShaderEditorSingle::OnEditSelectAll)

	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CShaderEditorSingle::OnUpdateNeedSelect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CShaderEditorSingle::OnUpdateNeedSelect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CShaderEditorSingle::OnUpdateNeedPaste)
//	ON_COMMAND(ID_EDIT_CLEAR, &CShaderEditorSingle::OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CShaderEditorSingle::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CShaderEditorSingle::OnUpdateEditRedo)
//	ON_COMMAND(ID_EDIT_SELECT_ALL, &CShaderEditorSingle::OnEditSelectAll)
	
END_MESSAGE_MAP()

CShaderEditorSingle::CShaderEditorSingle(){
}

CShaderEditorSingle::~CShaderEditorSingle(){
}


void CShaderEditorSingle::docFromEditor(){
	ASSERT(m_pDocument);
	if (!m_pDocument)
		throw EX(NullPointerException);

	CString &shader_code = m_pDocument->GetShaderSource();
	this->m_wndEditor.docFromEditor(shader_code);
}

void CShaderEditorSingle::docToEditor(){
	ASSERT(m_pDocument);
	if (!m_pDocument)
		throw EX(NullPointerException);

	this->m_wndEditor.docToEditor(m_pDocument->GetShaderSource());
}

// messages
int CShaderEditorSingle::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_accelerators = BaseApplication::getApplication().GetAcceleratorTable();
	m_accelerators = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));


	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndEditor.Create(0, this, rectDummy, 0, WS_CHILD | WS_VISIBLE | WS_TABSTOP)) {
		TRACE0("Cannot create shader editor sub-window\n");
		return -1;
	}
		

	m_wndEditor.SetParent(this);
	m_wndEditor.SetOwner(this);
	m_wndEditor.SetFocus();

	return 0;
}

void CShaderEditorSingle::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndEditor.SetFocus();
}

void CShaderEditorSingle::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);

	m_wndEditor.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndEditor.RedrawWindow();

	RedrawWindow();
}

BOOL CShaderEditorSingle::PreTranslateMessage(MSG* pMsg)
{
	if (m_accelerators ) {
		if (::TranslateAccelerator(theApp.GetMainWnd()->GetSafeHwnd(), m_accelerators, pMsg)) {
			  return(TRUE);
			}
	  }

	return CDockablePane::PreTranslateMessage(pMsg);
}

void CShaderEditorSingle::OnEditCopy(){
	ASSERT_VALID(this); GetActiveCtrl().Copy();
}
void CShaderEditorSingle::OnEditPaste(){
	ASSERT_VALID(this); GetActiveCtrl().Paste();
}
void CShaderEditorSingle::OnEditCut(){
	ASSERT_VALID(this); GetActiveCtrl().Cut();
}
void CShaderEditorSingle::OnEditUndo(){
	ASSERT_VALID(this); GetActiveCtrl().Undo();
}
void CShaderEditorSingle::OnEditRedo(){
	ASSERT_VALID(this); GetActiveCtrl().Redo();
}
void CShaderEditorSingle::OnEditSelectAll(){
	ASSERT_VALID(this); GetActiveCtrl().SelectAll();
}

void CShaderEditorSingle::OnUpdateNeedSelect(CCmdUI* pCmdUI){
	ASSERT_VALID(this);
  
	CScintillaCtrl& rCtrl = GetActiveCtrl();
	long nStartChar = rCtrl.GetSelectionStart();
	long nEndChar = rCtrl.GetSelectionEnd();
	pCmdUI->Enable(nStartChar != nEndChar);
}

void CShaderEditorSingle::OnUpdateNeedPaste(CCmdUI* pCmdUI){
	ASSERT_VALID(this); pCmdUI->Enable(GetActiveCtrl().CanPaste());
}

void CShaderEditorSingle::OnUpdateEditUndo(CCmdUI* pCmdUI){
	ASSERT_VALID(this); pCmdUI->Enable(GetActiveCtrl().CanUndo());
}

void CShaderEditorSingle::OnUpdateEditRedo(CCmdUI* pCmdUI){
	ASSERT_VALID(this); pCmdUI->Enable(GetActiveCtrl().CanRedo());
}
