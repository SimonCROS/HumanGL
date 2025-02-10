# HumanGL

> Learning basics of **graphic programming**, focusing on  **hierarchical modeling** and **matrix stack manipulation** using **openGL** and **modern C++**

## 🚀 Getting Started

### **Launch**

To build and run the project using **CMake**.  
Ensure you have **CMake (>=3.22)** and a C++23 compatible compiler installed.

### **Camera control keys**

Key | Feature                       | -
---|-------------------------------| :---:
W | Increase **pitch**             | ⬆️
S | Decrease **pitch**             | ⬇️
A | Decrease **yaw**               | ⬅️
D | Increase **yaw**               | ➡️
R | Reset camera position          | 🔄
Q | Increase **camera distance**   | ➕
E | Decrease **camera distance**   | ➖

### **Screenshots**

< | >
---| ---
![screen 1](./resources/screenshots/screen1.png) |![screen 2](./resources/screenshots/screen2.png)
![screen 3](./resources/screenshots/screen3.png) |![screen 4](./resources/screenshots/screen4.png)
![screen 5](./resources/screenshots/screen5.png) |![screen 6](./resources/screenshots/screen6.png)
![screen 7](./resources/screenshots/screen7.png) |![screen 8](./resources/screenshots/screen8.png)
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

## 🤖 Project main steps

1. load model from .gltf file
2. ...

## ⚙️ Project architecture - Modern cpp RAII

## 📋 License 🖼️ 


