import React, { useState } from 'react';
import './App.css';
import USDAuthoringPanel from './components/USDAuthoringPanel';
import USDViewer from './components/USDViewer';
import { useWASM } from './hooks/useWASM';

type AppMode = 'viewing' | 'editing';

function App() {
    const [mode, setMode] = useState<AppMode>('viewing');
    const [loadedUSDZ, setLoadedUSDZ] = useState<any>(null);
    const { isReady: wasmReady, initializeWASM, unpackUSDZForEditing, error: wasmError } = useWASM();

    const toggleMode = async () => {
        const newMode = mode === 'viewing' ? 'editing' : 'viewing';

        if (newMode === 'editing' && loadedUSDZ) {
            try {
                // Initialize WASM if not ready
                if (!wasmReady) {
                    await initializeWASM();
                }

                // Unpack USDZ for editing
                console.log('Switching to edit mode - unpacking USDZ with WASM...');
                await unpackUSDZForEditing(loadedUSDZ);

                setMode(newMode);
            } catch (error) {
                console.error('Failed to activate edit mode:', error);
                alert('Failed to activate editing mode. Please try again.');
                return;
            }
        } else {
            setMode(newMode);
        }
    };

    const handleUSDZLoad = (usdzData: any) => {
        setLoadedUSDZ(usdzData);
    };

    return (
        <div className="app">
            <div className="mode-toggle-container">
                <button
                    className={`mode-toggle ${mode === 'viewing' ? 'active' : ''}`}
                    onClick={() => mode !== 'viewing' && toggleMode()}
                >
                    👁️ Viewing Mode
                </button>
                <button
                    className={`mode-toggle ${mode === 'editing' ? 'active' : ''}`}
                    onClick={() => mode !== 'editing' && toggleMode()}
                    disabled={!loadedUSDZ}
                    title={!loadedUSDZ ? 'Load a USDZ file first' : 'Switch to editing mode'}
                >
                    ✏️ Editing Mode
                </button>
                <div className="mode-indicator">
                    Current: <strong>{mode === 'viewing' ? 'Viewing' : 'Editing'}</strong>
                    {mode === 'editing' && ' (WASM Active)'}
                </div>
            </div>

            {mode === 'editing' ? (
                <USDAuthoringPanel mode={mode} loadedUSDZ={loadedUSDZ} />
            ) : (
                <div className="viewing-mode-panel">
                    <div className="panel-title">File Browser</div>
                    <div style={{ padding: '20px', color: '#ccc', fontSize: '14px' }}>
                        <p>📁 Viewing Mode - Load USDZ files to view</p>
                        <p>Switch to Editing Mode to modify USD scenes</p>
                    </div>
                </div>
            )}

            <USDViewer mode={mode} onUSDZLoad={handleUSDZLoad} />
        </div>
    );
} export default App;