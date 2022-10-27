#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

#include <optional>
#include <set>

#include <cstdint> 
#include <limits>
#include <algorithm> 

#define VK_USE_PLATFORM_WIN32_KHR

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	
	bool isComplete() 
	{
		return graphicsFamily.has_value() &&
        presentFamily.has_value();

	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


 class TriangleApp 
 {
 public:
	 void run() 
	 {
		 initWindow();
		 initVulkan();
		 mainLoop();
		 cleanup();
	 }

private:
	SDL_Window* window = nullptr;
 	VkSurfaceKHR surface = VK_NULL_HANDLE;
 	
	bool isRunning = true;
	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
 	VkDevice device = VK_NULL_HANDLE;
 	VkQueue graphicsQueue;
 	VkQueue presentQueue;
 	
 	VkSwapchainKHR swapChain;
 	std::vector<VkImage> swapChainImages;
 	VkFormat swapChainImageFormat;
 	VkExtent2D swapChainExtent;
 	std::vector<VkImageView> swapChainImageViews;

 	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
 	VkPipeline graphicsPipeline;
 	std::vector<VkFramebuffer> swapChainFramebuffers;
 	
 	VkCommandPool commandPool;
 	VkCommandBuffer commandBuffer;

 	VkSemaphore imageAvailableSemaphore;
 	VkSemaphore renderFinishedSemaphore;
 	VkFence inFlightFence;

 	
	 void initWindow();
 	 void createSurface();
 	 void createImageViews();
 	 void createGraphicsPipeline();
	 VkShaderModule createShaderModule(const std::vector<char>& code);
	 void createRenderPass();
	 void createFramebuffers();
	 void createCommandPool();
	 void createCommandBuffer();
	 void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	 void createSyncObjects();
	 void initVulkan();
	 void drawFrame();
	 void mainLoop();
	 void cleanup();
	 void createInstance();
	 static bool checkValidationLayerSupport();
	 std::vector<const char*> getRequiredExtensions() const;
 	
	 static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* call_back_data, void* user_data);
	 void setupDebugMessenger();
	 static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	 static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	 void pickPhysicalDevice();
	 void createLogicalDevice();
	 QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	 bool isDeviceSuitable(VkPhysicalDevice device);
	 bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	 SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
	 VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	 void createSwapChain();
 };

