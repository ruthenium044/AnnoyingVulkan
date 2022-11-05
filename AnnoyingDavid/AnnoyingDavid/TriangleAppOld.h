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

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define VK_USE_PLATFORM_WIN32_KHR

const std::string MODEL_PATH = "models/skull/skull.obj";
const std::string TEXTURE_PATH = "models/skull/skull.jpg";


struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

 class TriangleAppOld
 {
 public:
 	void run() 
 	{
 		initVulkan();
 		cleanup();
 	}

 private:
 	
 	VkPipelineLayout pipelineLayout = nullptr;
 	VkPipeline graphicsPipeline = nullptr;
 	
 	
 	
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
 	
 	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
 	
 	void createCommandBuffer();
 	v

 	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
 	
 	void createIndexBuffer();
 	void createDescriptorSetLayout();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
 	
    void createTextureImage(const char* filepath);
 	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
 		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
 		VkImage& image, VkDeviceMemory& imageMemory);
 	
    void createTextureImageView();
    void createTextureSampler();
 	
    void loadModel(const char* filepath);
 	
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    void createColorResources();
    void initVulkan();
    void updateUniformBuffer(uint32_t currentImage);
   
   
 	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
 		VkImageLayout newLayout, uint32_t mipLevels);
    bool hasStencilComponent(VkFormat format);
    
    void drawFrame();
    void processInput(SDL_KeyboardEvent *key);
 	
 	void cleanup();
 	
 	
 	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	 VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* call_back_data, void* user_data);
 	
 	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
 	
 	
 	
 	
 	void cleanupSwapChain();
 };

