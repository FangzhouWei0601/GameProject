# Project Astrelia: 2.5D Action RPG Prototype

## Technical Implementation Highlights

### OpenGL Rendering Pipeline
- **Vertex Shader**: Custom implementation supporting 2D transformations
  - Handles model-view-projection matrix calculations
  - Supports 2D and pseudo-3D rendering techniques
  - Uses modern OpenGL 4.3+ core profile

- **Fragment Shader**: Advanced texture and color rendering
  - Dynamic color blending capabilities
  - Supports transparency and texture mapping
  - Flexible uniform-based color and alpha control

### Rendering Features
- Multiple texture support
- Sprite-based animation system
- Layer-based rendering architecture
- Dynamic camera transformations

### Core Rendering Techniques
- Quad rendering with full transformation support
- Texture region mapping
- Color tinting and transparency effects

## Key Technical Components

### Input Systems
- Keyboard and gamepad input management
- Multi-device support
- Configurable action mappings

### Game Mechanics
- 2.5D isometric movement
- Collision detection system
- Mechanism and trigger interactions
- Dynamic area transitions

### Performance Optimizations
- Efficient resource management
- Modular system design
- Low-overhead rendering techniques

## Development Environment
- Language: Modern C++ (C++17/20)
- Graphics API: OpenGL 4.3+
- Window Management: GLFW
- OpenGL Loading: GLAD
- Mathematics: GLM
- Audio: IrrKlang

## Building the Project
1. Clone repository
2. Initialize submodules
3. Use CMake to generate project files
4. Compile with Visual Studio or preferred compiler

## Dependencies
- OpenGL 4.3+
- GLFW 3.3+
- GLAD
- GLM
- IrrKlang
- nlohmann/json

## Future Roadmap
1. Enhanced PBR lighting
2. More complex game mechanics
3. Expanded procedural generation techniques
4. Performance and rendering optimizations

---

**Note**: This prototype demonstrates core game development techniques and OpenGL rendering principles.
