#include "pch.h"
#include "Graph.h"
#include <stack>
#include <vector>
using namespace std;

// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================

double lenShortest;
int n;
CTrack best(NULL);
CTrack track(NULL);
stack<CVertex*> visited;
CVertex* desti;
CVisits vis(NULL);

void SalesmanTrackBacktrackingRec(CVertex* actual)
{	
	bool counted = false;
	if (actual->m_PerVisitar) {
		n++;
		counted = true;
		actual->m_PerVisitar = false;

		while (!visited.empty()) {
			visited.top()->m_JaHePassat = false;
			visited.pop();
		}
	}
	else {
		actual->m_JaHePassat = true;
		visited.push(actual);
	}

	for (CEdge* e : actual->m_Edges) {
		if (e->m_Used)
			continue;

		if (e->m_pDestination->m_JaHePassat)
			continue;

		track.AddLast(e);

		if (lenShortest <= track.Length()) {
			track.Delete(e);
			continue;
		}		
		e->m_Used = true;

		if (e->m_pDestination == desti) {
			if (n == vis.m_Vertices.size()) {
				lenShortest = track.Length();
				best = track;
				goto backtrack;
			}
		}

		SalesmanTrackBacktrackingRec(e->m_pDestination);

	backtrack:
		track.Delete(e); 
		e->m_Used = false; 
	}

	if (counted) {
		n--;
		actual->m_PerVisitar = true;
	}
	actual->m_JaHePassat = false;
}


CTrack SalesmanTrackBacktracking(CGraph &graph, CVisits &visits)
{
	desti = visits.m_Vertices.back();
	CVertex* pInici = visits.m_Vertices.front();	
	vis = visits;
	vis.Delete(desti);
	vis.Delete(pInici);

	for (CVertex v : graph.m_Vertices) {
		v.m_JaHePassat = false;
	}

	for (CVertex* v : vis.m_Vertices) {
		v->m_PerVisitar = true;
		v->m_JaHePassat = false;
	}

	for (CEdge e : graph.m_Edges)
		e.m_Used = false;

	best = CTrack(&graph);
	track = CTrack(&graph);
	lenShortest = numeric_limits<double>::max();
	n = 0;

	SalesmanTrackBacktrackingRec(pInici);
	
	return best;
}


// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================

double lenCurrent;
double len;
int tableSize;

vector<int> indexes;
vector<int> bestIndexes;
vector<vector<double>> tableLength;

void SalesmanTrackBacktrackingGreedyRec(int pos)
{
	for (int i = 0; i < tableSize; i++) {
		len = tableLength[pos][i];
		if (lenCurrent + len >= lenShortest) {
			continue;
		}

		if (i == tableSize - 1 && indexes.size() != tableSize - 1)
			continue;

		for (int j : indexes) {
			if (i == j) {
				goto end;
			}
		}
		
		lenCurrent += len;
		indexes.push_back(i);

		if (i == tableSize - 1) {
			bestIndexes = indexes;
			lenShortest = lenCurrent;
			goto backtrackGreedy;
		}

		SalesmanTrackBacktrackingGreedyRec(i);

	backtrackGreedy:
		lenCurrent -= tableLength[pos][i];
		indexes.pop_back();
	end: ;
	}
}


CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	vector<vector<CTrack>> table;

	int n = 0;
	for (CVertex* v : visits.m_Vertices) {
		if (n == visits.m_Vertices.size() - 1)
			break;

		DijkstraQueue(graph, v);

		vector<CTrack> lista;
		vector<double> listaLength;
		for (CVertex* v2 : visits.m_Vertices) {
			CTrack t(&graph);
			CEdge* e = v2->m_pDijkstraPrevious;

			while (e != NULL) {
				t.AddFirst(e);
				e = e->m_pOrigin->m_pDijkstraPrevious;
			}
			lista.push_back(t);
			listaLength.push_back(v2->m_DijkstraDistance);
		}
		table.push_back(lista);
		tableLength.push_back(listaLength);
		n++;
	}

	tableSize = table.size() + 1;

	best = CTrack(&graph);
	lenShortest = numeric_limits<double>::max();
	lenCurrent = 0;
	indexes.push_back(0);

	SalesmanTrackBacktrackingGreedyRec(0);

	int j = 0;
	for (int i = 1; i < bestIndexes.size(); i++) {
		best.Append(table[bestIndexes[j]][bestIndexes[i]]);
		j++;
	}

	return best;
}
