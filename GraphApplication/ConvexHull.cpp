#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>



// =============================================================================
// CONVEX HULL =================================================================
// =============================================================================

double PosicioRespeteRecta(CGPoint& a, CGPoint& b, CGPoint& c) {
	return (a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y);
}


double AreaTriangle(CGPoint& a, CGPoint& b, CGPoint c) {
	return abs((a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y)) / 2.0;
}


list<CVertex*> ConvexHull(CVertex A, CVertex B, list<CVertex*> points)
{
	// Si lista de puntos está vacía retornar lista AB
	if (points.empty()){
		list<CVertex*> result;
		result.push_back(&A);
		result.push_back(&B);
		return result;
	}

	// Encontrar Punto C que maximice el triángulo
	double max = AreaTriangle(A.m_Point, B.m_Point, points.front()->m_Point);
	CVertex* C = points.front();

	for (list<CVertex*>::iterator iter = points.begin(); iter != points.end(); iter++) {
		if (AreaTriangle(A.m_Point, B.m_Point, (*iter)->m_Point) > max) {
			max = AreaTriangle(A.m_Point, B.m_Point, (*iter)->m_Point);
			C = (*iter);
		}
	}

	// Dividir puntos en dos conjuntos, fuera del area y dentro del area (por un lado de CB y por el otro de AC)
	list<CVertex*> up;
	list<CVertex*> down;

	for (list<CVertex*>::iterator iter = points.begin(); iter != points.end(); iter++) {
		if (PosicioRespeteRecta(A.m_Point, C->m_Point, (*iter)->m_Point) > 0)
			up.push_back(*iter);
		else if (PosicioRespeteRecta(C->m_Point, B.m_Point, (*iter)->m_Point) < 0)
			down.push_back(*iter);
	}

	// Calcular las dos convexas
	up = ConvexHull(A, (*C), up);
	down = ConvexHull((*C), B, down);

	// Juntar las dos convexas
	up.merge(down);

	return up;
}

// QuickHull ===================================================================

CConvexHull QuickHull(CGraph& graph)
{
	if (graph.m_Vertices.empty()) {
		return CConvexHull(&graph);
	}

	// Buscar Puntos extremos A y B
	CVertex A = graph.m_Vertices.front();
	CVertex B = graph.m_Vertices.front();

	for (list<CVertex>::iterator iter = graph.m_Vertices.begin(); iter != graph.m_Vertices.end(); iter++) {
		// Si el punto está más a la izquierda que A
		if (iter->m_Point.m_X < A.m_Point.m_X)
			A = (*iter);

		// Si el punto está más a la derecha que B
		if (iter->m_Point.m_X > B.m_Point.m_X)
			B = (*iter);
	}

	// Separar en dos conjuntos por la recta AB
	list<CVertex*> left;
	list<CVertex*> right;

	for (list<CVertex>::iterator iter = graph.m_Vertices.begin(); iter != graph.m_Vertices.end(); iter++) {
		if (PosicioRespeteRecta(A.m_Point, B.m_Point, iter->m_Point) < 0)
			left.push_back(&*iter);
		else if (PosicioRespeteRecta(A.m_Point, B.m_Point, iter->m_Point) > 0)
			right.push_back(&*iter);
	}

	// Calcular las dos Convexas
	left = ConvexHull(A, B, left);
	right = ConvexHull(B, A, right);

	// Juntar las dos Convexas
	left.merge(right);
	CConvexHull CH = CConvexHull(&graph);
	CH.m_Vertices = left;

	return CH;
}