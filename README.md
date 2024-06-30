# Rendering engine
Currently supporting OpenGL only.

The engine uses deferred shading, Screen Space Ambient Occlusion, HDR, Bloom and motion blur. The current scene renders the Crytek Sponza Atrium model from [McGuire, Computer Graphics Archive](http://graphics.cs.williams.edu/data/meshes.xml). The scene has 100 lights with boxes, a directional sunlight and shadows.

The scene's settings can be tweaked from a menu which is accessed by pressing the __ESC__. You can also change the between different debugging draw modes by pressing 1-6.

![alt text](http://fatsopanda.com/images/demo_640.png "demo screenshot")

## Documentation
CMake is used for building the project to ensure cross-platform building to be easier. The repo contains the needed dependencies for Windows building so there's no need to setup them by hand. For other environments, just follow the instructions found below.l

The project uses the following dependencies:
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [GLEW](https://github.com/nigels-com/glew)
- [GLM](http://glm.g-truc.net/0.9.8/index.html)
- [ASSIMP](http://www.assimp.org/)
- [stb_image](https://github.com/nothings/stb)
- [dear imgui,](https://github.com/ocornut/imgui)

### Rendering pipeline
The program follows a simple deferred shading pipeline:

1. Render scene models to a G-Buffer
 * Position and Depth (RGBA32F)
 * Normals (RGB16F)
 * Albedo Specular (RGBA)
 * Velocity (RG16F)
2. Render scene models to shadow map's depth texture
3. Create SSAO Textures and randomize point light locations
4. Render the lighting pass to a post-processing Framebuffer
 * Sample G-Buffer for Position, Normal and Specular information
 * Also bind the SSAO texture for SSAO
5. Blit G-Buffer's Depth buffer to the post-processing Framebuffer
6. Draw light objects to the post-processing Framebuffer
7. Render Skybox inside post-processing Framebuffer
8. Use post-processing effects

## Project contents
`./src/framework` project source files

`resources/shaders` shaders

`resources/models` models

`resources/textures` textures

`./cmake` CMake modules

`./lib` Static libraries for Windows builds.

`./includes` Headers and includes for project dependencies for Windows builds.

`./dlls` Required DLLs for Windows builds

## Building on Windows (Visual Studio project)
- Build the project with cmake and build the VS project
- All needed libraries and DLLs are included in the project
- The executable and DLLs will be under `./bin`

## Building on Linux
- Install cmake, sdl2, glew, glm, assimp with your favourite package manager
- Generate Makefile with CMake, build the project and run it

## OSX
- OSX is not supported currently as the used OpenGL version is higher than what OSX supports. Waiting for Metal implementations
