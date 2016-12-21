#pragma once

#include "CP_Polygon.h"

//��������������Ķ���ι������

enum EdgeLabel { E_INSIDE, E_OUTSIDE, E_SHARED1, E_SHARED2, E_NOLABEL };
enum ContourLabel { C_INSIDE, C_OUTSIDE, C_ISECTED, C_NOLABEL };
enum ContourPos { CP_INCONTOUR, CP_OUTCONTOUR, CP_NOLABEL };
enum DirFlag { D_PREVIOUS, D_NEXT, D_NOLABEL};
enum OperationFlag { UNION, INTERSECTION, DIFFERENCE_AB };
enum DirectionFlag { FORWARD, BACKWARD };

class Vertex;
class Edge;
class Descriptor;
class Contour;
class Domain;
class ExPolygon;

class Vertex
{
public:
	bool isCrossed;
	double x, y;
	int previous;//previous�ߵ��±�
	int next;//next�ߵ��±�
	int D_plus;//��Ӧprevious��
	int D_minus;//��Ӧnext��
	int nextVertex;//�����cross-vertex����洢��Ӧ����һ��vertex,��ʼΪ-1
	ExPolygon* nextPoly;//���ķѿռ�̫�࣬���Կ���ȥ�������ԣ�ʹ��D_plus��D_minus����ӻ�ȡ�غϵĵ�
	int preVertex;//�����cross-vertex����洢��Ӧ����һ��vertex,��ʼΪ-1
	ExPolygon* prePoly;//���ķѿռ�̫�࣬���Կ���ȥ�������ԣ�ʹ��D_plus��D_minus����ӻ�ȡ�غϵĵ�
	ExPolygon* polygon;

public:
	Vertex(ExPolygon* polygon);
	Vertex(ExPolygon* polygon,Vertex& pos);
	Vertex(ExPolygon* polygon, Vertex* pos);
	Vertex(double ix, double iy,ExPolygon* poly);
	Vertex(CP_Point& point,ExPolygon* poly);
	void getPoint(CP_Point& result);
};

class Descriptor
{
public:
	double angle;
	DirFlag direction;//��Ӧvertex��previous�߻���next��,+��Ӧprevious,-��Ӧnext
	int pre;//ǰһ��Descriptor
	int next;//��һ��Descriptor
	int vertex;//��Ӧ��vertex���±�
	ExPolygon* polygon;
	int groupid;//��¼��ǰһ��Descriptor��id
	int Did;//��¼��ǰdescriptor��id

public:
	Descriptor(int origin, int gid, int Did, ExPolygon* poly, DirFlag dir);
};

typedef vector<Descriptor> DescriptorArray;

typedef vector<Vertex> VertexArray;

class Edge
{
public:
	int a;//��ʼ��
	int b;//������
	EdgeLabel label;
	bool mark;//true���Ѿ��������false��δ����
	ExPolygon* polygon;

public:
	Edge(int one, int two, ExPolygon* poly);
	Edge();
	Edge(ExPolygon* poly);
	double getAngle(bool isA, double tolerance);//Ĭ�Ϸ�������(b-a)�ĽǶ�,��a���ĽǶȡ�b���Ƕ���Ҫ+PI��
	void reverse();//���߸ı�Ϊԭ���������
};

typedef vector<Edge> EdgeArray;
typedef vector<int> IntArray;

class Contour
{
public:
	IntArray vertexs;
	IntArray edges;
	ContourLabel label;//��־�û��Ƿ�����һ������ķ������ཻ
	ContourPos position;//�ڻ������⻷����δ֪
	int domain;
	ExPolygon* polygon;
	double maxX, minX, maxY, minY;

public:
	void setContour(CP_Loop& loop);
	void setContour(Contour& contour);
	void reverse();
	void getLoop(CP_Loop& result);
	Contour(int domainID, ExPolygon* opolygon);
	Contour(int domainID, ExPolygon* opolygon, ContourPos pos);
};

typedef vector<Contour> ContourArray;

class Domain
{
public:
	ContourArray contours;
	ExPolygon* polygon;
	int idInPolygon;
	
public:
	void setDomain(CP_Region& region);
	void getRegion(CP_Region& result);
	Domain(ExPolygon* origin){
		this->polygon = origin;
	}
};

typedef vector<Domain> DomainArray;

class ExPolygon
{
public:
	DomainArray domains;
	VertexArray vertexs;
	EdgeArray edges;
	DescriptorArray* descriptors;

public:
	void setPolygon(CP_Polygon& origin, DescriptorArray* des);
	void getCP_Polygon(CP_Polygon& result);
	void clearAll();
	ExPolygon() {
		descriptors = NULL;
	}
};

//direction==true,�����������Ϊ˳ʱ�룻direction==false,�����������Ϊ��ʱ��
extern void sortVertex(VertexArray& vertexs,IntArray& vertexIds, bool direction);
//direction==true,�������Ϊ˳ʱ�룻direction==false,�������Ϊ��ʱ��
extern bool getVertexDirection(const VertexArray& vertexs, IntArray& vertexIds);
//�����һϵ�е�Χ�ɵĶ���������������ʱ��Ϊ����˳ʱ��Ϊ��
extern double getAreaWithVertex(const VertexArray& vertexs, IntArray& vertexIds);
//�ж�һ�����Ƿ���ĳ��ExPolygon֮��
extern bool isInPolygon(const Vertex& vertex, const ExPolygon& polygon);
//�ж�һ�����Ƿ���ĳ��ExPolygon֮��
extern bool isInPolygon(const Edge& edge, const ExPolygon& polygon);
//�ж�һ�����Ƿ���ĳ��Domain֮��
extern bool isInDomain(const Vertex& vertex,const Domain& domain);
//�ж�һ�����Ƿ���ĳ��Contour���ɵ�����֮��
extern bool isInContour(const Vertex& vertex, const Contour& contour);
//�ж�һ��testContour�Ƿ�����һ��contour֮�ڣ�true����ȫ��contour�ڣ�false�����ٴ���һ������contour�����contour��
extern bool isInContour(const Contour& testContour, const Contour& contour);
//���в�������ĵ�һ�����������е��µı߽���
extern void makeIntersections(ExPolygon& a,ExPolygon& b, double tolerance, DescriptorArray& descriptors);
//�ж������߶��Ƿ��ཻ
extern int getIntersection(Edge& m, Edge& n, double& x, double& y,double tolerance);
//��һ����oldEdge�в���һ����newVertex�󣬲����±�newEdge��ʹ��ԭ�߱�Ϊ(a,v)���������±�(v,b)���������漰�����Խ��и���
extern void insertVertex(EdgeArray& edges, VertexArray& vertexs, int newVertex, int oldEdge, int newEdge, double tolerance, DescriptorArray& descriptors, int DesGroupId);
//��a��b֮�佨���غ���ϵ������˫���������
extern void combine(Vertex& a, Vertex& b, int Aid, int Bid, DescriptorArray& descriptors);
//�������ɵ�Descriptors������Ϻ�����ʹ�ö�����غϵ������������
extern void groupDescriptors(DescriptorArray& descriptors);
//��ͬһ���Descriptors������������
extern void sortDescriptors(vector<Descriptor*>& descriptors);
//���в�������ĵڶ�������Expolygon a��edge��contour����Labeling
extern void labeling(ExPolygon& a, ExPolygon& b, double tolerance);
//���ڵ�ǰ�ı��Ƿ����һ���غϵıߣ������ڣ�����Ϊͬ���������,�ݴ˴��ϱ��
extern void getEdgeLabel(Edge* edge, double tolerance);
//���в�������ĵ��������Ѽ������
extern void collectContour(ExPolygon& a, ExPolygon& b, ExPolygon& r, double tolerance, OperationFlag flag);
//����ʱ�洢��������������ݽṹ���г�ʼ��
extern void initResultPolygon(ExPolygon& r);
//����ֵ���Ƿ�Ҫ������ǰ�ߣ�����������ʲô�������
extern bool EdgeRule(Edge* edge, DirectionFlag& dir, OperationFlag operation, ExPolygon& a, ExPolygon& b);
//collect����������result��
extern void Collect(Vertex* v, DirectionFlag& dir, Contour& result, OperationFlag operation, ExPolygon& a, ExPolygon& b, double tolerance);
//�������غϵı߱��Ϊ�Ѵ���
extern void markSharedEdges(Edge* edge, double tolerance);
//���в�������ĵ��Ĳ�����������������
extern void combineContours(ExPolygon& origin, ExPolygon& result);
//Ĭ������Contourû�н��棬���ֻ�е���غ�
extern bool isInContourForced(const Contour& testContour, const Contour& contour);
//��֤��ǰ������Ƿ�Ϸ�
extern bool isLegal(ExPolygon& a, double tolerance);
//��֤��ǰloop�Ƿ�Ϸ�
extern bool isLegal(Contour& a, double tolerance);