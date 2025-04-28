// src/pages/Algorithm3.js
import React from 'react';
import CodeEditor from '../../components/CodeEditor';


const Algorithm3 = () => {
  const codeSnippet = `#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <list>
#include <vector>
#include <stack>
#include <algorithm>
#include <limits>
#include <chrono>
#include <cassert>

using namespace std;
using namespace std::chrono;

// ----- Graph I/O and Conversion Functions -----

bool isDirected(const unordered_map<int, list<int>>& adj) {
    for (const auto& pair : adj) {
        int u = pair.first;
        for (int v : pair.second) {
            auto it = adj.find(v);
            if (it == adj.end() || find(it->second.begin(), it->second.end(), u) == it->second.end())
                return true;
        }
    }
    return false;
}

void convertToUndirected(unordered_map<int, list<int>>& adj) {
    for (auto& pair : adj) {
        int u = pair.first;
        for (int v : pair.second) {
            auto it = adj.find(v);
            if (it != adj.end()) {
                if (find(it->second.begin(), it->second.end(), u) == it->second.end())
                    it->second.push_back(u);
            } else {
                adj[v] = list<int>{u};
            }
        }
    }
}

// ----- Global Data Structures for Clique Enumeration -----

vector<int> S;  // temporary array (unused placeholder)
vector<int> T;  // temporary array (unused placeholder)
stack<int> C;   // global stack for current clique

// Global statistics
int maxCliqueSize = 0;          // Largest clique size encountered
int totalMaximalCliques = 0;    // Total number of maximal cliques found
vector<int> cliqueSizeDistribution; // Distribution: index i -> count of maximal cliques of size i

// ----- Helper Functions for Maximality and Lexicographic Tests -----
//
// These implementations mimic the conditions in Lemmas 4–6 of the paper.
// They assume vertices are numbered in nondecreasing order of degree.

// Maximality test: Check that no vertex y (with y < candidate) outside the current clique
// is adjacent to every vertex in the clique.
bool checkMaximality(const stack<int>& cliqueStack, const vector<vector<int>>& neighborList, int candidate) {
    // Extract current clique from the stack into a vector.
    vector<int> clique;
    {
        stack<int> temp = cliqueStack;
        while (!temp.empty()) {
            clique.push_back(temp.top());
            temp.pop();
        }
        reverse(clique.begin(), clique.end());
    }
    int numVertices = neighborList.size();
    vector<bool> inClique(numVertices, false);
    for (int v : clique)
        inClique[v] = true;

    // For each neighbor y of candidate that is not in the clique, consider only y with y < candidate.
    for (int y : neighborList[candidate]) {
        if (inClique[y])
            continue; // already in clique
        if (y >= candidate)
            continue;
        // Check whether y is adjacent to every vertex in the current clique.
        bool adjacentToAll = true;
        for (int v : clique) {
            if (!binary_search(neighborList[v].begin(), neighborList[v].end(), y)) {
                adjacentToAll = false;
                break;
            }
        }
        if (adjacentToAll)
            return false; // Found a vertex y that could extend the clique.
    }
    return true;
}

// Lexicographic test: Ensure that no vertex j (with j < candidate) exists such that j is adjacent
// to every vertex in the current clique and to candidate. This helps avoid duplicate clique outputs.
bool checkLexico(const stack<int>& cliqueStack, const vector<vector<int>>& neighborList, int candidate) {
    // Extract current clique from the stack into a vector.
    vector<int> clique;
    {
        stack<int> temp = cliqueStack;
        while (!temp.empty()) {
            clique.push_back(temp.top());
            temp.pop();
        }
        reverse(clique.begin(), clique.end());
    }
    int numVertices = neighborList.size();
    vector<bool> inClique(numVertices, false);
    for (int v : clique)
        inClique[v] = true;

    // For every vertex j with index less than candidate and not in the clique:
    for (int j = 0; j < candidate; j++) {
        if (inClique[j])
            continue;
        // Candidate must be adjacent to j.
        if (!binary_search(neighborList[candidate].begin(), neighborList[candidate].end(), j))
            continue;
        bool adjacentToAll = true;
        for (int v : clique) {
            if (!binary_search(neighborList[v].begin(), neighborList[v].end(), j)) {
                adjacentToAll = false;
                break;
            }
        }
        if (adjacentToAll)
            return false; // There exists a vertex j < candidate that could have been added.
    }
    return true;
}

// ----- Helper Function: Check if the current clique is maximal -----
//
// A clique is maximal if no vertex outside the clique is adjacent to all vertices in the clique.
bool isMaximalClique(const stack<int>& cliqueStack, const vector<vector<int>>& neighborList, int numVertices) {
    vector<int> clique;
    {
        stack<int> temp = cliqueStack;
        while (!temp.empty()) {
            clique.push_back(temp.top());
            temp.pop();
        }
    }
    reverse(clique.begin(), clique.end());

    vector<bool> inClique(numVertices, false);
    for (int v : clique)
        inClique[v] = true;

    for (int v = 0; v < numVertices; v++) {
        if (!inClique[v]) {
            bool adjacentToAll = true;
            for (int u : clique) {
                if (!binary_search(neighborList[u].begin(), neighborList[u].end(), v)) {
                    adjacentToAll = false;
                    break;
                }
            }
            if (adjacentToAll)
                return false; // Found a vertex that can extend the clique.
        }
    }
    return true;
}

// ----- Clique Output Function -----
void outputClique(const stack<int>& cliqueStack) {
    vector<int> clique;
    {
        stack<int> temp = cliqueStack;
        while (!temp.empty()) {
            clique.push_back(temp.top());
            temp.pop();
        }
    }
    reverse(clique.begin(), clique.end());
    
    int cliqueSize = clique.size();
    maxCliqueSize = max(maxCliqueSize, cliqueSize);
    totalMaximalCliques++;
    cout << "\\nClique #" << totalMaximalCliques << ": ";
    for (int v : clique)
        cout << v << " ";
    cout << endl;
    
    if (cliqueSize >= (int)cliqueSizeDistribution.size())
        cliqueSizeDistribution.resize(cliqueSize + 1, 0);
    cliqueSizeDistribution[cliqueSize]++;
}

// ----- Vertex Deletion Function -----
//
// Deletes vertex 'v' from the graph represented by neighborList by removing it from all neighbor lists.
void deleteVertex(int v, vector<vector<int>>& neighborList) {
    int n = neighborList.size();
    for (int i = 0; i < n; i++) {
        auto& nbrs = neighborList[i];
        nbrs.erase(remove(nbrs.begin(), nbrs.end(), v), nbrs.end());
    }
    // Clear the neighbor list for vertex v to mark it as deleted.
    neighborList[v].clear();
}

// ----- Recursive UPDATE Procedure -----
// neighborList: for each vertex, its sorted list of neighbors (modifiable for vertex deletion)
// numVertices: total number of vertices
// i: current candidate vertex index
// cliqueStack: current clique (passed by reference)
void UPDATE(int i, vector<vector<int>>& neighborList, int numVertices, stack<int>& cliqueStack) {
    // Base case: if we've considered all vertices, check if the current clique is maximal.
    if (i >= numVertices) {
        if (!cliqueStack.empty() && isMaximalClique(cliqueStack, neighborList, numVertices))
            outputClique(cliqueStack);
        return;
    }
    
    // Save the current state of the neighborList for backtracking.
    vector<vector<int>> backupNeighborList = neighborList;
    
    // Check if candidate i is adjacent to every vertex already in the clique.
    bool canExtend = true;
    {
        stack<int> temp = cliqueStack;
        while (!temp.empty()) {
            int v = temp.top();
            temp.pop();
            if (!binary_search(neighborList[v].begin(), neighborList[v].end(), i)) {
                canExtend = false;
                break;
            }
        }
    }
    
    // Branch: Include candidate i if it can extend the clique.
    if (canExtend) {
        if (checkMaximality(cliqueStack, neighborList, i) &&
            checkLexico(cliqueStack, neighborList, i)) {
            cliqueStack.push(i);
            UPDATE(i + 1, neighborList, numVertices, cliqueStack);
            cliqueStack.pop();  // backtrack
        }
    }
    
    // Branch: Exclude candidate i.
    UPDATE(i + 1, neighborList, numVertices, cliqueStack);
    
    // Now perform the vertex deletion for candidate i.
    deleteVertex(i, neighborList);
    
    // Restore neighborList for backtracking (so that sibling recursive calls use the original state).
    neighborList = backupNeighborList;
}

// ----- Main Function -----
int main() {
    auto startTime = high_resolution_clock::now();
    
    bool fileIsDirected = false;  // Adjust if needed.
    
    int datasetId;
    cout << "Please select the dataset you want to use:" << endl;
    cout << "1. Wiki-Vote" << endl;
    cout << "2. Email-Enron" << endl;
    cout << "3. Skitter" << endl;

    cin >> datasetId;

    string filename;

    switch (datasetId) {
        case 1:
            cout << "Loading Wiki-Vote dataset..." << endl;
            filename = "Dataset/Wiki-Vote.txt";
            break;
        case 2:
            cout << "Loading Email-Enron dataset..." << endl;
            filename = "Dataset/Email-Enron.txt";
            break;
        case 3:
            cout << "Loading Skitter dataset..." << endl;
            filename = "Dataset/as-skitter.txt";
            break;
        default:
            cout << "Invalid dataset selection" << endl;
            return 1;
    }

    ifstream infile(filename);
    if (!infile) {
        cerr << "Error: Could not open the file 'datasets/graph.txt'" << endl;
        return 1;
    }
    
    string line;
    int numVertices, numEdges;
    if (getline(infile, line)) {
        istringstream iss(line);
        if (!(iss >> numVertices >> numEdges)) {
            cerr << "Error: First line must contain two integers." << endl;
            return 1;
        }
    } else {
        cerr << "Error: File is empty or improperly formatted." << endl;
        return 1;
    }
    
    vector<pair<int, int>> edges;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        int from, to;
        if (!(iss >> from >> to)) {
            cerr << "Error reading edge: " << line << endl;
            continue;
        }
        edges.push_back({from, to});
    }
    infile.close();
    
    int minVertex = numeric_limits<int>::max();
    for (const auto& edge : edges)
        minVertex = min({minVertex, edge.first, edge.second});
    bool needConversion = (minVertex >= 1);
    
    unordered_map<int, list<int>> adj;
    for (int i = 0; i < numVertices; i++)
        adj[i] = list<int>();
    
    for (const auto& edge : edges) {
        int u = edge.first, v = edge.second;
        if (needConversion) {
            u--; v--;
        }
        adj[u].push_back(v);
        if (!fileIsDirected) {
            if (find(adj[v].begin(), adj[v].end(), u) == adj[v].end())
                adj[v].push_back(u);
        }
    }
    
    if (fileIsDirected) {
        if (isDirected(adj)) {
            cout << "The graph is directed. Converting to undirected graph by adding missing reverse edges." << endl;
            convertToUndirected(adj);
        } else {
            cout << "The graph is already undirected." << endl;
        }
    } else {
        cout << "The file represents an undirected graph." << endl;
    }
    
    vector<pair<int, int>> degreeList;
    for (int i = 0; i < numVertices; i++)
        degreeList.push_back({(int)adj[i].size(), i});
    sort(degreeList.begin(), degreeList.end());
    
    cout << "\\nVertices sorted in nondecreasing order of degree:" << endl;
    for (const auto& p : degreeList)
        cout << "Vertex " << p.second << " with degree " << p.first << endl;
    
    vector<vector<int>> neighborList(numVertices);
    for (int i = 0; i < numVertices; i++) {
        for (int neighbor : adj[i])
            neighborList[i].push_back(neighbor);
        sort(neighborList[i].begin(), neighborList[i].end());
    }
    
    // Prepare auxiliary arrays (currently unused but available for further refinement)
    S.assign(numVertices, 0);
    T.assign(numVertices, 0);
    while (!C.empty())
        C.pop();
    
    cout << "\\nStarting clique enumeration (maximal cliques only)..." << endl;
    UPDATE(0, neighborList, numVertices, C);
    
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(endTime - startTime).count();
    
    cout << "\\n=== Clique Enumeration Statistics ===" << endl;
    cout << "Largest clique size found: " << maxCliqueSize << endl;
    cout << "Total number of maximal cliques: " << totalMaximalCliques << endl;
    cout << "Execution time: " << duration << " ms" << endl;
    
    cout << "\\nClique Size Distribution:" << endl;
    for (size_t size = 1; size < cliqueSizeDistribution.size(); size++) {
        if (cliqueSizeDistribution[size] > 0)
            cout << "Size " << size << ": " << cliqueSizeDistribution[size] << " clique(s)" << endl;
    }
    
    return 0;
}`;

  return (
    <div style={{ padding: '1rem' }}>
      <h1>Chiba</h1>
      
      <section>
        <h2>Overview</h2>
        <p>
        This project implements and evaluates the Clique Listing Algorithm based on arboricity, as proposed by Chiba and Nishizeki. The algorithm efficiently lists all maximal cliques in a graph with a worst-case time complexity of O(a(G) * m) per clique, where a(G) is the arboricity of the graph and m is the number of edges.
        </p>
      </section>
      
      <section>
        <h2>Implementation Details</h2>
        <ul>
        <li><strong>Graph Representation:</strong> Use adjacency lists for efficient traversal.</li>
        <li><strong>Vertex Ordering</strong> Sort vertices in non-decreasing degree order to optimize traversal.</li>
        <li><strong>Recursive Clique Listing:</strong>
            <ul>
            <li>Process vertices one by one.</li>
            <li>Expand cliques efficiently while ensuring no duplication.</li>
            <li>Use lexicographic filtering to avoid redundant clique generation.</li>
            </ul>
        </li>
        </ul>
        <h3>Code</h3>
        <CodeEditor code={codeSnippet} />
      </section>
      
      <section>
        <h2>Expectation</h2>
        <ul>
            Validation of O(a(G) * m) complexity on real-world networks.
        </ul>
      </section>
    </div>
  );
};

export default Algorithm3;
