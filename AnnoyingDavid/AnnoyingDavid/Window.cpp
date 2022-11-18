#include "Window.h"

#include <stdexcept>

namespace svk {
    Window::Window(int w, int h) :
        width(w), height(h) { initWindow(); }

    Window::~Window() {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR& surface) {
        auto result = SDL_Vulkan_CreateSurface(window, instance, &surface);
        if (result != SDL_TRUE) { throw std::runtime_error("failed to create window surface!"); }
    }

    void Window::initWindow() {
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow("NOt a game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    }
}
