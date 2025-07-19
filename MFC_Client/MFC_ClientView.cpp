
// MFC_ClientView.cpp: CMFCClientView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFC_Client.h"
#endif

#include "MFC_ClientDoc.h"
#include "MFC_ClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCClientView

IMPLEMENT_DYNCREATE(CMFCClientView, CView)

BEGIN_MESSAGE_MAP(CMFCClientView, CView)
END_MESSAGE_MAP()

// CMFCClientView 생성/소멸

CMFCClientView::CMFCClientView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMFCClientView::~CMFCClientView()
{
}

BOOL CMFCClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMFCClientView 그리기

void CMFCClientView::OnDraw(CDC* /*pDC*/)
{
	CMFCClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMFCClientView 진단

#ifdef _DEBUG
void CMFCClientView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCClientDoc* CMFCClientView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCClientDoc)));
	return (CMFCClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCClientView 메시지 처리기
