#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace svk
{
    class Window
    {
    public:
        Window(int w, int h);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        SDL_Window* GetWindow() { return window; };
        VkExtent2D getExtent()
        {
            return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        };
        
        void createWindowSurface(VkInstance instance, VkSurfaceKHR &surface);
    private:
        void initWindow();
        
        SDL_Window* window = nullptr;
        const uint32_t width = 800;
        const uint32_t height = 600;
    };
}
