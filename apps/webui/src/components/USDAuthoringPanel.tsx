import React, { useState } from 'react';

interface USDAuthoringPanelProps {
    mode: 'viewing' | 'editing';
    loadedUSDZ: any;
}

interface USDPrimitive {
    id: string;
    name: string;
    type: 'mesh' | 'camera' | 'light' | 'material';
    properties: Record<string, any>;
}

const USDAuthoringPanel: React.FC<USDAuthoringPanelProps> = ({ mode, loadedUSDZ }) => {
    const [primitives, setPrimitives] = useState<USDPrimitive[]>([]);
    const [wasmReady, setWasmReady] = useState(false);

    React.useEffect(() => {
        if (mode === 'editing' && loadedUSDZ) {
            initializeWasmEditing();
        }
    }, [mode, loadedUSDZ]);

    const initializeWasmEditing = async () => {
        try {
            // TODO: Initialize WASM module for USD editing
            console.log('Initializing WASM for USD editing...', loadedUSDZ);
            setWasmReady(true);
        } catch (error) {
            console.error('Failed to initialize WASM editing:', error);
        }
    };
    const [selectedPrimitive, setSelectedPrimitive] = useState<string | null>(null);
    const [primitiveName, setPrimitiveName] = useState('');
    const [primitiveType, setPrimitiveType] = useState<USDPrimitive['type']>('mesh');

    const addPrimitive = () => {
        if (!primitiveName.trim()) return;

        const newPrimitive: USDPrimitive = {
            id: Date.now().toString(),
            name: primitiveName,
            type: primitiveType,
            properties: {}
        };

        setPrimitives([...primitives, newPrimitive]);
        setPrimitiveName('');
    };

    const removePrimitive = (id: string) => {
        setPrimitives(primitives.filter(p => p.id !== id));
        if (selectedPrimitive === id) {
            setSelectedPrimitive(null);
        }
    };

    const exportToUSDZ = () => {
        // TODO: Implement WASM OpenUSD export
        console.log('Exporting to USDZ...', primitives);
        alert('USDZ export will be implemented with WASM OpenUSD integration');
    };

    return (
        <div className="authoring-panel">
            <div className="panel-title">USD Authoring</div>

            <div className="control-group">
                <label>Add Primitive</label>
                <input
                    type="text"
                    placeholder="Primitive name"
                    value={primitiveName}
                    onChange={(e) => setPrimitiveName(e.target.value)}
                />
                <select
                    value={primitiveType}
                    onChange={(e) => setPrimitiveType(e.target.value as USDPrimitive['type'])}
                >
                    <option value="mesh">Mesh</option>
                    <option value="camera">Camera</option>
                    <option value="light">Light</option>
                    <option value="material">Material</option>
                </select>
                <button onClick={addPrimitive}>Add Primitive</button>
            </div>

            <div className="control-group">
                <label>Scene Hierarchy</label>
                <div style={{ maxHeight: '200px', overflowY: 'auto', border: '1px solid #555', padding: '10px' }}>
                    {primitives.length === 0 ? (
                        <div style={{ color: '#888', fontStyle: 'italic' }}>No primitives added</div>
                    ) : (
                        primitives.map((primitive) => (
                            <div
                                key={primitive.id}
                                style={{
                                    padding: '5px',
                                    marginBottom: '5px',
                                    backgroundColor: selectedPrimitive === primitive.id ? '#007acc' : '#444',
                                    cursor: 'pointer',
                                    borderRadius: '3px',
                                    display: 'flex',
                                    justifyContent: 'space-between',
                                    alignItems: 'center'
                                }}
                                onClick={() => setSelectedPrimitive(primitive.id)}
                            >
                                <span>{primitive.name} ({primitive.type})</span>
                                <button
                                    onClick={(e) => {
                                        e.stopPropagation();
                                        removePrimitive(primitive.id);
                                    }}
                                    style={{
                                        background: '#ff4444',
                                        border: 'none',
                                        color: 'white',
                                        padding: '2px 6px',
                                        borderRadius: '3px',
                                        fontSize: '12px'
                                    }}
                                >
                                    ×
                                </button>
                            </div>
                        ))
                    )}
                </div>
            </div>

            <div className="control-group">
                <button onClick={exportToUSDZ} disabled={primitives.length === 0}>
                    Export to USDZ
                </button>
            </div>

            <div className="control-group">
                <label>Import USDZ File</label>
                <input
                    type="file"
                    accept=".usdz,.usd"
                    onChange={(e) => {
                        const file = e.target.files?.[0];
                        if (file) {
                            // TODO: Integrate with WASM OpenUSD for actual parsing
                            console.log('Selected USDZ file:', file.name);
                            alert(`USDZ file selected: ${file.name}\nWASM OpenUSD integration needed for parsing.`);
                        }
                    }}
                />
            </div>

            <div className="control-group">
                <label>WASM Integration Status</label>
                <div style={{
                    padding: '10px',
                    backgroundColor: '#333',
                    borderRadius: '4px',
                    fontSize: '12px',
                    color: '#ccc'
                }}>
                    Mode: {mode}<br />
                    OpenUSD WASM: {wasmReady ? '✅ Ready' : '⏳ Loading...'}<br />
                    USDZ Status: {loadedUSDZ ? '✅ Loaded' : '❌ No file'}<br />
                    Editing: {mode === 'editing' && wasmReady ? '✅ Active' : '❌ Disabled'}
                </div>
            </div>
        </div>
    );
};

export default USDAuthoringPanel;