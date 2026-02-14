#include <vector>

#include "LettersObj.hpp"

class Text{

    public:

    VkDestroyer( VkSampler )                                                Sampler;
    
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkImage )                  Image;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           ImageMemory;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkImageView )              ImageView;
    QueueParameters                                                         GraphicsQueue;
    VkCommandBuffer                                                         CommandBuffer;  


        Text(VkDevice logicalDevice, VkPhysicalDevice PhysicalDevice, QueueParameters& GraphicsQueue, VkCommandBuffer& CommandBuffer, SwapchainParameters& Swapchain, VkFormat DepthFormat, OrbitingCamera& camera, VkRenderPass renderPass){
        
            this->logicalDevice = logicalDevice;
            this->PhysicalDevice = PhysicalDevice;
            this->GraphicsQueue = GraphicsQueue;
            this->CommandBuffer = CommandBuffer;
            this->swapchainSize.extent = Swapchain.Size;
        }

        ~Text();

        bool Initialize()
        {
            InitVkDestroyer( logicalDevice, Sampler );        
            InitVkDestroyer( logicalDevice, Image );
            InitVkDestroyer( logicalDevice, ImageMemory );
            InitVkDestroyer( logicalDevice, ImageView );

            int width = 0;
            int height = 0;

            std::vector<unsigned char> image_data;
            LoadTextureDataFromFile( "Data/Textures/final.png", 1, image_data, &width, &height, nullptr,  nullptr );
        
            if( !VulkanCookbook::CreateCombinedImageSampler( PhysicalDevice, logicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8_UNORM, { (uint32_t)width, (uint32_t)height, 1 },
                1, 1, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, false, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, VK_FILTER_LINEAR,
                VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
                VK_SAMPLER_ADDRESS_MODE_REPEAT, 0.0f, false, 1.0f, false, VK_COMPARE_OP_ALWAYS, 0.0f, 1.0f, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                false, *Sampler, *Image, *ImageMemory, *ImageView ) ) 
                {
                    
                    return false;
                }

            VkImageSubresourceLayers image_subresource_layer = {
                VK_IMAGE_ASPECT_COLOR_BIT,    // VkImageAspectFlags     aspectMask
                0,                            // uint32_t               mipLevel
                0,                            // uint32_t               baseArrayLayer
                1                             // uint32_t               layerCount
            };

            if( !VulkanCookbook::UseStagingBufferToUpdateImageWithDeviceLocalMemoryBound( PhysicalDevice, logicalDevice, static_cast<VkDeviceSize>(image_data.size()),
                &image_data[0], *Image, image_subresource_layer, { 0, 0, 0 }, { (uint32_t)width, (uint32_t)height, 1 }, VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, GraphicsQueue.Handle, CommandBuffer, {} ) ) 
                {
                    return false;
                }           
        };
        

        void printLine(std::string line)
        {
           
        }

        bool draw(VkCommandBuffer CommandBuffer, VkFramebuffer framebuffer, VkRect2D sizew)
        {
            
            return true;
        }

        
        private:

            VkDevice logicalDevice;
            VkPhysicalDevice PhysicalDevice;
            VkRect2D swapchainSize;

            std::vector<LetterObj*> vectorBaseLetterObj;
            //std::vector<LinesObj> vectorLinesObj;

            int cursorX = 0;
            int cursorY = 0;

        public:

            
};