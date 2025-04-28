// src/components/HeatmapTable.js
import React from 'react';

const getColor = (val, min, max) => {
  // clamp ratio between 0 and 1
  const ratio = Math.max(0, Math.min(1, (val - min) / (max - min)));
  // hue from blue (240) to red (0)
  const hue = (1 - ratio) * 240;
  return `hsl(${hue}, 70%, 60%)`;
};

const HeatmapTable = ({
  title,
  rowLabels,
  colLabels,
  data,    // 2D array [row][col]
  min,     // number
  max,     // number
  unit = ''// string, e.g. 's' or ''
}) => (
  <div style={{ marginBottom: '2rem' }}>
    {title && <h3 style={{ marginBottom: '0.5rem' }}>{title}</h3>}
    <table style={{ borderCollapse: 'collapse', width: '100%', fontSize: '0.9rem' }}>
      <thead>
        <tr>
          <th style={{ padding: '6px 8px' }}></th>
          {colLabels.map((c, j) => (
            <th key={j} style={{ padding: '6px 8px', textAlign: 'center' }}>{c}</th>
          ))}
        </tr>
      </thead>
      <tbody>
        {data.map((row, i) => (
          <tr key={i}>
            <td style={{ padding: '6px 8px', fontWeight: 'bold' }}>{rowLabels[i]}</td>
            {row.map((val, j) => (
              <td
                key={j}
                style={{
                  backgroundColor: getColor(val, min, max),
                  padding: '8px',
                  textAlign: 'center',
                  color: '#000'
                }}
              >
                {val}{unit}
              </td>
            ))}
          </tr>
        ))}
      </tbody>
    </table>
  </div>
);

export default HeatmapTable;
