import { registerLine, removeLine } from './line_canvas.js';
import { inputConnection, outputConnection } from './node_container.js';

const nodeRequire =
  typeof window !== 'undefined' && typeof window.require === 'function'
    ? window.require
    : null;

if (!nodeRequire) {
  throw new Error('Node integration is required to load plugins');
}

const fs = nodeRequire('fs');
const path = nodeRequire('path');
const { pathToFileURL } = nodeRequire('url');

const pluginsDir = path.resolve(process.cwd(), 'plugins');

export const registry = {
    nodeTypes: new Map(),
    nodeInstances: []
};

const canvas = document.getElementById('canvas');

export async function loadPlugins() {

    window.vinoteAPI = {
        addNodeInstance,
    };

    if (!fs.existsSync(pluginsDir)) {
        console.warn(`Plugins directory "${pluginsDir}" not found - skipping load`);
        return;
    }

    const items = fs.readdirSync(pluginsDir, {withFileTypes: true});

    for (const item of items) {
        if (item.isDirectory()) {
            const pluginPath = path.join(pluginsDir, item.name);
            const manifestPath = path.join(pluginPath, 'plugin.json');
            if (!fs.existsSync(manifestPath)) {
                console.warn(`Plugin folder ${item.name} missing manifest - skipping`);
                continue;
            }
            const manifest = JSON.parse(fs.readFileSync(manifestPath, 'utf-8'));
            if (!Array.isArray(manifest.nodes)) {
                console.warn(`Plugin ${item.name} manifest has no nodes array - skipping`);
                continue;
            }
            for (const nodeDef of manifest.nodes) {
                if (!nodeDef.uiEntry || !nodeDef.logicEntry) {
                    console.warn(`Plugin ${item.name} node ${nodeDef.typeId} missing uiEntry or logicEntry - skipping`);
                    continue;
                }

                const uiModulePath = path.join(pluginPath, nodeDef.uiEntry);
                const logicModulePath = path.join(pluginPath, nodeDef.logicEntry);
                try {
                    await import(pathToFileURL(uiModulePath));
                    const logicModule = await import(pathToFileURL(logicModulePath));
                    
                    const found = logicModule.nodeTypes.find(n => n.typeId === nodeDef.typeId);
                    if (!found) {
                        console.warn(`Plugin ${item.name} logic module did not define typeId ${nodeDef.typeId}`);
                        continue;
                    }
                    registry.nodeTypes.set(nodeDef.typeId, {
                        typeId: nodeDef.typeId,
                        label: found.label || nodeDef.typeId,
                        inputs: found.inputs || [],
                        outputs: found.outputs || [],
                        uiTag: found.uiComponentTag,
                        logic: found.logic
                    });
                    console.log(`Loaded plugin node type: ${nodeDef.typeId}`);
                }
                catch (err) {
                    console.error(`Error loading plugin "${item.name}" nodeType "${nodeDef.typeId}":`, err);
                }
            }
        }
    }
}

export function addNodeInstance(nodeInstance) {
    const def = registry.nodeTypes.get(nodeInstance.typeId);
    if (!def) {
        console.error('unknown node type:', nodeInstance.typeId);
        return;
    }
    nodeInstance.nodeId = getNextId();

    const nodeEl = document.createElement(def.uiTag);

    nodeInstance.nodeEl = nodeEl;
    nodeEl.nodeId = nodeInstance.nodeId;

    registry.nodeInstances.push(nodeInstance);

    const container = document.createElement('node-container');
    container.style.position = 'absolute';
    container.style.left = nodeInstance.position?.x + 'px' || '0px';
    container.style.top = nodeInstance.position?.y + 'px' || '0px';

    container.setChild(nodeEl);
	container.setInputs(def.inputs);
	container.setOutputs(def.outputs);
    nodeEl.container = container;

	canvas.appendChild(container);

    def.logic.onCreate?.(nodeInstance);
    
	requestAnimationFrame(() => {
        nodeEl.focus?.();
    });
}

export function deleteNodeInstance(id) {
    const index = registry.nodeInstances.findIndex(n => n.nodeId === id);
    if (index === -1) {
        console.warn('attempted to delete missing node instance', id);
        return;
    }

    const [nodeInstance] = registry.nodeInstances.splice(index, 1);
    const def = registry.nodeTypes.get(nodeInstance.typeId);

    def?.logic?.onDelete?.(nodeInstance);
    nodeInstance.nodeEl?.container?.remove();
}

export function getNextId() {
    return registry.nodeInstances.length;
}

export function getNodeType(typeId) {
    return registry.nodeTypes.get(typeId);
}

export function connectNodes(connection) {
    if (typeof connectNodes.input === "undefined") {
        connectNodes.input = null;
    }
    if (typeof connectNodes.output === "undefined") {
        connectNodes.output = null;
    }
    if (connection instanceof inputConnection) {
        connectNodes.input?.classList.remove('selected');
        connectNodes.input = connection;
        connection.classList.add('selected');
    } else if (connection instanceof outputConnection) {
        connectNodes.output?.classList.remove('selected');
        connectNodes.output = connection;
        connection.classList.add('selected');
    }
    if (connectNodes.input && connectNodes.output) {
        if (connectNodes.input.sender) {
            removeLine(connectNodes.input.sender, connectNodes.input);
            connectNodes.input.sender.targets.delete(connectNodes.input);
        }
        if (connectNodes.input.node !== connectNodes.output.node) {
            connectNodes.input.sender = connectNodes.output;
            connectNodes.output.targets.add(connectNodes.input);
            registerLine(connectNodes.output, connectNodes.input, 'connection-line');
        }
        connectNodes.input.classList.remove('selected');
        connectNodes.output.classList.remove('selected');
        connectNodes.input = null;
        connectNodes.output = null;
    }
}

export function connectRelation(connection) {
    if (typeof connectRelation.first === "undefined") {
        connectRelation.first = null;
    }
    if (typeof connectRelation.second === "undefined") {
        connectRelation.second = null;
    }
    if (connectRelation.first == null) {
        connectRelation.first = connection;
        connectRelation.first.classList.add('selected');
    } else if (connection !== connectRelation.first){
        connectRelation.second = connection;
        removeLine(connectRelation.first, connectRelation.second);
        registerLine(connectRelation.first, connectRelation.second, 'relation-line');
        connectRelation.first.classList.remove('selected');
        connectRelation.first = null;
        connectRelation.second = null;
    }

}
