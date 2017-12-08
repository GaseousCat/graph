#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <tuple>

using namespace std;

class Graph
{
private:
	char type;
	int N = 0;
	int M = 0;
	bool W;
	bool D;
	vector<vector<int>>  adjMatrix;
	vector<map<int, int>> adjList;
	vector<tuple<int, int, int>> listOfEdges;

public:
	Graph() {}
	Graph(char ntype, bool nD, bool nW, int nN, int nM) : type(ntype), D(nD), W(nW), N(nN), M(nM)
	{
		switch (type)
		{
		case 'C':
		{
			adjMatrix.resize(N);
			for (int i = 0; i < N; i++)
				adjMatrix[i].resize(N);
			break;
		}
		case 'L':
		{
			adjList.resize(N);
			break;
		}
		case 'E':
		{
			listOfEdges.resize(M);
			break;
		}
		}
	}
	Graph(int nN) :type('E'), D(0), W(1), N(nN), M(0) {}
	Graph(const Graph& graph) : type(graph.type), D(graph.D), W(graph.W), N(graph.N), M(graph.M), adjMatrix(graph.adjMatrix),
		adjList(graph.adjList), listOfEdges(graph.listOfEdges) {}
	~Graph() {}

	void readGraph(string fileName)
	{
		adjList.clear();
		adjMatrix.clear();
		listOfEdges.clear();
		ifstream inS(fileName);
		inS >> type;

		switch (type)
		{
		case 'C':
		{
			inS >> N >> D >> W;
			adjMatrix.resize(N);
			for (int i = 0; i < N; i++)
				adjMatrix[i].resize(N);

			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N; j++)
				{
					int w;
					inS >> w;
					w ? M++ : 1;
					adjMatrix[i][j] = w;
				}
			}
			if (!D)
				M /= 2;
			break;
		}
		case 'L':
		{
			inS >> N >> D >> W;
			adjList.resize(N);
			string s;
			getline(inS, s);

			for (int i = 0; i < N; i++)
			{
				getline(inS, s);
				istringstream iss(s);
				int b, w = 1;
				if (s.size() != 0)
				{
					while (iss && s.size() != 0)
					{
						iss >> b;
						if (W)
						{
							iss >> w;
						}
						adjList[i][b] = w;
					}
				}
				M += adjList[i].size();
			}
			break;
		}
		case 'E':
		{
			inS >> N >> M >> D >> W;
			listOfEdges.resize(M);

			for (int i = 0; i < M; i++)
			{
				int a, b, w = 1;
				inS >> a >> b;
				if (W)
				{
					inS >> w;
				}
				get<0>(listOfEdges[i]) = a;
				get<1>(listOfEdges[i]) = b;
				get<2>(listOfEdges[i]) = w;
			}
			break;
		}
		}

		inS.close();
	}
	void addEdge(int from, int to, int weight)
	{
		if (!D && from > to)
		{
			swap(from, to);
		}

		switch (type)
		{
		case 'C':
		{
			M++;
			adjMatrix[from - 1][to - 1] = weight;
			D ? 1 : adjMatrix[to - 1][from - 1] = weight;
			break;
		}
		case 'L':
		{
			M++;
			adjList[from - 1][to] = weight;
			D ? 1 : adjList[to - 1][from] = weight;
			break;
		}
		case 'E':
		{
			M++;
			listOfEdges.resize(M);
			get<0>(listOfEdges[M - 1]) = from;
			get<1>(listOfEdges[M - 1]) = to;
			get<2>(listOfEdges[M - 1]) = weight;
			break;
		}
		}
	}
	void removeEdge(int from, int to)
	{
		switch (type)
		{
		case 'C':
		{
			if (adjMatrix[from - 1][to - 1] != 0)
			{
				adjMatrix[from - 1][to - 1] = 0;
				D ? 1 : adjMatrix[to - 1][from - 1] = 0;
				M--;
			}
			break;
		}
		case 'L':
		{
			adjList[from - 1].erase(to);
			M--;
			D ? 1 : adjList[to - 1].erase(from);
			break;
		}
		case 'E':
		{
			if (!D && from > to)
			{
				swap(from, to);
			}

			auto edge = find_if(listOfEdges.begin(), listOfEdges.end(), [from, to](tuple<int, int, int>& edge)
			{
				return (std::get<0>(edge) == from) && (std::get<1>(edge) == to);
			});

			listOfEdges.erase(edge);
			M--;
			break;
		}
		}
	}
	int changeEdge(int from, int to, int newWeight)
	{
		int oldWeight = 0;
		switch (type)
		{
		case 'C':
		{
			oldWeight = adjMatrix[from - 1][to - 1];
			adjMatrix[from - 1][to - 1] = newWeight;
			D ? 1 : adjMatrix[to - 1][from - 1] = newWeight;
			break;
		}
		case 'L':
		{
			oldWeight = adjList[from - 1][to];
			adjList[from - 1][to] = newWeight;
			D ? 1 : adjList[to - 1][from] = newWeight;
			break;
		}
		case 'E':
		{
			if (!D && from > to)
			{
				swap(from, to);
			}

			auto edge = find_if(listOfEdges.begin(), listOfEdges.end(), [from, to](tuple<int, int, int>& edge)
			{
				return (get<0>(edge) == from) && (get<1>(edge) == to);
			});

			oldWeight = get<2>(*edge);
			get<2>(*edge) = newWeight;
			break;
		}
		}
		return oldWeight;
	}
	void transformToAdjMatrix()
	{
		switch (type)
		{
		case 'C':
			break;
		case 'L':
		{
			adjMatrix.resize(N);
			for (int i = 0; i < N; i++)
			{
				adjMatrix[i].resize(N);
			}

			for (int i = 0; i < N; i++)
			{
				for (auto it = adjList[i].begin(); it != adjList[i].end(); it++)
				{
					int b = it->first;
					int w = it->second;
					adjMatrix[i][b - 1] = w;
					D ? 1 : adjMatrix[b - 1][i] = w;
				}
			}

			adjList.clear();
			adjList.shrink_to_fit();
			type = 'C';
			break;
		}
		case 'E':
		{
			adjMatrix.resize(N);
			for (int i = 0; i < N; i++)
			{
				adjMatrix[i].resize(N);
			}

			for (int i = 0; i < M; i++)
			{
				int a = get<0>(listOfEdges[i]);
				int b = get<1>(listOfEdges[i]);
				int w = get<2>(listOfEdges[i]);

				adjMatrix[a - 1][b - 1] = w;
				D ? 1 : adjMatrix[b - 1][a - 1] = w;
			}

			listOfEdges.clear();
			listOfEdges.shrink_to_fit();
			type = 'C';
			break;
		}
		}
	}
	void transformToAdjList()
	{
		switch (type)
		{
		case 'C':
		{
			adjList.resize(N);
			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
				{
					if (adjMatrix[i][j] != 0)
					{
						adjList[i][j + 1] = adjMatrix[i][j];
						D ? 1 : adjList[j][i + 1] = adjMatrix[i][j];
					}
				}

			adjMatrix.clear();
			adjMatrix.shrink_to_fit();
			type = 'L';
			break;
		}
		case 'L':
			break;
		case 'E':
		{
			adjList.resize(N);

			for (int i = 0; i < M; i++)
			{
				int a = get<0>(listOfEdges[i]);
				int b = get<1>(listOfEdges[i]);
				int w = get<2>(listOfEdges[i]);

				adjList[a - 1][b] = w;
				D ? 1 : adjList[b - 1][a] = w;
			}

			listOfEdges.clear();
			listOfEdges.shrink_to_fit();
			type = 'L';
			break;
		}
		}
	}
	void transformToListOfEdges()
	{
		switch (type)
		{
		case 'C':
		{
			listOfEdges.resize(M);
			int k = 0;

			for (int i = 0; i < N; i++)
				for (int j = 0; j < N; j++)
				{
					if (adjMatrix[i][j] != 0)
					{
						if (!D && i < j || D)
						{
							get<0>(listOfEdges[k]) = i + 1;
							get<1>(listOfEdges[k]) = j + 1;
							get<2>(listOfEdges[k]) = adjMatrix[i][j];
							k++;
						}
					}
				}

			adjMatrix.clear();
			adjMatrix.shrink_to_fit();
			type = 'E';
			break;
		}
		case 'L':
		{
			listOfEdges.resize(M);
			int k = 0;

			for (int i = 0; i < N; i++)
			{
				for (auto it = adjList[i].begin(); it != adjList[i].end(); it++)
				{
					int b = it->first;
					int w = it->second;
					if (!D && i < b || D)
					{
						get<0>(listOfEdges[k]) = i + 1;
						get<1>(listOfEdges[k]) = b;
						get<2>(listOfEdges[k]) = w;
						k++;
					}
				}
			}

			adjList.clear();
			adjList.shrink_to_fit();
			type = 'E';
			break;
		}
		case 'E':
			break;
		}
	}
	void writeGraph(string fileName)
	{
		ofstream outS(fileName);

		switch (type)
		{
		case 'C':
		{
			outS << type << ' ' << N << endl << D << ' ' << W << endl;

			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < N - 1; j++)
				{
					outS << adjMatrix[i][j] << ' ';
				}
				outS << adjMatrix[i][N - 1] << endl;
			}
			break;
		}
		case 'L':
		{
			outS << type << ' ' << N << endl << D << ' ' << W << endl;

			for (int i = 0; i < N; i++)
			{
				int adjListSize = adjList[i].size();
				int k = 0;

				for (auto it = adjList[i].begin(); it != adjList[i].end(); it++)
				{
					if (k == adjListSize - 1)
					{
						outS << it->first;
						if (W)
						{
							outS << ' ' << it->second;
						}
					}
					else
					{
						outS << it->first << ' ';
						if (W)
							outS << it->second << ' ';
					}
					k++;
				}
				outS << endl;
			}
			break;
		}
		case 'E':
		{
			outS << type << ' ' << N << ' ' << M << endl << D << ' ' << W << endl;

			for (int i = 0; i < M; i++)
			{
				outS << get<0>(listOfEdges[i]) << ' ' << get<1>(listOfEdges[i]);
				if (W)
				{
					outS << ' ' << get<2>(listOfEdges[i]);
				}
				outS << endl;
			}
			break;
		}
		}

		outS.close();
	}
};
