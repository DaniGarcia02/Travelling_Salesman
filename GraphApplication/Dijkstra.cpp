#include "pch.h"
#include "Graph.h"
#include <queue>

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================

void Dijkstra(CGraph& graph, CVertex *pStart)
{
	for (list<CVertex>::iterator iter = graph.m_Vertices.begin(); iter != graph.m_Vertices.end(); iter++) {
		iter->m_DijkstraDistance = std::numeric_limits<double>::max();
		iter->m_DijkstraVisit = false;
	}

	pStart->m_DijkstraDistance = 0;
	pStart->m_DijkstraVisit = false;

	CVertex* pActual = pStart;
	while (pActual != NULL) {
		for (CEdge* e : pActual->m_Edges) {
			if (pActual->m_DijkstraDistance + e->m_Length < e->m_pDestination->m_DijkstraDistance) {
				e->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + e->m_Length;
				e->m_pDestination->m_pDijkstraPrevious = e;
			}
		}
		pActual->m_DijkstraVisit = true;

		double minDistance = std::numeric_limits<double>::max();
		pActual = NULL;
		for (list<CVertex>::iterator iter = graph.m_Vertices.begin(); iter != graph.m_Vertices.end(); iter++) {
			if (!iter->m_DijkstraVisit) {
				if (iter->m_DijkstraDistance < minDistance) {
					minDistance = iter->m_DijkstraDistance;
					pActual = &*iter;
				}
			}
		}
	}
}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

void DijkstraQueue(CGraph& graph, CVertex *pStart)
{
	struct comparator {
		bool operator()(CVertex* v1, CVertex* v2) {
			return v1->m_DijkstraDistance >= v2->m_DijkstraDistance;
		}
	};
	priority_queue<CVertex*, std::vector<CVertex*>, comparator> queue;

	for (list<CVertex>::iterator iter = graph.m_Vertices.begin(); iter != graph.m_Vertices.end(); iter++) {
		iter->m_DijkstraDistance = std::numeric_limits<double>::max();
		iter->m_DijkstraVisit = false;
		iter->m_pDijkstraPrevious = NULL;
	}

	pStart->m_DijkstraDistance = 0;
	pStart->m_DijkstraVisit = false;

	queue.push(pStart);
	while (!queue.empty()) {
		CVertex* pActual = queue.top();
		queue.pop();
		if (!pActual->m_DijkstraVisit) {
			for (CEdge* e : pActual->m_Edges) {
				if (pActual->m_DijkstraDistance + e->m_Length < e->m_pDestination->m_DijkstraDistance) {
					e->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + e->m_Length;
					e->m_pDestination->m_pDijkstraPrevious = e;
					queue.push(e->m_pDestination);
				}
			}
		}
		pActual->m_DijkstraVisit = true;
	}
}
