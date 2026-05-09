# FoundryEngine
A lightweight modular C++ game engine focused on custom physics simulation, rendering architecture, and engine programming fundamentals.
Built from scratch using:
- C++
- OpenGL
- GLFW
- GLAD

---

## Overview
FoundryEngine is a personal engine programming project designed to explore real-time rendering, rigidbody physics, collision systems, and engine architecture without relying on external game engines.
The project emphasizes:
- modular engine design
- low-level graphics programming
- custom physics simulation
- runtime-safe object handling
- clean API separation between systems

---

## Current Features
### Graphics Engine
- OpenGL rendering pipeline
- Mesh rendering system
- Shader compilation and management
- Textured and flat-color materials
- Camera movement, follow and orbit systems
- Debug line rendering
- GPU buffer abstraction (VAO/VBO/EBO)

### Physics Engine
- Custom rigidbody system
- Sphere and OBB collision detection
- Listener-based collision enter/stay/exit events
- Impulse-based collision resolution
- Position correction solver

---

## Engine Architecture Features
- Modular DLL-style engine interfaces
- Handle-based object referencing
- Generation-safe slot map storage
- Separation between public APIs and internal systems
- RAII and smart pointer ownership
- Internal collision tracking system

---

## Architecture
The engine is divided into independent modules:
```text
FoundryEngine
┌── App
│
├── Core
│   ├── Math
│   ├── Geometry
│   └── Utilities
│
├── GraphicsEngine
│   ├── Rendering
│   ├── MeshBuffers
│   ├── Shaders
│   └── Camera
│
├── PhysicsEngine
│   ├── Rigidbody System
│   ├── Collision Detection
│   ├── Impulse Solver
┴   └── Collision Events
```

### Handle-Based Object System
The engine uses generation-safe handles instead of exposing raw pointers publicly.
Example:
struct RigidbodyHandle
{
    uint32_t index;
    uint32_t generation;
};
This prevents:
- dangling references
- invalid object access
- stale pointers after slot reuse
Internally, objects are stored in slot arrays using smart pointers.

### Physics Pipeline
The physics simulation currently follows this pipeline:
Detect Collisions
        ↓
Solve Collision Impulses
        ↓
Correct Penetrations
        ↓
Generate Collision Events

Collision events support:
- Enter
- Stay
- Exit
through listener callbacks.

### Rendering Pipeline
- The renderer currently supports:
- flat colored and textured materials rendering
- dynamic mesh translation
- debug rendering
Mesh data is uploaded through GPU mesh buffers and rendered using OpenGL draw calls.

---

## Current Demo
The current demo includes:
- controllable physics player
- collision-enabled platforms
- orbit/follow camera
- debug visualization
- textured environment rendering

---

## Technologies
- C++17
- OpenGL 3.3
- GLFW
- GLAD
- stb_image

---

## Build Instructions
### Requirements
- C++17 compatible compiler
- CMake 3.20+
- OpenGL 3.3
- Visual Studio 2022 (recommended on Windows)

### Clone Repository
```bash
git clone https://github.com/nicolasDaniele/FoundryEngine.git
cd FoundryEngine
```
### Generate Project Files
```bash
cmake -B build
```
### Build
```bash
cmake --build build --config Release
```
### Run
The demo executable will be generated inside build/app/FoundryEngine/Release/
Double click on FoundryEngine.exe or
```bash
cd build/app/FoundryEngine/Release
FoundryEngine.exe
```

---

## Demo Controls
| Key | Action |
|-----|--------|
| W | Move Forward |
| A | Move Left |
| S | Move Backward |
| D | Move Right |
| Right Shift | Jump |
| Left Mouse Button | Orbit Camera |
| ESC | Exit Demo |

---

## Current State
FoundryEngine is currently an experimental in-development project.
Some systems are still work-in-progress, including:
- friction/restitution response
- rigidbody orientation
- advanced collision stability
The current demo focuses primarily on:
- rendering architecture
- collision detection
- rigidbody simulation
- engine modularity
- event systems

---

## Planned Features
### Physics
- Proper friction and restitution
- Angular velocity and torque
- Rigidbody orientation
- Broadphase collision detection
- Raycasting

### Graphics
- Material system
- Lighting
- Model importing

### Engine
- Graphic user interface
- Asset management
- ECS architecture
- Audio module
- Level editor

### Debugger
- Debug text printing console

---

## Project Goals
This project exists primarily as:
- an engine programming learning project
- a rendering and physics sandbox
- a portfolio project
- a long-term experimental engine architecture playground
The focus is understanding how game engine systems work internally rather than building production-ready tooling.

---

## AUTHOR
Developed by Nicolas Daniele as an ongoing engine programming project.