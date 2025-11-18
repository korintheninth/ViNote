customElements.define('insert-menu-option', insertMenuOption);

class insertMenu extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({ mode: "open" });

		this.shadowRoot.innerHTML = `
            <div class="insert-menu">
                <ul id="insertMenuList"></ul>
            </div>
        `
        this.position = {x: null, y: null};
        this.nodeTypes = registry.nodeTypes;
        this.listEl = this.shadowRoot.getElementById('insertMenuList');
    }

    connectedCallback() {
        this.nodeTypes.forEach(nodeType => {
            const li = document.createElement('li');
            li.textContent = nodeType.label;
            li.tabIndex = -1;
            li.dataset.typeId = nodeType.typeId;
            this.listEl.appendChild(li);
        });

    }
}
customElements.define('insert-menu', insertMenu);