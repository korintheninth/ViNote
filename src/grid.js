// Grid configuration
let gridStep = 20; // Grid cell size in canvas pixels (stays constant)
let lineThickness = 2; // Grid line thickness in pixels
let currentZoom = 1;

// Grid axis dimensions (must match CSS)
const AXIS_LEFT_WIDTH = 40;
const AXIS_TOP_HEIGHT = 24;

const gridContainer = document.getElementById('grid-container');
const gridLines = document.getElementById('grid-lines');
const topAxis = document.getElementById('grid-axis-top');
const leftAxis = document.getElementById('grid-axis-left');
const coordDisplay = document.getElementById('coord-display');
const canvas = document.getElementById('canvas');

function renderGrid() {
    // Available space for grid (excluding axes)
    const width = window.innerWidth - AXIS_LEFT_WIDTH;
    const height = window.innerHeight - AXIS_TOP_HEIGHT;
    
    // Visual step on screen = gridStep scaled by zoom
    const visualStep = gridStep * currentZoom;
    
    // Update grid lines background
    gridLines.style.backgroundImage = `
        linear-gradient(to right, rgba(0, 0, 0, 0.15) ${lineThickness}px, transparent ${lineThickness}px),
        linear-gradient(to bottom, rgba(0, 0, 0, 0.15) ${lineThickness}px, transparent ${lineThickness}px)
    `;
    gridLines.style.backgroundSize = `${visualStep}px ${visualStep}px`;

    // Render top axis numbers (skip 0 to avoid clipping)
    topAxis.innerHTML = '';
    const colCount = Math.ceil(width / visualStep) + 1;
    for (let i = 1; i < colCount; i++) {
        const num = document.createElement('span');
        num.className = 'grid-number';
        num.textContent = i * gridStep;
        num.style.left = `${i * visualStep}px`;
        topAxis.appendChild(num);
    }

    // Render left axis numbers (skip 0 to avoid clipping)
    leftAxis.innerHTML = '';
    const rowCount = Math.ceil(height / visualStep) + 1;
    for (let i = 1; i < rowCount; i++) {
        const num = document.createElement('span');
        num.className = 'grid-number';
        num.textContent = i * gridStep;
        num.style.top = `${i * visualStep}px`;
        leftAxis.appendChild(num);
    }
}

// Update zoom level and re-render grid
export function updateZoom(zoomLevel) {
    currentZoom = zoomLevel;
    renderGrid();
}

// Initial render
renderGrid();

// Re-render on window resize
window.addEventListener('resize', renderGrid);

// Track mouse position and update coordinate display
canvas.addEventListener('mousemove', (event) => {
    // Get position relative to canvas
    const rect = canvas.getBoundingClientRect();
    const screenX = event.clientX - rect.left;
    const screenY = event.clientY - rect.top;
    
    // Convert to canvas coordinates (account for zoom)
    const canvasX = Math.round(screenX / currentZoom);
    const canvasY = Math.round(screenY / currentZoom);
    
    coordDisplay.innerHTML = `x:${canvasX}<br>y:${canvasY}`;
});

canvas.addEventListener('mouseleave', () => {
    coordDisplay.innerHTML = 'x:—<br>y:—';
});

// Export functions and getters/setters
export function getGridStep() {
    return gridStep;
}

export function setGridStep(step) {
    gridStep = step;
    renderGrid();
}

export function getLineThickness() {
    return lineThickness;
}

export function setLineThickness(thickness) {
    lineThickness = thickness;
    renderGrid();
}

export { renderGrid };
