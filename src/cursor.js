import { getGridStep } from './grid.js';
import { getZoomLevel } from './zoom.js';

const canvas = document.getElementById('canvas');
const scrollWrapper = document.getElementById('scroll-wrapper');

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
    
    const wrapperRect = scrollWrapper ? scrollWrapper.getBoundingClientRect() : canvas.getBoundingClientRect();
    const zoomLevel = getZoomLevel();
    const scrollLeft = scrollWrapper ? scrollWrapper.scrollLeft : 0;
    const scrollTop = scrollWrapper ? scrollWrapper.scrollTop : 0;
    
    // Get mouse position relative to scroll wrapper viewport
    const screenX = event.clientX - wrapperRect.left;
    const screenY = event.clientY - wrapperRect.top;
    
    // Convert to canvas coordinates (including scroll offset)
    const canvasX = (screenX + scrollLeft) / zoomLevel;
    const canvasY = (screenY + scrollTop) / zoomLevel;
    
    // Snap to grid
    const snappedX = snapToGrid(canvasX);
    const snappedY = snapToGrid(canvasY);
    
    // Convert back to screen coordinates
    const cursorScreenX = wrapperRect.left + (snappedX * zoomLevel - scrollLeft);
    const cursorScreenY = wrapperRect.top + (snappedY * zoomLevel - scrollTop);
    
    customCursor.style.left = `${cursorScreenX}px`;
    customCursor.style.top = `${cursorScreenY}px`;
}

// Toggle snap cursor on/off
export function toggleSnapCursor() {
    snapEnabled = !snapEnabled;
    
    const target = scrollWrapper || canvas;
    if (snapEnabled) {
        target.classList.add('hide-cursor');
        customCursor.classList.add('visible');
    } else {
        target.classList.remove('hide-cursor');
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

const mouseTarget = scrollWrapper || canvas;
mouseTarget.addEventListener('mouseenter', () => {
    if (snapEnabled) {
        customCursor.classList.add('visible');
    }
});

mouseTarget.addEventListener('mouseleave', () => {
    customCursor.classList.remove('visible');
});

