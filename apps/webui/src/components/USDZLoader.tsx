import React, { useState, useCallback } from 'react';
import { Group } from 'three';

interface USDZLoaderProps {
    onLoad?: (object: Group) => void;
    onError?: (error: Error) => void;
}

// USDZ Model Component (Placeholder - will be replaced with WASM OpenUSD)
const USDZModel: React.FC<{ url: string; onLoad?: (object: Group) => void }> = ({ url, onLoad }) => {
    const [model, setModel] = React.useState<Group | null>(null);

    React.useEffect(() => {
        // Placeholder: Create a simple group to represent loaded USDZ
        const group = new Group();
        group.name = 'USDZ_Placeholder';

        // Add a placeholder mesh to indicate successful "loading"
        // In real implementation, this would parse the USDZ file
        setModel(group);
        if (onLoad) {
            onLoad(group);
        }
    }, [url, onLoad]);

    if (!model) return null;

    return <primitive object={model} />;
};

// File Input Component for USDZ Loading
const USDZFileLoader: React.FC<USDZLoaderProps> = ({ onLoad, onError }) => {
    const [file, setFile] = useState<File | null>(null);
    const [loading, setLoading] = useState(false);
    const [loadedModel, setLoadedModel] = useState<string | null>(null);

    const loadUSDZFile = useCallback(async (file: File) => {
        setLoading(true);
        try {
            // Create object URL for the file
            const url = URL.createObjectURL(file);
            setLoadedModel(url);

            // Clean up the URL after a delay to allow loading
            setTimeout(() => {
                URL.revokeObjectURL(url);
            }, 5000);

        } catch (error) {
            console.error('Error loading USDZ file:', error);
            if (onError) {
                onError(error as Error);
            }
        } finally {
            setLoading(false);
        }
    }, [onError]);

    const handleFileChange = useCallback((event: React.ChangeEvent<HTMLInputElement>) => {
        const selectedFile = event.target.files?.[0];
        if (selectedFile && selectedFile.name.toLowerCase().endsWith('.usdz')) {
            setFile(selectedFile);
            loadUSDZFile(selectedFile);
        } else {
            if (onError) {
                onError(new Error('Please select a valid USDZ file'));
            }
        }
    }, [onError, loadUSDZFile]);

    const handleModelLoad = useCallback((object: Group) => {
        console.log('USDZ model loaded:', object);
        if (onLoad) {
            onLoad(object);
        }
    }, [onLoad]);

    const clearModel = useCallback(() => {
        setFile(null);
        setLoadedModel(null);
    }, []);

    return (
        <div className="usdz-loader">
            <div className="control-group">
                <label>Load USDZ File</label>
                <input
                    type="file"
                    accept=".usdz"
                    onChange={handleFileChange}
                    disabled={loading}
                />
                {loading && <div style={{ color: '#007acc', fontSize: '12px' }}>Loading...</div>}
                {file && (
                    <div style={{ marginTop: '10px' }}>
                        <div style={{ fontSize: '12px', color: '#ccc' }}>
                            Loaded: {file.name} ({(file.size / 1024 / 1024).toFixed(2)} MB)
                        </div>
                        <button
                            onClick={clearModel}
                            style={{
                                marginTop: '5px',
                                padding: '4px 8px',
                                fontSize: '12px',
                                backgroundColor: '#ff4444',
                                border: 'none',
                                color: 'white',
                                borderRadius: '3px',
                                cursor: 'pointer'
                            }}
                        >
                            Clear
                        </button>
                    </div>
                )}
            </div>

            {/* Render the loaded model */}
            {loadedModel && (
                <React.Suspense fallback={<div>Loading USDZ model...</div>}>
                    <USDZModel url={loadedModel} onLoad={handleModelLoad} />
                </React.Suspense>
            )}
        </div>
    );
};

export default USDZFileLoader;
export { USDZModel };