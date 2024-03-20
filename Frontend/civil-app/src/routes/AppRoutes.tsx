import React from 'react';
import { Routes, Route, Router } from 'react-router-dom';
import DashboardComponent from '../components/dashboard-components/DashboardComponent';

const AppRoutes: React.FC = () => {
  return (
      <Routes>
        <Route path="/dashboard" element={<DashboardComponent />} />
        <Route path="/" element={<DashboardComponent />} />
      </Routes>
  );
};


export const API_URL = 'http://localhost:8080/';
export default AppRoutes;