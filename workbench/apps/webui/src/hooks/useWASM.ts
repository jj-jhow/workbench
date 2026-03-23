// Hook for WASM OpenUSD integration
import { useState, useCallback, useRef } from 'react';

declare global {
  interface Window {
    OpenUSDModule?: (config?: Record<string, unknown>) => Promise<EmscriptenModule>;
  }
}

interface EmscriptenModule {
  unpackUSDZ: (data: string) => any;
  packUSDZ: (data: string) => string | null;
  createPrimitive: (type: string, properties: any) => any;
  updatePrimitive: (id: string, properties: any) => void;
  deletePrimitive: (id: string) => void;
  exportUSDZ: () => string | null;
}

interface WASMModule {
  unpackUSDZ: (data: any) => Promise<any>;
  packUSDZ: (data: any) => Promise<any>;
  createPrimitive: (type: string, properties: any) => any;
  updatePrimitive: (id: string, properties: any) => void;
  deletePrimitive: (id: string) => void;
  exportUSDZ: () => Promise<Blob>;
}

interface UseWASMReturn {
  isReady: boolean;
  isLoading: boolean;
  error: string | null;
  wasmModule: WASMModule | null;
  initializeWASM: () => Promise<void>;
  unpackUSDZForEditing: (usdzData: any) => Promise<any>;
}

function checkCrossOriginIsolation(): void {
  if (typeof crossOriginIsolated !== 'undefined' && !crossOriginIsolated) {
    console.warn(
      'Cross-origin isolation is not enabled. ' +
      'SharedArrayBuffer (required by pthreads WASM) will not be available. ' +
      'Set Cross-Origin-Opener-Policy: same-origin and ' +
      'Cross-Origin-Embedder-Policy: require-corp headers on your server.'
    );
  }
}

function loadScript(src: string): Promise<void> {
  return new Promise((resolve, reject) => {
    if (document.querySelector(`script[src="${src}"]`)) {
      resolve();
      return;
    }
    const script = document.createElement('script');
    script.src = src;
    script.onload = () => resolve();
    script.onerror = () => reject(new Error(`Failed to load script: ${src}`));
    document.head.appendChild(script);
  });
}

function uint8ArrayToString(bytes: Uint8Array): string {
  const chunks: string[] = [];
  const chunkSize = 8192;
  for (let i = 0; i < bytes.length; i += chunkSize) {
    chunks.push(String.fromCharCode(...bytes.subarray(i, i + chunkSize)));
  }
  return chunks.join('');
}

function stringToUint8Array(str: string): Uint8Array {
  const buf = new Uint8Array(str.length);
  for (let i = 0; i < str.length; i++) {
    buf[i] = str.charCodeAt(i);
  }
  return buf;
}

export const useWASM = (): UseWASMReturn => {
  const [isReady, setIsReady] = useState(false);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [wasmModule, setWasmModule] = useState<WASMModule | null>(null);
  const moduleRef = useRef<EmscriptenModule | null>(null);

  const initializeWASM = useCallback(async () => {
    if (moduleRef.current) {
      setIsReady(true);
      return;
    }

    setIsLoading(true);
    setError(null);

    try {
      checkCrossOriginIsolation();

      // Load the Emscripten-generated JS loader
      await loadScript('/openusd.js');

      if (!window.OpenUSDModule) {
        throw new Error('OpenUSDModule factory not found after loading openusd.js');
      }

      const instance = await window.OpenUSDModule();
      moduleRef.current = instance;

      // Wrap raw embind functions into the WASMModule interface
      const wrappedModule: WASMModule = {
        unpackUSDZ: async (data: any) => {
          let bytes: Uint8Array;
          if (data instanceof ArrayBuffer) {
            bytes = new Uint8Array(data);
          } else if (data instanceof Uint8Array) {
            bytes = data;
          } else if (data instanceof File || data instanceof Blob) {
            bytes = new Uint8Array(await data.arrayBuffer());
          } else {
            throw new Error('unpackUSDZ: expected ArrayBuffer, Uint8Array, File, or Blob');
          }
          const binaryStr = uint8ArrayToString(bytes);
          return instance.unpackUSDZ(binaryStr);
        },

        packUSDZ: async (data: any) => {
          const input = typeof data === 'string' ? data : JSON.stringify(data);
          const result = instance.packUSDZ(input);
          if (!result) return null;
          const bytes = stringToUint8Array(result);
          return new Blob([bytes], { type: 'model/vnd.usdz+zip' });
        },

        createPrimitive: (type: string, properties: any) => {
          return instance.createPrimitive(type, properties);
        },

        updatePrimitive: (id: string, properties: any) => {
          instance.updatePrimitive(id, properties);
        },

        deletePrimitive: (id: string) => {
          instance.deletePrimitive(id);
        },

        exportUSDZ: async () => {
          const result = instance.exportUSDZ();
          if (!result) return new Blob([], { type: 'model/vnd.usdz+zip' });
          const bytes = stringToUint8Array(result);
          return new Blob([bytes], { type: 'model/vnd.usdz+zip' });
        },
      };

      setWasmModule(wrappedModule);
      setIsReady(true);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load WASM module');
    } finally {
      setIsLoading(false);
    }
  }, []);

  const unpackUSDZForEditing = useCallback(async (usdzData: any) => {
    if (!wasmModule) {
      throw new Error('WASM module not ready');
    }

    return await wasmModule.unpackUSDZ(usdzData);
  }, [wasmModule]);

  return {
    isReady,
    isLoading,
    error,
    wasmModule,
    initializeWASM,
    unpackUSDZForEditing,
  };
};