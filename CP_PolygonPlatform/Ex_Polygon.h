#pragma once

#include "CP_Polygon.h"

//不考虑所有奇异的多边形构造情况

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
	int previous;//previous边的下标
	int next;//next边的下标
	int D_plus;//对应previous边
	int D_minus;//对应next边
	int nextVertex;//如果是cross-vertex，则存储对应的下一个vertex,初始为-1
	ExPolygon* nextPoly;//若耗费空间太多，可以考虑去掉该属性，使用D_plus和D_minus来间接获取重合的点
	int preVertex;//如果是cross-vertex，则存储对应的上一个vertex,初始为-1
	ExPolygon* prePoly;//若耗费空间太多，可以考虑去掉该属性，使用D_plus和D_minus来间接获取重合的点
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
	DirFlag direction;//对应vertex的previous边还是next边,+对应previous,-对应next
	int pre;//前一个Descriptor
	int next;//后一个Descriptor
	int vertex;//对应的vertex的下标
	ExPolygon* polygon;
	int groupid;//记录当前一组Descriptor的id
	int Did;//记录当前descriptor的id

public:
	Descriptor(int origin, int gid, int Did, ExPolygon* poly, DirFlag dir);
};

typedef vector<Descriptor> DescriptorArray;

typedef vector<Vertex> VertexArray;

class Edge
{
public:
	int a;//起始点
	int b;//结束点
	EdgeLabel label;
	bool mark;//true：已经处理过，false：未处理
	ExPolygon* polygon;

public:
	Edge(int one, int two, ExPolygon* poly);
	Edge();
	Edge(ExPolygon* poly);
	double getAngle(bool isA, double tolerance);//默认返回向量(b-a)的角度,即a处的角度。b处角度需要+PI。
	void reverse();//将边改变为原方向的逆向
};

typedef vector<Edge> EdgeArray;
typedef vector<int> IntArray;

class Contour
{
public:
	IntArray vertexs;
	IntArray edges;
	ContourLabel label;//标志该环是否与另一个区域的发生了相交
	ContourPos position;//内环或是外环或是未知
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

//direction==true,调整点的序列为顺时针；direction==false,调整点的序列为逆时针
extern void sortVertex(VertexArray& vertexs,IntArray& vertexIds, bool direction);
//direction==true,点的序列为顺时针；direction==false,点的序列为逆时针
extern bool getVertexDirection(const VertexArray& vertexs, IntArray& vertexIds);
//获得由一系列点围成的多边形有向面积，逆时针为正，顺时针为负
extern double getAreaWithVertex(const VertexArray& vertexs, IntArray& vertexIds);
//判断一个点是否在某个ExPolygon之内
extern bool isInPolygon(const Vertex& vertex, const ExPolygon& polygon);
//判断一条边是否在某个ExPolygon之内
extern bool isInPolygon(const Edge& edge, const ExPolygon& polygon);
//判断一个点是否在某个Domain之内
extern bool isInDomain(const Vertex& vertex,const Domain& domain);
//判断一个点是否在某个Contour构成的区域之内
extern bool isInContour(const Vertex& vertex, const Contour& contour);
//判断一个testContour是否在另一个contour之内，true：完全在contour内，false：至少存在一个点在contour外或在contour上
extern bool isInContour(const Contour& testContour, const Contour& contour);
//进行布尔运算的第一步：生成所有的新的边交点
extern void makeIntersections(ExPolygon& a,ExPolygon& b, double tolerance, DescriptorArray& descriptors);
//判断两条线段是否相交
extern int getIntersection(Edge& m, Edge& n, double& x, double& y,double tolerance);
//在一条边oldEdge中插入一个点newVertex后，产生新边newEdge，使得原边变为(a,v)，并产生新边(v,b)。对所有涉及的属性进行更新
extern void insertVertex(EdgeArray& edges, VertexArray& vertexs, int newVertex, int oldEdge, int newEdge, double tolerance, DescriptorArray& descriptors, int DesGroupId);
//在a，b之间建立重合联系，考虑双向链表情况
extern void combine(Vertex& a, Vertex& b, int Aid, int Bid, DescriptorArray& descriptors);
//将初生成的Descriptors进行组合和排序，使得多个点重合的情况可以满足
extern void groupDescriptors(DescriptorArray& descriptors);
//对同一组的Descriptors进行排序并连接
extern void sortDescriptors(vector<Descriptor*>& descriptors);
//进行布尔运算的第二步：对Expolygon a中edge和contour进行Labeling
extern void labeling(ExPolygon& a, ExPolygon& b, double tolerance);
//对于当前的边是否存在一个重合的边，若存在，方向为同向或是逆向,据此打上标记
extern void getEdgeLabel(Edge* edge, double tolerance);
//进行布尔运算的第三步：搜集结果环
extern void collectContour(ExPolygon& a, ExPolygon& b, ExPolygon& r, double tolerance, OperationFlag flag);
//对暂时存储第三步结果的数据结构进行初始化
extern void initResultPolygon(ExPolygon& r);
//返回值：是否要包含当前边，若包含，以什么方向包含
extern bool EdgeRule(Edge* edge, DirectionFlag& dir, OperationFlag operation, ExPolygon& a, ExPolygon& b);
//collect操作，生成result环
extern void Collect(Vertex* v, DirectionFlag& dir, Contour& result, OperationFlag operation, ExPolygon& a, ExPolygon& b, double tolerance);
//将所有重合的边标记为已处理
extern void markSharedEdges(Edge* edge, double tolerance);
//进行布尔运算的第四步：将结果环进行组合
extern void combineContours(ExPolygon& origin, ExPolygon& result);
//默认两个Contour没有交叉，最多只有点的重合
extern bool isInContourForced(const Contour& testContour, const Contour& contour);
//验证当前多边形是否合法
extern bool isLegal(ExPolygon& a, double tolerance);
//验证当前loop是否合法
extern bool isLegal(Contour& a, double tolerance);