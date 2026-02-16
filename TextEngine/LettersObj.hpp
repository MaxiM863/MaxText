#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "CookbookSampleFramework.h"

#include "OrbitingCamera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

using namespace VulkanCookbook;

class LetterObj
{
    
    public:

    LetterObj(){};    

    std::vector<VulkanCookbook::Mesh>                                       Model;

    uint32_t                                                                stride;

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkBuffer )                 bufferVertex;
    std::vector<VkDeviceMemory>                                             vectorVertexBufferMemory;

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkBuffer )                 StagingBuffer;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           StagingBufferMemory;
    
    bool                                                                    UpdateUniformBuffer;

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkBuffer )                 UniformBuffer;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           UniformBufferMemory;    

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDescriptorSetLayout )    DescriptorSetLayout;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDescriptorPool )         DescriptorPool;
    
    std::vector<VkDescriptorSet>                                            DescriptorSets;

    
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkPipelineLayout )         PipelineLayout;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkPipeline )               Pipeline;

    uint32_t                                                                glyphWidth;
    uint32_t                                                                glyphHeight;   

    VulkanCookbook::Matrix4x4                                               model_matrix;

    uint32_t                                                                charSetSize;


public:

    bool Initialize( VkDevice logicalDevice, VkPhysicalDevice PhysicalDevice, QueueParameters& GraphicsQueue, VkCommandBuffer& CommandBuffer, SwapchainParameters& Swapchain, VkFormat DepthFormat, VkRenderPass RenderPass, const VkSampler& Sampler, const VkImage& Image, const VkImageView& ImageView, std::string charSet, char* bufferDeltaCharacters )
    {        
        // This model send The relative position from 0, of each character of the string to be printed, and the relative position of the texture coordinates of each character in the texture atlas. The texture atlas is a 32x32 grid of characters, so each character occupies a 1/32th by 1/32th portion of the texture. 
        // The relative position of the texture coordinates is calculated based on the ASCII value of the character, which determines its position in the grid. For example, if the ASCII value of a character is 65 (which corresponds to 'A'). The relative position of the texture coordinates for 'A' would be 
        // (65 % 32) / 32.0 for the x-coordinate and (65 / 32) / 32.0 for the y-coordinate. This way, the shader can sample the correct portion of the texture atlas to render each character. And the width and height of each character in the texture atlas are also stored in the model, which can be used to calculate
        // the correct texture position when rendering the characters on the screen.

        charSetSize = charSet.size();
        
        char* buffer = new char[charSet.size() * (3 + 4) * sizeof(float)];
 
        // Relative position of the character in the string
        float relative_x = 0.0f; // Adding a small gap between characters
        float relative_y = 0.0f;
        
        for(int i = 0; i < charSet.size(); i++)
        {
            char c = charSet[i];
            uint32_t ascii_value = static_cast<uint32_t>(c);
            float x = (ascii_value % 32) / 32.0f;
            float y = (ascii_value / 32) / 32.0f;
            float width = bufferDeltaCharacters[i * 2 + 0];
            float height = bufferDeltaCharacters[i * 2 + 1];

            // Store the data in the buffer
            buffer[i * (3 + 4) * sizeof(float) + 0] = relative_x;
            buffer[i * (3 + 4) * sizeof(float) + 1] = relative_y;
            buffer[i * (3 + 4) * sizeof(float) + 2] = 0.0f; // Z coordinate, not used for text rendering

            relative_x += width + 0.02f; // Adding a small gap between characters

            buffer[i * (3 + 4) * sizeof(float) + 3] = x;
            buffer[i * (3 + 4) * sizeof(float) + 4] = y;
            buffer[i * (3 + 4) * sizeof(float) + 5] = width;
            buffer[i * (3 + 4) * sizeof(float) + 6] = height;
        }
         

        

        InitVkDestroyer( logicalDevice, bufferVertex );
        if( !VulkanCookbook::CreateBuffer( logicalDevice, sizeof( float ) * charSet.size() * (3 + 4), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, *bufferVertex ) )
        {

            return false;
        }

            
        int a = 0;
        VkDeviceMemory tmpB2;
        vectorVertexBufferMemory.push_back(tmpB2);   

        if( !VulkanCookbook::AllocateAndBindMemoryObjectToBuffer( PhysicalDevice, logicalDevice, *bufferVertex, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, tmpB2 ) ) {
            return false;
        }

        if( !VulkanCookbook::UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound( PhysicalDevice, logicalDevice, sizeof( float ) * charSet.size() * (3 + 4), buffer, *bufferVertex, 0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            GraphicsQueue.Handle, CommandBuffer, {} ) )
        {
            return false;
        }        

        // Staging buffer
        InitVkDestroyer( logicalDevice, StagingBuffer );
      
        if( !VulkanCookbook::CreateBuffer( logicalDevice, 2 * 16 * sizeof(float), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, *StagingBuffer ) ) {
            return false;
        }
      
        InitVkDestroyer( logicalDevice, StagingBufferMemory );

        if( !VulkanCookbook::AllocateAndBindMemoryObjectToBuffer( PhysicalDevice, logicalDevice, *StagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, *StagingBufferMemory ) ) {
            return false;
        }

        // Uniform buffer
        InitVkDestroyer( logicalDevice, UniformBuffer );
        InitVkDestroyer( logicalDevice, UniformBufferMemory );
      
        if( !VulkanCookbook::CreateUniformBuffer( PhysicalDevice, logicalDevice, 2 * 16 * sizeof( float ), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            *UniformBuffer, *UniformBufferMemory ) ) 
        {
            return false;
        }

        if( !UpdateStagingBuffer( true, logicalDevice, Swapchain.Size.width, Swapchain.Size.height, PrepareTranslationMatrix(0.0f,0.0f,0.0f))) // not important, not in loop game
        {
            return false;
        }

        // Descriptor set with uniform buffer
        std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_binding = 
        {
            {
                0,                                          
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,        
                2,                                          
                VK_SHADER_STAGE_VERTEX_BIT,                 
                nullptr
            }
            ,
            {
                1,                                          // uint32_t             binding
                VK_DESCRIPTOR_TYPE_SAMPLER,  // VkDescriptorType     descriptorType
                2,                                          // uint32_t             descriptorCount
                VK_SHADER_STAGE_FRAGMENT_BIT,               // VkShaderStageFlags   stageFlags
                nullptr                                     // const VkSampler    * pImmutableSamplers
            }
        };

        InitVkDestroyer( logicalDevice, DescriptorSetLayout );

        if( !VulkanCookbook::CreateDescriptorSetLayout( logicalDevice, { descriptor_set_layout_binding }, *DescriptorSetLayout ) ) {
            return false;
        }

        std::vector<VkDescriptorPoolSize> descriptor_pool_sizes = 
        {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,          // VkDescriptorType     type
                2                                           // uint32_t             descriptorCount
            },
            {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     type
                2                                           // uint32_t             descriptorCount
            }
        };

        InitVkDestroyer( logicalDevice, DescriptorPool );

        if( !VulkanCookbook::CreateDescriptorPool( logicalDevice, true, 4, descriptor_pool_sizes, *DescriptorPool ) ) {
            return false;
        }

        if( !VulkanCookbook::AllocateDescriptorSets( logicalDevice, *DescriptorPool, { *DescriptorSetLayout, *DescriptorSetLayout }, DescriptorSets ) ) {
            return false;
        }

        VulkanCookbook::BufferDescriptorInfo buffer_descriptor_update = 
        {
            DescriptorSets[0],                          // VkDescriptorSet                      TargetDescriptorSet
            0,                                          // uint32_t                             TargetDescriptorBinding
            0,                                          // uint32_t                             TargetArrayElement
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,          // VkDescriptorType                     TargetDescriptorType
            {                                           // std::vector<VkDescriptorBufferInfo>  BufferInfos
            {
                *UniformBuffer,                           // VkBuffer                             buffer
                0,                                        // VkDeviceSize                         offset
                2*16*sizeof(float)                             // VkDeviceSize                         range
            }
            }
        };

        VulkanCookbook::ImageDescriptorInfo image_descriptor_update = {
            DescriptorSets[0],                          // VkDescriptorSet                      TargetDescriptorSet
            1,                                          // uint32_t                             TargetDescriptorBinding
            0,                                          // uint32_t                             TargetArrayElement
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType                     TargetDescriptorType
            {                                           // std::vector<VkDescriptorImageInfo>   ImageInfos
                {
                    Sampler,                                 // VkSampler                            sampler
                    ImageView,                               // VkImageView                          imageView
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        imageLayout
                }
            }
        };

        VulkanCookbook::UpdateDescriptorSets( logicalDevice, { image_descriptor_update }, { buffer_descriptor_update }, {}, {} );

        

        // Graphics pipeline

        InitVkDestroyer( logicalDevice, PipelineLayout );
        if( !VulkanCookbook::CreatePipelineLayout( logicalDevice, { *DescriptorSetLayout }, {}, *PipelineLayout ) ) {
            return false;
        }

        std::vector<unsigned char> vertex_shader_spirv;
        if( !VulkanCookbook::GetBinaryFileContents( "Data/Shaders/11 Lighting/01 Rendering a geometry with vertex diffuse lighting/b.spv", vertex_shader_spirv ) ) {
            return false;
        }

        VulkanCookbook::VkDestroyer(VulkanCookbook::VkShaderModule) vertex_shader_module;
        InitVkDestroyer( logicalDevice, vertex_shader_module );
        if( !VulkanCookbook::CreateShaderModule( logicalDevice, vertex_shader_spirv, *vertex_shader_module ) ) {
            return false;
        }

        std::vector<unsigned char> fragment_shader_spirv;
        if( !VulkanCookbook::GetBinaryFileContents( "Data/Shaders/11 Lighting/01 Rendering a geometry with vertex diffuse lighting/a.spv", fragment_shader_spirv ) ) {
            return false;
        }

        VulkanCookbook::VkDestroyer(VulkanCookbook::VkShaderModule) fragment_shader_module;
        InitVkDestroyer( logicalDevice, fragment_shader_module );
        if( !VulkanCookbook::CreateShaderModule( logicalDevice, fragment_shader_spirv, *fragment_shader_module ) ) {
            return false;
        }

        std::vector<unsigned char> geometry_shader_spirv;
        if( !VulkanCookbook::GetBinaryFileContents( "Data/Shaders/11 Lighting/01 Rendering a geometry with vertex diffuse lighting/c.spv", geometry_shader_spirv ) ) {
            return false;
        }

        VulkanCookbook::VkDestroyer(VulkanCookbook::VkShaderModule) geometry_shader_spirv_module;
        InitVkDestroyer( logicalDevice, geometry_shader_spirv_module );
        if( !VulkanCookbook::CreateShaderModule( logicalDevice, geometry_shader_spirv, *geometry_shader_spirv_module ) ) {
            return false;
        }

        std::vector<VulkanCookbook::ShaderStageParameters> shader_stage_params = 
        {
            {
                VK_SHADER_STAGE_VERTEX_BIT,       // VkShaderStageFlagBits        ShaderStage
                *vertex_shader_module,            // VkShaderModule               ShaderModule
                "main",                           // char const                 * EntryPointName
                nullptr                           // VkSpecializationInfo const * SpecializationInfo
            },
            {
                VK_SHADER_STAGE_FRAGMENT_BIT,     // VkShaderStageFlagBits        ShaderStage
                *fragment_shader_module,          // VkShaderModule               ShaderModule
                "main",                           // char const                 * EntryPointName
                nullptr                           // VkSpecializationInfo const * SpecializationInfo
            },
            {
                VK_SHADER_STAGE_GEOMETRY_BIT,     // VkShaderStageFlagBits        ShaderStage
                *geometry_shader_spirv_module,          // VkShaderModule               ShaderModule
                "main",                           // char const                 * EntryPointName
                nullptr                           // VkSpecializationInfo const * SpecializationInfo
            }
        };

        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
        SpecifyPipelineShaderStages( shader_stage_params, shader_stage_create_infos );

        std::vector<VkVertexInputBindingDescription> vertex_input_binding_descriptions = 
        {
            {
            0,                            // uint32_t                     binding
            (3 + 4) * sizeof(float),                 // uint32_t                     stride
            VK_VERTEX_INPUT_RATE_VERTEX   // VkVertexInputRate            inputRate
            }
        };
  
        std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions = 
        {
            { // Position
            0,                                                                        // uint32_t   location
            0,                                                                        // uint32_t   binding
            VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
            0                                                                         // uint32_t   offset
            },
            { // Texcoords
            1,                                                                        // uint32_t   location
            0,                                                                        // uint32_t   binding
            VK_FORMAT_A8B8G8R8_UINT_PACK32,                                           // VkFormat   format
            3 * sizeof( float )
            }
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info;
        VulkanCookbook::SpecifyPipelineVertexInputState( vertex_input_binding_descriptions, vertex_attribute_descriptions, vertex_input_state_create_info );

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info;
        VulkanCookbook::SpecifyPipelineInputAssemblyState( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false, input_assembly_state_create_info );

        VulkanCookbook::ViewportInfo viewport_infos = {
            {                     // std::vector<VkViewport>   Viewports
            {
                0.0f,               // float          x
                0.0f,               // float          y
                500.0f,             // float          width
                500.0f,             // float          height
                0.0f,               // float          minDepth
                1.0f                // float          maxDepth
            }
            },
            {                     // std::vector<VkRect2D>     Scissors
            {
                {                   // VkOffset2D     offset
                0,                  // int32_t        x
                0                   // int32_t        y
                },
                {                   // VkExtent2D     extent
                500,                // uint32_t       width
                500                 // uint32_t       height
                }
            }
            }
        };

        VkPipelineViewportStateCreateInfo viewport_state_create_info;
        VulkanCookbook::SpecifyPipelineViewportAndScissorTestState( viewport_infos, viewport_state_create_info );

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info;
        VulkanCookbook::SpecifyPipelineRasterizationState( false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, false, 0.0f, 0.0f, 0.0f, 1.0f, rasterization_state_create_info );

        VkPipelineMultisampleStateCreateInfo multisample_state_create_info;
        VulkanCookbook::SpecifyPipelineMultisampleState( VK_SAMPLE_COUNT_1_BIT, false, 0.0f, nullptr, false, false, multisample_state_create_info );

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info;
        VulkanCookbook::SpecifyPipelineDepthAndStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0.0f, 1.0f, false, {}, {}, depth_stencil_state_create_info );

        std::vector<VkPipelineColorBlendAttachmentState> attachment_blend_states = {
            {
            false,                          // VkBool32                 blendEnable
            VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcColorBlendFactor
            VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstColorBlendFactor
            VK_BLEND_OP_ADD,                // VkBlendOp                colorBlendOp
            VK_BLEND_FACTOR_ONE,            // VkBlendFactor            srcAlphaBlendFactor
            VK_BLEND_FACTOR_ONE,            // VkBlendFactor            dstAlphaBlendFactor
            VK_BLEND_OP_ADD,                // VkBlendOp                alphaBlendOp
            VK_COLOR_COMPONENT_R_BIT |      // VkColorComponentFlags    colorWriteMask
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT
            }
        };
        VkPipelineColorBlendStateCreateInfo blend_state_create_info;
        VulkanCookbook::SpecifyPipelineBlendState( false, VK_LOGIC_OP_COPY, attachment_blend_states, { 1.0f, 1.0f, 1.0f, 1.0f }, blend_state_create_info );

        std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamic_state_create_info;
        VulkanCookbook::SpecifyPipelineDynamicStates( dynamic_states, dynamic_state_create_info );

        VkGraphicsPipelineCreateInfo pipeline_create_info;
        VulkanCookbook::SpecifyGraphicsPipelineCreationParameters( 0, shader_stage_create_infos, vertex_input_state_create_info, input_assembly_state_create_info,
            nullptr, &viewport_state_create_info, rasterization_state_create_info, &multisample_state_create_info, &depth_stencil_state_create_info, &blend_state_create_info,
            &dynamic_state_create_info, *PipelineLayout, RenderPass, 0, VK_NULL_HANDLE, -1, pipeline_create_info );

        std::vector<VkPipeline> pipeline;
        if( !VulkanCookbook::CreateGraphicsPipelines( logicalDevice, { pipeline_create_info }, VK_NULL_HANDLE, pipeline ) ) {
            return false;
        }
        InitVkDestroyer( logicalDevice, Pipeline );
        *Pipeline = pipeline[0];

        return true;
    }

    bool draw_1(VkCommandBuffer CommandBuffer, VkFramebuffer framebuffer, VkRect2D sizew)
    {
        if( UpdateUniformBuffer ) {

            UpdateUniformBuffer = false;

            BufferTransition pre_transfer_transition = {
              *UniformBuffer,               // VkBuffer         Buffer
              VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    CurrentAccess
              VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    NewAccess
              VK_QUEUE_FAMILY_IGNORED,      // uint32_t         CurrentQueueFamily
              VK_QUEUE_FAMILY_IGNORED       // uint32_t         NewQueueFamily
            };
            SetBufferMemoryBarrier( CommandBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, { pre_transfer_transition } );

            std::vector<VkBufferCopy> regions = {
              {
                0,                        // VkDeviceSize     srcOffset
                0,                        // VkDeviceSize     dstOffset
                2*16 * sizeof( float )    // VkDeviceSize     size
              }
            };
            CopyDataBetweenBuffers( CommandBuffer, *StagingBuffer, *UniformBuffer, regions );

            BufferTransition post_transfer_transition = {
              *UniformBuffer,               // VkBuffer         Buffer
              VK_ACCESS_TRANSFER_WRITE_BIT, // VkAccessFlags    CurrentAccess
              VK_ACCESS_UNIFORM_READ_BIT,   // VkAccessFlags    NewAccess
              VK_QUEUE_FAMILY_IGNORED,      // uint32_t         CurrentQueueFamily
              VK_QUEUE_FAMILY_IGNORED       // uint32_t         NewQueueFamily
            };
            SetBufferMemoryBarrier( CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, { post_transfer_transition } );          
        }

        


        BindVertexBuffers( CommandBuffer, 0, {{ *bufferVertex, 0 }} );
        
        BindDescriptorSets( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 0, {DescriptorSets[0]}, { 0,0 } );
        
        BindPipelineObject( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *Pipeline );

        DrawGeometry( CommandBuffer, charSetSize, 1, 0, 0 );                
        

        return true;
    }
    

    bool UpdateStagingBuffer( bool force, VkDevice LogicalDevice, float width, float height, VulkanCookbook::Matrix4x4 vvv )
    {  

        if(force)
        {

            UpdateUniformBuffer = true;

            VulkanCookbook::Matrix4x4 model_view_matrix =  PrepareScalingMatrix(1.0f, 1.0f, 1.0f);

            if( !VulkanCookbook::MapUpdateAndUnmapHostVisibleMemory( LogicalDevice, *StagingBufferMemory, 0, sizeof( model_view_matrix[0] ) * model_view_matrix.size(), &model_view_matrix[0], true, nullptr ) ) {
                return false;
            }

            VulkanCookbook::Matrix4x4 perspective_matrix = VulkanCookbook::PrepareOrthographicProjectionMatrix( -width, width, -height, height, 0.0f, 1.0f);

            if( !VulkanCookbook::MapUpdateAndUnmapHostVisibleMemory( LogicalDevice, *StagingBufferMemory, sizeof( model_view_matrix[0] ) * model_view_matrix.size(), sizeof( perspective_matrix[0] ) * perspective_matrix.size(), &perspective_matrix[0], true, nullptr ) ) {
                
                return false;
            }
        } 

        return true;
    }
};