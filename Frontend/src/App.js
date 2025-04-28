// src/App.js
import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import Title from './components/Title';
import Navbar from './components/Navbar';
import IntroPage from './pages/IntroPage';
import Algorithm1 from './pages/algorithms/Algorithm1';
import Algorithm2 from './pages/algorithms/Algorithm2';
import Algorithm3 from './pages/algorithms/Algorithm3';
import CliqueDistributionPage from './components/HistogramChart';

function App() {
  return (
    <Router>
      <Title />
      <Navbar />

      {/* Main Content */}
      <Routes>
        <Route path="/" element={<IntroPage />} />
        <Route path="/algo/1" element={<Algorithm1 />} />
        <Route path="/algo/2" element={<Algorithm2 />} />
        <Route path="/algo/3" element={<Algorithm3 />} />
      </Routes>
    </Router>
  );
}

export default App;
