// Grid configuration
let gridStep = 20; // Grid cell size in canvas pixels (stays constant)
let lineThickness = 2; // Grid line thickness in pixels
let currentZoom = 1;
let scrollOffset = { left: 0, top: 0 };

// Grid axis dimensions (must match CSS)
const AXIS_LEFT_WIDTH = 40;
const AXIS_TOP_HEIGHT = 24;

const gridContainer = document.getElementById('grid-container');
const gridLines = document.getElementById('grid-lines');
const topAxis = document.getElementById('grid-axis-top');
const leftAxis = document.getElementById('grid-axis-left');
const coordDisplay = document.getElementById('coord-display');
const canvas = document.getElementById('canvas');
const scrollWrapper = document.getElementById('scroll-wrapper');

function renderGrid() {
    // Available space for grid (excluding axes)
    const width = window.innerWidth - AXIS_LEFT_WIDTH;
    const height = window.innerHeight - AXIS_TOP_HEIGHT;
    
    // Visual step on screen = gridStep scaled by zoom
    const visualStep = gridStep * currentZoom;
    
    // Calculate scroll offset in grid units
    const scrollX = scrollOffset.left / currentZoom;
    const scrollY = scrollOffset.top / currentZoom;
    
    // Calculate the offset for grid alignment with scroll
    const gridOffsetX = -(scrollOffset.left % visualStep);
    const gridOffsetY = -(scrollOffset.top % visualStep);
    
    // Update grid lines background with scroll offset
    gridLines.style.backgroundImage = `
        linear-gradient(to right, rgba(0, 0, 0, 0.15) ${lineThickness}px, transparent ${lineThickness}px),
        linear-gradient(to bottom, rgba(0, 0, 0, 0.15) ${lineThickness}px, transparent ${lineThickness}px)
    `;
    gridLines.style.backgroundSize = `${visualStep}px ${visualStep}px`;
    gridLines.style.backgroundPosition = `${gridOffsetX}px ${gridOffsetY}px`;

    // Calculate the first visible grid line index
    const firstVisibleCol = Math.floor(scrollX / gridStep);
    const firstVisibleRow = Math.floor(scrollY / gridStep);

    // Render top axis numbers
    topAxis.innerHTML = '';
    const colCount = Math.ceil(width / visualStep) + 2;
    for (let i = 0; i < colCount; i++) {
        const gridIndex = firstVisibleCol + i + 1;
        const num = document.createElement('span');
        num.className = 'grid-number';
        num.textContent = gridIndex * gridStep;
        num.style.left = `${gridOffsetX + (i + 1) * visualStep}px`;
        topAxis.appendChild(num);
    }

    // Render left axis numbers
    leftAxis.innerHTML = '';
    const rowCount = Math.ceil(height / visualStep) + 2;
    for (let i = 0; i < rowCount; i++) {
        const gridIndex = firstVisibleRow + i + 1;
        const num = document.createElement('span');
        num.className = 'grid-number';
        num.textContent = gridIndex * gridStep;
        num.style.top = `${gridOffsetY + (i + 1) * visualStep}px`;
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

// Update grid on scroll
if (scrollWrapper) {
    scrollWrapper.addEventListener('scroll', () => {
        scrollOffset.left = scrollWrapper.scrollLeft;
        scrollOffset.top = scrollWrapper.scrollTop;
        renderGrid();
    });
}

// Track mouse position and update coordinate display
// Use scroll wrapper as target since it's the scrollable container
const mouseTarget = scrollWrapper || canvas;
mouseTarget.addEventListener('mousemove', (event) => {
    // Get position relative to scroll wrapper
    const wrapperRect = scrollWrapper ? scrollWrapper.getBoundingClientRect() : canvas.getBoundingClientRect();
    const screenX = event.clientX - wrapperRect.left;
    const screenY = event.clientY - wrapperRect.top;
    
    // Convert to canvas coordinates (account for zoom and scroll)
    const canvasX = Math.round((screenX + scrollOffset.left) / currentZoom);
    const canvasY = Math.round((screenY + scrollOffset.top) / currentZoom);
    
    coordDisplay.innerHTML = `x:${canvasX}<br>y:${canvasY}`;
});

mouseTarget.addEventListener('mouseleave', () => {
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
