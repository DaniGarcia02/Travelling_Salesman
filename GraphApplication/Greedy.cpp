#include "pch.h"
#include "Graph.h"
// SalesmanTrackGreedy =========================================================

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits &visits)
{
	CTrack resultat(&graph);
	CTrack track(&graph);

	CVertex* v = visits.m_Vertices.front();
	list<CVertex*> candidats = visits.m_Vertices;
	candidats.pop_front();
	candidats.pop_back();

	list<CVertex*>::iterator it;

	while (!candidats.empty()) {
		DijkstraQueue(graph, v);
		double minDistance = std::numeric_limits<double>::max();
		
		for (list<CVertex*>::iterator iter = candidats.begin(); iter != candidats.end(); iter++) {
			if ((*iter)->m_DijkstraDistance <= minDistance) {
				minDistance = (*iter)->m_DijkstraDistance;
				it = iter;
			}
		}

		track.Clear();
		track.AddFirst((*it)->m_pDijkstraPrevious);
		
		CEdge* e = (*it)->m_pDijkstraPrevious;
		while (e->m_pOrigin != v) {
			track.AddFirst(e->m_pOrigin->m_pDijkstraPrevious);
			e = e->m_pOrigin->m_pDijkstraPrevious;
		}
		
		resultat.Append(track);
		candidats.erase(it);
		v = (*it);
	}

	DijkstraQueue(graph, v);
	track.Clear();
	CEdge* e = visits.m_Vertices.back()->m_pDijkstraPrevious;
	track.AddFirst(e);

	while (e->m_pOrigin != v) {
		track.AddFirst(e->m_pOrigin->m_pDijkstraPrevious);
		e = e->m_pOrigin->m_pDijkstraPrevious;
	}

	resultat.Append(track);

	return resultat;
}
