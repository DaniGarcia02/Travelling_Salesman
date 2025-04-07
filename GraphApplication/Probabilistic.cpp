#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <random>
#include <chrono>

// SalesmanTrackProbabilistic ==================================================

CTrack SalesmanTrackProbabilistic(CGraph& graph, CVisits& visits)
{
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

	vector<int> nums;
	vector<int> bestPath;
	vector<int> newPath;
	double bestLength;
	double bestTotalLength = numeric_limits<double>::max();
	vector<int> bestTotalPath;

	int last = visits.m_Vertices.size() - 1;

	for (int i = 1; i < visits.m_Vertices.size() - 1; i++) {
		nums.push_back(i);
	}

	int numSize = nums.size();
	int size = numSize + 2;

	double newLength;
	int aux;
	bool upgrade;

	for (int x = 0; x < visits.m_Vertices.size() * 12000; x++) {

		// Initialize first random path
		bestPath.clear();
		bestLength = 0;

		random_shuffle(nums.begin(), nums.end());

		bestPath.push_back(0);
		for (int i = 0; i < numSize; i++) {
			bestPath.push_back(nums[i]);
		}
		bestPath.push_back(last);

		n = 0;
		for (int i = 1; i < size; i++) {
			bestLength += tableLength[bestPath[n]][bestPath[i]];
			n++;
		}

		// Gradient Descent
		upgrade = true;
		while (upgrade) {
			upgrade = false;
			newPath = bestPath;

			for (int j = 1; j < tableLength.size() - 1; j++) {
				for (int i = j + 1; i < tableLength.size(); i++) {
					aux = newPath[j];
					newPath[j] = newPath[i];
					newPath[i] = aux;

					n = 0;
					newLength = 0;
					for (int k = 1; k < size; k++) {
						newLength += tableLength[newPath[n]][newPath[k]];
						n++;
					}

					if (newLength < bestLength) {
						bestPath = newPath;
						bestLength = newLength;
						upgrade = true;
					}

					aux = newPath[j];
					newPath[j] = newPath[i];
					newPath[i] = aux;
				}
			}
		}

		if (bestLength < bestTotalLength) {
			bestTotalLength = bestLength;
			bestTotalPath = bestPath;
		}
	}

	CTrack s = CTrack(&graph);

	n = 0;
	for (int i = 1; i < bestTotalPath.size(); i++) {
		s.Append(table[bestTotalPath[n]][bestTotalPath[i]]);
		n++;
	}

	return s;
}
