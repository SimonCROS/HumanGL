# HumanGL

> Learning basics of **graphic programming**, focusing on  **hierarchical modeling** and **matrix stack manipulation** using **openGL** and **modern C++**

## 🚀 Getting Started

### **Launch**

To build and run the project using **CMake**.  
Ensure you have **CMake (>=3.22)** and a C++23 compatible compiler installed.

### **Camera control keys**

Key | Feature                       | ⚡
---|-------------------------------| :---:
W | Increase **pitch**             | ⬆️
S | Decrease **pitch**             | ⬇️
A | Decrease **yaw**               | ⬅️
D | Increase **yaw**               | ➡️
R | Reset camera position          | 🔄
Q | Increase **camera distance**   | ➕
E | Decrease **camera distance**   | ➖

### **Screencasts**

🎥 | 🎥
:---:| :---:
 | 
![screen 1](./resources/screenshots/golem01.png) |![screen 2](./resources/screenshots/golem01.png)
Display or hide | Object specific polygon display  
![screen 3](./resources/screenshots/golem01.png) |![screen 4](./resources/screenshots/golem01.png)
Camera zoom | Camera moves 
![screen 5](./resources/screenshots/golem01.png) |![screen 6](./resources/screenshots/frog01.png)
Choose camera focus | Jump animation !
![screen 7](./resources/screenshots/golem01.png) |![screen 8](./resources/screenshots/golem01.png)
Select golem animation | Set golem part size properties
![screen 9](./resources/screenshots/golem01.png) |![screen 10](./resources/screenshots/frog01.png)
Set size properties of any node | Coa !
---

## 🗂️ Project Structure

```
HumanGL/
├── src/               # Source code
├── tests/             # Testing ft_glm::functions versus glm::functions
├── lib/               # External libraries (stb, imgui, glad)
├── cmake/             # CMake configuration files
├── build/             # Build directory (generated)
└── CMakeLists.txt     # Main build configuration
```

---

## 📦 Dependencies

**GLFW**  
GLFW is a library for creating windows and managing input (mouse, keyboard) in OpenGL-based applications.

**glad**  
Glad is an OpenGL extension loader, managing OpenGL versions and extensions in applications.

**GLM / ft_glm**  
**GLM** is a math library for graphics programming. We use a custom variant called **ft_glm**, designed to replace the standard GLM. While GLM is included in the project, it's used solely for testing purposes to compare performance and precision with ft_glm.

**stb**  
stb is a collection of lightweight utility libraries, mainly for image handling, compression. Used in project to load textures form PNG or JPEG sources files.

**imgui**  
ImGui gives us an easy access to creation of interactive user interfaces.

---

## ✅ Project main steps

1. **Set up the OpenGL environment**  
   Initialize a basic OpenGL setup by creating the window context through GLFW. Configure essential components such as **shaders** for rendering graphics and set up the **camera** to control the view and perspective within the 3D scene.
2. **Load a 3D animated object from a `.gltf` file**  
   Our [js script](./resources/models/gltf_to_microgltf.js) parses a `.gltf` file and its associated binary data into a custom `microgltf` struct. This process imports all defined animations, textures, transformation matrices, and shapes, making them ready for rendering within the application. Original .gltf file are at [/resources/models/](./resources/models/) and parsed models are at [/src/Models/](./src/Models/).
3. **Run the graphic engine**  
   The main class [`Engine`](./src/Engine/Engine.h) is designed to instantiate and manage all the required [`Objects`](./src/Engine/Object.h). For each frame, the engine calls the methods `willUpdate()`, `update()`, `render()`, and `postRender()` in this order for all contained objects, defining the **lifecycle of the graphics loop**.
4. **Animate and  transform the hierachical model**  
   The interface is connected to the [`MeshRenderer`](./src/Components/MeshRenderer.h) of the [Minecraft Golem](https://sketchfab.com/3d-models/mutant-iron-golem-minecraft-animated-a139255741d14274961e5b51d3c8c04f) model, allowing you to select and play different animations. Additionally, each node that makes up the hierarchical model can be targeted to apply custom scaling along the X, Y, and Z axes.

## ⚙️ Code architecture - Modern cpp RAII

## 📋 🖼️ ⚡ 👉 🎥 🗔 ✨ ✅ ⚙️


