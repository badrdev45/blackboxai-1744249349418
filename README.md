# First-Person Shooter Game

A DirectX 11-based first-person shooter game implemented in C++. This project demonstrates basic FPS mechanics including player movement, camera control, and shooting mechanics.

## Features

- First-person camera control with mouse look
- WASD movement controls
- Jumping mechanics with basic physics
- Modern UI system with main menu, HUD, and pause menu
- DirectX 11 rendering with basic lighting
- Keyboard and mouse input handling

## Prerequisites

- Windows OS
- DirectX 11 compatible graphics card
- Visual Studio 2019 or later with C++ development tools
- Windows SDK 10.0 or later

## Project Structure

```
├── src/
│   ├── main.cpp           # Application entry point and window creation
│   ├── Game.h/cpp        # Main game logic and state management
│   ├── Renderer.h/cpp    # DirectX 11 rendering system
│   ├── Camera.h/cpp      # First-person camera implementation
│   ├── Input.h/cpp       # Input handling system
│   ├── Player.h/cpp      # Player movement and shooting mechanics
│   └── UIOverlay.h/cpp   # User interface rendering
├── shaders/
│   ├── VertexShader.hlsl # Vertex shader for 3D rendering
│   └── PixelShader.hlsl  # Pixel shader with basic lighting
└── README.md
```

## Controls

- **W/A/S/D**: Move forward/left/backward/right
- **Space**: Jump
- **Mouse**: Look around
- **Left Mouse Button**: Shoot
- **Escape**: Pause game/Show menu

## Building the Project

1. Open the solution in Visual Studio
2. Ensure the Windows SDK is properly configured
3. Build the solution in Release or Debug configuration
4. Run the executable

## Implementation Details

### Rendering System
- DirectX 11 based rendering
- Basic lighting system with ambient and directional light
- Support for textures and basic materials

### Input System
- Raw input processing for smooth mouse movement
- Keyboard state tracking
- Support for both toggle and hold input styles

### Camera System
- First-person perspective camera
- Smooth mouse look with configurable sensitivity
- Proper view frustum setup for 3D rendering

### Player System
- Physics-based movement
- Collision detection (basic implementation)
- Shooting mechanics with cooldown

### UI System
- Modern, clean interface design
- Support for different game states (menu, playing, paused)
- Dynamic HUD elements (health, ammo)

## Future Improvements

1. Add weapon system with multiple weapon types
2. Implement enemy AI
3. Add sound effects and background music
4. Enhance graphics with post-processing effects
5. Add multiplayer support
6. Implement save/load system
7. Add more advanced physics and collision detection

## Contributing

Feel free to fork this project and submit pull requests for any improvements.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
