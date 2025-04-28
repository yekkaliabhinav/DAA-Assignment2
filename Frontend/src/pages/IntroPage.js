// src/pages/IntroPage.js
import React from 'react';
import HeatmapTable from '../components/HeatmapTable';

const hClique = ['2','3','4','5','6'];
const datasets = ['NetScience','AS-733','Ca-HepTh','as-Caida'];

// Exact Algorithm data
const exactCDS = [
  [9.5, 57.0, 242.2, 775.2, 1938.0],
  [3.5,  5.8,   4.9,   2.0,    0.4],
  [3.0,  5.0,   5.0,   3.0,    1.0],
  [2.4,  1.6,   0.4,   0.0,    0.0],
];
const exactTime = [
  [  12,   22,  52, 102, 315],
  [   6,    1,   1, 0.2,0.01],
  [ 293,   32,  23,  25,  18],
  [5072,1875, 309,  96, 182],
];

// CoreExact Algorithm data
const coreCDS = [
  [9.6, 57.0, 242.4, 774.2, 1939.4],
  [3.3,  5.6,   4.9,   2.1,   0.4],
  [3.4,  5.1,   5.0,   3.2,  0.95],
  [0.0,  0.0,   0.4,   0.0,   0.0],
];
const coreTime = [
  [  22,   34,  72, 135, 397],
  [   8,    2,   3, 0.6, 0.8],
  [ 325,   48,  55,  16, 19],
  [6436,4563, 656, 531,496],
];

const IntroPage = () => (
  <div style={{ padding: '1rem' }}>
    <h1>Advanced Analysis of Densest Subgraph Discovery Algorithms</h1>
    <p>
    Densest subgraph discovery is a fundamental problem in graph theory with applications in social network analysis, bioinformatics, and data mining. Our project analyzes three key algorithms: the Exact algorithm, the CoreExact Decomposition algorithm, and the CoreExact algorithm. We study their theoretical foundations, implementation details, computational complexities, and real-world applicability. This webpage presents a summary of our findings and insights
    </p>

    <h2>Exact Algorithm</h2>
    <HeatmapTable
      title="Clique-Density Score (CDS)"
      rowLabels={datasets}
      colLabels={hClique}
      data={exactCDS}
      min={0}
      max={2000}
    />
    <HeatmapTable
      title="Execution Time (seconds)"
      rowLabels={datasets}
      colLabels={hClique}
      data={exactTime}
      min={0}
      max={6000}
      unit="s"
    />

    <h2>CoreExact Algorithm</h2>
    <HeatmapTable
      title="Clique-Density Score (CDS)"
      rowLabels={datasets}
      colLabels={hClique}
      data={coreCDS}
      min={0}
      max={2000}
    />
    <HeatmapTable
      title="Execution Time (seconds)"
      rowLabels={datasets}
      colLabels={hClique}
      data={coreTime}
      min={0}
      max={7000}
      unit="s"
    />

    <h2>Summary of Key Findings:</h2>
    <ul>
      <li>Exact is precise but heavy for large graphs.</li>
      <li>CoreExact Decomposition is fast and lightweight, useful for core analysis.</li>
      <li>CoreExact is optimized and scalable, recommended for large datasets.</li>
    </ul>
  </div>
);

export default IntroPage;
