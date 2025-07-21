# ViNote

A C note-taking application with OpenGL rendering.

## Features

- Text rendering with FreeType
- OpenGL-based graphics
- File dialogs for note management
- Modern UI with custom widgets

## Dependencies

- OpenGL
- GLEW (OpenGL Extension Wrangler Library)
- GLFW (Graphics Library Framework)
- FreeType (Font rendering library)

## Building

To build the project, run:

```bash
make
```

The executable will be created in the `build/Debug/` directory.

## Project Structure

- `src/` - Source code files
- `libs/` - Header files and external libraries
- `fonts/` - Font files
- `textures/` - Image assets
- `build/` - Build output directory
- `shaders/` - GLSL shader files

## Requirements

- C compiler with C99 support
- Make build system
- Windows (currently configured for Windows builds)

## License

This project is licensed under the Mozilla Public License 2.0 - see the [LICENSE](LICENSE) file for details.
