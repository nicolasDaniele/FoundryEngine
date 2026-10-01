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
- Shader compilation and management, with automatic shared-define injection (e.g. `MAX_LIGHTS`) so shaders stay in sync with engine-side constants
- Flat-color and textured materials, each available in lit and unlit variants
- Blinn-Phong lighting system (directional, point and spot lights), driven by a single per-frame uniform buffer shared across every lit shader
- Quaternion-based mesh rotation
- Camera movement, orbit, and a follow system with independently configurable per-axis distance
- Debug line rendering, including colored debug shapes (e.g. point light gizmos, rotation-aware collider wireframes) kept separate from the default single-color debug batch
- GPU buffer abstraction (VAO/VBO/EBO)

### Physics Engine
- Custom rigidbody system with quaternion-based orientation
- Sphere and OBB collision detection, rotation-aware
- Listener-based collision enter/stay/exit events
- Impulse-based collision resolution, including angular response (torque accumulation, cached local inertia tensor rotated into world space)
- Rolling resistance for spherical rigidbodies (contact-gated, independent from Coulomb friction and from angular damping)
- Runtime-configurable per-rigidbody mass, friction, restitution, linear damping and angular damping
- Position correction solver

---

## Engine Architecture Features
- Modular DLL-style engine interfaces
- Handle-based object referencing, with O(1) free-list reuse of slots
- Generation-safe slot map storage
- Separation between public APIs and internal systems
- RAII and smart pointer ownership
- Internal collision tracking system
- Shared math library (`Core`: vectors, matrices, quaternions) used consistently across the Graphics and Physics modules

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
│   ├── Lighting
│   └── Camera
│
├── PhysicsEngine
│   ├── Rigidbody System
│   ├── Collision Detection
│   ├── Impulse Solver
│   └── Collision Events
│
┴── Debugger
    └── Debug Visualization
```

### Handle-Based Object System
The engine uses generation-safe handles instead of exposing raw pointers publicly.
Example:
```
struct RigidbodyHandle
{
    uint32_t index;
    uint32_t generation;
};
```
This prevents:
- dangling references
- invalid object access
- stale pointers after slot reuse

Internally, objects are stored in slot arrays using smart pointers, with a free list of reusable indices so creating/destroying objects at runtime doesn't require scanning the whole slot array.

### Physics Pipeline
The physics simulation currently follows this pipeline:
```text
Apply Forces
        ↓
Integrate Velocity
        ↓
Integrate Position / Orientation
        ↓
Detect Collisions
        ↓
Solve Collision Impulses
        ↓
Correct Penetrations
        ↓
Generate Collision Events
```

Position and orientation are integrated before collision detection runs, so detection always sees the current frame's actual geometry rather than the previous frame's.

Collision events support:
- Enter
- Stay
- Exit

through listener callbacks.

### Rendering Pipeline
The renderer currently supports:
- flat-colored and textured materials, each with lit and unlit variants
- Blinn-Phong lighting with directional, point, and spot lights
- dynamic mesh translation, rotation, and scale
- debug rendering, including per-shape colored debug geometry

Mesh data is uploaded through GPU mesh buffers and rendered using OpenGL draw calls. Lit shaders read camera and light data from a shared per-frame uniform buffer instead of per-mesh uniforms.

---

## Current Demo
The current demo includes:
- a controllable physics player that rolls and picks up rotation from contact
- collision-enabled platforms, including a rotatable platform for testing rotation-aware collision
- an orbit/follow camera with configurable per-axis follow distance
- lit and textured environment rendering
- point light visualization via debug gizmos
- debug visualization (colliders and lights)

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
| Space Bar | Jump |
| Left Mouse Button | Orbit Camera |
| T | Toggle Draw Debug |
| ESC | Exit Demo |

---

## Current State
FoundryEngine is currently an experimental in-development project.
Some systems are still work-in-progress, including:
- per-axis rotation locking for rigidbodies (e.g. letting a box be pushed without ever tipping over)
- simultaneous/warm-started contact solving (the current sequential impulse solver can leave a small torque bias when resolving multiple contact points on the same body in one step)
- broadphase collision detection
- advanced collision stability at high angular velocities (continuous collision detection / sub-stepping)

The current demo focuses primarily on:
- rendering architecture
- lighting
- collision detection
- rigidbody simulation, including rotation
- engine modularity
- event systems

---

## Planned Features
### Physics
- Broadphase collision detection
- Raycasting
- Continuous collision detection / sub-stepping for fast-rotating or fast-moving colliders

### Graphics
- Shadow mapping
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