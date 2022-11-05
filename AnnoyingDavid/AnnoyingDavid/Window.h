#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <sys/stat.h>
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
        bool wasWindowResized() { return frameBufferResized;}
        void resetWindowResizedFlag() { frameBufferResized = false;}
        
        void createWindowSurface(VkInstance instance, VkSurfaceKHR &surface);
    private:
        static void frameBufferResizeCallback(SDL_Window *window, int width, int height);
        void initWindow();
        
        SDL_Window* window = nullptr;
        uint32_t width = 800;
        uint32_t height = 600;
        bool frameBufferResized = false;
    };
}
