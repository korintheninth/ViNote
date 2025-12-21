import { getGridStep } from './grid.js';
import { getZoomLevel } from './zoom.js';

const canvas = document.getElementById('canvas');

// Create custom cursor element
const customCursor = document.createElement('div');
customCursor.id = 'custom-cursor';
document.body.appendChild(customCursor);

let snapEnabled = false;

// Snap a value to the nearest grid step
export function snapToGrid(value) {
    const gridStep = getGridStep();
    return Math.round(value / gridStep) * gridStep;
}

// Update cursor position
function updateCursor(event) {
    if (!snapEnabled) return;
    
    const rect = canvas.getBoundingClientRect();
    const zoomLevel = getZoomLevel();
    
    // Get mouse position relative to canvas
    const screenX = event.clientX - rect.left;
    const screenY = event.clientY - rect.top;
    
    // Convert to canvas coordinates
    const canvasX = screenX / zoomLevel;
    const canvasY = screenY / zoomLevel;
    
    // Snap to grid
    const snappedX = snapToGrid(canvasX);
    const snappedY = snapToGrid(canvasY);
    
    // Convert back to screen coordinates
    const cursorScreenX = rect.left + (snappedX * zoomLevel);
    const cursorScreenY = rect.top + (snappedY * zoomLevel);
    
    customCursor.style.left = `${cursorScreenX}px`;
    customCursor.style.top = `${cursorScreenY}px`;
}

// Toggle snap cursor on/off
export function toggleSnapCursor() {
    snapEnabled = !snapEnabled;
    
    if (snapEnabled) {
        canvas.classList.add('hide-cursor');
        customCursor.classList.add('visible');
    } else {
        canvas.classList.remove('hide-cursor');
        customCursor.classList.remove('visible');
    }
    
    return snapEnabled;
}

export function isSnapEnabled() {
    return snapEnabled;
}

export function setSnapEnabled(enabled) {
    if (enabled !== snapEnabled) {
        toggleSnapCursor();
    }
}

// Event listeners
document.addEventListener('mousemove', updateCursor);

canvas.addEventListener('mouseenter', () => {
    if (snapEnabled) {
        customCursor.classList.add('visible');
    }
});

canvas.addEventListener('mouseleave', () => {
    customCursor.classList.remove('visible');
});

