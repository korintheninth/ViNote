import { getZoomLevel } from "./zoom.js";
import { isSnapEnabled, snapToGrid } from "./cursor.js";

let canvas = document.getElementById('canvas');

export function getMousePosition(event) {
    const rect = canvas.getBoundingClientRect();
    let left = (event.clientX - rect.left) / getZoomLevel();
    let top = (event.clientY - rect.top) / getZoomLevel();
    
    if (isSnapEnabled()) {
        left = snapToGrid(left);
        top = snapToGrid(top);
    }
    
    return { left, top };
}