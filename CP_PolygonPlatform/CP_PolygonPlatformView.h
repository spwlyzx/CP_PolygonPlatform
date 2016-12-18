// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// CP_PolygonPlatformView.h : CCP_PolygonPlatformView 类的接口
//

#pragma once

class CCP_PolygonPlatformView : public CView
{
protected: // 仅从序列化创建
	CCP_PolygonPlatformView();
	DECLARE_DYNCREATE(CCP_PolygonPlatformView)

// 特性
public:
	CCP_PolygonPlatformDoc* GetDocument() const;

// 操作
public:
	void mb_statusSetText(char* s1, char* s2);
	void initBooleanData();

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCP_PolygonPlatformView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnComboAorb();
	afx_msg void OnUpdateComboAorb(CCmdUI *pCmdUI);
	afx_msg void OnEdgeNumber();
	afx_msg void OnNewRightOutloop();
	afx_msg void OnNewRightInloop();
	afx_msg void OnSelectPoint();
	afx_msg void OnUpdateSelectPoint(CCmdUI *pCmdUI);
	afx_msg void OnSelectLoop();
	afx_msg void OnUpdateSelectLoop(CCmdUI *pCmdUI);
	afx_msg void OnSelectRegion();
	afx_msg void OnUpdateSelectRegion(CCmdUI *pCmdUI);
	afx_msg void OnSelectPolygon();
	afx_msg void OnUpdateSelectPolygon(CCmdUI *pCmdUI);
	afx_msg void OnSelectOnly();
	afx_msg void OnUpdateSelectOnly(CCmdUI *pCmdUI);
	afx_msg void OnAddOutloop();
	afx_msg void OnAddInloop();
	afx_msg void OnAddPoint();
	afx_msg void OnDelete();
	afx_msg void OnMoveSame();
	afx_msg void OnUpdateMoveSame(CCmdUI *pCmdUI);
	afx_msg void OnCheck();
	afx_msg void OnPolygonUnion();
	afx_msg void OnPolygonIntersection();
	afx_msg void OnPolygonAB();
	afx_msg void OnPolygonBA();
	afx_msg void OnTolerance();
	afx_msg void OnViewA();
	afx_msg void OnUpdateViewA(CCmdUI *pCmdUI);
	afx_msg void OnViewB();
	afx_msg void OnUpdateViewB(CCmdUI *pCmdUI);
	afx_msg void OnViewResult();
	afx_msg void OnUpdateViewResult(CCmdUI *pCmdUI);
	afx_msg void OnViewFit();
	afx_msg void OnViewStandard();
	afx_msg void OnViewPointId();
	afx_msg void OnUpdateViewPointId(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // CP_PolygonPlatformView.cpp 中的调试版本
inline CCP_PolygonPlatformDoc* CCP_PolygonPlatformView::GetDocument() const
   { return reinterpret_cast<CCP_PolygonPlatformDoc*>(m_pDocument); }
#endif

extern void gb_drawLoop(CDC* pDC, CP_Loop& p,
	double scale, CP_Point translation, int screenX, int screenY,
	int r, int g, int b, int size);
extern void gb_drawPointArrayLine(CDC* pDC, VT_PointArray& pa,
	double scale, CP_Point translation, int screenX, int screenY,
	int r, int g, int b, int size);
extern void gb_drawPointArrayPoint(CDC* pDC, VT_PointArray& pa,
	double scale, CP_Point translation, int screenX, int screenY,
	int r, int g, int b, int size);
extern void gb_drawPointGlobal(CDC* pDC, CP_Point pointGlobal,
	double scale, CP_Point translation, int screenX, int screenY,
	int r, int g, int b, int size);
extern void gb_drawPointScreen(CDC* pDC, int x, int y,
	int r, int g, int b, int size);
extern void gb_drawPolygonLoop(CDC* pDC, CP_Polygon& p,
	double scale, CP_Point translation, int screenX, int screenY,
	int outR, int outG, int outB,
	int inR, int inG, int inB,
	int size);
extern void gb_drawPolygonPoint(CDC* pDC, CP_Polygon& p,
	double scale, CP_Point translation, int screenX, int screenY,
	int r, int g, int b, int size);
extern void gb_drawPolygonPointID(CDC* pDC, CP_Polygon& p,
	double scale, CP_Point translation, int screenX, int screenY,
	int r, int g, int b);
