#include "TriangleAppOld.h"

void TriangleAppOld::initVulkan() 
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	
	createSwapChain();
	
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	
	createTextureImage(TEXTURE_PATH.c_str());
	createTextureImageView();
	createTextureSampler();
	loadModel(MODEL_PATH.c_str());
	
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffer();
	createSyncObjects();
}









