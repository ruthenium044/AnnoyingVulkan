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
#include <array>

#include <glm/glm.hpp>

#define VK_USE_PLATFORM_WIN32_KHR

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const int MAX_FRAMES_IN_FLIGHT = 2;

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

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}
};
const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
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
	VkInstance instance = nullptr;

	VkDebugUtilsMessengerEXT debugMessenger = nullptr;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
 	VkDevice device = VK_NULL_HANDLE;
 	VkQueue graphicsQueue = nullptr;
 	VkQueue presentQueue = nullptr;
 	
 	VkSwapchainKHR swapChain = nullptr;
 	std::vector<VkImage> swapChainImages;
 	VkFormat swapChainImageFormat;
 	VkExtent2D swapChainExtent = {};
 	std::vector<VkImageView> swapChainImageViews;

 	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
 	VkPipeline graphicsPipeline = nullptr;
 	std::vector<VkFramebuffer> swapChainFramebuffers;
 	
 	VkCommandPool commandPool = nullptr;
 	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
 	uint32_t currentFrame = 0;
 	bool framebufferResized = false;

 	VkBuffer vertexBuffer = nullptr;
 	VkDeviceMemory vertexBufferMemory = nullptr;
 	VkBuffer indexBuffer = nullptr;
 	VkDeviceMemory indexBufferMemory = nullptr;
 	
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
	 void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
	                   VkDeviceMemory& bufferMemory);
	 void createVertexBuffer();
	 uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	 void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	 void createIndexBuffer();
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
 	void recreateSwapChain();
 	void cleanupSwapChain();
 };

