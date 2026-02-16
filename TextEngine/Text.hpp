#include <vector>

#include "CookbookSampleFramework.h"

#include "LettersObj.hpp"

class Text 
{

    public:

    VkDestroyer( VkSampler )                                                Sampler;
    
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkImage )                  Image;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           ImageMemory;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkImageView )              ImageView;
    QueueParameters                                                         GraphicsQueue;
    VkCommandBuffer                                                         CommandBuffer;  

    LetterObj**                                                             letters;
    int                                                                     maxLinesScreen;


        Text(VkDevice logicalDevice, VkPhysicalDevice PhysicalDevice, QueueParameters& GraphicsQueue, VkCommandBuffer& CommandBuffer, SwapchainParameters& swapchain, VkFormat DepthFormat, OrbitingCamera& camera, VkRenderPass renderPass, int maxLinesScreen){
        
            this->logicalDevice = logicalDevice;
            this->PhysicalDevice = PhysicalDevice;
            this->GraphicsQueue = GraphicsQueue;
            this->CommandBuffer = CommandBuffer;

            letters = new LetterObj*[maxLinesScreen];
            this->maxLinesScreen = maxLinesScreen;
        }

        ~Text();

        bool Initialize(SwapchainParameters& Swapchain, VkFormat DepthFormat, VkRenderPass renderPass)
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
                
            std::fstream file("Data/Textures/DataGlyphs.bin", std::ios::in | std::ios::binary);
            
            char* buffer = new char[2 * 1024];

            file.read((char*)&buffer, 2 * 1024);

            file.close();

            for(int i = 0; i < maxLinesScreen; i++)
            {
                letters[i] = new LetterObj();
                
                if( !letters[i]->Initialize( logicalDevice, PhysicalDevice, GraphicsQueue, CommandBuffer, Swapchain, DepthFormat, renderPass, *Sampler, *Image, *ImageView, "allo monde!", buffer ) )
                {
                    return false;
                }
            }
        };

        void printLine(std::string line)
        {
            
        }

        bool draw(VkCommandBuffer CommandBuffer, VkFramebuffer framebuffer, VkRect2D sizew, QueueParameters PresentQueue, SwapchainParameters& Swapchain, VkRenderPass RenderPass, VkDevice LogicalDevice, std::vector<FrameResources>& FramesResources)
        {            
            auto prepare_frame = [&]( std::vector<VkCommandBuffer> command_buffer, uint32_t swapchain_image_index, VkFramebuffer framebuffer ) 
            {
                int i = 0;
                
                if( !BeginCommandBufferRecordingOperation( command_buffer[i], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr ) ) {
                    return false;
                }
        
                if( PresentQueue.FamilyIndex != GraphicsQueue.FamilyIndex ) {
                    ImageTransition image_transition_before_drawing = {
                        Swapchain.Images[swapchain_image_index],  // VkImage              Image
                        VK_ACCESS_MEMORY_READ_BIT,                // VkAccessFlags        CurrentAccess
                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,     // VkAccessFlags        NewAccess
                        VK_IMAGE_LAYOUT_UNDEFINED,                // VkImageLayout        CurrentLayout
                        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VkImageLayout        NewLayout
                        PresentQueue.FamilyIndex,                 // uint32_t             CurrentQueueFamily
                        GraphicsQueue.FamilyIndex,                // uint32_t             NewQueueFamily
                        VK_IMAGE_ASPECT_COLOR_BIT                 // VkImageAspectFlags   Aspect
                    };
                
                    SetImageMemoryBarrier( command_buffer[i], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, { image_transition_before_drawing } );
                }
            
                // Drawing
                BeginRenderPass( command_buffer[i], RenderPass , framebuffer, {{0,0}, Swapchain.Size}, { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0 } }, VK_SUBPASS_CONTENTS_INLINE );

                VkViewport viewport = {
                    0.0f,                                       // float    x
                    0.0f,                                       // float    y
                    static_cast<float>(sizew.extent.width),   // float    width
                    static_cast<float>(sizew.extent.height),  // float    height
                    0.0f,                                       // float    minDepth
                    1.0f,                                       // float    maxDepth
                };

                SetViewportStateDynamically( CommandBuffer, 0, { viewport } );

                VkRect2D scissor = 
                {
                    {                                           // VkOffset2D     offset
                        0,                                          // int32_t        x
                        0                                           // int32_t        y
                    },
                    {                                           // VkExtent2D     extent
                        sizew.extent.width,                       // uint32_t       width
                        sizew.extent.height                       // uint32_t       height
                    }
                };
                
                SetScissorStateDynamically( CommandBuffer, 0, { scissor } );

                for(int i = 0; i < maxLinesScreen; i++)
                {
                    if( !letters[i]->draw_1(CommandBuffer, framebuffer, sizew) )
                    {
                        return false;
                    }
                }                

                EndRenderPass( command_buffer[i] );

                if( !EndCommandBufferRecordingOperation( command_buffer[i] ) ) {
                    return false;
                }
                
                return true;
            };

            return IncreasePerformanceThroughIncreasingTheNumberOfSeparatelyRenderedFrames( LogicalDevice, GraphicsQueue.Handle, PresentQueue.Handle,
            Swapchain.Handle.Object.Handle, Swapchain.Size, Swapchain.ImageViewsRaw, RenderPass, {}, prepare_frame, FramesResources );
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