const connectionLayer = document.getElementById('connection-layer');
const canvas = document.getElementById('canvas');

if (!connectionLayer || !canvas) {
  throw new Error('Canvas or connection layer missing');
}

connectionLayer.setAttribute('width', '100%');
connectionLayer.setAttribute('height', '100%');

const connections = new Set();

export function updateLine({ line, out, inp }) {
  const canvasRect = canvas.getBoundingClientRect();
  const outBtn = out.shadowRoot.querySelector('button');
  const inBtn = inp.shadowRoot.querySelector('button');
  const outRect = outBtn.getBoundingClientRect();
  const inRect = inBtn.getBoundingClientRect();
  const x1 = outRect.left + outRect.width / 2 - canvasRect.left;
  const y1 = outRect.top + outRect.height / 2 - canvasRect.top;
  const x2 = inRect.left + inRect.width / 2 - canvasRect.left;
  const y2 = inRect.top + inRect.height / 2 - canvasRect.top;
  line.setAttribute('d', `M${x1},${y1} C${x1 + 300},${y1} ${x2 - 300},${y2} ${x2},${y2}`);
}

export function registerLine(out, inp, type) {
  const line = document.createElementNS('http://www.w3.org/2000/svg', 'path');
  line.classList.add(type);
  connectionLayer.appendChild(line);
  const conn = { line, out, inp };
  connections.add(conn);
  updateLine(conn);
}

export function removeLine(out, inp) {
  for (const conn of connections) {
    if ((conn.out === out && conn.inp === inp) || (conn.out === inp && conn.inp === out)) {
      conn.line.remove();
      connections.delete(conn);
      break;
    }
  }
}

export function removeConnections(connection) {
  for (const conn of connections) {
    if (conn.out === connection || conn.inp === connection) {
      conn.line.remove();
      connections.delete(conn);
    }
  }
}

function refresh() {
  connections.forEach(updateLine);
  requestAnimationFrame(refresh);
}

requestAnimationFrame(refresh);
