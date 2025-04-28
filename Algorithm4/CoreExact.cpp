#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <limits>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <set>
#include <functional>

using namespace std;

// Structure representing a graph for 
class GraphStructure {
private:
    int vertexCount; // Total vertices
    vector<unordered_set<int>> adjacencyList; // Adjacency list for edge storage
    mutable vector<vector<int>> cliqueCacheH; // Cache for h-cliques
    mutable vector<vector<int>> cliqueCacheHMinus1; // Cache for (h-1)-cliques
    mutable vector<vector<int>> vertexCliqueMapping; // Maps vertices to cliques
    mutable bool isCacheReady = false;

    // Check if vertex is connected to all in the given set
    bool hasConnectionsToAll(int vertex, const vector<int>& group) const {
        if (vertex < 0 || vertex >= vertexCount) return false;
        int idx = 0;
        while (idx < group.size()) {
            int u = group[idx];
            if (u < 0 || u >= vertexCount || adjacencyList[vertex].count(u) == 0) {
                return false;
            }
            idx++;
        }
        return true;
    }

    // Iterative triangle detection for h=3
    void detectTriangles(vector<vector<int>>& resultCliques) const {
        resultCliques.clear();
        cout << "Detecting triangles efficiently... " << flush;
        int triangleCount = 0;

        int u = 0;
        while (u < vertexCount) {
            for (int v : adjacencyList[u]) {
                if (v <= u) continue;
                for (int w : adjacencyList[u]) {
                    if (w <= v) continue;
                    if (adjacencyList[v].count(w)) {
                        resultCliques.push_back({u, v, w});
                        triangleCount++;
                        if (triangleCount % 10000 == 0) {
                            cout << "*" << flush;
                        }
                    }
                }
            }
            u++;
        }
        cout << " Identified " << resultCliques.size() << " triangles." << endl;
    }

    // Iterative clique detection for small h values
    void detectCliques(int h, vector<vector<int>>& resultCliques) const {
        resultCliques.clear();
        if (h == 3) {
            detectTriangles(resultCliques);
            return;
        }

        cout << " Searching for " << h << "-cliques with iterative backtracking... " << flush;
        const size_t CLIQUE_LIMIT = 1000000;
        size_t iterationLimit = 100000000;
        size_t currentIteration = 0;

        function<void(vector<int>&, int)> exploreCliques = [&](vector<int>& currentClique, int startVertex) {
            currentIteration++;
            if (currentIteration % 100000 == 0) {
                cout << "*" << flush;
                if (currentIteration % 5000000 == 0) {
                    cout << " [Iteration " << currentIteration << ", " << resultCliques.size() << " cliques]" << endl;
                }
            }

            if (resultCliques.size() >= CLIQUE_LIMIT || currentIteration >= iterationLimit) {
                return;
            }

            if (currentClique.size() == h) {
                resultCliques.push_back(currentClique);
                return;
            }

            int i = startVertex;
            while (i < vertexCount && resultCliques.size() < CLIQUE_LIMIT) {
                if (hasConnectionsToAll(i, currentClique)) {
                    currentClique.push_back(i);
                    exploreCliques(currentClique, i + 1);
                    currentClique.pop_back();
                }
                i++;
            }
        };

        vector<int> currentClique;
        exploreCliques(currentClique, 0);
        cout << " Located " << resultCliques.size() << " cliques"
             << (resultCliques.size() >= CLIQUE_LIMIT ? " (limit hit)" : "") << "." << endl;
    }

public:
    GraphStructure(int vertices) : vertexCount(vertices) {
        if (vertices <= 0) {
            vertexCount = 0;
            cerr << "Warning: Invalid graph size detected. Initializing empty graph." << endl;
        }
        adjacencyList.resize(vertexCount);
    }

    void insertEdge(int u, int v) {
        if (u < 0 || u >= vertexCount || v < 0 || v >= vertexCount) return;
        adjacencyList[u].insert(v);
        adjacencyList[v].insert(u);
    }

    int getTotalVertices() const { return vertexCount; }

    bool edgeExists(int u, int v) const {
        if (u < 0 || u >= vertexCount || v < 0 || v >= vertexCount) return false;
        return adjacencyList[u].count(v) > 0;
    }

    void prepareCliqueCache(int h) const {
        if (isCacheReady) return;
        cout << "Preparing clique cache for h=" << h << "... " << flush;
        auto startTime = chrono::high_resolution_clock::now();

        cliqueCacheH.clear();
        cliqueCacheHMinus1.clear();
        vertexCliqueMapping.resize(vertexCount);

        try {
            detectCliques(h, cliqueCacheH);
            if (h > 1) {
                detectCliques(h - 1, cliqueCacheHMinus1);
            }

            size_t cliqueIdx = 0;
            while (cliqueIdx < cliqueCacheH.size()) {
                for (int v : cliqueCacheH[cliqueIdx]) {
                    if (v >= 0 && v < vertexCount) {
                        vertexCliqueMapping[v].push_back(cliqueIdx);
                    }
                }
                cliqueIdx++;
            }

            auto endTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
            cout << " Completed! Discovered " << cliqueCacheH.size() << " h-cliques and "
                 << cliqueCacheHMinus1.size() << " (h-1)-cliques in " << duration << "ms." << endl;
            isCacheReady = true;
        } catch (const exception& e) {
            cout << "Error during clique cache preparation: " << e.what() << endl;
            cliqueCacheH.clear();
            cliqueCacheHMinus1.clear();
        }
    }

    const vector<vector<int>>& fetchHCliques(int h) const {
        prepareCliqueCache(h);
        return cliqueCacheH;
    }

    const vector<vector<int>>& fetchHMinus1Cliques(int h) const {
        prepareCliqueCache(h);
        return cliqueCacheHMinus1;
    }

    int getCliqueDegree(int v, int h) const {
        if (v < 0 || v >= vertexCount) return 0;
        prepareCliqueCache(h);
        return vertexCliqueMapping[v].size();
    }

    int computeMaxCliqueDegree(int h) const {
        prepareCliqueCache(h);
        int maxDeg = 0;
        int v = 0;
        while (v < vertexCount) {
            maxDeg = max(maxDeg, static_cast<int>(vertexCliqueMapping[v].size()));
            v++;
        }
        return maxDeg;
    }

    int countHCliques(int h) const {
        prepareCliqueCache(h);
        return cliqueCacheH.size();
    }

    double computeCliqueDensity(int h) const {
        int cliqueCount = countHCliques(h);
        if (vertexCount == 0) return 0.0;
        return static_cast<double>(cliqueCount) / vertexCount;
    }

    GraphStructure extractSubgraph(const vector<int>& vertices) const {
        GraphStructure subgraph(vertices.size());
        unordered_map<int, int> vertexIndexMap;
        size_t i = 0;
        while (i < vertices.size()) {
            if (vertices[i] >= 0 && vertices[i] < vertexCount) {
                vertexIndexMap[vertices[i]] = i;
            }
            i++;
        }

        i = 0;
        while (i < vertices.size()) {
            size_t j = i + 1;
            while (j < vertices.size()) {
                int u = vertices[i], v = vertices[j];
                if (u >= 0 && u < vertexCount && v >= 0 && v < vertexCount && edgeExists(u, v)) {
                    subgraph.insertEdge(vertexIndexMap[u], vertexIndexMap[v]);
                }
                j++;
            }
            i++;
        }
        return subgraph;
    }
};

// Dinic's max-flow algorithm
int computeDinicFlow(const vector<vector<int>>& capacities, int source, int sink, vector<int>& cutVertices) {
    int nodeCount = capacities.size();
    if (source < 0 || source >= nodeCount || sink < 0 || sink >= nodeCount) {
        cerr << "Invalid source/sink in flow computation." << endl;
        return 0;
    }

    vector<vector<int>> flow(nodeCount, vector<int>(nodeCount, 0));
    vector<int> levels(nodeCount);
    vector<int> nextEdge(nodeCount);

    auto buildLevelGraph = [&]() -> bool {
        fill(levels.begin(), levels.end(), -1);
        levels[source] = 0;
        queue<int> q;
        q.push(source);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            int v = 0;
            while (v < nodeCount) {
                if (levels[v] < 0 && capacities[u][v] > flow[u][v]) {
                    levels[v] = levels[u] + 1;
                    q.push(v);
                }
                v++;
            }
        }
        return levels[sink] >= 0;
    };

    function<int(int, int)> findBlockingFlow = [&](int u, int flowCap) -> int {
        if (u == sink) return flowCap;
        int& i = nextEdge[u];
        while (i < nodeCount) {
            int v = i;
            if (levels[v] == levels[u] + 1 && capacities[u][v] > flow[u][v]) {
                int bottleneck = findBlockingFlow(v, min(flowCap, capacities[u][v] - flow[u][v]));
                if (bottleneck > 0) {
                    flow[u][v] += bottleneck;
                    flow[v][u] -= bottleneck;
                    return bottleneck;
                }
            }
            i++;
        }
        return 0;
    };

    int totalFlow = 0;
    int iterationCount = 0;
    cout << "Executing Dinic's flow algorithm: " << flush;

    while (buildLevelGraph()) {
        iterationCount++;
        if (iterationCount % 10 == 0) {
            cout << "*" << flush;
        }
        fill(nextEdge.begin(), nextEdge.end(), 0);
        int pushed;
        while ((pushed = findBlockingFlow(source, numeric_limits<int>::max())) > 0) {
            totalFlow += pushed;
        }
    }
    cout << " Finished!" << endl;

    vector<bool> visited(nodeCount, false);
    queue<int> q;
    q.push(source);
    visited[source] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        int v = 0;
        while (v < nodeCount) {
            if (capacities[u][v] > flow[u][v] && !visited[v]) {
                visited[v] = true;
                q.push(v);
            }
            v++;
        }
    }

    cutVertices.clear();
    int i = 0;
    while (i < nodeCount) {
        if (visited[i]) {
            cutVertices.push_back(i);
        }
        i++;
    }
    return totalFlow;
}

// ptimized clique-densest subgraph detection
GraphStructure findOptimalCliqueSubgraph(const GraphStructure& graph, int h) {
    int n = graph.getTotalVertices();
    cout << " Processing graph with " << n << " vertices for " << h << "-clique densest subgraph." << endl;

    if (n <= 0) {
        cerr << "Empty graph provided." << endl;
        return graph;
    }

    cout << "Computing maximum " << h << "-clique degree... " << flush;
    int maxDegree = graph.computeMaxCliqueDegree(h);
    cout << "Max degree: " << maxDegree << endl;

    if (maxDegree == 0) {
        cout << "No " << h << "-cliques detected. Consider reducing h." << endl;
        return graph;
    }

    const auto& hCliques = graph.fetchHCliques(h);
    const auto& hMinus1Cliques = graph.fetchHMinus1Cliques(h);

    if (hCliques.empty() || (h > 1 && hMinus1Cliques.empty())) {
        cout << "Insufficient cliques for analysis." << endl;
        return graph;
    }

    double lowerBound = 0, upperBound = maxDegree;
    double precisionThreshold = 1.0 / (n * n);
    vector<int> currentSubgraph, optimalSubgraph;
    double optimalDensity = 0;
    int iterationCount = 0;
    const int MAX_BINARY_SEARCH = 20;

    cout << " Initiating binary search: " << flush;

    try {
        while (upperBound - lowerBound >= precisionThreshold && iterationCount < MAX_BINARY_SEARCH) {
            iterationCount++;
            double progress = (upperBound - lowerBound) / maxDegree * 100.0;
            cout << "\r4: Binary search progress: " << fixed << setprecision(1)
                 << (100.0 - progress) << "% (α=" << lowerBound << ".." << upperBound << ") " << flush;

            double alpha = (lowerBound + upperBound) / 2;
            cout << "\n4: Constructing flow network for α=" << alpha << "... " << flush;

            size_t nodeCount = 1 + n + min(hMinus1Cliques.size(), (size_t)100000) + 1;
            if (nodeCount > 1000000) {
                cout << "Network too large (" << nodeCount << " nodes). Applying sampling." << endl;
                nodeCount = 1000000;
            }

            vector<vector<pair<int, int>>> sparseNetwork(nodeCount);
            int source = 0, sink = nodeCount - 1, vertexStart = 1, cliqueStart = vertexStart + n;

            int v = 0;
            while (v < n) {
                int cap = graph.getCliqueDegree(v, h);
                if (cap > 0) {
                    sparseNetwork[source].push_back({vertexStart + v, cap});
                }
                v++;
            }

            v = 0;
            while (v < n) {
                sparseNetwork[vertexStart + v].push_back({sink, ceil(alpha * h)});
                v++;
            }

            cout << "Adding flow network edges... " << flush;
            const size_t CLIQUE_PROCESS_LIMIT = min(hMinus1Cliques.size(), (size_t)10000);
            cout << "Processing " << CLIQUE_PROCESS_LIMIT << " of " << hMinus1Cliques.size() << " cliques..." << endl;

            size_t i = 0;
            while (i < CLIQUE_PROCESS_LIMIT && i < hMinus1Cliques.size()) {
                if (cliqueStart + i >= nodeCount) break;
                const auto& clique = hMinus1Cliques[i];
                for (int v : clique) {
                    if (v >= 0 && v < n && vertexStart + v < nodeCount) {
                        sparseNetwork[cliqueStart + i].push_back({vertexStart + v, numeric_limits<int>::max()});
                    }
                }

                int v = 0;
                while (v < n) {
                    if (find(clique.begin(), clique.end(), v) != clique.end()) {
                        v++;
                        continue;
                    }
                    bool extendable = true;
                    for (int u : clique) {
                        if (!graph.edgeExists(v, u)) {
                            extendable = false;
                            break;
                        }
                    }
                    if (extendable && vertexStart + v < nodeCount && cliqueStart + i < nodeCount) {
                        sparseNetwork[vertexStart + v].push_back({cliqueStart + i, 1});
                    }
                    v++;
                }
                i++;
            }

            vector<vector<int>> capacityMatrix;
            if (nodeCount > 100000) {
                cout << "Using sparse flow computation for large network..." << endl;
                unordered_set<int> activeNodes;
                activeNodes.insert(source);
                activeNodes.insert(sink);

                int u = 0;
                while (u < nodeCount) {
                    if (!sparseNetwork[u].empty()) {
                        activeNodes.insert(u);
                        for (const auto& edge : sparseNetwork[u]) {
                            activeNodes.insert(edge.first);
                        }
                    }
                    u++;
                }

                unordered_map<int, int> nodeMapping;
                int idx = 0;
                for (int node : activeNodes) {
                    nodeMapping[node] = idx++;
                }

                capacityMatrix.resize(idx, vector<int>(idx, 0));
                u = 0;
                while (u < nodeCount) {
                    if (activeNodes.count(u)) {
                        for (const auto& edge : sparseNetwork[u]) {
                            if (activeNodes.count(edge.first)) {
                                capacityMatrix[nodeMapping[u]][nodeMapping[edge.first]] = edge.second;
                            }
                        }
                    }
                    u++;
                }
                source = nodeMapping[source];
                sink = nodeMapping[sink];
            } else {
                capacityMatrix.resize(nodeCount, vector<int>(nodeCount, 0));
                int u = 0;
                while (u < nodeCount) {
                    for (const auto& edge : sparseNetwork[u]) {
                        capacityMatrix[u][edge.first] = edge.second;
                    }
                    u++;
                }
            }

            sparseNetwork.clear();
            vector<int> minCut;
            computeDinicFlow(capacityMatrix, source, sink, minCut);
            capacityMatrix.clear();

            if (minCut.size() <= 1) {
                upperBound = alpha;
                cout << "Cut only includes source. Adjusting upper bound to " << upperBound << endl;
            } else {
                lowerBound = alpha;
                currentSubgraph.clear();
                for (int node : minCut) {
                    if (node != source && node >= vertexStart && node < cliqueStart) {
                        int origVertex = node - vertexStart;
                        if (origVertex >= 0 && origVertex < n) {
                            currentSubgraph.push_back(origVertex);
                        }
                    }
                }

                if (!currentSubgraph.empty()) {
                    if (currentSubgraph.size() < 1000) {
                        GraphStructure subgraph = graph.extractSubgraph(currentSubgraph);
                        double density = subgraph.computeCliqueDensity(h);
                        if (density > optimalDensity) {
                            optimalDensity = density;
                            optimalSubgraph = currentSubgraph;
                        }
                        cout << "Cut has " << currentSubgraph.size() << " vertices, density " << density
                             << ". Raising lower bound to " << lowerBound << endl;
                    } else {
                        optimalSubgraph = currentSubgraph;
                        cout << "Cut has " << currentSubgraph.size() << " vertices. Raising lower bound to "
                             << lowerBound << endl;
                    }
                }
            }
        }
    } catch (const exception& e) {
        cout << "Error in binary search: " << e.what() << endl;
        cout << "Falling back to best subgraph found." << endl;
    }

    cout << "\nBinary search concluded. Estimated density: " << lowerBound << endl;

    if (!optimalSubgraph.empty()) {
        return graph.extractSubgraph(optimalSubgraph);
    } else if (!currentSubgraph.empty()) {
        return graph.extractSubgraph(currentSubgraph);
    }
    return graph;
}

int main(int argc, char** argv) {
    try {
        cout << "Loading graph data..." << endl;
        unordered_map<int, int> vertexMapping;
        ifstream inputStream;
        string inputFile = argv[1];
        inputStream.open(inputFile);

        if (!inputStream.is_open()) {
            cout << "File not accessible, reading from standard input..." << endl;
            ofstream tempFile("temp_graph.txt");
            string line;
            while (getline(cin, line)) {
                tempFile << line << endl;
            }
            tempFile.close();
            inputStream.open("temp_graph.txt");
            if (!inputStream.is_open()) {
                cerr << "Failed to create temporary file!" << endl;
                return 1;
            }
        }

        int vertexCount, edgeCount, cliqueSize, dummy;
        cin >> cliqueSize;
        inputStream >> vertexCount >> edgeCount;

        if (vertexCount <= 0 || edgeCount < 0 || cliqueSize <= 0) {
            cerr << "Invalid parameters: vertices=" << vertexCount << ", edges=" << edgeCount
                 << ", cliqueSize=" << cliqueSize << endl;
            return 1;
        }

        if (vertexCount > 1000000) {
            cerr << "Graph exceeds size limit of 1,000,000 vertices." << endl;
            return 1;
        }

        cout << "Initializing graph with " << vertexCount << " vertices and " << edgeCount
             << " edges..." << endl;
        GraphStructure graph(vertexCount);

        int invalidEdgeCount = 0, nodeIndex = 0;
        int i = 0;
        while (i < edgeCount) {
            int u, v;
            if (!(inputStream >> u >> v >> dummy)) {
                cerr << "Error reading edge #" << i << endl;
                break;
            }
            if (vertexMapping.count(u)) {
                u = vertexMapping[u];
            } else {
                vertexMapping[u] = nodeIndex++;
            }
            if (vertexMapping.count(v)) {
                v = vertexMapping[v];
            } else {
                vertexMapping[v] = nodeIndex++;
            }
            u = vertexMapping[u];
            v = vertexMapping[v];

            if (u < 0 || u >= vertexCount || v < 0 || v >= vertexCount) {
                invalidEdgeCount++;
                if (invalidEdgeCount < 10) {
                    cerr << "Warning: Invalid edge (" << u << ", " << v << ")" << endl;
                }
            } else {
                graph.insertEdge(u, v);
            }
            i++;
        }
        inputStream.close();

        if (invalidEdgeCount > 0) {
            cerr << "Warning: Ignored " << invalidEdgeCount << " invalid edges." << endl;
        }

        cout << "Graph loaded: " << vertexCount << " vertices, " << edgeCount << " edges." << endl;
        cout << "Searching for " << cliqueSize << "-clique densest subgraph..." << endl;

        auto startTime = chrono::high_resolution_clock::now();
        GraphStructure denseSubgraph = findOptimalCliqueSubgraph(graph, cliqueSize);
        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();

        cout << "\nExecution completed in " << duration << " seconds." << endl;
        cout << " Found clique-dense subgraph with " << denseSubgraph.getTotalVertices()
             << " vertices." << endl;

        if (denseSubgraph.getTotalVertices() < 10000) {
            cout <<  cliqueSize << "-clique count: " << denseSubgraph.countHCliques(cliqueSize)
                 << endl;
            cout <<  cliqueSize << "-clique density: "
                 << denseSubgraph.computeCliqueDensity(cliqueSize) << endl;
        } else {
            cout << "Large subgraph detected, omitting detailed clique analysis." << endl;
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    } catch (...) {
        cerr << "Unexpected error occurred." << endl;
    }
    return 0;
}
