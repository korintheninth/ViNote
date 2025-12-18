let zoomLevel = 1;
let zoomStep = 0.05;
let maxZoomLevel = 10;
let minZoomLevel = 0.1;
let maxSize = {width: 0, height: 0};
let canvas = document.getElementById('canvas');

export function getZoomLevel() {
    return zoomLevel
}

export function zoomIn(pos) {
    zoomLevel = Math.min(zoomLevel * (1 + zoomStep), maxZoomLevel);

    canvas.style.transform = `scale(${zoomLevel})`;
    canvas.style.width = `${window.innerWidth / zoomLevel}px`;
    canvas.style.height = `${window.innerHeight / zoomLevel}px`;
}

export function zoomOut(pos) {
    zoomLevel = Math.max(zoomLevel * (1 - zoomStep), minZoomLevel);

    canvas.style.transform = `scale(${zoomLevel})`;
    canvas.style.width = `${window.innerWidth / zoomLevel}px`;
    canvas.style.height = `${window.innerHeight / zoomLevel}px`;
}