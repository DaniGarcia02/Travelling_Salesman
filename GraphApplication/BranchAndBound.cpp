#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <vector>
#include <algorithm>
using namespace std;

// SalesmanTrackBranchAndBound1 ===================================================

struct Path {
	vector<int> indexes;
	int next;
	double length;
};

CTrack SalesmanTrackBranchAndBound1(CGraph& graph, CVisits& visits)
{
	struct comparator {
		bool operator()(Path& p1, Path& p2) {
			return p1.length >= p2.length;
		}
	};
	priority_queue<Path, std::vector<Path>, comparator> queue;

	vector<vector<CTrack>> table;
	vector<vector<double>> tableLength;

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
			
			t.length = v2->m_DijkstraDistance;

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
	
	int tableSize = table.size();
	int tableRowSize = tableSize + 1;

	CTrack greedySolution = SalesmanTrackGreedy(graph, visits);
	double lenMax = greedySolution.Length();
	Path solution;

	for (int i = 1; i < tableRowSize - 1; i++) {
		Path p;
		
		p.length = table[0][i].length;
		p.next = i;
		p.indexes.push_back(0);
		p.indexes.push_back(i);

		queue.push(p);
	}

	double length;
	while (!queue.empty()) {
		Path pActual = queue.top();
		queue.pop();

		for (int j = 0; j <= tableSize; j++) {
			length = tableLength[pActual.next][j];

			if (length + pActual.length >= lenMax) 
				continue;

			if (find(pActual.indexes.begin(), pActual.indexes.end(), j) != pActual.indexes.end()) 
				continue;

			pActual.indexes.push_back(j);
			pActual.length += length;
			pActual.next = j;

			if (j == n) {
				if (pActual.indexes.size() == tableRowSize) {
					solution = pActual;
					lenMax = pActual.length;
				}
				continue;
			}
			queue.push(pActual);

			pActual.indexes.pop_back();
			pActual.length -= length;
			pActual.next = pActual.indexes.back();
		}
	}

	CTrack s = CTrack(&graph);

	if (solution.indexes.empty())
		s = greedySolution;

	int j = 0;
	for (int i = 1; i < solution.indexes.size(); i++) {
		s.Append(table[solution.indexes[j]][solution.indexes[i]]);
		j++;
	}

	return s;
}



// SalesmanTrackBranchAndBound2 ===================================================


struct PathB2 {
	vector<int> indexes;
	vector<int> unexploredIndexes;
	int next;
	double upperBound;
	double lowerBound;
	double length;
};


CTrack SalesmanTrackBranchAndBound2(CGraph& graph, CVisits &visits)
{
	struct comparator {
		bool operator()(PathB2& p1, PathB2& p2) {
			return p1.lowerBound >= p2.lowerBound;
		}
	};
	priority_queue<PathB2, std::vector<PathB2>, comparator> queue;

	vector<vector<CTrack>> table;
	vector<vector<double>> tableLength;
	vector<int> tableIndexes;

	int n = 0;
	tableIndexes.push_back(n);
	for (CVertex* v : visits.m_Vertices) {
		if (n == visits.m_Vertices.size() - 1)
			break;

		DijkstraQueue(graph, v);

		vector<CTrack> lista;
		vector<double> listaLength;
		for (CVertex* v2 : visits.m_Vertices) {
			CTrack t(&graph);
			CEdge* e = v2->m_pDijkstraPrevious;

			t.length = v2->m_DijkstraDistance;

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
		tableIndexes.push_back(n);
	}

	int tableSize = table.size();
	int tableRowSize = tableSize + 1;

	double min;
	double max;
	vector<double> lowerBounds;
	vector<double> upperBounds;

	for (int i = 0; i < tableRowSize; i++) {
		min = numeric_limits<double>::max();
		max = 0;
		for (int j = 0; j < tableSize; j++) {
			if (i == j) {
				continue;
			}

			if (tableLength[j][i] < min) {
				min = tableLength[j][i];
			}
			if (tableLength[j][i] > max) {
				max = tableLength[j][i];
			}
		}
		upperBounds.push_back(max);
		lowerBounds.push_back(min);
	}

	CTrack greedySolution = SalesmanTrackGreedy(graph, visits);
	double bestUpperBound = greedySolution.Length();
	PathB2 solution;
	double lb;
	double ub;

	for (int i = 0; i < tableRowSize; i++) {
		PathB2 p;

		if (i == 0 || i == tableSize) {
			continue;
		}

		lb = 0;
		ub = 0;

		p.indexes.push_back(0);
		p.indexes.push_back(i);

		p.unexploredIndexes = tableIndexes;

		p.unexploredIndexes.erase(p.unexploredIndexes.begin());
		p.unexploredIndexes.erase(p.unexploredIndexes.begin() + i - 1);

		for (int j : p.unexploredIndexes) {
			lb += lowerBounds[j];
			ub += upperBounds[j];
		}

		p.next = i;
		p.length = table[0][i].length;
		p.lowerBound = lb + p.length;
		p.upperBound = ub + p.length;

		queue.push(p);
	}

	double length, prevLb, prevUb, pActualLength;
	while (!queue.empty()) {
		PathB2 pActual = queue.top();
		queue.pop();

		if (pActual.lowerBound == pActual.upperBound) {
			solution = pActual;
			break;
		}

		lb = 0;
		ub = 0;
		for (int i : pActual.unexploredIndexes) {
			lb += lowerBounds[i];
			ub += upperBounds[i];
		}
		pActualLength = pActual.length;

		for (int j = 0; j <= tableSize; j++) {
			if (find(pActual.indexes.begin(), pActual.indexes.end(), j) != pActual.indexes.end())
				continue;

			if (j == n && pActual.indexes.size() != n) {
				continue;
			}

			length = tableLength[pActual.next][j];

			lb -= lowerBounds[j];
			ub -= upperBounds[j];

			if (lb + length + pActualLength >= bestUpperBound + 0.000001) {
				lb += lowerBounds[j];
				ub += upperBounds[j];
				continue;
			}

			if (ub + length + pActualLength < bestUpperBound) {
				bestUpperBound = ub + length + pActual.length;
			}

			pActual.indexes.push_back(j);
			pActual.length += length;
			prevLb = pActual.lowerBound;
			prevUb = pActual.upperBound;
			pActual.lowerBound = pActual.length + lb;
			pActual.upperBound = pActual.length + ub;
			pActual.next = j;

			pActual.unexploredIndexes.erase(remove(pActual.unexploredIndexes.begin(), pActual.unexploredIndexes.end(), j), pActual.unexploredIndexes.end());
			queue.push(pActual);

			pActual.indexes.pop_back();
			pActual.unexploredIndexes.push_back(j);
			pActual.length -= length;
			pActual.lowerBound = prevLb;
			pActual.upperBound = prevUb;
			pActual.next = pActual.indexes.back();
			lb += lowerBounds[j];
			ub += upperBounds[j];
		}
	}

	CTrack s = CTrack(&graph);

	if (solution.indexes.empty())
		s = greedySolution;

	int j = 0;
	for (int i = 1; i < solution.indexes.size(); i++) {
		s.Append(table[solution.indexes[j]][solution.indexes[i]]);
		j++;
	}

	return s;
}

// SalesmanTrackBranchAndBound3 ===================================================


struct PathB3 {
	vector<int> indexes;
	vector<int> unexploredIndexes;
	double upperBound;
	double lowerBound;
	double length;
};


CTrack SalesmanTrackBranchAndBound3(CGraph& graph, CVisits& visits)
{
	struct comparator {
		bool operator()(PathB3& p1, PathB3& p2) {
			return p1.lowerBound >= p2.lowerBound;
		}
	};
	priority_queue<PathB3, std::vector<PathB3>, comparator> queue;

	vector<vector<CTrack>> table;
	vector<vector<double>> tableLength;
	vector<int> tableIndexes;

	int n = 0;
	tableIndexes.push_back(n);
	for (CVertex* v : visits.m_Vertices) {
		if (n == visits.m_Vertices.size() - 1)
			break;

		DijkstraQueue(graph, v);

		vector<CTrack> lista;
		vector<double> listaLength;
		for (CVertex* v2 : visits.m_Vertices) {
			CTrack t(&graph);
			CEdge* e = v2->m_pDijkstraPrevious;

			t.length = v2->m_DijkstraDistance;

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
		tableIndexes.push_back(n);
	}

	int tableSize = table.size();
	int tableRowSize = tableSize + 1;

	double min;
	double max;

	CTrack greedySolution = SalesmanTrackGreedy(graph, visits);
	double bestUpperBound = greedySolution.Length();
	PathB3 solution;
	double lb;
	double ub;

	for (int i = 0; i < tableRowSize; i++) {
		PathB3 p;

		if (i == 0 || i == tableSize) {
			continue;
		}

		lb = 0;
		ub = 0;

		p.indexes.push_back(0);
		p.indexes.push_back(i);

		p.unexploredIndexes = tableIndexes;

		p.unexploredIndexes.erase(p.unexploredIndexes.begin());

		for (int i : p.unexploredIndexes) {
			min = numeric_limits<double>::max();
			max = 0;
			for (int j : p.unexploredIndexes) {
				if (i == j || j == n || i == p.indexes.back() || (j == p.indexes.back() && i == n && p.indexes.size() != n)) {
					continue;
				}

				if (tableLength[j][i] < min) {
					min = tableLength[j][i];
				}
				if (tableLength[j][i] > max) {
					max = tableLength[j][i];
				}
			}
			
			if (min != numeric_limits<double>::max()) {
				lb += min;
				ub += max;
			}
		}

		p.length = table[0][i].length;
		p.lowerBound = lb + p.length;
		p.upperBound = ub + p.length;

		queue.push(p);
	}

	double length, prevLb, prevUb, pActualLength;
	while (!queue.empty()) {
		PathB3 pActual = queue.top();
		queue.pop();

		if (pActual.lowerBound == pActual.upperBound) {
			solution = pActual;
			break;
		}

		pActualLength = pActual.length;

		for (int j = 0; j <= tableSize; j++) {
			if (find(pActual.indexes.begin(), pActual.indexes.end(), j) != pActual.indexes.end())
				continue;

			if (j == n && pActual.indexes.size() != n) {
				continue;
			}

			length = tableLength[pActual.indexes.back()][j];

			lb = 0;
			ub = 0;
			for (int i : pActual.unexploredIndexes) {
				min = numeric_limits<double>::max();
				max = 0;
				for (int k : pActual.unexploredIndexes) {
					if (i == k || k == n || i == pActual.indexes.back() || (k == pActual.indexes.back() && i == n && pActual.indexes.size() != n)) {
						continue;
					}

					if (tableLength[k][i] < min) {
						min = tableLength[k][i];
					}
					if (tableLength[k][i] > max) {
						max = tableLength[k][i];
					}
				}

				if (min != numeric_limits<double>::max()) {
					lb += min;
					ub += max;
				}
			}

			if (lb + pActualLength >= bestUpperBound + 0.000001) {
				continue;
			}

			if (ub + pActualLength < bestUpperBound) {
				bestUpperBound = ub + length + pActual.length;
			}

			pActual.length += length;
			prevLb = pActual.lowerBound;
			prevUb = pActual.upperBound;
			pActual.lowerBound = pActual.length + lb;
			pActual.upperBound = pActual.length + ub;
			pActual.unexploredIndexes.erase(remove(pActual.unexploredIndexes.begin(), pActual.unexploredIndexes.end(), pActual.indexes.back()), pActual.unexploredIndexes.end());
			pActual.indexes.push_back(j);
			queue.push(pActual);

			pActual.indexes.pop_back();
			pActual.unexploredIndexes.push_back(pActual.indexes.back());
			pActual.length -= length;
			pActual.lowerBound = prevLb;
			pActual.upperBound = prevUb;
		}
	}
	
	vector<int> auxK, auxI;
	int vk, vi;
	if (solution.indexes.size() <= n && !solution.indexes.empty()) {
		for (int i : solution.unexploredIndexes) {
			min = numeric_limits<double>::max();
			max = 0;
			for (int k : solution.unexploredIndexes) {
				if (i == k || k == n || i == solution.indexes.back() || (k == solution.indexes.back() && i == n && solution.indexes.size() != n)) {
					continue;
				}

				if (tableLength[k][i] < min) {
					min = tableLength[k][i];
					vk = k;
					vi = i;
				}
				if (tableLength[k][i] > max) {
					max = tableLength[k][i];
					vk = k;
					vi = i;
				}
			}

			if (min != numeric_limits<double>::max()) {
				auxK.push_back(vk);
				auxI.push_back(vi);
			}
		}

		while (solution.indexes.size() <= n) {
			for (int k = 0; k < auxK.size(); k++) {
				if (solution.indexes.back() == auxK[k]) {
					solution.indexes.push_back(auxI[k]);
				}
			}
		}
	}
	
	CTrack s = CTrack(&graph);

	if (solution.indexes.empty())
		s = greedySolution;

	int j = 0;
	for (int i = 1; i < solution.indexes.size(); i++) {
		s.Append(table[solution.indexes[j]][solution.indexes[i]]);
		j++;
	}

	return s;
}

// SalesmanTrackBranchAndBound4 ===================================================


CTrack SalesmanTrackBranchAndBound4(CGraph& graph, CVisits& visits)
{
	return CTrack(&graph);
}
