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

// Class to represent a graph
class Graph {
private:
    int n; // Number of vertices
    vector<unordered_set<int>> adj; // Adjacency list using sets for faster lookups
    
    // Cache for cliques to avoid recalculation
    mutable vector<vector<int>> hCliquesCache;
    mutable vector<vector<int>> hMinus1CliquesCache;
    mutable vector<vector<int>> vertexToCliqueMap;
    mutable bool cacheInitialized = false;
    
    // Efficient check if vertex v is connected to all vertices in current
    bool isConnectedToAll(int v, const vector<int>& current) const {
        if (v < 0 || v >= n) return false;
        
        for (int u : current) {
            if (u < 0 || u >= n || adj[v].find(u) == adj[v].end()) {
                return false;
            }
        }
        return true;
    }

    // Non-recursive implementation of clique finding for h=3 (triangles)
    void findTriangles(vector<vector<int>>& cliques) const {
        cliques.clear();
        
        cout << "Finding triangles using optimized method... " << flush;
        int count = 0;
        
        // For each edge (u, v)
        for (int u = 0; u < n; u++) {
            // For each neighbor v of u
            for (int v : adj[u]) {
                if (v <= u) continue; // Process each edge once
                
                // For each neighbor w of u
                for (int w : adj[u]) {
                    if (w <= v) continue; // Ensure w > v > u to avoid duplicates
                    
                    // Check if w is also a neighbor of v
                    if (adj[v].find(w) != adj[v].end()) {
                        cliques.push_back({u, v, w});
                        
                        // Print progress
                        count++;
                        if (count % 10000 == 0) {
                            cout << "." << flush;
                        }
                    }
                }
            }
        }
        
        cout << " Found " << cliques.size() << " triangles." << endl;
    }
    
    // Efficient clique finder with iterative approach for small h
    void findCliquesIterative(int h, vector<vector<int>>& cliques) const {
        cliques.clear();
        
        // Special case for triangles (h=3)
        if (h == 3) {
            findTriangles(cliques);
            return;
        }
        
        cout << "Finding " << h << "-cliques using backtracking (limited search)... " << flush;
        
        // Set a limit for the search
        const size_t MAX_CLIQUES = 1000000; // Adjust based on your memory constraints
        size_t maxIterations = 100000000;   // Prevent excessive recursion
        size_t iterations = 0;
        
        // Use backtracking with limits
        function<void(vector<int>&, int)> backtrack = [&](vector<int>& current, int start) {
            iterations++;
            
            // Show progress
            if (iterations % 100000 == 0) {
                cout << "." << flush;
                if (iterations % 5000000 == 0) {
                    cout << " [" << iterations << " iterations, " << cliques.size() << " cliques]" << endl;
                }
            }
            
            // Limit reached
            if (cliques.size() >= MAX_CLIQUES || iterations >= maxIterations) {
                return;
            }
            
            if (current.size() == h) {
                cliques.push_back(current);
                return;
            }
            
            for (int i = start; i < n && cliques.size() < MAX_CLIQUES; i++) {
                if (isConnectedToAll(i, current)) {
                    current.push_back(i);
                    backtrack(current, i + 1);
                    current.pop_back();
                }
            }
        };
        
        vector<int> current;
        backtrack(current, 0);
        
        cout << " Found " << cliques.size() << " cliques" 
             << (cliques.size() >= MAX_CLIQUES ? " (limit reached)" : "") 
             << "." << endl;
    }
    
public:
    Graph(int vertices) : n(vertices) {
        if (vertices <= 0) {
            n = 0;
            cerr << "Warning: Invalid graph size. Creating empty graph." << endl;
        }
        adj.resize(n);
    }
    
    void addEdge(int u, int v) {
        if (u < 0 || u >= n || v < 0 || v >= n) {
            return; // Silently ignore invalid edges
        }
        adj[u].insert(v);
        adj[v].insert(u);
    }
    
    // Get the number of vertices
    int getVertexCount() const {
        return n;
    }
    
    // Check if edge exists
    bool hasEdge(int u, int v) const {
        if (u < 0 || u >= n || v < 0 || v >= n) return false;
        return adj[u].find(v) != adj[u].end();
    }
    
    // Initialize cache of h-cliques and their membership map
    void initializeCliqueCache(int h) const {
        if (cacheInitialized) return;
        
        cout << "Precomputing cliques for h=" << h << "..." << flush;
        auto start = chrono::high_resolution_clock::now();
        
        hCliquesCache.clear();
        hMinus1CliquesCache.clear();
        vertexToCliqueMap.resize(n);
        
        // Find cliques using iterative approach to avoid stack overflow
        try {
            findCliquesIterative(h, hCliquesCache);
            
            if (h > 1) {
                findCliquesIterative(h-1, hMinus1CliquesCache);
            }
            
            // Build mapping from vertices to cliques they belong to
            for (size_t i = 0; i < hCliquesCache.size(); i++) {
                for (int v : hCliquesCache[i]) {
                    if (v >= 0 && v < n) {
                        vertexToCliqueMap[v].push_back(i);
                    }
                }
            }
            
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            
            cout << " Done! Found " << hCliquesCache.size() << " h-cliques and " 
                 << hMinus1CliquesCache.size() << " (h-1)-cliques in " << duration << "ms" << endl;
            
            cacheInitialized = true;
        }
        catch (const exception& e) {
            cout << "Error in clique computation: " << e.what() << endl;
            // Return with empty caches
            hCliquesCache.clear();
            hMinus1CliquesCache.clear();
        }
    }
    
    // Get all h-cliques
    const vector<vector<int>>& getHCliques(int h) const {
        initializeCliqueCache(h);
        return hCliquesCache;
    }
    
    // Get all (h-1)-cliques
    const vector<vector<int>>& getHMinus1Cliques(int h) const {
        initializeCliqueCache(h);
        return hMinus1CliquesCache;
    }
    
    // Calculate clique degree of a vertex
    int cliqueDegree(int v, int h) const {
        if (v < 0 || v >= n) return 0;
        initializeCliqueCache(h);
        return vertexToCliqueMap[v].size();
    }
    
    // Find maximum clique degree
    int findMaxCliqueDegree(int h) const {
        initializeCliqueCache(h);
        
        int maxDegree = 0;
        for (int v = 0; v < n; v++) {
            maxDegree = max(maxDegree, static_cast<int>(vertexToCliqueMap[v].size()));
        }
        return maxDegree;
    }
    
    // Count h-cliques in the graph
    int countCliques(int h) const {
        initializeCliqueCache(h);
        return hCliquesCache.size();
    }
    
    // Calculate h-clique density
    double cliqueDensity(int h) const {
        int cliqueCount = countCliques(h);
        if (n == 0) return 0.0;
        return static_cast<double>(cliqueCount) / n;
    }
    
    // Get induced subgraph
    Graph getInducedSubgraph(const vector<int>& vertices) const {
        Graph subgraph(vertices.size());
        unordered_map<int, int> indexMap;
        
        for (size_t i = 0; i < vertices.size(); i++) {
            if (vertices[i] >= 0 && vertices[i] < n) {
                indexMap[vertices[i]] = i;
            }
        }
        
        for (size_t i = 0; i < vertices.size(); i++) {
            for (size_t j = i + 1; j < vertices.size(); j++) {
                int u = vertices[i];
                int v = vertices[j];
                if (u >= 0 && u < n && v >= 0 && v < n && hasEdge(u, v)) {
                    subgraph.addEdge(indexMap[u], indexMap[v]);
                }
            }
        }
        
        return subgraph;
    }
};

// Dinic's algorithm for maximum flow
int dinicMaxFlow(const vector<vector<int>>& capacity, int s, int t, vector<int>& minCut) {
    int n = capacity.size();
    if (s < 0 || s >= n || t < 0 || t >= n) {
        cerr << "Invalid source or sink in flow network" << endl;
        return 0;
    }
    
    vector<vector<int>> flow(n, vector<int>(n, 0));
    vector<int> level(n);
    vector<int> ptr(n);
    
    // BFS to create level graph
    auto bfs = [&]() -> bool {
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        
        queue<int> q;
        q.push(s);
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int v = 0; v < n; v++) {
                if (level[v] < 0 && capacity[u][v] > flow[u][v]) {
                    level[v] = level[u] + 1;
                    q.push(v);
                }
            }
        }
        
        return level[t] >= 0;
    };
    
    // DFS to find blocking flow
    function<int(int, int)> dfs = [&](int u, int flowCap) -> int {
        if (u == t) return flowCap;
        
        for (int& i = ptr[u]; i < n; i++) {
            int v = i;
            if (level[v] == level[u] + 1 && capacity[u][v] > flow[u][v]) {
                int bottleneck = dfs(v, min(flowCap, capacity[u][v] - flow[u][v]));
                if (bottleneck > 0) {
                    flow[u][v] += bottleneck;
                    flow[v][u] -= bottleneck;
                    return bottleneck;
                }
            }
        }
        
        return 0;
    };
    
    int maxFlow = 0;
    int dinicIterations = 0;
    cout << "Running Dinic's max-flow algorithm: " << flush;
    
    // Main Dinic's algorithm loop
    while (bfs()) {
        dinicIterations++;
        if (dinicIterations % 10 == 0) {
            cout << "." << flush;
        }
        
        fill(ptr.begin(), ptr.end(), 0);
        while (int pushed = dfs(s, numeric_limits<int>::max())) {
            maxFlow += pushed;
        }
    }
    
    cout << " Done!" << endl;
    
    // Find min-cut
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(s);
    visited[s] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (int v = 0; v < n; v++) {
            if (capacity[u][v] > flow[u][v] && !visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    
    minCut.clear();
    for (int i = 0; i < n; i++) {
        if (visited[i]) {
            minCut.push_back(i);
        }
    }
    
    return maxFlow;
}

// Find the Clique Densest Subgraph with memory optimizations
Graph findCliqueDenseSubgraph(const Graph& G, int h) {
    int n = G.getVertexCount();
    cout << "Analyzing graph with " << n << " vertices for " << h << "-clique densest subgraph" << endl;
    
    if (n <= 0) {
        cerr << "Empty graph, nothing to analyze." << endl;
        return G;
    }
    
    // Find the maximum clique degree to set upper bound
    cout << "Finding maximum " << h << "-clique degree... " << flush;
    int maxCliqueDegree = G.findMaxCliqueDegree(h);
    cout << "Max degree: " << maxCliqueDegree << endl;
    
    if (maxCliqueDegree == 0) {
        cout << "No " << h << "-cliques found in the graph. Try a smaller h value." << endl;
        return G;  // Return original graph if no h-cliques exist
    }
    
    // Cache all necessary cliques
    const auto& hCliques = G.getHCliques(h);
    const auto& hMinus1Cliques = G.getHMinus1Cliques(h);
    
    if (hCliques.empty() || (h > 1 && hMinus1Cliques.empty())) {
        cout << "Not enough cliques found for analysis." << endl;
        return G;
    }
    
    // Initialize binary search bounds
    double l = 0;
    double u = maxCliqueDegree;
    double precision = 1.0 / (n * n);  // Relaxed precision for large graphs
    
    vector<int> D; // Current densest subgraph
    vector<int> bestD; // Best subgraph found so far
    double bestDensity = 0;
    
    // Binary search for optimal density
    int iterCount = 0;
    cout << "Binary search progress: " << flush;
    
    // Limit binary search iterations for very large graphs
    const int MAX_ITERATIONS = 20;
    
    try {
        while (u - l >= precision && iterCount < MAX_ITERATIONS) {
            iterCount++;
            double progress = (u - l) / maxCliqueDegree * 100.0;
            
            cout << "\rBinary search: " << fixed << setprecision(1) << (100.0 - progress) << "% (α=" << l << ".." << u << ") " << flush;
            
            double alpha = (l + u) / 2;
            
            // Build sparse flow network
            cout << "\nBuilding flow network for α=" << alpha << "... " << flush;
            
            // Use bounds checking for sizes
            size_t numNodes = 1 + n + min(hMinus1Cliques.size(), (size_t)100000) + 1; // Limit cliques processed
            
            if (numNodes > 1000000) {
                cout << "Flow network too large (" << numNodes << " nodes). Using sampling approach." << endl;
                numNodes = 1000000;  // Limit network size
            }
            
            // Use sparse representation for large graphs
            vector<vector<pair<int, int>>> sparseCapacity(numNodes);
            
            int s = 0;
            int t = numNodes - 1;
            int vertexOffset = 1;
            int cliqueOffset = vertexOffset + n;
            
            // Add edges from s to vertices
            for (int v = 0; v < n; v++) {
                int cap = G.cliqueDegree(v, h);
                if (cap > 0) {
                    sparseCapacity[s].push_back({vertexOffset + v, cap});
                }
            }
            
            // Add edges from vertices to t
            for (int v = 0; v < n; v++) {
                sparseCapacity[vertexOffset + v].push_back({t, ceil(alpha * h)});
            }
            
            // Add edges from vertices to (h-1)-cliques and from (h-1)-cliques to vertices
            cout << "Building flow network edges... " << flush;
            
            // Always use sampling approach for large graphs
            const size_t MAX_CLIQUES_TO_PROCESS = min(hMinus1Cliques.size(), (size_t)10000);
            
            cout << "Processing " << MAX_CLIQUES_TO_PROCESS << " of " 
                 << hMinus1Cliques.size() << " cliques..." << endl;
            
            for (size_t i = 0; i < MAX_CLIQUES_TO_PROCESS && i < hMinus1Cliques.size(); i++) {
                if (cliqueOffset + i >= numNodes) break;  // Safety check
                
                const auto& clique = hMinus1Cliques[i];
                
                // Add edges from (h-1)-cliques to vertices
                for (int v : clique) {
                    if (v >= 0 && v < n && vertexOffset + v < numNodes) {
                        sparseCapacity[cliqueOffset + i].push_back({vertexOffset + v, numeric_limits<int>::max()});
                    }
                }
                
                // Add edges from vertices to (h-1)-cliques (sampling)
                for (int v = 0; v < n; v++) {
                    if (find(clique.begin(), clique.end(), v) != clique.end()) continue;
                    
                    bool canExtend = true;
                    for (int u : clique) {
                        if (!G.hasEdge(v, u)) {
                            canExtend = false;
                            break;
                        }
                    }
                    
                    if (canExtend && vertexOffset + v < numNodes && cliqueOffset + i < numNodes) {
                        sparseCapacity[vertexOffset + v].push_back({cliqueOffset + i, 1});
                    }
                }
            }
            
            // Use more memory-efficient approach for very large networks
            vector<vector<int>> capacity;
            
            if (numNodes > 100000) {
                cout << "Using sparse max-flow implementation for large network..." << endl;
                // Create a smaller network with only active nodes
                unordered_set<int> activeNodes;
                activeNodes.insert(s);
                activeNodes.insert(t);
                
                for (int u = 0; u < numNodes; u++) {
                    if (!sparseCapacity[u].empty()) {
                        activeNodes.insert(u);
                        for (const auto& edge : sparseCapacity[u]) {
                            activeNodes.insert(edge.first);
                        }
                    }
                }
                
                // Create mapping to compact indices
                unordered_map<int, int> nodeMap;
                int idx = 0;
                for (int node : activeNodes) {
                    nodeMap[node] = idx++;
                }
                
                // Create compact capacity matrix
                capacity.resize(idx, vector<int>(idx, 0));
                for (int u = 0; u < numNodes; u++) {
                    if (activeNodes.find(u) != activeNodes.end()) {
                        for (const auto& edge : sparseCapacity[u]) {
                            if (activeNodes.find(edge.first) != activeNodes.end()) {
                                capacity[nodeMap[u]][nodeMap[edge.first]] = edge.second;
                            }
                        }
                    }
                }
                
                // Update s and t to new indices
                s = nodeMap[s];
                t = nodeMap[t];
            } else {
                // Regular approach for smaller networks
                capacity.resize(numNodes, vector<int>(numNodes, 0));
                for (int u = 0; u < numNodes; u++) {
                    for (const auto& edge : sparseCapacity[u]) {
                        capacity[u][edge.first] = edge.second;
                    }
                }
            }
            
            // Free memory
            sparseCapacity.clear();
            
            // Find min-cut
            vector<int> minCut;
            dinicMaxFlow(capacity, s, t, minCut);

            
            // Free memory
            capacity.clear();
            
            if (minCut.size() <= 1) { // Only s is in the cut
                u = alpha;
                cout << "Cut contains only source. Reducing upper bound to " << u << endl;
            } else {
                l = alpha;
                
                // For large networks with remapped nodes, we need to convert back
                // This is a simplified version for the general case
                
                // Extract vertices from the cut (excluding s)
                D.clear();
                for (int node : minCut) {
                    if (node != s && node >= vertexOffset && node < cliqueOffset) {
                        int originalVertex = node - vertexOffset;
                        if (originalVertex >= 0 && originalVertex < n) {
                            D.push_back(originalVertex);
                        }
                    }
                }
                
                // Update best subgraph if this one is non-empty
                if (!D.empty()) {
                    // Compute actual density (optional for large graphs)
                    if (D.size() < 1000) {
                        Graph subgraph = G.getInducedSubgraph(D);
                        double density = subgraph.cliqueDensity(h);
                        if (density > bestDensity) {
                            bestDensity = density;
                            bestD = D;
                        }
                        cout << "Cut contains " << D.size() << " vertices with density " << density << ". Increasing lower bound to " << l << endl;
                    } else {
                        bestD = D;
                        cout << "Cut contains " << D.size() << " vertices. Increasing lower bound to " << l << endl;
                    }
                }
            }
        }
    }
    catch (const exception& e) {
        cout << "Error during binary search: " << e.what() << endl;
        cout << "Using best subgraph found so far..." << endl;
    }
    
    cout << "\nBinary search complete. Final density estimate: " << l << endl;
    
    // Return the best subgraph found
    if (!bestD.empty()) {
        return G.getInducedSubgraph(bestD);
    } else if (!D.empty()) {
        return G.getInducedSubgraph(D);
    } else {
        // If no non-trivial subgraph found, return original graph
        return G;
    }
}

int main(int argc, char** argv) {
    try {
        // Read input from file or stdin
        unordered_map<int, int> mp;
        ifstream inputFile;
        cout << "Reading input..." << endl;
        
        string filename = argv[1];
        inputFile.open(filename);
        
        if (!inputFile.is_open()) {
            cout << "File not found, reading from stdin..." << endl;
            
            // Create a file from stdin
            ofstream tempFile("temp_input.txt");
            string line;
            while (getline(cin, line)) {
                tempFile << line << endl;
            }
            tempFile.close();
            
            inputFile.open("temp_input.txt");
            if (!inputFile.is_open()) {
                cerr << "Error creating temporary file!" << endl;
                return 1;
            }
        }
        
        int n, m, h, waste;
        cin>>h;
        inputFile >> n >> m;
        
        // Input validation
        if (n <= 0 || m < 0 || h <= 0) {
            cerr << "Invalid input parameters: n=" << n << ", m=" << m << ", h=" << h << endl;
            cerr << "Require: n > 0, m >= 0, h > 0" << endl;
            return 1;
        }
        
        if (n > 1000000) {
            cerr << "Graph too large! Maximum supported size is 1,000,000 vertices." << endl;
            return 1;
        }
        
        cout << "Creating graph with " << n << " vertices and " << m << " edges..." << endl;
        Graph G(n);
        
        // Read edges with validation
        int invalidEdges = 0;
        int nodes = 0;
        for (int i = 0; i < m; i++) {
            int u, v;
            if (!(inputFile >> u >> v >> waste)) {
                cerr << "Error reading edge #" << i << endl;
                break;
            }
            if(mp.find(u) != mp.end()) 
                u = mp[u];
            else
                mp[u] = nodes++;
            if(mp.find(v) != mp.end())
                v = mp[v];
            else
                mp[v] = nodes++;
            u = mp[u];
            v = mp[v];
            // Check if vertices are valid
            if (u < 0 || u >= n || v < 0 || v >= n) {
                invalidEdges++;
                if (invalidEdges < 10) {
                    cerr << "Warning: Invalid edge (" << u << ", " << v << ")" << endl;
                }
                continue;
            }
            
            G.addEdge(u, v);
            
            // Print progress indicator for large inputs
           // if (m > 10000 && i % (m/100) == 0) {
             //   cout << "\rReading edges: " << (i*100/m) << "% complete" << flush;
            //}
        }
        inputFile.close();
        
        if (invalidEdges > 0) {
            cerr << "Warning: " << invalidEdges << " invalid edges were ignored" << endl;
        }
        
        if (m > 10000) cout << "\rReading edges: 100% complete" << endl;
        
        cout << "Original Graph has " << n << " vertices and " << m << " edges." << endl;
        
        // For very large graphs, limit h
       // if (n > 10000 && h > 3) {
         //   cout << "WARNING: Graph is very large (" << n << " vertices). Using h=3 instead of " << h << endl;
           // h = 3;
        //}
        
        cout << "Looking for " << h << "-clique densest subgraph..." << endl;
        
        // Start time tracking
        auto startTime = chrono::high_resolution_clock::now();
        
        // Find the clique-dense subgraph
        Graph D = findCliqueDenseSubgraph(G, h);
        
        // End time tracking
        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
        
        cout << "\nCompleted in " << duration << " seconds!" << endl;
        cout << "Clique-Dense Subgraph found with " << D.getVertexCount() << " vertices!" << endl;
        
        if (D.getVertexCount() < 10000) {
            cout << "Number of " << h << "-cliques in CDS: " << D.countCliques(h) << endl;
            cout << h << "-clique density of CDS: " << D.cliqueDensity(h) << endl;
        } else {
            cout << "Subgraph is large, skipping detailed clique analysis to save memory." << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    catch (...) {
        cerr << "Unknown error occurred" << endl;
    }
    
    return 0;
}
