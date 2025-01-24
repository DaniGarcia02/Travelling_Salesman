#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
using namespace std;

// =============================================================================
// SpanningTreePrim ============================================================
// =============================================================================


CSpanningTree SpanningTreePrim(CGraph& graph)
{
	struct comparator {
		bool operator()(CEdge* pE1, CEdge* pE2) {
			return pE1->m_Length > pE2->m_Length;
		}
	};
	priority_queue<CEdge*, std::vector<CEdge*>, comparator> queue;

	CSpanningTree tree(&graph);

	if (graph.m_Edges.empty())
		return tree;

	graph.m_Vertices.front().m_Pertany = true;

	for (CEdge* e : graph.m_Vertices.front().m_Edges) {
		if (!e->m_pDestination->m_Pertany)
			queue.push(e);
	}

	while (!queue.empty()) {
		CEdge* pE = queue.top();
		if (!pE->m_pDestination->m_Pertany) {
			tree.m_Edges.push_back(pE);
			pE->m_pDestination->m_Pertany = true;
			queue.pop();
			for (CEdge* e : pE->m_pDestination->m_Edges) {
				if (!e->m_pDestination->m_Pertany)
					queue.push(e);
			}
		}
		else
			queue.pop();
	}

	return tree;
}