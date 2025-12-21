import { updateZoom } from './grid.js';

let zoomLevel = 1;
let zoomStep = 0.05;
let maxZoomLevel = 10;
let minZoomLevel = 0.1;
let maxSize = {width: 0, height: 0};
let canvas = document.getElementById('canvas');

const AXIS_LEFT_WIDTH = 40;
const AXIS_TOP_HEIGHT = 24;

export function getZoomLevel() {
    return zoomLevel
}

function applyZoom() {
    const canvasWidth = (window.innerWidth - AXIS_LEFT_WIDTH) / zoomLevel;
    const canvasHeight = (window.innerHeight - AXIS_TOP_HEIGHT) / zoomLevel;
    
    canvas.style.transform = `scale(${zoomLevel})`;
    canvas.style.width = `${canvasWidth}px`;
    canvas.style.height = `${canvasHeight}px`;
    updateZoom(zoomLevel);
}

export function zoomIn(pos) {
    zoomLevel = Math.min(zoomLevel * (1 + zoomStep), maxZoomLevel);
    applyZoom();
}

export function zoomOut(pos) {
    zoomLevel = Math.max(zoomLevel * (1 - zoomStep), minZoomLevel);
    applyZoom();
}
