#pragma once
#include <memory>
#include <string>

#include "Device.h"

namespace svk {
    class Texture {
    public:
        Texture(Device &dev, const std::string &textureFilepath);
        Texture(
            Device &dev,
            VkFormat form,
            VkExtent3D ext,
            VkImageUsageFlags usageFlags,
            VkSampleCountFlagBits sampleCount);
        ~Texture();

        // delete copy constructors
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        VkImageView imageView() const { return mTextureImageView; }
        VkSampler sampler() const { return mTextureSampler; }
        VkImage getImage() const { return mTextureImage; }
        VkImageView getImageView() const { return mTextureImageView; }
        VkDescriptorImageInfo getImageInfo() const { return descriptor; }
        VkImageLayout getImageLayout() const { return mTextureLayout; }
        VkExtent3D getExtent() const { return extent; }
        VkFormat getFormat() const { return format; }

        void updateDescriptor();
        void transitionLayout(
            VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);

        static std::unique_ptr<Texture> createTextureFromFile(
            Device &device, const std::string &filepath);

    private:
        void createTextureImage(const std::string &filepath);
        void createTextureImageView(VkImageViewType viewType);
        void createTextureSampler();

        VkDescriptorImageInfo descriptor{};

        Device &device;
        VkImage mTextureImage = nullptr;
        VkDeviceMemory mTextureImageMemory = nullptr;
        VkImageView mTextureImageView = nullptr;
        VkSampler mTextureSampler = nullptr;
        VkFormat format;
        VkImageLayout mTextureLayout;
        uint32_t mMipLevels{1};
        uint32_t mLayerCount{1};
        VkExtent3D extent{};
    };
}
