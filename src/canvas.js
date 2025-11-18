import { addNodeInstance, getNextId, getNodeType, loadPlugins } from "./node_manager.js";

const canvas = document.getElementById('canvas');

if (!canvas) {
    throw new Error('Canvas container not found');
}
loadPlugins()
    .then(() => {
        console.log('All plugins loaded.');
    })
    .catch(err => {
        console.error('Failed to load plugins folder:', err);
    });

const createNote = (left, top) => {
    const nodeInstance = {
        typeId: 'functionNode',
        position: {
            x: left,
            y: top
        },
    }
    addNodeInstance(nodeInstance);
};

canvas.addEventListener('click', (event) => {
    const rect = canvas.getBoundingClientRect();
    const left = event.clientX - rect.left;
    const top = event.clientY - rect.top;

    createNote(left, top);
});
