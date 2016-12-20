// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ������� 
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// CP_PolygonPlatformDoc.h : CCP_PolygonPlatformDoc ��Ľӿ�
//

#pragma once
#include "CP_Triangle.h"
#include "Ex_Polygon.h"

class CCP_PolygonPlatformDoc : public CDocument
{
protected: // �������л�����
	CCP_PolygonPlatformDoc();
	DECLARE_DYNCREATE(CCP_PolygonPlatformDoc)

// ����
public:
	////////////////////////////////////////////////////////////////
	// ���������������
	CP_Polygon m_a, m_b;
	double m_tolerance; // λ���ݲ�
	double m_scale; // ���ű���
	CP_Point m_translation; // ����ƽ����
	////////////////////////////////////////////////////////////////
	// ���������������
	CP_Polygon m_result;
	CP_TriagleMesh m_triagleMesh;
	bool m_flagBuildA; // true: A; false B��
	bool m_flagSelect; // true: �Ѿ�ʰȡ��; false: û��ʰȡ�κζ�����
	int  m_flagSelectType;
	// 0: û������; 1: ��; 2: ��; 3: ����; 4: �����; 5: �ʷ������Ρ�

	int  m_flagSelectPolygon, m_flagSelectRegion, m_flagSelectID;
	// ��λʰȡ������
	// m_flagSelectPolygon(0: A; 1: B)

	VT_IntArray m_flagSelectIDSetInA, m_flagSelectIDSetInB;
	bool m_flagShowSelect; // true: ֻ��ʾѡ��; false: ������ʾ��
	int  m_edgeNumber; // ������εı�����
	int  m_flagAdd; // 0: û������; 1: �⻷; 2: �ڻ�; 3: �㡣
	int  m_flagAddIDPolygon, m_flagAddIDRegion, m_flagAddIDLoop, m_flagAddIDPointInLoop;
	VT_PointArray m_flagAddPointArray;
	bool m_flagShowA, m_flagShowB, m_flagShowPointID;
	// true: ��ʾ; false: ����ʾ��

	bool m_flagMoveSame; // true: �ƶ��غϵĵ�; false: �غϵ�ֿ��ƶ���
	bool m_flagShowTriangleFace;  // true: ��ʾ; false: ����ʾ��

	// ������һЩ��ʱ�Ա���
	CP_Point m_basePoint;
	bool m_flagMouseDown; // true: ����������; false: �ſ�������

	//���ڲ������������м���������
	ExPolygon ex_A;
	ExPolygon ex_B;

	//���ڴ��沼��������
	ExPolygon ex_result;
	bool showBooleanResult;// true: ��ʾ����������; false: ����ʾ����������
	DescriptorArray descriptors;

// ����
public:
	void mb_initData();
	
// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CCP_PolygonPlatformDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

extern void gb_SerializePolygon(CArchive& ar, CP_Polygon& p);

