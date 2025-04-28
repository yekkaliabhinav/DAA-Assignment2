// src/components/HistogramTimeChart.js
import React from 'react';
import {
  BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer
} from 'recharts';

const HistogramTimeChart = ({ data, title, color }) => {
  return (
    <div style={{ margin: '2rem 0' }}>
      <h3 style={{ textAlign: 'center', marginBottom: '1rem' }}>{title}</h3>
      <ResponsiveContainer width="100%" height={300}>
        <BarChart data={data}>
          <CartesianGrid strokeDasharray="3 3" />
          {/* name => Algorithm #, value => time taken */}
          <XAxis dataKey="name" label={{ value: 'Algorithm Number', position: 'insideBottom', offset: -5 }} />
          <YAxis label={{ value: 'Time (seconds)', angle: -90, position: 'insideLeft' }} />
          <Tooltip />
          <Legend />
          {/* A single Bar component, because each object in data is a single bar */}
          <Bar dataKey="value" fill={color} name="Time Taken" />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
};

export default HistogramTimeChart;
