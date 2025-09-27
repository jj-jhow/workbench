import React from 'react';
import './App.css';
import USDAuthoringPanel from './components/USDAuthoringPanel';
import USDViewer from './components/USDViewer';

function App() {
    return (
        <div className="app">
            <USDAuthoringPanel />
            <USDViewer />
        </div>
    );
}

export default App;