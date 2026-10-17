# Falling Engine


https://github.com/user-attachments/assets/7fbc6674-4783-4154-81bf-a48ca887804b


A small and performant data-oriented C++ game engine with:
1. Centralized Asset Database
2. Sparse-Set Entity Component System
3. A (mostly) Stateless OpenGL Rendering Backend

I've been working on Falling Engine for many weeks now, really trying
to understand the design principles behind game engines and high-performance
realtime software overall instead of just one-shotting this with an LLM.

And also it's just really cool to be able to say that I'm making a game engine!

Now that I've finished the renderer, completing the big 3 systems required to
put a mesh on the screen, I'm probably gonna split my attention across a
couple other projects I've been wanting to make.

For now, you can inspect the code (main execution is in `game/main.cpp`) and/or run the demo shown in the clip above.

## Performance Notes

I've just been testing performance with a simple FPS tracker + logger and the game demo.
The demo is a simple character model I made in Blender, ~400 triangles and 8 meshes, instantiated many
times to form a cube structure. Each model is also rotated by some amount of degrees randomly selected
from a narrow range.

Each model consists of a parent entity and 8 entities with mesh components, resulting in 9 entities per instantiation. Each parent entity's transform gets rotated, and the raw world transform is computed for
each mesh-containing child entity before being submitted to the renderer.

Debug builds run `1000` models (`9000` entities) at 35 FPS. \
Release builds run `4096` models (`36864` entities) at 65 FPS.

Machine Specs
- GPU: NVIDIA GeForce RTX 4060
- CPU: Intel Core i7-13620H
- Display: 1920x1200 165hz (tests ran with maximized window)

## How to Run the Demo

These instructions target Debian/Ubuntu Linux. I haven't tried it on a Windows machine yet (sorry!).

You will need:

- Git, CMake 3.16 or newer, Ninja, and a C++20-compatible compiler
- [vcpkg](https://github.com/microsoft/vcpkg) (for library dependencies)
- A machine with OpenGL 3.3 support
- Linux dev. packages used by GLFW and OpenGL

Install the system dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build git curl zip unzip tar \
  pkg-config libgl1-mesa-dev libglu1-mesa-dev libxinerama-dev \
  libxcursor-dev xorg-dev
```

Install vcpkg if you do not already have it, then set `VCPKG_ROOT` to its
absolute path:

```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
export VCPKG_ROOT=/absolute/path/to/vcpkg
```

Clone the repository, then configure and build a debug version of the game:

```bash
git clone https://github.com/ng-daniel/falling-engine.git falling-engine
cd falling-engine
cmake --preset debug
cmake --build build/debug --target game
```

*To build all executables (such as the asset browser/inspector and some
engine tests), run this instead of targeting the game specifically:*

```bash
cmake --build build/debug
```

The configure step reads `vcpkg.json` and installs the C++ dependencies
(OpenGL, GLFW, GLAD, GLM, and Dear ImGui) automatically. Then, from the
repository root, run:

```bash
./build/debug/game/game
```

For an optimized release build, use the release preset and its corresponding
build directory:

```bash
cmake --preset release
cmake --build build/release --target game
./build/release/game/game
```

Run the executable from the repository root because the demo loads assets from
`./game/assets`. Use `WASD` to move,
`Q/E` to move vertically, and the arrow keys to look around.
Close the window to exit.


## Roadmap

Goal
- make a minimum viable engine with solid fundamentals and expandability
- make some example projects
- develop a small singleplayer FPS game with this engine

Completed
- asset manager + serialization (BASICALLY DONE!!!)
- scenes, ECS, components (ALSO DONE!!!)
- renderer, (debug) camera (DONE!!!)
- profiler (to identify bottlenecks) (DONE, for CPU, no GPU profiling yet)
- input system (DONE)
- scripting and custom game logic (DONE)

MVP Remaining Items
- physics and collisions

QOL Features (in the order of how much I want to do them...)
- visual post-processing
- particle system
- audio system
-
-
- UI system

Really Cool Stuff (probably not anytime soon)
- vulkan graphics backend
- scriptable runtime
- networking/multiplayer
