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

// CP_PolygonPlatformDoc.cpp : CCP_PolygonPlatformDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "CP_PolygonPlatform.h"
#endif

#include "CP_PolygonPlatformDoc.h"

#include <propkey.h>
#include <sstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCP_PolygonPlatformDoc

IMPLEMENT_DYNCREATE(CCP_PolygonPlatformDoc, CDocument)

BEGIN_MESSAGE_MAP(CCP_PolygonPlatformDoc, CDocument)
END_MESSAGE_MAP()


// CCP_PolygonPlatformDoc ����/����

CCP_PolygonPlatformDoc::CCP_PolygonPlatformDoc()
{
	// TODO: �ڴ����һ���Թ������
	mb_initData();
}

CCP_PolygonPlatformDoc::~CCP_PolygonPlatformDoc()
{
}

void CCP_PolygonPlatformDoc::mb_initData()
{
	m_a.mb_clear();
	m_b.mb_clear();
	ex_A.clearAll();
	ex_B.clearAll();
	m_tolerance = 1e-6; // λ���ݲ�
	m_scale = 1.0; // ���ű���
	m_translation.m_x = 0.0; // ����ƽ����
	m_translation.m_y = 0.0; // ����ƽ����
	m_result.mb_clear();
	m_flagBuildA = true; // true: A; false B��
	m_flagSelect = false;
	m_flagSelectType = 0;
	m_flagSelectPolygon = 0;
	m_flagSelectRegion = 0;
	m_flagSelectID = 0; // ��λʰȡ������
	m_flagShowSelect = false; // true:ֻ��ʾѡ�񼯡�
	m_edgeNumber = 3; // ������εı�����
	m_flagMouseDown = false; // true: ����������
	m_flagAdd = 0;
	m_flagShowA = true;
	m_flagShowB = true;
	m_flagShowPointID = false;
	m_flagMoveSame = false;
	m_flagSelectIDSetInA.clear();
	m_flagSelectIDSetInB.clear();
	m_triagleMesh.mb_clear();
	m_flagShowTriangleFace = false;  // true: ��ʾ; false: ����ʾ��
	showBooleanResult = true;
} // ��CCP_PolygonPlatformDoc�ĳ�Ա����mb_initData����

BOOL CCP_PolygonPlatformDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)
	mb_initData();
	CString string;
	CMFCRibbonBar* robbon_bar = ((CFrameWndEx*)AfxGetMainWnd())->GetRibbonBar(); //��ȡRibbon bar ���
	if (robbon_bar == NULL)
		return TRUE;
	CMFCRibbonEdit* slider = (CMFCRibbonEdit*)robbon_bar->FindByID(ID_TOLERANCE); // ��ȡ�༭����
	if (slider == NULL)
		return TRUE;
	string.Format("%g", m_tolerance);
	slider->SetEditText(string);
	CMFCRibbonComboBox* pbox = (CMFCRibbonComboBox*)robbon_bar->FindByID(ID_COMBO_AorB); // ��ȡ�༭����
	if (pbox == NULL)
		return TRUE;
	pbox->AddItem("�����A");
	pbox->AddItem("�����B");
	pbox->SelectItem(0);
	return TRUE;
}

// CCP_PolygonPlatformDoc ���л�
void CCP_PolygonPlatformDoc::Serialize(CArchive& ar)
{
	CString line;
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
		ar.WriteString("# Polygon Data(Ӻ����: �����������������(�廪��ѧ, 2016��))\r\n");
		ar.WriteString("# �ļ�����ʱ��: ");
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format("%4d-%2d-%2d ", st.wYear, st.wMonth, st.wDay);
		strTime.Format("%2d:%2d:%2d\r\n\r\n", st.wHour, st.wMinute, st.wSecond);
		ar.WriteString(strDate.GetString());
		ar.WriteString(strTime.GetString());
		line.Format("Tolerance %g\r\n\r\n", m_tolerance);
		ar.WriteString(line.GetString());
		line.Format("Coordinate %g %g %g\r\n\r\n", m_scale, m_translation.m_x, m_translation.m_y);
		ar.WriteString(line.GetString());
		line.Format("A Polygon\r\n\r\n");
		ar.WriteString(line.GetString());
		gb_SerializePolygon(ar, m_a);
		line.Format("B Polygon\r\n\r\n");
		ar.WriteString(line.GetString());
		gb_SerializePolygon(ar, m_b);
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
		mb_initData();
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'T')
				{
					string temp;
					stringstream ss(line.GetString());
					ss >> temp;
					ss >> m_tolerance;
					break;
				}
			}
		} // while����
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'C')
				{
					string temp;
					stringstream ss(line.GetString());
					ss >> temp;
					ss >> m_scale >> m_translation.m_x >> m_translation.m_y;
					break;
				}
			}
		} // while����
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'A')
				{
					gb_SerializePolygon(ar, m_a);
					break;
				}
			}
		} // while����
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'B')
				{
					gb_SerializePolygon(ar, m_b);
					break;
				}
			}
		} // while����
		CString string;
		CMFCRibbonBar* robbon_bar = ((CFrameWndEx*)AfxGetMainWnd())->GetRibbonBar(); //��ȡRibbon bar ���
		if (robbon_bar == NULL)
			return;
		CMFCRibbonEdit* slider = (CMFCRibbonEdit*)robbon_bar->FindByID(ID_TOLERANCE); // ��ȡ�༭����
		if (slider == NULL)
			return;
		string.Format("%g", m_tolerance);
		slider->SetEditText(string);
		CMFCRibbonComboBox* pbox = (CMFCRibbonComboBox*)robbon_bar->FindByID(ID_COMBO_AorB); // ��ȡ�༭����
		if (pbox == NULL)
			return;
		pbox->AddItem("�����A");
		pbox->AddItem("�����B");
		pbox->SelectItem(0);
	} // ��Χif/else����
}

// CCP_PolygonPlatformDoc ����
void gb_SerializePolygon(CArchive& ar, CP_Polygon& p)
{
	CString line;
	unsigned int i, j, k;
	if (ar.IsStoring())
	{	// storing code
		line.Format("Pointsize %d\r\n", p.m_pointArray.size());
		ar.WriteString(line.GetString());
		for (i = 0; i<p.m_pointArray.size(); i++)
		{
			line.Format("%g %g\r\n", p.m_pointArray[i].m_x, p.m_pointArray[i].m_y);
			ar.WriteString(line.GetString());
		} // for����
		line.Format("\r\nRegionsize %d\r\n", p.m_regionArray.size());
		ar.WriteString(line.GetString());
		for (i = 0; i<p.m_regionArray.size(); i++)
		{
			line.Format("Region %d\r\n", i);
			ar.WriteString(line.GetString());
			line.Format("Loopsize %d\r\n", p.m_regionArray[i].m_loopArray.size());
			ar.WriteString(line.GetString());
			for (j = 0; j<p.m_regionArray[i].m_loopArray.size(); j++)
			{
				line.Format("Loop %d\r\n", j);
				ar.WriteString(line.GetString());
				line.Format("PointIDsize %d\r\n", p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size());
				ar.WriteString(line.GetString());
				for (k = 0; k<p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size(); k++)
				{
					line.Format("%d ", p.m_regionArray[i].m_loopArray[j].m_pointIDArray[k]);
					ar.WriteString(line.GetString());
				} // for����
				if (p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size()>0)
				{
					line.Format("\r\n");
					ar.WriteString(line.GetString());
				}
			} // for����
		} // for����
		line.Format("\r\n");
		ar.WriteString(line.GetString());
	}
	else
	{	// loading code
		p.m_pointArray.clear();
		p.m_regionArray.clear();
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'P')
				{
					string temp;
					stringstream ss(line.GetString());
					ss >> temp;
					ss >> i;
					break;
				}
			}
		} // while����
		if (i == 0)
			return;
		p.m_pointArray.resize(i);
		for (i = 0; i<p.m_pointArray.size(); i++)
		{
			ar.ReadString(line);
			stringstream ss(line.GetString());
			ss >> p.m_pointArray[i].m_x;
			ss >> p.m_pointArray[i].m_y;
				} // for����
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'R')
				{
					string temp;
					stringstream ss(line.GetString());
					ss >> temp;
					ss >> i;
					break;
				}
			}
		} // while����
		if (i == 0)
			return;
		p.m_regionArray.resize(i);
		for (i = 0; i<p.m_regionArray.size(); i++)
		{
			p.m_regionArray[i].m_polygon = &p;
			p.m_regionArray[i].m_regionIDinPolygon = i;
			ar.ReadString(line);
			ar.ReadString(line);
			string temp;
			stringstream ss(line.GetString());
			ss >> temp;
			ss >> j;
			p.m_regionArray[i].m_loopArray.resize(j);
			for (j = 0; j<p.m_regionArray[i].m_loopArray.size(); j++)
			{
				p.m_regionArray[i].m_loopArray[j].m_polygon = &p;
				p.m_regionArray[i].m_loopArray[j].m_regionIDinPolygon = i;
				p.m_regionArray[i].m_loopArray[j].m_loopIDinRegion = j;
				ar.ReadString(line);
				ar.ReadString(line);
				string temp;
				stringstream ss(line.GetString());
				ss >> temp;
				ss >> k;
				p.m_regionArray[i].m_loopArray[j].m_pointIDArray.resize(k);
				if (k>0)
				{
					ar.ReadString(line);
					stringstream ss(line.GetString());
					for (k = 0; k<p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size(); k++)
					{
						ss >> p.m_regionArray[i].m_loopArray[j].m_pointIDArray[k];
					} // for����
				} // if����
			} // for����
		} // for����
	} // ��Χif/else����
} // ����gb_SerializePolygon����


#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CCP_PolygonPlatformDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CCP_PolygonPlatformDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CCP_PolygonPlatformDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCP_PolygonPlatformDoc ���

#ifdef _DEBUG
void CCP_PolygonPlatformDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCP_PolygonPlatformDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCP_PolygonPlatformDoc ����
