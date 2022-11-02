#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include <optional>
#include <set>

#include <cstdint> 
#include <limits>
#include <algorithm>
#include <array>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFORM_WIN32_KHR

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::string MODEL_PATH = "models/skull/skull.obj";
const std::string TEXTURE_PATH = "models/skull/skull.jpg";

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
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
	glm::vec3 normal;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
		attributeDescriptions[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)};
		attributeDescriptions[1] = {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)};
		attributeDescriptions[2] = {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord)};
		attributeDescriptions[3] = {3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)};
		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
	}
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
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

 	std::vector<Vertex> vertices;
 	std::vector<uint32_t> indices;
 	
 	VkBuffer vertexBuffer = nullptr;
 	VkDeviceMemory vertexBufferMemory = nullptr;
 	VkBuffer indexBuffer = nullptr;
 	VkDeviceMemory indexBufferMemory = nullptr;
 	std::vector<VkBuffer> uniformBuffers;
 	std::vector<VkDeviceMemory> uniformBuffersMemory;

 	VkDescriptorSetLayout descriptorSetLayout = nullptr;
    VkDescriptorPool descriptorPool = nullptr;
 	std::vector<VkDescriptorSet> descriptorSets;
 	
 	uint32_t mipLevels = 0;
 	VkImage textureImage = nullptr;
 	VkDeviceMemory textureImageMemory = nullptr;
 	VkImageView textureImageView = nullptr;
 	VkSampler textureSampler = nullptr;

 	VkImage depthImage = nullptr;
    VkDeviceMemory depthImageMemory = nullptr;
    VkImageView depthImageView = nullptr;

 	VkImage colorImage = nullptr;
 	VkDeviceMemory colorImageMemory = nullptr;
 	VkImageView colorImageView = nullptr;
 	
 	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
 	
 	glm::vec3 cameraPos   = glm::vec3(0.0f, -11.0f,  60.0f);
 	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
 	glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
 	float deltaTime = 0.0f;	// Time between current frame and last frame
 	float lastFrame = 0.0f; // Time of last frame
 	
    void initWindow();
 	void createSurface();
 	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
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
 	void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
 	
    void createTextureImage(const char* filepath);
 	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
 		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
 		VkImage& image, VkDeviceMemory& imageMemory);
 	void createImageViews();
    void createTextureImageView();
    void createTextureSampler();
    void createDepthResources();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
 	
    void loadModel(const char* filepath);
 	
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    void createColorResources();
    void initVulkan();
    void updateUniformBuffer(uint32_t currentImage);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
 	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
 		VkImageLayout newLayout, uint32_t mipLevels);
    bool hasStencilComponent(VkFormat format);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void drawFrame();
    void processInput(SDL_KeyboardEvent *key);
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

