#include "stdafx.h"
#include "Ex_Polygon.h"

void ExPolygon::setPolygon(CP_Polygon& origin)
{
	for (int i = 0; i < origin.m_regionArray.size(); i++)
	{
		Domain domain(this);
		domain.setDomain(origin.m_regionArray[i]);
		domains.push_back(domain);
	}
}

void ExPolygon::getCP_Polygon(CP_Polygon& result)
{
	result.mb_clear();
	int size = domains.size();
	for (int i = 0; i < size; i++)
	{
		CP_Region region;
		region.m_polygon = &result;
		region.m_regionIDinPolygon = i;
		domains[i].getRegion(region);
		result.m_regionArray.push_back(region);
	}
}

void ExPolygon::clearAll() {
	for (Domain d : domains) {
		d.outContour.edges.clear();
		d.outContour.vertexs.clear();
		for (Contour p : d.inContours) {
			p.edges.clear();
			p.vertexs.clear();
		}
	}
	domains.clear();
}

void Domain::setDomain(CP_Region& region)
{
	if (region.m_loopArray.size() <= 0) {
		return;
	}
	int size = region.m_loopArray.size();
	for (int i = 0; i < size; i++) {
		if (i == 0) {
			outContour.position = CP_OUTCONTOUR;
			outContour.setContour(region.m_loopArray[i]);
		}else{
			Contour contour(this, this->polygon, CP_INCONTOUR);
			contour.setContour(region.m_loopArray[i]);
			inContours.push_back(contour);
		}
	}
}

//Ĭ��result�е�m_polygon��m_regionIDinPolygon�Ѿ���ֵ
void Domain::getRegion(CP_Region& result)
{
	CP_Loop outloop;
	outloop.m_polygon = result.m_polygon;
	outloop.m_loopIDinRegion = 0;
	outloop.m_regionIDinPolygon = result.m_regionIDinPolygon;
	outContour.getLoop(outloop);
	result.m_loopArray.push_back(outloop);
	
	int size = inContours.size();
	for (int i = 0; i < size; i++) {
		CP_Loop tempInloop;
		tempInloop.m_polygon = result.m_polygon;
		tempInloop.m_loopIDinRegion = i + 1;
		tempInloop.m_regionIDinPolygon = result.m_regionIDinPolygon;
		inContours[i].getLoop(tempInloop);
		result.m_loopArray.push_back(tempInloop);
	}
}

//������ָ��Contour���ڻ������⻷�����
void Contour::setContour(CP_Loop& loop)
{
	VT_PointArray& points = loop.m_polygon->m_pointArray;
	VT_IntArray& point_id = loop.m_pointIDArray;
	int size = point_id.size();
	for (int i = 0; i < size;i++) {
		Vertex temp(points[point_id[i]]);
		vertexs.push_back(temp);
	}
	sortVertex(vertexs, position != CP_OUTCONTOUR);
	size = vertexs.size();
	int j;
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		Edge temp(&vertexs[i],&vertexs[j]);
		edges.push_back(temp);
		vertexs[i].next = &edges[i];
		vertexs[j].previous = &edges[i];
	}
}

//Ĭ��result��m_polygon,m_loopIDinRegion,m_regionIDinPolygon���Ѿ���ֵ
void Contour::getLoop(CP_Loop& result)
{
	VT_PointArray& points = result.m_polygon->m_pointArray;
	VT_IntArray& pointsIDs = result.m_pointIDArray;
	int base = points.size();
	int size = vertexs.size();
	for (int i = 0; i < size; i++) {
		CP_Point temp;
		vertexs[i].getPoint(temp);
		points.push_back(temp);
		pointsIDs.push_back(i + base);
	}
}

//direction==true,�����������Ϊ˳ʱ�룻direction==false,�����������Ϊ��ʱ��
void sortVertex(VertexArray& vertexs, bool direction)
{
	if (getVertexDirection(vertexs) == direction)
	{
		return;
	}
	else
	{
		reverse(vertexs.begin(),vertexs.end());
	}
}

//direction==true,�������Ϊ˳ʱ�룻direction==false,�������Ϊ��ʱ��
bool getVertexDirection(const VertexArray& vertexs)
{
	return getAreaWithVertex(vertexs) < 0;
}

//�����һϵ�е�Χ�ɵĶ���������������ʱ��Ϊ����˳ʱ��Ϊ��
double getAreaWithVertex(const VertexArray& vertexs)
{
	int size = vertexs.size();
	double x1, y1, x2, y2;
	double total = 0;
	int j;
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		x1 = vertexs[i].x;
		y1 = vertexs[i].y;
		x2 = vertexs[j].x;
		y2 = vertexs[j].y;
		total += (x1 - x2)*(y1 + y2);
	}
	return total / 2;
}
