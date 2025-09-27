import React, { useState, useRef, Suspense } from 'react';
import { Canvas } from '@react-three/fiber';
import { OrbitControls, Grid, Box, Sphere, Cone } from '@react-three/drei';
import { Mesh, Group } from 'three';
import USDZFileLoader from './USDZLoader';

interface USDViewerProps {
    mode: 'viewing' | 'editing';
    onUSDZLoad: (usdzData: any) => void;
}

interface Camera {
    id: string;
    name: string;
    position: [number, number, number];
    target: [number, number, number];
}

interface Animation {
    id: string;
    name: string;
    duration: number;
    isPlaying: boolean;
}

interface LODLevel {
    id: string;
    name: string;
    complexity: 'low' | 'medium' | 'high';
}

const Scene: React.FC = () => {
    const meshRef = useRef<Mesh>(null);

    return (
        <>
            <ambientLight intensity={0.5} />
            <pointLight position={[10, 10, 10]} />
            <Grid args={[10, 10]} />

            {/* Sample USD objects - will be replaced with actual USDZ content */}
            <Box ref={meshRef} position={[-2, 1, 0]} args={[1, 1, 1]}>
                <meshStandardMaterial color="orange" />
            </Box>

            <Sphere position={[2, 1, 0]} args={[0.5]}>
                <meshStandardMaterial color="lightblue" />
            </Sphere>

            <Cone position={[0, 1, -2]} args={[0.5, 1]}>
                <meshStandardMaterial color="lightgreen" />
            </Cone>

            <OrbitControls enablePan={true} enableZoom={true} enableRotate={true} />
        </>
    );
};

const USDViewer: React.FC<USDViewerProps> = ({ mode, onUSDZLoad }) => {
    const [loadedUSDZ, setLoadedUSDZ] = useState<Group | null>(null);
    const [cameras] = useState<Camera[]>([
        { id: '1', name: 'Main Camera', position: [5, 5, 5], target: [0, 0, 0] },
        { id: '2', name: 'Close-up Camera', position: [2, 2, 2], target: [0, 0, 0] },
        { id: '3', name: 'Top View', position: [0, 10, 0], target: [0, 0, 0] }
    ]); const [animations] = useState<Animation[]>([
        { id: '1', name: 'Rotation', duration: 3.0, isPlaying: false },
        { id: '2', name: 'Scale Pulse', duration: 2.0, isPlaying: false },
        { id: '3', name: 'Position Float', duration: 4.0, isPlaying: false }
    ]);

    const [lodLevels] = useState<LODLevel[]>([
        { id: '1', name: 'High Quality', complexity: 'high' },
        { id: '2', name: 'Medium Quality', complexity: 'medium' },
        { id: '3', name: 'Low Quality', complexity: 'low' }
    ]);

    const [selectedCamera, setSelectedCamera] = useState<string>('1');
    const [selectedLOD, setSelectedLOD] = useState<string>('1');
    const [playingAnimations, setPlayingAnimations] = useState<Set<string>>(new Set());

    const toggleAnimation = (animationId: string) => {
        const newPlayingAnimations = new Set(playingAnimations);
        if (newPlayingAnimations.has(animationId)) {
            newPlayingAnimations.delete(animationId);
        } else {
            newPlayingAnimations.add(animationId);
        }
        setPlayingAnimations(newPlayingAnimations);

        // TODO: Implement actual animation control
        console.log('Toggle animation:', animationId);
    };

    const switchCamera = (cameraId: string) => {
        setSelectedCamera(cameraId);
        // TODO: Implement camera switching in Three.js scene
        console.log('Switch to camera:', cameraId);
    };

    const changeLOD = (lodId: string) => {
        setSelectedLOD(lodId);
        // TODO: Implement LOD switching
        console.log('Switch to LOD:', lodId);
    };

    const handleUSDZLoad = (object: Group) => {
        console.log('USDZ loaded in viewer:', object);
        setLoadedUSDZ(object);
        onUSDZLoad(object); // Notify parent App component
    };

    const handleUSDZError = (error: Error) => {
        console.error('USDZ loading error:', error);
        alert(`USDZ Loading Error: ${error.message}`);
    }; return (
        <div className="viewer-panel">
            <Canvas camera={{ position: [5, 5, 5], fov: 75 }}>
                <Suspense fallback={null}>
                    <Scene />
                </Suspense>
            </Canvas>

            <div className="viewer-controls">
                <div>
                    <label>Camera</label>
                    <select
                        value={selectedCamera}
                        onChange={(e) => switchCamera(e.target.value)}
                    >
                        {cameras.map((camera) => (
                            <option key={camera.id} value={camera.id}>
                                {camera.name}
                            </option>
                        ))}
                    </select>
                </div>

                <div>
                    <label>Level of Detail</label>
                    <select
                        value={selectedLOD}
                        onChange={(e) => changeLOD(e.target.value)}
                    >
                        {lodLevels.map((lod) => (
                            <option key={lod.id} value={lod.id}>
                                {lod.name}
                            </option>
                        ))}
                    </select>
                </div>

                <div>
                    <label>Animations</label>
                    {animations.map((animation) => (
                        <button
                            key={animation.id}
                            onClick={() => toggleAnimation(animation.id)}
                            style={{
                                backgroundColor: playingAnimations.has(animation.id) ? '#007acc' : '#333',
                                marginBottom: '5px'
                            }}
                        >
                            {playingAnimations.has(animation.id) ? '⏸️' : '▶️'} {animation.name}
                        </button>
                    ))}
                </div>

                <div>
                    <USDZFileLoader onLoad={handleUSDZLoad} onError={handleUSDZError} />
                </div>

                <div style={{
                    marginTop: '20px',
                    padding: '10px',
                    backgroundColor: 'rgba(0, 0, 0, 0.3)',
                    borderRadius: '4px',
                    fontSize: '12px'
                }}>
                    <strong>USDZ Status:</strong><br />
                    Mode: {mode === 'viewing' ? 'Viewing Only' : 'Editing Enabled'}<br />
                    {loadedUSDZ ? `Loaded: ${loadedUSDZ.name || 'USDZ Model'}` : 'No file loaded'}<br />
                    <em>{loadedUSDZ ? 'USDZ model active' : 'Showing sample Three.js scene'}</em>
                </div>
            </div>
        </div>
    );
};

export default USDViewer;