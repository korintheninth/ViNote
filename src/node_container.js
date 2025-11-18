import { removeConnections } from "./line_canvas.js";
import { connectNodes, connectRelation, deleteNodeInstance, registry } from "./node_manager.js";

export class relationConnection extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		
		this.shadowRoot.innerHTML = `
			<link rel="stylesheet" href="../css/index.css">
			<button tabindex = "-1" class="relation"></button>
		`;

		this.addEventListener("click", e => {
			e.stopPropagation();
			connectRelation(this);
		});

		this.addEventListener('contextmenu', e => {
			e.preventDefault();
			removeConnections(this);
		})
	}
}
customElements.define('relation-connection', relationConnection);

export class inputConnection extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		
		this.shadowRoot.innerHTML = `
			<link rel="stylesheet" href="../css/index.css">
			<button tabindex = "-1" class="input"></button>
		`;

		this.addEventListener("click", e => {
			e.stopPropagation();
			connectNodes(this);
		});

		this.addEventListener('contextmenu', e => {
			e.preventDefault();
			this.deleteConnections();
		})

		this.type = null;
		this.index = null;
		this.node = null;
		this.value = null;
		this.sender = null;
	}

	setType(type) {
		this.type = type;
	}

	deleteConnections() {
		removeConnections(this);
		this.sender?.targets.delete(this);
		this.sender = null;
	}

	changed(value) {
		if (value != this.value) {
			this.value = value;
			this.node?.inputChange(this.index, value);
		}
	}
}
customElements.define('input-connection', inputConnection);

export class outputConnection extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		
		this.shadowRoot.innerHTML = `
			<link rel="stylesheet" href="../css/index.css">
			<button tabindex = "-1" class="output"></button>
		`;
		
		this.addEventListener("click", e => {
			e.stopPropagation();
			connectNodes(this);
		});

		this.addEventListener('contextmenu', e => {
			e.preventDefault();
			this.deleteConnections();
		})
		
		this.type = null;
		this.index = null;
		this.node = null;
		this.value = null;
		this.targets = new Set();
	}

	setType(type) {
		this.type = type;
	}

	deleteConnections() {
		removeConnections(this);
		for (const target of this.targets) {
			target.sender = null;
			this.targets.delete(target);
		}
	}

	changed(value) {
		if (value != this.value) {
			this.value = value;
			for (const target of this.targets) {
				target.changed(this.value);
			}
		}
	}
}
customElements.define('output-connection', outputConnection);

class nodeContainer extends HTMLElement {
	constructor() {
		super();
		this.attachShadow({mode: 'open'});
		
		this.shadowRoot.innerHTML = `
			<link rel="stylesheet" href="../css/index.css">
			<div class="relation-stack">
				<div class="relation-slot relation-slot--top">
					<relation-connection></relation-connection>
				</div>
				<div class="wrapper">
					<div class="inputs">
						<slot name="input_slot"></slot>
					</div>
					<div class="slot-wrapper">
						<slot name="node_slot"></slot>
					</div>
					<div class="outputs">
						<slot name="output_slot"></slot>
					</div>
				</div>
				<div class="relation-slot relation-slot--bottom">
					<relation-connection></relation-connection>
				</div>
			</div>
		`;

		this.node = null;
		this.outputs = [];
		this.inputs = [];
		this.relations = Array.from(this.shadowRoot.querySelectorAll('relation-connection'));
	}

	connectedCallback() {
		this.addEventListener("click", e => e.stopPropagation());
		this.addEventListener("keydown", e => {
			if (e.key === "Delete") {
				this.inputs.forEach(input => input.deleteConnections())
				this.outputs.forEach(output => output.deleteConnections())
				this.relations.forEach(relation => removeConnections(relation))
				deleteNodeInstance(this.node.nodeId);
			}
		});
		this.pointerInit = null;
		this.addEventListener("pointerdown", e => this.startMove(e));
		this.addEventListener("pointermove", e => this.move(e));
		this.addEventListener("pointerup", e => this.stopMove(e));
		this.addEventListener("pointerleave", e => this.move(e));
	}

	setChild(childElem) {
		this.node = childElem;
		childElem.slot = 'node_slot';
		this.appendChild(childElem);
	}

	setInputs(inputs) {
		inputs.forEach((input, index) => {
			const inp = document.createElement('input-connection');
			inp.slot = 'input_slot';
			inp.type = input;
			inp.index = index;
			inp.node = this.node;
			this.node?.inputs.push(inp);
			this.appendChild(inp);
			this.inputs.push(inp);
		});
	}

	startMove(e) {
		this.pointerInit = {x: e.clientX, y: e.clientY};
		e.stopPropagation();
	}
	move(e) {
		if (this.pointerInit === null) {
			return;
		}
		const offsetx = e.clientX - this.pointerInit.x;
		const offsety = e.clientY - this.pointerInit.y;
		registry.nodeInstances[this.node.nodeId].position.x += offsetx;
		registry.nodeInstances[this.node.nodeId].position.y += offsety;
		this.style.left = `${parseFloat(this.style.left) + offsetx}px`;
		this.style.top = `${parseFloat(this.style.top) + offsety}px`;
		this.pointerInit.x = e.clientX;
		this.pointerInit.y = e.clientY;
		e.stopPropagation();
	}

	stopMove(e) {
		this.pointerInit = null;
		e.stopPropagation();
	}

	setOutputs(outputs) {
		outputs.forEach((output, index) => {
			const out = document.createElement('output-connection');
			out.slot = 'output_slot';
			out.type = output;
			out.index = index;
			out.node = this.node;
			this.node?.outputs.push(out);
			this.appendChild(out);
			this.outputs.push(out);
		});
	}
}

customElements.define('node-container', nodeContainer);
