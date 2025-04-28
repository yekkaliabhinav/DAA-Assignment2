// src/pages/AlgorithmPage.js
import React from 'react';
import { useParams } from 'react-router-dom';
import CodeEditor from '../components/CodeEditor';

const AlgorithmPage = () => {
  const { id: algoId } = useParams();

  // Dummy code snippet for demonstration
  const codeSnippet = `// Sample code for Algorithm ${algoId}\nfunction expand() {\n  // Implementation details here\n}`;

  return (
    <div style={{ padding: '1rem' }}>
      <h1>Algorithm {algoId}</h1>
      
      <section>
        <h2>Overview</h2>
        <p>
          This project implements the maximal clique enumeration algorithm proposed by Tomita, Tanaka, 
          and Takahashi, which is designed to generate all maximal cliques in an undirected graph. The algorithm 
          builds on the depth-first search (DFS) approach used in the Bron–Kerbosch algorithm but introduces pruning 
          techniques to improve efficiency. A key contribution of the paper is proving that the worst-case time complexity 
          of the algorithm is O(3ⁿ/³), which is optimal for an n-vertex graph. Additionally, the algorithm outputs 
          maximal cliques in a tree-like format, reducing memory consumption compared to traditional methods.
        </p>
      </section>
      
      <section>
        <h2>Implementation Details</h2>
        <ul>
          <li><strong>Graph Representation:</strong> Use adjacency lists for efficient traversal.</li>
          <li><strong>Recursive Depth-First Search:</strong> Implement the EXPAND function to enumerate maximal cliques.</li>
          <li><strong>Pruning Strategies:</strong> Maintain a CAND set (candidates for expansion) and FINI set (processed vertices). Select pivot vertices to reduce recursive calls.</li>
          <li><strong>Output Optimization:</strong> Format output to save space without listing all cliques explicitly.</li>
        </ul>
        <h3>Code</h3>
        <CodeEditor code={codeSnippet} />
      </section>
      
      <section>
        <h2>Expectation</h2>
        <ul>
          <li>Validate the O(3ⁿ/³) complexity with empirical results.</li>
          <li>Demonstrate reduced space requirements compared to standard Bron–Kerbosch output.</li>
          <li>Identify dataset characteristics that affect performance.</li>
        </ul>
      </section>
      
      <section>
        <h2>Observation</h2>
        <p>
          Some observations my friends found during their tests.
        </p>
      </section>
    </div>
  );
};

export default AlgorithmPage;
