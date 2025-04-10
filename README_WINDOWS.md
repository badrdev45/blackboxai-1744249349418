# Setting Up and Running the DirectX 11 FPS Game on Windows

## Prerequisites
1. Windows 10 or later
2. Visual Studio 2019 or later with:
   - Desktop development with C++ workload
   - Windows 10 SDK
   - DirectX 11 SDK (included in Windows SDK)

## Setup Instructions

1. Create a new Visual Studio Project:
   - Open Visual Studio
   - Create New Project
   - Select "Windows Desktop Application"
   - Name it "FPSGame"

2. Copy all the source files from the `src` folder to your project:
   ```
   main.cpp
   Game.h / Game.cpp
   Renderer.h / Renderer.cpp
   Camera.h / Camera.cpp
   Input.h / Input.cpp
   Player.h / Player.cpp
   UIOverlay.h / UIOverlay.cpp
   ```

3. Copy the shader files to a `shaders` folder in your project directory:
   ```
   shaders/VertexShader.hlsl
   shaders/PixelShader.hlsl
   ```

4. Configure Project Settings:
   - Right-click the project in Solution Explorer
   - Select Properties
   - Set Configuration to "All Configurations"
   - Under C/C++ > General:
     - Add Additional Include Directories: `$(ProjectDir)`
   - Under Linker > Input:
     - Add Additional Dependencies:
       ```
       d3d11.lib
       dxgi.lib
       d3dcompiler.lib
       ```

## Building and Running

1. Set the build configuration to "Release" or "Debug"
2. Build Solution (F7)
3. Run the game (F5)

## Controls

- WASD: Move forward, left, backward, right
- Mouse: Look around
- Space: Jump
- ESC: Exit game
- Left Mouse Button: Shoot

## Features

1. First-Person Movement:
   - Smooth WASD movement
   - Mouse-look camera control
   - Jumping mechanics with basic physics
   - Collision detection

2. 3D Graphics:
   - DirectX 11 rendering
   - Basic lighting system
   - Textured walls and floor
   - Simple geometric objects

3. User Interface:
   - Crosshair
   - Health display
   - Ammo counter
   - Main menu and pause menu

## Troubleshooting

1. If the game fails to start:
   - Ensure DirectX 11 is properly installed
   - Update graphics drivers
   - Verify Windows SDK installation

2. If you see compilation errors:
   - Check Visual Studio installation includes C++ desktop development
   - Verify Windows SDK is properly installed
   - Make sure all project dependencies are set correctly

3. If shaders fail to compile:
   - Verify shader files are in the correct location
   - Check shader syntax matches DirectX 11 HLSL requirements

## Performance Tips

1. Run in Release mode for better performance
2. Ensure your graphics drivers are up to date
3. Close other resource-intensive applications

## System Requirements

- OS: Windows 10 or later
- CPU: Intel Core i3/AMD Ryzen 3 or better
- GPU: DirectX 11 compatible graphics card
- RAM: 4GB minimum
- Storage: 100MB free space

## Additional Notes

- The game uses DirectX 11 for rendering, ensuring good performance on modern Windows systems
- All 3D models and textures are generated procedurally for simplicity
- The code is well-commented and structured for easy understanding and modification
