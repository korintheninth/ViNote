import { loadPlugins } from "./node_manager.js";
import "./insert_menu.js";
import "./cursor.js";
import "./keybinds.js";
import { zoomIn, zoomOut } from "./zoom.js";
import { getMousePosition } from "./utils.js";
import { initInfiniteScroll } from "./infinite_scroll.js";

const canvas = document.getElementById('canvas');

if (!canvas) {
    throw new Error('Canvas container not found');
}

// Initialize infinite scroll
initInfiniteScroll();

loadPlugins()
    .then(() => {
        console.log('All plugins loaded.');
    })
    .catch(err => {
        console.error('Failed to load plugins folder:', err);
    });

const openInsertMenu = (left, top) => {
    const menu = document.createElement('insert-menu');
    menu.style.position = 'absolute';
    menu.style.left = left + 'px';
    menu.style.top = top + 'px';
    menu.position = { x: left, y: top };
    canvas.appendChild(menu);
};

canvas.addEventListener('click', event => {
    const { left, top } = getMousePosition(event);
    openInsertMenu(left, top);
});

canvas.addEventListener('wheel', event => {
    if (event.ctrlKey) {
        event.preventDefault();
        if (event.deltaY < 0) {
            zoomIn(event);
        } else {
            zoomOut(event);
        }
    }
});