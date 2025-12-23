// Infinite scroll module - makes canvas infinitely scrollable
// by maintaining a margin beyond the viewport that expands as user scrolls

import { registry } from './node_manager.js';

const AXIS_LEFT_WIDTH = 40;
const AXIS_TOP_HEIGHT = 24;
const SCROLL_MARGIN = 0.1; // margin beyond viewport

let scrollWrapper = null;
let canvas = null;
let currentZoom = 1;

// Track the content bounds (positions of placed nodes)
let contentBounds = {
    minX: Infinity,
    minY: Infinity,
    maxX: -Infinity,
    maxY: -Infinity
};

export function initInfiniteScroll() {
    canvas = document.getElementById('canvas');
    scrollWrapper = document.getElementById('scroll-wrapper');
    
    if (!scrollWrapper || !canvas) {
        console.error('Scroll wrapper or canvas not found');
        return;
    }
    
    // Initial sizing
    updateCanvasSize();
    
    // Listen for scroll events
    scrollWrapper.addEventListener('scroll', onScroll);
    
    // Listen for window resize
    window.addEventListener('resize', updateCanvasSize);
}

function getViewportSize() {
    return {
        width: window.innerWidth - AXIS_LEFT_WIDTH,
        height: window.innerHeight - AXIS_TOP_HEIGHT
    };
}

function getMarginSize() {
    const viewport = getViewportSize();
    return {
        width: viewport.width * SCROLL_MARGIN,
        height: viewport.height * SCROLL_MARGIN
    };
}

function onScroll() {
    updateCanvasSize();
}

export function updateCanvasSize() {
    if (!scrollWrapper || !canvas) return;
    
    const viewport = getViewportSize();
    const margin = getMarginSize();
    const scroll = {
        left: scrollWrapper.scrollLeft,
        top: scrollWrapper.scrollTop
    };
    
    // All calculations are in logical (unscaled) canvas coordinates
    // The canvas will be scaled by transform, so we need to account for that
    
    // Viewport size in logical coordinates
    const logicalViewportWidth = viewport.width / currentZoom;
    const logicalViewportHeight = viewport.height / currentZoom;
    
    // Margin in logical coordinates
    const logicalMarginWidth = margin.width / currentZoom;
    const logicalMarginHeight = margin.height / currentZoom;
    
    // Scroll position in logical coordinates
    const logicalScrollLeft = scroll.left / currentZoom;
    const logicalScrollTop = scroll.top / currentZoom;
    
    // Calculate minimum required size based on content
    const contentMinWidth = contentBounds.maxX !== -Infinity 
        ? contentBounds.maxX + 100 // Add padding after content
        : 0;
    const contentMinHeight = contentBounds.maxY !== -Infinity 
        ? contentBounds.maxY + 100 
        : 0;
    
    // Calculate the size needed to maintain margin beyond current scroll + viewport
    const scrollRequiredWidth = logicalScrollLeft + logicalViewportWidth + logicalMarginWidth;
    const scrollRequiredHeight = logicalScrollTop + logicalViewportHeight + logicalMarginHeight;
    
    // Base minimum size (viewport + margin in logical coordinates)
    const baseWidth = logicalViewportWidth + logicalMarginWidth;
    const baseHeight = logicalViewportHeight + logicalMarginHeight;
    
    // Final size is the maximum of all constraints (in logical coordinates)
    const newWidth = Math.max(baseWidth, scrollRequiredWidth, contentMinWidth);
    const newHeight = Math.max(baseHeight, scrollRequiredHeight, contentMinHeight);
    
    // Set canvas dimensions (these are logical dimensions, will be scaled by transform)
    canvas.style.width = `${newWidth}px`;
    canvas.style.height = `${newHeight}px`;
}

// Recalculate content bounds from all node instances
export function updateContentBounds() {
    contentBounds = {
        minX: Infinity,
        minY: Infinity,
        maxX: -Infinity,
        maxY: -Infinity
    };
    
    for (const nodeInstance of registry.nodeInstances) {
        const container = nodeInstance.nodeEl?.container;
        if (!container) continue;
        
        const rect = container.getBoundingClientRect();
        const canvasRect = canvas.getBoundingClientRect();
        
        // Convert to canvas coordinates
        const x = (rect.left - canvasRect.left) / currentZoom;
        const y = (rect.top - canvasRect.top) / currentZoom;
        const right = x + rect.width / currentZoom;
        const bottom = y + rect.height / currentZoom;
        
        contentBounds.minX = Math.min(contentBounds.minX, x);
        contentBounds.minY = Math.min(contentBounds.minY, y);
        contentBounds.maxX = Math.max(contentBounds.maxX, right);
        contentBounds.maxY = Math.max(contentBounds.maxY, bottom);
    }
    
    // Trigger resize to potentially shrink canvas if content was removed
    updateCanvasSize();
}

// Called when zoom level changes
export function setZoom(zoomLevel) {
    currentZoom = zoomLevel;
    updateCanvasSize();
}

// Get the scroll wrapper for external use (e.g., for grid calculations)
export function getScrollWrapper() {
    return scrollWrapper;
}

// Get current scroll position
export function getScrollPosition() {
    if (!scrollWrapper) return { left: 0, top: 0 };
    return {
        left: scrollWrapper.scrollLeft,
        top: scrollWrapper.scrollTop
    };
}
