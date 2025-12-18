import { registry, addNodeInstance } from "./node_manager.js";
import { getZoomLevel } from "./zoom.js";

class insertMenu extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({ mode: "open" });

		this.shadowRoot.innerHTML = `
            <link rel="stylesheet" href="../css/index.css">
            <div class="insert-menu">
                <div id="insertMenuList"></div>
            </div>
        `
        this.position = {x: null, y: null};
        this.nodeTypes = registry.nodeTypes;
        this.listEl = this.shadowRoot.getElementById('insertMenuList');
        this.addEventListener("mouseleave", () => this.remove())
    }

    connectedCallback() {
        this.style.transform = `scale(${1 / getZoomLevel()})`;
        this.style.transformOrigin = `0 0`;
        this.nodeTypes.forEach(nodeType => {
            const button = document.createElement('button');
            button.textContent = nodeType.label;
            button.tabIndex = -1;
            button.dataset.typeId = nodeType.typeId;
            button.addEventListener('click', e => {
                e.stopImmediatePropagation()
                const left = this.position.x || 0;
                const top = this.position.y || 0;
                const nodeInstance = {
                    typeId: nodeType.typeId,
                    position: {
                        x: left,
                        y: top
                    },
                };
                addNodeInstance(nodeInstance);
                this.remove();
            });
            this.listEl.appendChild(button);
        });

    }
}
customElements.define('insert-menu', insertMenu);