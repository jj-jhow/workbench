# Workbench WebUI

A React.js web application for USD authoring and viewing with Three.js integration.

## Features

### USD Authoring Panel
- Interactive primitive creation (Mesh, Camera, Light, Material)
- Scene hierarchy management
- Properties editing
- USDZ export (via WASM OpenUSD - to be integrated)

### USD Viewer Panel
- Three.js-based 3D rendering
- Camera switching
- Level of Detail (LOD) control
- Animation playback controls
- Interactive scene navigation

## Setup

1. Navigate to the webui directory:
```bash
cd apps/workbench/webui
```

2. Install dependencies:
```bash
npm install
```

3. Start the development server:
```bash
npm start
```

The application will open at http://localhost:3000

## Architecture

```
webui/
├── public/
│   └── index.html
├── src/
│   ├── components/
│   │   ├── USDAuthoringPanel.tsx
│   │   └── USDViewer.tsx
│   ├── App.tsx
│   ├── App.css
│   ├── index.tsx
│   └── index.css
├── package.json
├── tsconfig.json
└── README.md
```

## Future Integrations

### WASM OpenUSD
- Compile OpenUSD to WebAssembly for client-side USD manipulation
- Integrate with the authoring panel for real-time USD scene creation
- Enable USDZ export functionality

### Enhanced Three.js Integration
- USD/USDZ file loading and parsing
- Advanced material and lighting support
- Animation timeline controls
- Advanced LOD management

## Development

The application is built with:
- React 18 with TypeScript
- Three.js for 3D rendering
- React Three Fiber for React integration
- React Three Drei for additional 3D components

## Build

```bash
npm run build
```

Creates optimized production build in the `build/` directory.