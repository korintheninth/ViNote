# ViNote

A visual node-based editor built with Electron, featuring a flexible plugin system for creating custom node types.

## Features

- 🎨 Visual node-based interface
- 🔌 Plugin system for extensibility
- 🔗 Node connections and relationships
- 📦 Modular architecture

## Project Structure

```
vinote/
├── src/              # Main application source code
│   ├── main.js      # Electron main process
│   ├── canvas.js    # Canvas rendering
│   ├── node_manager.js  # Node management and plugin loading
│   ├── node_container.js  # Node container components
│   └── ...
├── plugins/         # Plugin directory
│   └── base/        # Base plugin with example nodes
├── html/            # HTML templates
├── css/             # Stylesheets
└── package.json     # Project configuration
```

## Plugin System

ViNote supports a plugin-based architecture. Plugins are located in the `plugins/` directory and must include:

- `plugin.json` - Plugin manifest defining node types
- UI components - Custom elements for node visualization
- Logic modules - Node behavior and processing logic

### Example Plugin Structure

```
plugins/
└── my-plugin/
    ├── plugin.json
    ├── dist/
    │   ├── plugin.js        # Logic module
    │   └── myNodeUI.js      # UI component
    └── package.json
```

## License

MIT

## Author

Erdem Berat Mamati

