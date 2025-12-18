import { getZoomLevel } from "./zoom.js";
let canvas = document.getElementById('canvas');

export function getMousePosition(event) {
    const rect = canvas.getBoundingClientRect();
    const left = event.clientX - rect.left;
    const top = event.clientY - rect.top;
    return { left: left / getZoomLevel(), top: top / getZoomLevel() };
}