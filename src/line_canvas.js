import { getZoomLevel } from "./zoom.js";

const connectionLayer = document.getElementById('connection-layer');
const canvas = document.getElementById('canvas');

if (!connectionLayer || !canvas) {
  throw new Error('Canvas or connection layer missing');
}

connectionLayer.setAttribute('width', '100%');
connectionLayer.setAttribute('height', '100%');

const connections = new Set();

function calculateBezierPath(x1, y1, x2, y2, cx, cy) {
  const dx = x2 - x1;
  const dy = y2 - y1;
  const lineLen = Math.sqrt(dx * dx + dy * dy) || 1;
  
  const unitX = dx / lineLen;
  const unitY = dy / lineLen;
  
  const dist1 = Math.sqrt((cx - x1) ** 2 + (cy - y1) ** 2);
  const dist2 = Math.sqrt((x2 - cx) ** 2 + (y2 - cy) ** 2);
  
  const handleLen1 = dist1 * 0.2;
  const handleLen2 = dist2 * 0.2;
  
  const c1x = x1 + Math.abs(dx) * 0.3;
  const c1y = y1;
  const c2x = cx - unitX * handleLen1;
  const c2y = cy - unitY * handleLen1;
  
  const c3x = cx + unitX * handleLen2;
  const c3y = cy + unitY * handleLen2;
  const c4x = x2 - Math.abs(dx) * 0.3;
  const c4y = y2;
  
  return `M${x1},${y1} C${c1x},${c1y} ${c2x},${c2y} ${cx},${cy} C${c3x},${c3y} ${c4x},${c4y} ${x2},${y2}`;
}

export function updateLine({ line, out, inp, controlPoint, offsetX = 0, offsetY = 0 }) {
  const canvasRect = canvas.getBoundingClientRect();
  const outBtn = out.shadowRoot.querySelector('button');
  const inBtn = inp.shadowRoot.querySelector('button');
  const outRect = outBtn.getBoundingClientRect();
  const inRect = inBtn.getBoundingClientRect();
  const zoomLevel = getZoomLevel();
  
  const x1 = (outRect.left + outRect.width / 2 - canvasRect.left) / zoomLevel;
  const y1 = (outRect.top + outRect.height / 2 - canvasRect.top) / zoomLevel;
  const x2 = (inRect.left + inRect.width / 2 - canvasRect.left) / zoomLevel;
  const y2 = (inRect.top + inRect.height / 2 - canvasRect.top) / zoomLevel;
  
  const midX = (x1 + x2) / 2 + offsetX;
  const midY = (y1 + y2) / 2 + offsetY;
  
  line.setAttribute('d', calculateBezierPath(x1, y1, x2, y2, midX, midY));
  
  if (controlPoint) {
    controlPoint.setAttribute('cx', midX);
    controlPoint.setAttribute('cy', midY);
  }
}

function createControlPoint(conn) {
  const controlPoint = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
  controlPoint.setAttribute('r', '6');
  controlPoint.classList.add('control-point');
  controlPoint.style.pointerEvents = 'all';
  controlPoint.style.cursor = 'grab';
  
  let isDragging = false;
  let startX, startY;
  let initialOffsetX, initialOffsetY;
  
  controlPoint.addEventListener('click', (e) => {
    e.stopPropagation();
  });
  
  controlPoint.addEventListener('mousedown', (e) => {
    e.stopPropagation();
    isDragging = true;
    controlPoint.style.cursor = 'grabbing';
    
    const zoomLevel = getZoomLevel();
    startX = e.clientX / zoomLevel;
    startY = e.clientY / zoomLevel;
    initialOffsetX = conn.offsetX;
    initialOffsetY = conn.offsetY;
    
    const onMouseMove = (e) => {
      if (!isDragging) return;
      
      const zoomLevel = getZoomLevel();
      const currentX = e.clientX / zoomLevel;
      const currentY = e.clientY / zoomLevel;
      
      conn.offsetX = initialOffsetX + (currentX - startX);
      conn.offsetY = initialOffsetY + (currentY - startY);
      
      updateLine(conn);
    };
    
    const onMouseUp = () => {
      isDragging = false;
      controlPoint.style.cursor = 'grab';
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
    };
    
    document.addEventListener('mousemove', onMouseMove);
    document.addEventListener('mouseup', onMouseUp);
  });
  
  return controlPoint;
}

export function registerLine(out, inp, type) {
  const line = document.createElementNS('http://www.w3.org/2000/svg', 'path');
  line.classList.add(type);
  connectionLayer.appendChild(line);
  
  const conn = { 
    line, 
    out, 
    inp, 
    offsetX: 0, 
    offsetY: 0,
    controlPoint: null
  };
  
  const controlPoint = createControlPoint(conn);
  conn.controlPoint = controlPoint;
  connectionLayer.appendChild(controlPoint);
  
  connections.add(conn);
  updateLine(conn);
}

export function removeLine(out, inp) {
  for (const conn of connections) {
    if ((conn.out === out && conn.inp === inp) || (conn.out === inp && conn.inp === out)) {
      conn.line.remove();
      if (conn.controlPoint) {
        conn.controlPoint.remove();
      }
      connections.delete(conn);
      break;
    }
  }
}

export function removeConnections(connection) {
  for (const conn of connections) {
    if (conn.out === connection || conn.inp === connection) {
      conn.line.remove();
      if (conn.controlPoint) {
        conn.controlPoint.remove();
      }
      connections.delete(conn);
    }
  }
}

function refresh() {
  connections.forEach(updateLine);
  requestAnimationFrame(refresh);
}

requestAnimationFrame(refresh);
