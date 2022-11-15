#pragma once
#include <optional>
#include <vector>

#include "Window.h"

namespace svk
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };
    
    class Device
    {
    #ifdef NDEBUG
            const bool enableValidationLayers = false;
    #else
            const bool enableValidationLayers = true;
    #endif
        
    public:
        Device(Window &win);
        ~Device();
        
        Device(const Device &) = delete;
        Device operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

        VkCommandPool getCommandPool() { return commandPool; }
        VkDevice getDevice() { return device; }
        VkSurfaceKHR getSurface() { return surface; }
        VkQueue GetGraphicsQueue() { return graphicsQueue; }
        VkQueue GetPresentQueue() { return presentQueue; }

        SwapChainSupportDetails getSwapChainSupport()
        { return querySwapChainSupport(physicalDevice); }
        QueueFamilyIndices findPhysicalQueueFamilies()
        { return findQueueFamilies(physicalDevice); }
 
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
            VkImageTiling tiling, VkFormatFeatureFlags features);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo( const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
        uint32_t mipLevels = 1, uint32_t layerCount = 1);

        VkPhysicalDeviceProperties properties{};
        
    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char*> getRequiredExtensions() const;
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

        VkInstance instance{};
        VkDebugUtilsMessengerEXT debugMessenger = nullptr;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        Window* window{};
        VkCommandPool commandPool = nullptr;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = nullptr;
 	    VkQueue presentQueue = nullptr;

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    };
}