// src/components/HistogramChart.js
import React from 'react';
import {
  BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer
} from 'recharts';

const HistogramChart = ({ data, title, color }) => {
  return (
    <div style={{ margin: '2rem 0' }}>
      <h3 style={{ textAlign: 'center', marginBottom: '1rem' }}>{title}</h3>
      <ResponsiveContainer width="100%" height={300}>
        <BarChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="name" label={{ value: 'Clique Size', position: 'insideBottom', offset: -5 }} />
          <YAxis label={{ value: 'Frequency', angle: -90, position: 'insideLeft' }} />
          <Tooltip />
          <Legend />
          <Bar dataKey="value" fill={color} name="Frequency" />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
};

export default HistogramChart;
