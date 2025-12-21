import { toggleSnapCursor } from './cursor.js';

// Keybinding definitions
const keybinds = {
    'g': toggleSnapCursor,
};

// Handle keydown events
document.addEventListener('keydown', (event) => {
    const key = event.key.toLowerCase();
    
    // Skip if user is typing in an input field
    if (event.target.tagName === 'INPUT' || event.target.tagName === 'TEXTAREA') {
        return;
    }
    
    const action = keybinds[key];
    if (action) {
        action();
    }
});

// Export for dynamic keybind registration
export function registerKeybind(key, callback) {
    keybinds[key.toLowerCase()] = callback;
}

export function unregisterKeybind(key) {
    delete keybinds[key.toLowerCase()];
}

