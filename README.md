# Vulkan

Mini project following tutorials and learning new things about graphics programming

![image](https://github.com/user-attachments/assets/d79d11c3-bb0b-48e0-9ff4-94ed369bbb90)


For learning purposes and to develop my graphics programming knowledge, I have put together a small Vulkan engine. I used online resources to teach myself about the topic and learn the needed things on the go. 

The whole process gave me lots of insights into how such engines work and how to use them. The main challenge was to configure Vulkan. The boilerplate is the biggest challenge of not knowing how to design the data structures to manage the application reasonably. Therefore, I had to refactor it a few times to get a satisfying result.

Instead of using GLFW like most tutorials, I decided to use SDL for the event and window handling. The host program (the program on the CPU) is capable of managing multiple models with distinct properties such as colour, texture, different uniform data, and so on. For the shaders, I have decided to make a simple shader that supports multiple lights. It consists of basic light models and supports only a limited number of point lights.

## Controls:

moveLeft = A;

moveRight = D;

moveForward = W;

moveBackward = S;

moveUp = E;

moveDown = Q;

lookLeft = Arrow left;

lookRight = Arrow right;

lookUp = Arrow up;

lookDown = Arrow down;


refs

https://github.com/blurrypiano/littleVulkanEngine

https://vulkan-tutorial.com/

https://learnopengl.com/
