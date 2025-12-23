import { updateZoom } from './grid.js';
import { setZoom as setInfiniteScrollZoom, updateCanvasSize } from './infinite_scroll.js';

let zoomLevel = 1;
let zoomStep = 0.05;
let maxZoomLevel = 10;
let minZoomLevel = 0.1;
let canvas = document.getElementById('canvas');
let scrollWrapper = document.getElementById('scroll-wrapper');

export function getZoomLevel() {
    return zoomLevel
}

function applyZoom() {
    canvas.style.transform = `scale(${zoomLevel})`;
    setInfiniteScrollZoom(zoomLevel);
    updateCanvasSize();
    updateZoom(zoomLevel);
}

export function zoomIn(event) {
    const oldZoom = zoomLevel;
    const newZoom = Math.min(zoomLevel * (1 + zoomStep), maxZoomLevel);
    applyZoomToPoint(event, oldZoom, newZoom);
}

export function zoomOut(event) {
    const oldZoom = zoomLevel;
    const newZoom = Math.max(zoomLevel * (1 - zoomStep), minZoomLevel);
    applyZoomToPoint(event, oldZoom, newZoom);
}

function applyZoomToPoint(event, oldZoom, newZoom) {
    if (!scrollWrapper || oldZoom === newZoom) return;
    
    // Get mouse position relative to the scroll wrapper viewport
    const wrapperRect = scrollWrapper.getBoundingClientRect();
    const mouseX = event.clientX - wrapperRect.left;
    const mouseY = event.clientY - wrapperRect.top;
    
    // Calculate the canvas position under the mouse (before zoom)
    const canvasX = (scrollWrapper.scrollLeft + mouseX) / oldZoom;
    const canvasY = (scrollWrapper.scrollTop + mouseY) / oldZoom;
    
    // Apply the new zoom level
    zoomLevel = newZoom;
    applyZoom();
    
    // Calculate new scroll position to keep the same canvas point under the mouse
    const newScrollLeft = canvasX * newZoom - mouseX;
    const newScrollTop = canvasY * newZoom - mouseY;
    
    scrollWrapper.scrollLeft = newScrollLeft;
    scrollWrapper.scrollTop = newScrollTop;
}
