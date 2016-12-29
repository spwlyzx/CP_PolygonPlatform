#include "stdafx.h"
#include "Ex_Polygon.h"

Contour::Contour(int domainID, ExPolygon* opolygon, ContourPos pos) {
	this->domain = domainID;
	this->polygon = opolygon;
	label = C_NOLABEL;
	position = pos;
}

Contour::Contour(int domainID, ExPolygon* opolygon) {
	this->domain = domainID;
	this->polygon = opolygon;
	label = C_NOLABEL;
	position = CP_NOLABEL;
}

Edge::Edge(int one, int two, ExPolygon* poly) {
	a = one;
	b = two;
	polygon = poly;
	label = E_NOLABEL;
	mark = false;
}

Edge::Edge() {
	a = -1;
	b = -1;
	label = E_NOLABEL;
	mark = false;
	polygon = NULL;
}

Edge::Edge(ExPolygon* poly) {
	a = -1;
	b = -1;
	label = E_NOLABEL;
	mark = false;
	polygon = poly;
}

//���߸ı�Ϊԭ���������
void Edge::reverse()
{
	int temp = b;
	b = a;
	a = temp;
}

Descriptor::Descriptor(int origin, int gid, int did, ExPolygon* poly, DirFlag dir) {
	vertex = origin;
	direction = dir;
	pre = -1;
	next = -1;
	groupid = gid;
	polygon = poly;
	Did = did;
}

Vertex::Vertex(ExPolygon* poly) {
	isCrossed = false;
	D_plus = -1;
	D_minus = -1;
	previous = -1;
	next = -1;
	nextVertex = -1;
	preVertex = -1;
	polygon = poly;
	prePoly = NULL;
	nextPoly = NULL;
}

Vertex::Vertex(ExPolygon* poly, Vertex& pos)
{
	polygon = poly;
	x = pos.x;
	y = pos.y;
	isCrossed = false;
	D_plus = -1;
	D_minus = -1;
	previous = -1;
	next = -1;
	nextVertex = -1;
	preVertex = -1;
	prePoly = NULL;
	nextPoly = NULL;
}

Vertex::Vertex(ExPolygon* poly, Vertex* pos)
{
	polygon = poly;
	x = pos->x;
	y = pos->y;
	isCrossed = false;
	D_plus = -1;
	D_minus = -1;
	previous = -1;
	next = -1;
	nextVertex = -1;
	preVertex = -1;
	prePoly = NULL;
	nextPoly = NULL;
}

Vertex::Vertex(double ix, double iy, ExPolygon* poly) {
	x = ix;
	y = iy;
	isCrossed = false;
	D_plus = -1;
	D_minus = -1;
	previous = -1;
	next = -1;
	nextVertex = -1;
	polygon = poly;
	preVertex = -1;
	prePoly = NULL;
	nextPoly = NULL;
}

Vertex::Vertex(CP_Point& point, ExPolygon* poly) {
	x = point.m_x;
	y = point.m_y;
	isCrossed = false;
	D_plus = -1;
	D_minus = -1;
	previous = -1;
	next = -1;
	nextVertex = -1;
	polygon = poly;
	preVertex = -1;
	prePoly = NULL;
	nextPoly = NULL;
}

void ExPolygon::setPolygon(CP_Polygon& origin, DescriptorArray* des)
{
	descriptors = des;
	clearAll();
	int size = origin.m_pointArray.size();
	for (int i = 0; i < size; i++)
	{
		Vertex vertex(origin.m_pointArray[i], this);
		vertexs.push_back(vertex);
	}
	size = origin.m_regionArray.size();
	for (int i = 0; i < size; i++)
	{
		Domain domain(this);
		domain.idInPolygon = i;
		domains.push_back(domain);
		domains[i].setDomain(origin.m_regionArray[i]);
	}
}

void ExPolygon::getCP_Polygon(CP_Polygon& result)
{
	result.mb_clear();
	int size = vertexs.size();
	for (int i = 0; i < size; i++) {
		CP_Point point;
		vertexs[i].getPoint(point);
		result.m_pointArray.push_back(point);
	}
	size = domains.size();
	for (int i = 0; i < size; i++)
	{
		CP_Region region;
		region.m_polygon = &result;
		region.m_regionIDinPolygon = domains[i].idInPolygon;
		domains[i].getRegion(region);
		result.m_regionArray.push_back(region);
	}
}

void ExPolygon::clearAll() {
	domains.clear();
	vertexs.clear();
	edges.clear();
	if (!descriptors)
		return;
	descriptors->clear();
}

void Domain::setDomain(CP_Region& region)
{
	if (region.m_loopArray.size() <= 0) {
		return;
	}
	int size = region.m_loopArray.size();
	for (int i = 0; i < size; i++) {
		ContourPos flag;
		if (i == 0) {
			flag = CP_OUTCONTOUR;
		}
		else {
			flag = CP_INCONTOUR;
		}
		Contour contour(idInPolygon, this->polygon, flag);
		contour.setContour(region.m_loopArray[i]);
		contours.push_back(contour);
	}
}

//Ĭ��result�е�m_polygon��m_regionIDinPolygon�Ѿ���ֵ
void Domain::getRegion(CP_Region& result)
{
	int size = contours.size();
	for (int i = 0; i < size; i++) {
		CP_Loop tempInloop;
		tempInloop.m_polygon = result.m_polygon;
		tempInloop.m_loopIDinRegion = i;
		tempInloop.m_regionIDinPolygon = result.m_regionIDinPolygon;
		contours[i].getLoop(tempInloop);
		result.m_loopArray.push_back(tempInloop);
	}
}

//������ָ��Contour���ڻ������⻷�����
void Contour::setContour(CP_Loop& loop)
{
	VT_IntArray& point_id = loop.m_pointIDArray;
	int size = point_id.size();
	if (size == 0)
		return;

	//�����
	for (int i = 0; i < size; i++) {
		vertexs.push_back(point_id[i]);
	}
	//��֤��Ĵ���Ϸ�
	sortVertex(polygon->vertexs, vertexs, position != CP_OUTCONTOUR);
	//��ȡ��ǰcontour��AABB��
	double x = polygon->vertexs[vertexs[0]].x;
	double y = polygon->vertexs[vertexs[0]].y;
	maxX = x;
	minX = x;
	maxY = y;
	minY = y;
	for (int i = 1; i < size; i++) {
		x = polygon->vertexs[vertexs[i]].x;
		y = polygon->vertexs[vertexs[i]].y;
		if (x > maxX) {
			maxX = x;
		}
		else if (x < minX) {
			minX = x;
		}
		if (y > maxY) {
			maxY = y;
		}
		else if (y < minY) {
			minY = y;
		}
	}
	//�����
	int j;
	int base = polygon->edges.size();
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		Edge temp(vertexs[i], vertexs[j], polygon);
		polygon->edges.push_back(temp);
		edges.push_back(base + i);
		polygon->vertexs[vertexs[i]].next = edges[i];
		polygon->vertexs[vertexs[j]].previous = edges[i];
	}
}

//Ĭ��Contourû�б�ע���⻷��ֻ�����
void Contour::setContour(Contour& initContour)
{
	int size = initContour.vertexs.size();
	if (size == 0)
		return;
	int base = polygon->vertexs.size();
	VertexArray& initVertex = initContour.polygon->vertexs;
	//�����
	for (int i = 0; i < size; i++) {
		Vertex temp(polygon, initVertex[initContour.vertexs[i]]);
		polygon->vertexs.push_back(temp);
		vertexs.push_back(i + base);
	}
	//��ȡ��ǰcontour��AABB��
	double x = polygon->vertexs[vertexs[0]].x;
	double y = polygon->vertexs[vertexs[0]].y;
	maxX = x;
	minX = x;
	maxY = y;
	minY = y;
	for (int i = 1; i < size; i++) {
		x = polygon->vertexs[vertexs[i]].x;
		y = polygon->vertexs[vertexs[i]].y;
		if (x > maxX) {
			maxX = x;
		}
		else if (x < minX) {
			minX = x;
		}
		if (y > maxY) {
			maxY = y;
		}
		else if (y < minY) {
			minY = y;
		}
	}
	////�����
	int j;
	base = polygon->edges.size();
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		Edge temp(vertexs[i], vertexs[j], polygon);
		polygon->edges.push_back(temp);
		edges.push_back(base + i);
		polygon->vertexs[vertexs[i]].next = edges[i];
		polygon->vertexs[vertexs[j]].previous = edges[i];
	}
}

//Ĭ��result��m_polygon,m_loopIDinRegion,m_regionIDinPolygon���Ѿ���ֵ
void Contour::getLoop(CP_Loop& result)
{
	VT_IntArray& pointsIDs = result.m_pointIDArray;
	int size = vertexs.size();
	for (int i = 0; i < size; i++) {
		pointsIDs.push_back(vertexs[i]);
	}
}

//���������ķ���ı�Ϊ����
void Contour::reverse()
{
	//�ı���˳��
	std::reverse(vertexs.begin(), vertexs.end());
	//�ı�0~(n-2)���ߵĴ��򣬸ı�0~(n-1)���ߵķ���
	std::reverse(edges.begin(), edges.end() - 1);
	for (int edgesId : edges) {
		polygon->edges[edgesId].reverse();
	}
}

//�����������еı��Լ������С��
void Contour::buildup()
{
	int size = vertexs.size();
	//��ȡ��ǰcontour��AABB��
	double x = polygon->vertexs[vertexs[0]].x;
	double y = polygon->vertexs[vertexs[0]].y;
	maxX = x;
	minX = x;
	maxY = y;
	minY = y;
	for (int i = 1; i < size; i++) {
		x = polygon->vertexs[vertexs[i]].x;
		y = polygon->vertexs[vertexs[i]].y;
		if (x > maxX) {
			maxX = x;
		}
		else if (x < minX) {
			minX = x;
		}
		if (y > maxY) {
			maxY = y;
		}
		else if (y < minY) {
			minY = y;
		}
	}
	////�����
	int j;
	int base = polygon->edges.size();
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		Edge temp(vertexs[i], vertexs[j], polygon);
		polygon->edges.push_back(temp);
		edges.push_back(base + i);
		polygon->vertexs[vertexs[i]].next = edges[i];
		polygon->vertexs[vertexs[j]].previous = edges[i];
	}
}

void Vertex::getPoint(CP_Point& result) {
	result.m_x = x;
	result.m_y = y;
}

//��һ����oldEdge�в���һ����newVertex�󣬲����±�newEdge��ʹ��ԭ�߱�Ϊ(a,v)���������±�(v,b)���������漰�����Խ��и���
void insertVertex(EdgeArray& edges, VertexArray& vertexs, int newVertex, int oldEdge, int newEdge, double tolerance, DescriptorArray& descriptors, int DesGroupId)
{
	//��Ҫ���µ������У�
	//	�ɱ�oldEdge��b��previous
	//	newEdge:a,b
	//	�ɱ�oldEdge:b
	//	newVertex�ģ�next,previous,isCrossed
	vertexs[edges[oldEdge].b].previous = newEdge;
	edges[newEdge].a = newVertex;
	edges[newEdge].b = edges[oldEdge].b;
	edges[oldEdge].b = newVertex;
	vertexs[newVertex].isCrossed = true;
	vertexs[newVertex].previous = oldEdge;
	vertexs[newVertex].next = newEdge;

	//�½�newVertex��D+��D-
	int plusID = descriptors.size();
	int minusID = plusID + 1;

	Descriptor plus(newVertex, DesGroupId, plusID, edges[newEdge].polygon, D_PREVIOUS);
	Descriptor minus(newVertex, DesGroupId, minusID, edges[newEdge].polygon, D_NEXT);

	plus.angle = edges[oldEdge].getAngle(false, tolerance);
	minus.angle = edges[newEdge].getAngle(true, tolerance);

	descriptors.push_back(plus);
	descriptors.push_back(minus);

	vertexs[newVertex].D_plus = plusID;
	vertexs[newVertex].D_minus = minusID;
}

//Ĭ�Ϸ�������(b-a)�ĽǶ�,��a���ĽǶȡ�b���Ƕ���Ҫ+PI��
double Edge::getAngle(bool isA, double tolerance)
{
	double toReturn = 0;
	const VertexArray& vertexs = polygon->vertexs;
	double x = vertexs[b].x - vertexs[a].x;
	double y = vertexs[b].y - vertexs[a].y;
	if (abs(x) <= tolerance) {
		if (y > 0) {
			toReturn = HALF_PI;
		}
		else {
			toReturn = HALF_PI * 3;
		}
	}
	else {
		double tan = y / x;
		double degree = atan(tan);
		if (x > 0) {
			toReturn = degree > 0 ? degree : degree + DOUBLE_PI;
		}
		else {
			toReturn = degree + PI;
		}
	}
	if (!isA) {
		toReturn = toReturn > PI ? toReturn - PI : toReturn + PI;
	}
	return toReturn;
}

//direction==true,�����������Ϊ˳ʱ�룻direction==false,�����������Ϊ��ʱ��
void sortVertex(VertexArray& vertexs, IntArray& vertexIds, bool direction)
{
	if (getVertexDirection(vertexs, vertexIds) == direction)
		return;
	else
		reverse(vertexIds.begin(), vertexIds.end());
}

//direction==true,�������Ϊ˳ʱ�룻direction==false,�������Ϊ��ʱ��
bool getVertexDirection(const VertexArray& vertexs, IntArray& vertexIds)
{
	return getAreaWithVertex(vertexs, vertexIds) < 0;
}

//�����һϵ�е�Χ�ɵĶ���������������ʱ��Ϊ����˳ʱ��Ϊ��
double getAreaWithVertex(const VertexArray& vertexs, IntArray& vertexIds)
{
	int size = vertexIds.size();
	double x1, y1, x2, y2;
	double total = 0;
	int j;
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		x1 = vertexs[vertexIds[i]].x;
		y1 = vertexs[vertexIds[i]].y;
		x2 = vertexs[vertexIds[j]].x;
		y2 = vertexs[vertexIds[j]].y;
		total += (x1 - x2)*(y1 + y2);
	}
	return total / 2;
}

//�ж�һ�����Ƿ���ĳ��ExPolygon֮��
bool isInPolygon(const Vertex& vertex, const ExPolygon& polygon)
{
	for (const Domain& domain : polygon.domains) {
		if (isInDomain(vertex, domain))
			return true;
	}
	return false;
}

//�ж�һ�����Ƿ���ĳ��ExPolygon֮��
bool isInPolygon(const Edge* edge, const ExPolygon& polygon)
{
	Vertex& a = edge->polygon->vertexs[edge->a];
	Vertex& b = edge->polygon->vertexs[edge->b];
	Vertex newVertex((a.x + b.x) / 2, (a.y + b.y) / 2, edge->polygon);
	return isInPolygon(newVertex, polygon);
}

//�ж�һ�����Ƿ���ĳ��Domain֮��
bool isInDomain(const Edge& edge, const Domain& domain)
{
	Vertex& a = edge.polygon->vertexs[edge.a];
	Vertex& b = edge.polygon->vertexs[edge.b];
	Vertex newVertex((a.x + b.x) / 2, (a.y + b.y) / 2, edge.polygon);
	return isInDomain(newVertex, domain);
}

//�ж�һ�����Ƿ���ĳ��Domain֮��
bool isInDomain(const Vertex& v, const Domain& domain)
{
	if (!isInContour(v, domain.contours[0]))
		return false;
	int size = domain.contours.size();
	for (int j = 1; j < size; j++) {
		if (isInContour(v, domain.contours[j]))
			return false;
	}
	return true;
}

//�ж�һ�����Ƿ���ĳ��Domain֮��
bool isOutDomain(const Contour& contour, const Domain& domain)
{
	for (int edgeId : contour.edges)
	{
		if (isInDomain(contour.polygon->edges[edgeId], domain)) {
			return false;
		}
	}
	return true;
}

//�ж�һ�����Ƿ���ĳ��Contour���ɵ�����֮��
bool isInContour(const Vertex& v, const Contour& contour)
{
	bool oddNodes = false;
	double x = v.x;
	double y = v.y;
	if (contour.maxX != contour.minX && contour.maxY != contour.minY) {
		if (x<contour.minX || x>contour.maxX || y<contour.minY || y>contour.maxY)
			return false;
	}
	const EdgeArray& edges = contour.polygon->edges;
	const VertexArray& vertexs = contour.polygon->vertexs;
	const IntArray& edgeIds = contour.edges;
	int size = edgeIds.size();
	for (int i = 0; i < size; i++) {
		double ax = vertexs[edges[edgeIds[i]].a].x;
		double ay = vertexs[edges[edgeIds[i]].a].y;
		double bx = vertexs[edges[edgeIds[i]].b].x;
		double by = vertexs[edges[edgeIds[i]].b].y;
		if (ax == x && ay == y || bx == x && by == y) {
			return true;
		}
		if ((ay < y && by >= y
			|| by < y && ay >= y)
			&& (ax <= x || bx <= x)) {
			oddNodes ^= (ax + (y - ay) / (by - ay)*(bx - ax) < x);
		}
	}
	return oddNodes;
}

//�ж�һ��testContour�Ƿ�����һ��contour֮�ڣ�true����ȫ��contour�ڣ�false�����ٴ���һ������contour�����contour��
bool isInContour(const Contour& testContour, const Contour& contour)
{
	const VertexArray& vertexs = testContour.polygon->vertexs;
	const IntArray& vertexIds = testContour.vertexs;
	int size = vertexIds.size();
	for (int i = 0; i < size; i++) {
		if (!isInContour(vertexs[vertexIds[i]], contour))
			return false;
	}
	return true;
}

//Ĭ������Contourû�н��棬���ֻ�е���غ�
bool isInContourForced(const Contour& testContour, const Contour& contour)
{
	const VertexArray& vertexs = testContour.polygon->vertexs;
	const IntArray& vertexIds = testContour.vertexs;
	int size = vertexIds.size();
	for (int i = 0; i < size; i++) {
		if (isInContour(vertexs[vertexIds[i]], contour))
			return true;
	}
	return false;
}

//���в�������ĵ�һ�����������е��µı߽���
void makeIntersections(ExPolygon& a, ExPolygon& b, double tolerance, DescriptorArray& descriptors)
{
	double x, y;
	EdgeArray& edgesA = a.edges;
	EdgeArray& edgesB = b.edges;
	VertexArray& vertexsA = a.vertexs;
	VertexArray& vertexsB = b.vertexs;
	Contour* contourA;
	Contour* contourB;
	for (Domain& domainA : a.domains) {
		for (Domain& domainB : b.domains) {
			for (int p = 0; p < (int)domainA.contours.size(); p++) {
				contourA = &domainA.contours[p];
				for (int k = 0; k < (int)domainB.contours.size(); k++) {
					contourB = &domainB.contours[k];
					for (int i = 0; i < (int)contourA->edges.size(); i++) {
						for (int j = 0; j < (int)contourB->edges.size(); j++) {
							int flag = getIntersection(edgesA[contourA->edges[i]], edgesB[contourB->edges[j]], x, y, tolerance);
							if (flag == 0) {
								continue;
							}
							else if (flag == 1) {
								//��¼�²���ĵ�ͱߵ��±�
								int indexVA = vertexsA.size();
								int indexVB = vertexsB.size();
								int indexEA = edgesA.size();
								int indexEB = edgesB.size();

								//�����µĵ�
								Vertex newVertex1(x, y, &a);
								Vertex newVertex2(x, y, &b);

								vertexsA.push_back(newVertex1);
								contourA->vertexs.insert(contourA->vertexs.begin() + i + 1, indexVA);
								vertexsB.push_back(newVertex2);
								contourB->vertexs.insert(contourB->vertexs.begin() + j + 1, indexVB);

								//�����µı�
								Edge newEdge1(&a);
								Edge newEdge2(&b);
								edgesA.push_back(newEdge1);
								contourA->edges.insert(contourA->edges.begin() + i + 1, indexEA);
								edgesB.push_back(newEdge2);
								contourB->edges.insert(contourB->edges.begin() + j + 1, indexEB);

								//���±ߵ����Խ��и���
								int groupid = a.descriptors->size();
								insertVertex(edgesA, vertexsA, indexVA, contourA->edges[i], indexEA, tolerance, descriptors, groupid);
								insertVertex(edgesB, vertexsB, indexVB, contourB->edges[j], indexEB, tolerance, descriptors, groupid);

								//��������֮����غϹ�ϵ
								combine(vertexsA[indexVA], vertexsB[indexVB], indexVA, indexVB, descriptors);

								j++;
							}
							//edgesA[contourA.edges[i]].aΪ����
							else if (flag == 2) {

							}
							//edgesA[contourA.edges[i]].bΪ����
							else if (flag == 3) {

							}
							//edgesB[contourB.edges[j]].aΪ����
							else if (flag == 4) {

							}
							//edgesB[contourB.edges[j]].bΪ����
							else if (flag == 5) {

							}
							else if (flag == 6) {

							}
							//����
						}
					}
				}
			}
		}
	}
	groupDescriptors(descriptors);
}

//�ж������߶��Ƿ��ཻ
//0�����ཻ
//1����ͨ�ཻ
//2������Ϊ�߶�m�ĵ�һ������a
//3������Ϊ�߶�m�ĵڶ�������b
//4������Ϊ�߶�n�ĵ�һ������c
//5������Ϊ�߶�n�ĵڶ�������d
//6�����߶��غ�������������
//7:���غϣ�����Ϊ(a,c)
//8:���غϣ�����Ϊ(a,d)
//9:���غϣ�����Ϊ(b,c)
//10:���غϣ�����Ϊ(b,d)
//11:��ȫ�غϣ�����һ������Ϊ(a,c)һ��Ϊ(b,d)
//12:��ȫ�غϣ�����һ������Ϊ(a,d)һ��Ϊ(b,c)
//13:�����غϣ��غϵ�Ϊ(a,c)����һ������Ϊd
//14:�����غϣ��غϵ�Ϊ(a,c)����һ������Ϊb
//15:�����غϣ��غϵ�Ϊ(a,d)����һ������Ϊc
//16:�����غϣ��غϵ�Ϊ(a,d)����һ������Ϊb
//17:�����غϣ��غϵ�Ϊ(b,c)����һ������Ϊd
//18:�����غϣ��غϵ�Ϊ(b,c)����һ������Ϊa
//19:�����غϣ��غϵ�Ϊ(b,d)����һ������Ϊc
//20:�����غϣ��غϵ�Ϊ(b,d)����һ������Ϊa
int getIntersection(Edge& m, Edge& n, double& x, double& y, double tolerance)
{
	const VertexArray& Mv = m.polygon->vertexs;
	const VertexArray& Nv = n.polygon->vertexs;
	double ax = Mv[m.a].x;
	double ay = Mv[m.a].y;
	double bx = Mv[m.b].x;
	double by = Mv[m.b].y;
	double cx = Nv[n.a].x;
	double cy = Nv[n.a].y;
	double dx = Nv[n.b].x;
	double dy = Nv[n.b].y;
	double max_mx = 0, min_mx = 0, max_nx = 0, min_nx = 0, max_my = 0, min_my = 0, max_ny = 0, min_ny = 0;

	if (ax >= bx) {
		double max_mx = ax;
		double min_mx = bx;
	}
	else {
		double max_mx = bx;
		double min_mx = ax;
	}
	if (ay >= by) {
		double max_my = ay;
		double min_my = by;
	}
	else {
		double max_my = by;
		double min_my = ay;
	}
	if (cx >= dx) {
		double max_nx = cx;
		double min_nx = dx;
	}
	else {
		double max_nx = dx;
		double min_nx = cx;
	}
	if (cy >= dy) {
		double max_ny = cy;
		double min_ny = dy;
	}
	else {
		double max_ny = dy;
		double min_ny = cy;
	}

	if (max_mx<min_nx || min_mx>max_nx || max_my<min_ny || min_my>max_ny) {
		return 0;
	}

	double abc = (ax - cx)*(by - cy) - (ay - cy)*(bx - cx);
	double abd = (ax - dx)*(by - dy) - (ay - dy)*(bx - dx);
	int ABC = 0, ABD = 0;
	if (abc > tolerance) {
		ABC = 1;
	}
	else if (abc < -tolerance) {
		ABC = -1;
	}
	else {
		ABC = 0;
	}
	if (abd > tolerance) {
		ABD = 1;
	}
	else if (abd < -tolerance) {
		ABD = -1;
	}
	else {
		ABD = 0;
	}
	int condition1 = ABC*ABD;

	if (condition1 > 0) {
		return 0;
	}
	else if (condition1 == 0) {
		double cda = (cx - ax)*(dy - ay) - (cy - ay)*(dx - ax);
		double cdb = cda + abc - abd;
		int CDA = 0, CDB = 0;
		if (cda > tolerance) {
			CDA = 1;
		}
		else if (cda < -tolerance) {
			CDA = -1;
		}
		else {
			CDA = 0;
		}
		if (cdb > tolerance) {
			CDB = 1;
		}
		else if (cdb < -tolerance) {
			CDB = -1;
		}
		else {
			CDB = 0;
		}
		int condition2 = CDA*CDB;

		if (condition2 > 0) {
			return 0;
		}
		else if (condition2 == 0) {
			if (ABD != 0 && CDB != 0) {
				return 7;
			}
			else if (ABC != 0 && CDB != 0) {
				return 8;
			}
			else if (ABD != 0 && CDA != 0) {
				return 9;
			}
			else if (ABC != 0 && CDA != 0) {
				return 10;
			}
			if (abs(ax - cx) + abs(ay - cy) <= tolerance) {
				if (abs(bx - dx) + abs(by - dy) <= tolerance) {
					return 11;
				}
				else {
					double px = bx - ax;
					double py = by - ay;
					double qx = dx - cx;
					double qy = dy - cy;
					if (px*qx + py*qy > 0) {
						if (abs(bx - ax) + abs(by - ay) - abs(dx - ax) - abs(dy - ay) > 0) {
							return 13;
						}
						else {
							return 14;
						}
					}
					else {
						return 7;
					}
				}
			}
			else if (abs(ax - dx) + abs(ay - dy) <= tolerance) {
				if (abs(bx - cx) + abs(by - cy) <= tolerance) {
					return 12;
				}
				else {
					double px = bx - ax;
					double py = by - ay;
					double qx = cx - dx;
					double qy = cy - dy;
					if (px*qx + py*qy > 0) {
						if (abs(bx - ax) + abs(by - ay) - abs(cx - ax) - abs(cy - ay) > 0) {
							return 15;
						}
						else {
							return 16;
						}
					}
					else {
						return 8;
					}
				}
			}
			else if (abs(bx - cx) + abs(by - cy) <= tolerance) {
				double px = ax - bx;
				double py = ay - by;
				double qx = dx - cx;
				double qy = dy - cy;
				if (px*qx + py*qy > 0) {
					if (abs(ax - bx) + abs(ay - by) - abs(dx - bx) - abs(dy - by) > 0) {
						return 17;
					}
					else {
						return 18;
					}
				}
				else {
					return 9;
				}
			}
			else if (abs(bx - dx) + abs(by - dy) <= tolerance) {
				double px = ax - bx;
				double py = ay - by;
				double qx = cx - dx;
				double qy = cy - dy;
				if (px*qx + py*qy > 0) {
					if (abs(ax - bx) + abs(ay - by) - abs(cx - bx) - abs(cy - by) > 0) {
						return 19;
					}
					else {
						return 20;
					}
				}
				else {
					return 10;
				}
			}
			return 6;
		}
		else {
			if (ABC == 0) {
				x = cx;
				y = cy;
				return 4;
			}
			else {
				x = dx;
				y = dy;
				return 5;
			}
		}
	}
	else {
		double cda = (cx - ax)*(dy - ay) - (cy - ay)*(dx - ax);
		double cdb = cda + abc - abd;
		int CDA = 0, CDB = 0;
		if (cda > tolerance) {
			CDA = 1;
		}
		else if (cda < -tolerance) {
			CDA = -1;
		}
		else {
			CDA = 0;
		}
		if (cdb > tolerance) {
			CDB = 1;
		}
		else if (cdb < -tolerance) {
			CDB = -1;
		}
		else {
			CDB = 0;
		}
		int condition2 = CDA*CDB;

		if (condition2 > 0) {
			return 0;
		}
		else if (condition2 == 0) {
			if (CDA == 0) {
				x = ax;
				y = by;
				return 2;
			}
			else {
				x = bx;
				y = by;
				return 3;
			}
		}
		else {
			double t = cda / (abd - abc);
			double dx = t*(bx - ax);
			double dy = t*(by - ay);
			x = ax + dx;
			y = ay + dy;
			return 1;
		}
	}
}

//��a��b֮�佨���غ���ϵ������˫���������
void combine(Vertex& a, Vertex& b, int aid, int bid, DescriptorArray& descriptors)
{
	if (a.preVertex < 0 && b.preVertex < 0) {
		a.prePoly = b.polygon;
		a.nextPoly = b.polygon;
		b.prePoly = a.polygon;
		b.nextPoly = a.polygon;
		a.preVertex = bid;
		a.nextVertex = bid;
		b.preVertex = aid;
		b.nextVertex = aid;
	}
	else if (a.preVertex < 0) {
		Vertex& bNext = b.nextPoly->vertexs[b.nextVertex];

		bNext.prePoly = a.polygon;
		bNext.preVertex = aid;

		a.nextPoly = b.nextPoly;
		a.nextVertex = b.nextVertex;

		b.nextPoly = a.polygon;
		b.nextVertex = aid;

		a.prePoly = b.polygon;
		a.preVertex = bid;

		int groupid = descriptors[b.D_plus].groupid;
		descriptors[a.D_plus].groupid = groupid;
		descriptors[a.D_minus].groupid = groupid;
	}
	else if (b.preVertex < 0) {
		Vertex& aNext = a.nextPoly->vertexs[a.nextVertex];

		aNext.prePoly = b.polygon;
		aNext.preVertex = bid;

		b.nextPoly = a.nextPoly;
		b.nextVertex = a.nextVertex;

		a.nextPoly = b.polygon;
		a.nextVertex = bid;

		b.prePoly = a.polygon;
		b.preVertex = aid;

		int groupid = descriptors[a.D_plus].groupid;
		descriptors[b.D_plus].groupid = groupid;
		descriptors[b.D_minus].groupid = groupid;
	}
	else {
		Vertex& bNext = b.nextPoly->vertexs[b.nextVertex];
		Vertex& aNext = a.nextPoly->vertexs[a.nextVertex];

		int temp = b.nextVertex;

		b.nextPoly = aNext.polygon;
		b.nextVertex = a.nextVertex;

		aNext.prePoly = b.polygon;
		aNext.preVertex = bid;

		a.nextPoly = bNext.polygon;
		a.nextVertex = temp;

		bNext.prePoly = a.polygon;
		bNext.preVertex = aid;

		int groupid = descriptors[b.D_plus].groupid;
		Vertex* now = &b.nextPoly->vertexs[b.nextVertex];
		while (descriptors[now->D_plus].groupid != groupid) {
			descriptors[now->D_plus].groupid = groupid;
			descriptors[now->D_minus].groupid = groupid;
			now = &now->nextPoly->vertexs[now->nextVertex];
		}
	}
}

//�������ɵ�Descriptors������Ϻ�����ʹ�ö�����غϵ������������
void groupDescriptors(DescriptorArray& descriptors)
{
	int tempid = 0;
	int size = descriptors.size();
	bool flag = true;//groupid��δ����Ϊtrue������Ϊfalse
	vector<int> record;
	//��ͬһ�����㴦������Descriptor����pre�Լ�next��ϵ
	for (int i = 0; i < size; i++) {
		tempid = descriptors[i].groupid;
		for (int j = 0; j < (int)record.size(); j++) {
			if (record[j] == tempid) {
				flag = false;
				break;
			}
		}
		if (flag) {
			record.push_back(tempid);
			vector<Descriptor*> temp;
			temp.push_back(&descriptors[i]);
			for (int j = i + 1; j < size; j++) {
				if (descriptors[j].groupid == tempid) {
					temp.push_back(&descriptors[j]);
				}
			}
			sortDescriptors(temp);
		}
		flag = true;
	}
}

//��ͬһ���Descriptors������������
void sortDescriptors(vector<Descriptor*>& descriptors)
{
	Descriptor* temp = NULL;
	int size = descriptors.size();
	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (descriptors[i]->angle > descriptors[j]->angle) {
				temp = descriptors[i];
				descriptors[i] = descriptors[j];
				descriptors[j] = temp;
			}
		}
	}
	int j = 0;
	for (int i = 0; i < size; i++) {
		j = (i + 1) % size;
		descriptors[i]->next = descriptors[j]->Did;
		descriptors[j]->pre = descriptors[i]->Did;
	}
}

//���в�������ĵڶ�������Expolygon a��edge��contour����Labeling
void labeling(ExPolygon& a, ExPolygon& b, double tolerance)
{
	EdgeArray& edgesA = a.edges;
	VertexArray& vertexsA = a.vertexs;

	for (Domain& domainA : a.domains) {
		for (Contour& contourA : domainA.contours) {
			bool isSected = false;
			for (int vertexIdA : contourA.vertexs) {
				if (vertexsA[vertexIdA].isCrossed) {
					isSected = true;
					break;
				}
			}
			if (isSected) {
				contourA.label = ContourLabel::C_ISECTED;
				int size = contourA.edges.size();
				for (int i = 0; i < size; i++) {
					Edge* temp = &edgesA[contourA.edges[i]];
					if (vertexsA[temp->a].isCrossed || vertexsA[temp->b].isCrossed) {
						getEdgeLabel(temp, tolerance);
					}
					else {
						if (i == 0) {
							if (isInPolygon(temp, b)) {
								temp->label = EdgeLabel::E_INSIDE;
							}
							else {
								temp->label = EdgeLabel::E_OUTSIDE;
							}
						}
						else {
							temp->label = edgesA[contourA.edges[i - 1]].label;
						}
					}
				}
			}
			else {
				if (isInPolygon(vertexsA[contourA.vertexs[0]], b)) {
					contourA.label = ContourLabel::C_INSIDE;
				}
				else {
					contourA.label = ContourLabel::C_OUTSIDE;
				}
			}
		}
	}
}

//���ڵ�ǰ�ı��Ƿ����һ���غϵıߣ������ڣ�����Ϊͬ��������򡣾ݴ˴��ϱ��
void getEdgeLabel(Edge* edge, double tolerance)
{
	Vertex& va = edge->polygon->vertexs[edge->a];
	Vertex& vb = edge->polygon->vertexs[edge->b];
	DescriptorArray* descriptors = va.polygon->descriptors;
	if (va.isCrossed) {
		double nowAngle = descriptors->at(va.D_minus).angle;
		double pre = descriptors->at(descriptors->at(va.D_minus).pre).angle;
		DirFlag preFlag = descriptors->at(descriptors->at(va.D_minus).pre).direction;
		if (abs(pre - nowAngle) <= tolerance) {
			if (preFlag == DirFlag::D_PREVIOUS) {
				edge->label = EdgeLabel::E_SHARED2;
				return;
			}
			else if (preFlag == DirFlag::D_NEXT) {
				edge->label = EdgeLabel::E_SHARED1;
				return;
			}
		}
		double next = descriptors->at(descriptors->at(va.D_minus).next).angle;
		DirFlag nextFlag = descriptors->at(descriptors->at(va.D_minus).next).direction;
		if (abs(next - nowAngle) <= tolerance) {
			if (nextFlag == DirFlag::D_PREVIOUS) {
				edge->label = EdgeLabel::E_SHARED2;
				return;
			}
			else if (nextFlag == DirFlag::D_NEXT) {
				edge->label = EdgeLabel::E_SHARED1;
				return;
			}
		}
		if (preFlag == DirFlag::D_PREVIOUS) {
			edge->label = EdgeLabel::E_OUTSIDE;
			return;
		}
		else {
			edge->label = EdgeLabel::E_INSIDE;
			return;
		}
	}
	else {
		double nowAngle = descriptors->at(vb.D_plus).angle;
		double pre = descriptors->at(descriptors->at(vb.D_plus).pre).angle;
		DirFlag preFlag = descriptors->at(descriptors->at(vb.D_plus).pre).direction;
		if (abs(pre - nowAngle) <= tolerance) {
			if (preFlag == DirFlag::D_PREVIOUS) {
				edge->label = EdgeLabel::E_SHARED1;
				return;
			}
			else if (preFlag == DirFlag::D_NEXT) {
				edge->label = EdgeLabel::E_SHARED2;
				return;
			}
		}
		double next = descriptors->at(descriptors->at(vb.D_plus).next).angle;
		DirFlag nextFlag = descriptors->at(descriptors->at(vb.D_plus).next).direction;
		if (abs(next - nowAngle) <= tolerance) {
			if (nextFlag == DirFlag::D_PREVIOUS) {
				edge->label = EdgeLabel::E_SHARED1;
				return;
			}
			else if (nextFlag == DirFlag::D_NEXT) {
				edge->label = EdgeLabel::E_SHARED2;
				return;
			}
		}
		if (preFlag == DirFlag::D_PREVIOUS) {
			edge->label = EdgeLabel::E_OUTSIDE;
			return;
		}
		else {
			edge->label = EdgeLabel::E_INSIDE;
			return;
		}
	}
}

//���в�������ĵ��������Ѽ�����������Ľ��r�бߵ���Ϣ���ɿ�
void collectContour(ExPolygon& a, ExPolygon& b, ExPolygon& r, double tolerance, OperationFlag flag)
{
	initResultPolygon(r);
	ContourArray& resultContours = r.domains[0].contours;
	for (Domain& domainA : a.domains) {
		for (Contour& contourA : domainA.contours) {
			if (contourA.label != ContourLabel::C_ISECTED) {
				if (contourA.label == C_INSIDE && flag == INTERSECTION
					|| contourA.label == C_OUTSIDE && flag == UNION
					|| contourA.label == C_OUTSIDE && flag == DIFFERENCE_AB) {
					Contour temp(0, &r);
					resultContours.push_back(temp);
					resultContours[resultContours.size() - 1].setContour(contourA);
				}
			}
			else {
				int size = contourA.edges.size();
				DirectionFlag dir;
				for (int i = 0; i < size; i++) {
					if (EdgeRule(&a.edges[contourA.edges[i]], dir, flag, a, b) && !a.edges[contourA.edges[i]].mark) {
						Contour temp(0, &r);
						int index = resultContours.size();
						resultContours.push_back(temp);
						if (dir == FORWARD) {
							Collect(&a.vertexs[a.edges[contourA.edges[i]].a], dir, resultContours[index], flag, a, b, tolerance);
						}
						else {
							Collect(&a.vertexs[a.edges[contourA.edges[i]].b], dir, resultContours[index], flag, a, b, tolerance);
						}
					}
				}
			}
		}
	}
	for (Domain& domainB : b.domains) {
		for (Contour& contourB : domainB.contours) {
			if (contourB.label != ContourLabel::C_ISECTED) {
				if (contourB.label == C_INSIDE && flag == INTERSECTION || contourB.label == C_OUTSIDE && flag == UNION) {
					Contour temp(0, &r);
					resultContours.push_back(temp);
					resultContours[resultContours.size() - 1].setContour(contourB);
				}
				else if (contourB.label == C_INSIDE && flag == DIFFERENCE_AB) {
					Contour temp(0, &r);
					resultContours.push_back(temp);
					resultContours[resultContours.size() - 1].setContour(contourB);
					resultContours[resultContours.size() - 1].reverse();
				}
			}
			else {
				int size = contourB.edges.size();
				DirectionFlag dir;
				for (int i = 0; i < size; i++) {
					if (EdgeRule(&b.edges[contourB.edges[i]], dir, flag, a, b) && !b.edges[contourB.edges[i]].mark) {
						Contour temp(0, &r);
						int index = resultContours.size();
						resultContours.push_back(temp);
						if (dir == FORWARD) {
							Collect(&b.vertexs[b.edges[contourB.edges[i]].a], dir, resultContours[index], flag, a, b, tolerance);
						}
						else {
							Collect(&b.vertexs[b.edges[contourB.edges[i]].b], dir, resultContours[index], flag, a, b, tolerance);
						}
					}
				}
			}
		}
	}
}

//����ʱ�洢��������������ݽṹ���г�ʼ��
void initResultPolygon(ExPolygon& r)
{
	r.clearAll();
	Domain domain(&r);
	domain.idInPolygon = 0;
	r.domains.push_back(domain);
}

//����ֵ���Ƿ�Ҫ������ǰ�ߣ�����������ʲô�������
bool EdgeRule(Edge* edge, DirectionFlag& dir, OperationFlag operation, ExPolygon& a, ExPolygon& b)
{
	EdgeLabel flag = edge->label;
	if (operation == INTERSECTION) {
		if (flag == E_INSIDE || flag == E_SHARED1 || flag == E_SHARED2) {
			dir = FORWARD;
			return true;
		}
	}
	else if (operation == UNION) {
		if (flag == E_OUTSIDE || flag == E_SHARED1) {
			dir = FORWARD;
			return true;
		}
	}
	else if (operation == DIFFERENCE_AB) {
		if ((flag == E_OUTSIDE || flag == E_SHARED2) && edge->polygon == &a) {
			dir = FORWARD;
			return true;
		}
		if ((flag == E_INSIDE || flag == E_SHARED2) && edge->polygon == &b) {
			dir = BACKWARD;
			return true;
		}
	}
	return false;
}

//collect����������result��
void Collect(Vertex* v, DirectionFlag& dir, Contour& result, OperationFlag operation, ExPolygon& a, ExPolygon& b, double tolerance)
{
	VertexArray& vertexs = result.polygon->vertexs;
	int index;
	Edge* edgeTemp;

	if (dir == FORWARD) {
		edgeTemp = &v->polygon->edges[v->next];
	}
	else {
		edgeTemp = &v->polygon->edges[v->previous];
	}

	bool failure = false;

	do {
		index = vertexs.size();
		Vertex temp(result.polygon, v);
		vertexs.push_back(temp);
		result.vertexs.push_back(index);

		edgeTemp->mark = true;
		if (edgeTemp->label == E_SHARED1 || edgeTemp->label == E_SHARED2) {
			markSharedEdges(edgeTemp, tolerance);
		}
		if (dir == FORWARD) {
			v = &v->polygon->vertexs[v->polygon->edges[v->next].b];
		}
		else {
			v = &v->polygon->vertexs[v->polygon->edges[v->previous].a];
		}
		if (v->isCrossed) {
			Descriptor* d;
			DescriptorArray* descriptors = v->polygon->descriptors;
			if (dir == FORWARD) {
				d = &descriptors->at(descriptors->at(v->D_plus).pre);
			}
			else {
				d = &descriptors->at(descriptors->at(v->D_minus).pre);
			}
			bool notFound = true;
			bool isNext;
			Edge* tempE;
			Vertex* tempV;
			int begin = d->Did;
			do {
				tempV = &d->polygon->vertexs[d->vertex];
				if (d->direction == D_NEXT) {
					tempE = &tempV->polygon->edges[tempV->next];
					isNext = true;
				}
				else {
					tempE = &tempV->polygon->edges[tempV->previous];
					isNext = false;
				}
				DirectionFlag newDir;
				if (!tempE->mark && EdgeRule(tempE, newDir, operation, a, b)) {
					v = tempV;
					if ((isNext && newDir == FORWARD) || (!isNext && newDir == BACKWARD)) {
						dir = newDir;
						notFound = false;
					}
				}
				d = &descriptors->at(d->pre);
				if (d->Did == begin) {
					notFound = false;
					failure = true;
				}
			} while (notFound);
		}
		if (dir == FORWARD) {
			edgeTemp = &v->polygon->edges[v->next];
		}
		else {
			edgeTemp = &v->polygon->edges[v->previous];
		}
	} while (!edgeTemp->mark && !failure);
}

//�������غϵı߱��Ϊ�Ѵ���
void markSharedEdges(Edge* edge, double tolerance)
{
	Vertex* a = &edge->polygon->vertexs[edge->a];
	Vertex* b = &edge->polygon->vertexs[edge->b];

	if (edge->label == E_SHARED1) {
		Edge* tempE;
		Vertex* temp = &a->nextPoly->vertexs[a->nextVertex];
		while (temp != a) {
			tempE = &temp->polygon->edges[temp->next];
			if (abs(tempE->polygon->vertexs[tempE->b].x - b->x) + abs(tempE->polygon->vertexs[tempE->b].y - b->y) <= tolerance) {
				tempE->mark = true;
			}
			temp = &temp->nextPoly->vertexs[temp->nextVertex];
		}
	}
	if (edge->label == E_SHARED2) {
		Edge* tempE;
		Vertex* temp = &a->nextPoly->vertexs[a->nextVertex];
		while (temp != a) {
			tempE = &temp->polygon->edges[temp->previous];
			if (abs(tempE->polygon->vertexs[tempE->a].x - b->x) + abs(tempE->polygon->vertexs[tempE->a].y - b->y) <= tolerance) {
				tempE->mark = true;
			}
			temp = &temp->nextPoly->vertexs[temp->nextVertex];
		}
	}
}

//���в�������ĵ��Ĳ�����������������
void combineContours(ExPolygon& origin, ExPolygon& result)
{
	Domain domain(&origin);
	domain.idInPolygon = 1;
	origin.domains.push_back(domain);
	ContourArray& originContours = origin.domains[0].contours;
	for (int i = 0; i < originContours.size();) {
		originContours[i].buildup();
		if (getVertexDirection(origin.vertexs, originContours[i].vertexs)) {
			originContours[i].position = CP_INCONTOUR;
			origin.domains[1].contours.push_back(originContours[i]);
			originContours.erase(originContours.begin() + i);
		}
		else {
			originContours[i].position = CP_OUTCONTOUR;
			i++;
		}
	}
	int outSize = origin.domains[0].contours.size();
	vector<int> out;
	for (int i = 0; i < outSize; i++) {
		out.push_back(i);
	}
	ContourArray& outArray = origin.domains[0].contours;
	for (int i = 0; i < outSize; i++) {
		for (int j = i + 1; j < outSize; j++) {
			if (!isInContourForced(outArray[out[i]], outArray[out[j]])) {
				int temp = out[i];
				out[i] = out[j];
				out[j] = temp;
			}
		}
	}
	for (int i = 0; i < outSize; i++) {
		Domain tempDomain(&result);
		tempDomain.idInPolygon = i;
		result.domains.push_back(tempDomain);
		Contour tempContour(i, &result, ContourPos::CP_OUTCONTOUR);
		result.domains[i].contours.push_back(tempContour);
		result.domains[i].contours[0].setContour(outArray[out[i]]);
	}
	for (Contour& incontour : origin.domains[1].contours) {
		for (int i = 0; i < outSize; i++) {
			if (isInContourForced(incontour, outArray[out[i]])) {
				Contour tempContour(i, &result, ContourPos::CP_INCONTOUR);
				result.domains[i].contours.push_back(tempContour);
				result.domains[i].contours[result.domains[i].contours.size() - 1].setContour(incontour);
			}
		}
	}
}

//��֤��ǰ������Ƿ�Ϸ�
bool isLegal(ExPolygon& a, double tolerance)
{
	//����һ�������ǲ����ڸö����
	if (a.domains.size() < 1) {
		return true;
	}
	//�������������б߲��ཻ�������Ƕ����غ�
	for (int i = 0; i < a.edges.size(); i++) {
		for (int j = 0; j < a.edges.size(); j++) {
			if (i == j)
				continue;
			double x, y;
			int condition = getIntersection(a.edges[i], a.edges[j], x, y, tolerance);
			if (!(condition == 0 || (condition >= 7 && condition <= 10))) {
				return false;
			}
		}
	}
	//���������У�����ֻ��һ���⻷�����ڻ������⻷��
	//���������е��⻷����������������
	for (Domain& domain : a.domains) {
		for (Contour& contour : domain.contours) {
			//�����ڻ��Ϸ���Ϊ�򵥶����
			if (!isLegal(contour, tolerance)) {
				return false;
			}
		}
		//�����ڻ����ڻ������⻷�ڣ����ڻ����໥Ƕ��
		if (domain.contours.size() > 1) {
			for (int i = 1; i < domain.contours.size(); i++) {
				if (!isInContour(domain.contours[i], domain.contours[0])) {
					return false;
				}
			}
			for (int i = 1; i < domain.contours.size(); i++) {
				for (int j = 1; j < domain.contours.size(); j++) {
					if (i == j)
						continue;
					if (isInContour(domain.contours[i], domain.contours[j])) {
						return false;
					}
				}
			}
		}
		//ÿ���⻷�ض�����������֮��
		for (Domain& otherDomain : a.domains) {
			if (otherDomain.idInPolygon == domain.idInPolygon) {
				continue;
			}
			else {
				if (!isOutDomain(domain.contours[0], otherDomain)) {
					return false;
				}
			}
		}
	}
	return true;
}

//��֤��ǰloop�Ƿ�Ϸ�
bool isLegal(Contour& a, double tolerance)
{
	//EdgeArray& edges = a.polygon->edges;
	if (a.edges.size() <= 2) {
		return false;
	}
	VertexArray vertexs = a.polygon->vertexs;
	//�ж����е㲻�غ�
	for (int i = 0; i < a.vertexs.size(); i++) {
		for (int j = 0; j < a.vertexs.size(); j++) {
			if (i == j)
				continue;
			Vertex& m = vertexs[a.vertexs[i]];
			Vertex& n = vertexs[a.vertexs[j]];
			if (abs(m.x - n.x) + abs(m.y - n.y) <= tolerance) {
				return false;
			}
		}
	}
	return true;
}

//�����⻷��Ĭ�����һ������
void insertOutloop(VT_PointArray& points, CP_Polygon& polygon)
{
	CP_Region region;
	int	regionID = polygon.m_regionArray.size();
	region.m_regionIDinPolygon = regionID;
	polygon.m_regionArray.push_back(region);
	polygon.m_regionArray[regionID].m_polygon = &polygon;

	CP_Loop loop;
	loop.m_loopIDinRegion = 0;
	loop.m_regionIDinPolygon = regionID;
	polygon.m_regionArray[regionID].m_loopArray.push_back(loop);
	polygon.m_regionArray[regionID].m_loopArray[0].m_polygon = &polygon;
	int base = polygon.m_pointArray.size();
	for (int i = 0; i < points.size(); i++) {
		polygon.m_pointArray.push_back(points[i]);
		polygon.m_regionArray[regionID].m_loopArray[0].m_pointIDArray.push_back(i + base);
	}
}

//�����ڻ���������regionID<0���Ӧregion�в������⻷����Ƿ�
void insertInloop(VT_PointArray& points, int regionID, CP_Polygon& polygon)
{
	if (regionID < 0 || polygon.m_regionArray[regionID].m_loopArray.size() == 0) {
		return;
	}
	else {
		int loopId = polygon.m_regionArray[regionID].m_loopArray.size();
		CP_Loop loop;
		loop.m_loopIDinRegion = loopId;
		loop.m_regionIDinPolygon = regionID;
		polygon.m_regionArray[regionID].m_loopArray.push_back(loop);
		polygon.m_regionArray[regionID].m_loopArray[loopId].m_polygon = &polygon;
		int base = polygon.m_pointArray.size();
		for (int i = 0; i < points.size(); i++) {
			polygon.m_pointArray.push_back(points[i]);
			polygon.m_regionArray[regionID].m_loopArray[loopId].m_pointIDArray.push_back(i + base);
		}
	}
}