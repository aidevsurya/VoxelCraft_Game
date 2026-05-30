# VoxelCraft: A Pure C++ Minecraft Clone From Scratch

A high-performance, infinite 3D voxel world engine built completely from scratch in pure C++ using **Raylib** for hardware context, windowing, and low-level graphics mapping. This project was built **without the use of any commercial game engine** (Unity, Unreal, or Godot). All architectural pipelines—including custom AABB collision physics, runtime 3D raycasting, procedural noise generation, and memory-safe asset bindings—were written by hand.

## 🚀 Key Technical Achievements

* **No Game Engine Architecture:** Built entirely inside a custom C++ game loop wrapper, managing real-time delta timings, application state changes, and VRAM memory registers manually.
* **Procedural Infinite Terrain:** Implemented a deterministic 2D pseudo-random mathematical noise algorithm to generate infinite, dynamic hills, valleys, and block distributions.
* **Advanced Face Culling Optimization:** Designed an engine-side visibility pass that skips drawing fully enclosed, underground blocks. This drops the active vertex load by over 85%, preventing GPU bottlenecks.
* **Mathematical Physics Pipeline:** Hand-coded traditional, axis-separated AABB (Axis-Aligned Bounding Box) collision algorithms with rounding padding thresholds to guarantee solid wall interactions without clipping.
* **3D Grid Marching Raycast:** Created a microscopic 3D vector raycast system ($0.05$ unit increments up to a max 5-block reach) that continuously tracks crosshair screen intersections for pixel-perfect block destruction and placement overlays.
* **Synchronized First-Person Asset Pipeline:** Streamed and cached genuine `.glb` 3D assets (`diamond_axe`, `block_grass`, `block_tnt`) natively to VRAM with crispy pixel-perfect filtering (`TEXTURE_FILTER_POINT`) and edge clamp configurations.

## 🕹️ Controls Layout
* **W / A / S / D (or Arrow Keys):** True directional movement. (Features classic inverted strafe layout: A/Left moves right, D/Right moves left).
* **Spacebar:** Manual jumping over 1-block vertical obstacles.
* **Mouse Left-Click:** Triggers a synchronized right-hand axe chop animation that shatters the targeted block exactly at the mid-point impact frame.
* **Mouse Right-Click:** Instantly places a custom explosive TNT block on the highlighted surface face.
* **Crosshair Selection Matrix:** Wraps a sharp, dynamic 3D wireframe box boundary highlight around whatever voxel the player points toward.

## 🛠️ Build & Compilation

To compile and link the multi-source structural files directly using the GCC compiler chain, run the following target command in your terminal window:

```bash
g++ src/main.cpp src/main_menu.cpp src/world.cpp src/player.cpp -o game.exe -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm
```

📂 Asset Requirements
Ensure the following packed binary .glb model modules sit inside the root file directory alongside the compiled executable target path:

block_grass.glb

block_stone.glb

block_tnt.glb

iron_ore.glb

diamond_axe.glb


---

# Game Play Video:


https://github.com/user-attachments/assets/a1e3ed36-6a1d-4f9a-813b-bb76edbc4f36



