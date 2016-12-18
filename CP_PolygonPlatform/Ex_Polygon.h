#pragma once

#include <vector>
#include "CP_Polygon.h"

//��������������Ķ���ι������

enum EdgeLabel { E_INSIDE, E_OUTSIDE, E_SHARED1, E_SHARED2, E_NOLABEL };
enum ContourLabel { C_INSIDE, C_OUTSIDE, C_ISECTED, C_NOLABEL };
enum ContourPos { CP_INCONTOUR, CP_OUTCONTOUR, CP_NOLABEL };
enum DirFlag { D_PREVIOUS, D_NEXT, D_NOLABEL};

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
	Edge* previous;
	Edge* next;
	Descriptor* D_plus;//��Ӧprevious��
	Descriptor* D_minus;//��Ӧnext��
	Vertex* nextVertex;//�����cross-vertex����洢��Ӧ����һ��vertex

public:
	Vertex(){
		isCrossed = false;
		D_plus = NULL;
		D_minus = NULL;
		previous = NULL;
		next = NULL;
		nextVertex = NULL;
	}
	Vertex(CP_Point& point){
		x = point.m_x;
		y = point.m_y;
		isCrossed = false;
		D_plus = NULL;
		D_minus = NULL;
		previous = NULL;
		next = NULL;
		nextVertex = NULL;
	}
	~Vertex() {
		delete D_plus;
		delete D_minus;
	}
	void getPoint(CP_Point& result) {
		result.m_x = x;
		result.m_y = y;
	}
};

class Descriptor
{
public:
	double angle;
	DirFlag direction;//��Ӧvertex��previous�߻���next��
	Descriptor* pre;
	Descriptor* next;
	Vertex* vertex;

public:
	Descriptor(Vertex* origin) {
		this->vertex = origin;
		direction = D_NOLABEL;
		pre = NULL;
		next = NULL;
	}
	~Descriptor() {
		pre->next = NULL;
		next->pre = NULL;
		if (direction == D_PREVIOUS)
			vertex->D_plus = NULL;
		else if (direction == D_NEXT)
			vertex->D_minus = NULL;
	}
};

typedef vector<Vertex> VertexArray;

class Edge
{
public:
	Vertex* a;
	Vertex* b;
	EdgeLabel label;
	bool mark;//true���Ѿ��������false��δ����

public:
	Edge(Vertex* one, Vertex* two) {
		a = one;
		b = two;
		label = E_NOLABEL;
		mark = false;
	}
	//Edge getReverse();
	//double getAngle();//Ĭ�Ϸ�������(b-a)�ĽǶ�,��a���ĽǶȡ�b���Ƕ���Ҫ+PI��
	//void insertVertex(Vertex& newVertex, Edge& one, Edge& two);
};

typedef vector<Edge> EdgeArray;

class Contour
{
public:
	VertexArray vertexs;
	EdgeArray edges;
	ContourLabel label;//��־�û��Ƿ�����һ������ķ������ཻ
	ContourPos position;//�ڻ������⻷����δ֪
	Domain* domain;
	ExPolygon* polygon;

public:
	void setContour(CP_Loop& loop);
	void getLoop(CP_Loop& result);
	Contour(Domain* odomain, ExPolygon* opolygon) {
		this->domain = odomain;
		this->polygon = opolygon;
		label = C_NOLABEL;
		position = CP_NOLABEL;
	}
	Contour(Domain* odomain, ExPolygon* opolygon, ContourPos pos) {
		this->domain = odomain;
		this->polygon = opolygon;
		label = C_NOLABEL;
		position = pos;
	}
};

typedef vector<Contour> ContourArray;

class Domain
{
public:
	ContourArray inContours;
	Contour outContour;
	ExPolygon* polygon;
	
public:
	void setDomain(CP_Region& region);
	void getRegion(CP_Region& result);
	Domain(ExPolygon* origin) : outContour(this,origin){
		this->polygon = origin;
	}
};

typedef vector<Domain> DomainArray;

class ExPolygon
{
public:
	DomainArray domains;
public:
	void setPolygon(CP_Polygon& origin);
	void getCP_Polygon(CP_Polygon& result);
	void clearAll();
};

extern void sortVertex(VertexArray& vertexs, bool direction);
extern bool getVertexDirection(const VertexArray& vertexs);
extern double getAreaWithVertex(const VertexArray& vertexs);