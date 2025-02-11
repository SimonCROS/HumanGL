<table style="border: 0px;">
    <tr>
        <td >
        <div style="border-radius:12px; overflow:hidden;"> 
            <img src="./resources/screenshots/humanGL.png" alt="start screen" style="width: 280px;">
            </div>
        </td>
        <td>
            <div style="margin-left: 20px; width:260px;">
                <h1><b>HumanGL</b></h1>
                <p> Learning basics of <b>graphic programming</b>, focusing on  <b>hierarchical modeling</b> and <b>matrix stack manipulation</b> using <b>openGL</b> and <b>modern C++</b></p>
            </div>
        </td>
    </tr>
</table>
<br/>

## 🚀 Getting Started

### **Launch**

To build and run the project using **CMake**.  
Ensure you have **CMake (>=3.22)** and a C++20 compatible compiler installed.

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

#### <u>Ui</u>

<img alt="" width="400" src="./resources/screenshots/general-ui.gif"/>

#### <u>Animations</u>

<img alt="golem animations" width="400" src="./resources/screenshots/golem-animations.gif"/>

#### <u>Frog</u>
![frog infinite jump](./resources/screenshots/frog-infinite-jump.gif)![frog infinite jump lines](./resources/screenshots/frog-infinite-jump-lines.gif)


## 🗂️ Project Structure

```
HumanGL/
├── resources/         # Assets (.gltf files, shaders, etc.)
├── tests/             # Testing ft_glm::functions versus glm::functions
├── lib/               # External libraries (stb, imgui, glad)
├── cmake/             # CMake configuration files
├── build/             # Build directory (generated)
├── CMakeLists.txt     # Main build configuration
└── src/               # Source code
    ├── .../
```

---

## 📦 Dependencies

👉 **GLFW**  
GLFW is a library for creating windows and managing input (mouse, keyboard).

👉 **glad**  
Glad is an OpenGL loader.

👉 **stb**  
stb is a collection of lightweight utility libraries, mainly for image handling and compression. Used in project to load textures from PNG or JPEG sources files.

👉 **imgui**  
ImGui gives us easy access to creation of interactive user interfaces.

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

## ⚙️ Modern Cpp principles

### **RAII**  
> The project follows the [**RAII**](https://en.cppreference.com/w/cpp/language/raii) (**R**esource **A**cquisition **I**s **I**nitialization) principle, which guarantees that the resource is available to any function that may access the object.

An example following the principle using a **move constructor** in class.  
```c++
Window(Window&& other) noexcept : m_window(std::exchange(other.m_window, nullptr)) { }
```

### Ressource guaranty with `std::expected`  

> *Note* : for compatibility purpose we used a custom [Expected](./src/Expected.h) class, modeled after **c++23 std::expected**  

For example, to ensure that a model is properly loaded, the result of the function call is wrapped in an `Expected` container. This way, the function either returns a valid value or an error message, ensuring a safer flow without the need for multiple try-catch blocks.
```c++
// Definition (Factory design Pattern)
static auto Create(int width, int height, const std::string& title) -> Expected<Window, std::string>;

// Instanciate
auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
        return Unexpected("Failed to create window: " + std::move(e_window).error());
```

### Ressource guaranty with `std::optional`  

The class template std::optional manages an optional contained value, a value that may or may not be present. [c++17](https://en.cppreference.com/w/cpp/utility/optional)

```c++
struct AnimatedTransform
{
    std::optional<ft_glm::vec3> translation;
    std::optional<ft_glm::quat> rotation;
    std::optional<ft_glm::vec3> scale;
};

if (anim.translation.has_value()) // ensure wrapped value isn't std::nullopt
```

## 🏗️ Code Architecture

### Design Patterns

We used a **combination of design patterns** to **manage objects and their components within the Engine**.  
The **Composite** pattern is used to organize and manipulate a hierarchy of objects and their components uniformly.  
The **Strategy** pattern is used to dynamically manage the lifecycles of the engine components.

Some parts of our engine are based on Unity.

### Composite design pattern

The `instantiate()` method in the `Engine` class creates and adds new objects to a collection using a **composite**. This means that each object added to m_objects serves as a container for other components or objects, allowing the engine to treat them uniformly.

```c++
auto Engine::instantiate() -> Object& {
    return m_objects.emplace(std::make_unique<Object>()).first;
}
```

Then, the [`Object`](./src/Engine/Object.h) 👉 `addComponent()` method allows us to add specific components (such as `Animator`, `MeshRenderer`, `UserInterface` etc.) to an object. This method uses dynamic_cast to ensure that the added component is of the expected type, and it handles component construction flexibly through the passing of various parameters.

```c++
template <class T, class... Args>
requires std::derived_from<T, EngineComponent> && std::constructible_from<T, Object&, Args...>
auto addComponent(Args&&... args) -> T& {
    return dynamic_cast<T&>(**m_components.emplace(std::make_unique<T>(
*this, std::forward<Args>(args)...)).first);
}
```

### Strategy design pattern

The Strategy pattern comes into play with the [`EngineComponent`](./src/Engine/EngineComponent.h) class and its lifecycle defined by the `onWillUpdate()`, `onUpdate()`, `onRender()`, and `onPostRender()` methods. Each EngineComponent can have different behaviors for these stages, allowing dynamic modifications.

```c++
class EngineComponent {
    explicit EngineComponent(Object& object) : m_object(object) {}
    virtual ~EngineComponent() = default;

    virtual auto onWillUpdate(Engine& engine) -> void {}
    virtual auto onUpdate(Engine& engine) -> void {}
    virtual auto onRender(Engine& engine) -> void {}
    virtual auto onPostRender(Engine& engine) -> void {}
};
```

### Nested Composite and Strategy Design Patterns

> This design showcases how the [`UserInterface`](./src/Components/UserInterface.h) class **encapsulates another layer of Composite and Strategy patterns within its own structure**.

**Nested composite**  

The UserInterface class acts as a **composite container** for different [InterfaceBlock](./src/InterfaceBlocks/) in a collection of m_blocks. The blocks are stored in a sorted map [(VectorMultiMap)](./src/Utility/VectorMultiMap.h), which ensures they are drawn in the correct order.

```c++
template <class T, class... Args>
requires std::derived_from<T, InterfaceBlock> && std::constructible_from<T, UserInterface&, Args...>
auto addBlock(const float order, Args&&... args) -> void {
    m_blocks.emplace(order, std::make_unique<T>(*this, std::forward<Args>(args)...));
}
```

**Nested strategy**

The `InterfaceBlock` class defines a **strategy interface** for how each individual UI block is drawn. Each derived class implement their specific drawing logic.

```c++
virtual auto onDrawUI(uint16_t blockId, Engine& engine, UserInterface& interface) -> void = 0;
```

## 📝 Credits

**["Frog Jumping"](https://sketchfab.com/3d-models/frog-jumping-fd0821c081b34c33807da145941725e8)** by [sMarthz](https://sketchfab.com/sMarthz) is licensed under [Creative Commons Attribution](https://creativecommons.org/licenses/by/4.0/).  
**["Mutant Iron Golem Minecraft Animated"](https://sketchfab.com/3d-models/mutant-iron-golem-minecraft-animated-a139255741d14274961e5b51d3c8c04f)** by [ghostaryan83](https://sketchfab.com/ghostaryan83) is licensed under [Creative Commons Attribution](https://creativecommons.org/licenses/by/4.0/).  
**["Minecraft Quaint Village"](https://sketchfab.com/3d-models/minecraft-quaint-village-f828b80b61c745b290f9b79c8b3b2993)** by [E](https://sketchfab.com/Ervinuu) is licensed under [Creative Commons Attribution](https://creativecommons.org/licenses/by/4.0/).  
