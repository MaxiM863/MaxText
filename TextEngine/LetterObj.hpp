#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "CookbookSampleFramework.h"

#include "OrbitingCamera.h"
#include "Glyph.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

using namespace VulkanCookbook;

class LetterObj
{
    
    public:
    
    VkDestroyer( VkSampler )                Sampler;
    
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkImage )                  Image;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           ImageMemory;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkImageView )              ImageView;
    

    std::vector<VulkanCookbook::Mesh>                                       Model;

    uint32_t                                                                stride;

    std::vector<VkBuffer>                                                   vectorVertexBuffer;
    std::vector<VkDeviceMemory>                                             vectorVertexBufferMemory;

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkBuffer )                 StagingBuffer;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           StagingBufferMemory;
    
    bool                                                                    UpdateUniformBuffer;

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkBuffer )                 UniformBuffer;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDeviceMemory )           UniformBufferMemory;    

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDescriptorSetLayout )    DescriptorSetLayout;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkDescriptorPool )         DescriptorPool;
    
    std::vector<VkDescriptorSet>                                            DescriptorSets;

    VulkanCookbook::VkDestroyer( VulkanCookbook::VkRenderPass )             RenderPass;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkPipelineLayout )         PipelineLayout;
    VulkanCookbook::VkDestroyer( VulkanCookbook::VkPipeline )               Pipeline;

    uint32_t                                                                glyphWidth;
    uint32_t                                                                glyphHeight;   



private:

    

public:

    bool Initialize( VkDevice logicalDevice, VkPhysicalDevice PhysicalDevice, QueueParameters& GraphicsQueue, VkCommandBuffer& CommandBuffer, SwapchainParameters& Swapchain, VkFormat DepthFormat, OrbitingCamera& camera, std::vector<unsigned char> image_data, int width, int height ){

        
        glyphWidth = width;
        glyphHeight = height;

        InitVkDestroyer( logicalDevice, Sampler );        
        InitVkDestroyer( logicalDevice, Image );
        InitVkDestroyer( logicalDevice, ImageMemory );
        InitVkDestroyer( logicalDevice, ImageView );
      
        if( !VulkanCookbook::CreateCombinedImageSampler( PhysicalDevice, logicalDevice, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8A8_UNORM, { (uint32_t)width, (uint32_t)height, 1 },
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
        
        Mesh mesh;

        if( !VulkanCookbook::Load3DModelFromObjFile( "Data/Models/glyphQuad.obj", true, true, true, true, mesh, &stride ) ) {
            return false;
        }

        Model.push_back(mesh);

        for(int i = 0; i < Model.size(); i++)
        {//InitVkDestroyer( *LogicalDevice, *vectorVertexBufferMemory.at(i) );
            VkBuffer tmpB;
            vectorVertexBuffer.push_back(tmpB);

            //InitVkDestroyer( *LogicalDevice, vectorVertexBuffer );
            if( !VulkanCookbook::CreateBuffer( logicalDevice, sizeof( float ) * Model.at(i).Data.size(),
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vectorVertexBuffer.at(i) ) ) 
            {

                return false;
            }

            
            int a = 0;
            VkDeviceMemory tmpB2;
            vectorVertexBufferMemory.push_back(tmpB2);   

            if( !VulkanCookbook::AllocateAndBindMemoryObjectToBuffer( PhysicalDevice, logicalDevice, vectorVertexBuffer.at(i), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vectorVertexBufferMemory.at(i) ) ) {
                return false;
            }

            if( !VulkanCookbook::UseStagingBufferToUpdateBufferWithDeviceLocalMemoryBound( PhysicalDevice, logicalDevice, sizeof( Model.at(i).Data[0] ) * Model.at(i).Data.size(),
                &Model.at(i).Data[0], vectorVertexBuffer.at(i), 0, 0, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                GraphicsQueue.Handle, CommandBuffer, {} ) )
            {
                return false;
            }
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

        if( !UpdateStagingBuffer( true, logicalDevice, Swapchain.Size.width, Swapchain.Size.height, camera.GetMatrix() ) ) 
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
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // VkDescriptorType     descriptorType
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
                0*2*16*sizeof(float),                                        // VkDeviceSize                         offset
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
                    *Sampler,                                 // VkSampler                            sampler
                    *ImageView,                               // VkImageView                          imageView
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // VkImageLayout                        imageLayout
                }
            }
        };

        VulkanCookbook::UpdateDescriptorSets( logicalDevice, { image_descriptor_update }, { buffer_descriptor_update }, {}, {} );

        // Render pass
        std::vector<VkAttachmentDescription> attachment_descriptions = {
            {
            0,                                                // VkAttachmentDescriptionFlags     flags
            Swapchain.Format,                                 // VkFormat                         format
            VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
            VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
            VK_ATTACHMENT_STORE_OP_STORE,                     // VkAttachmentStoreOp              storeOp
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR                   // VkImageLayout                    finalLayout
            },
            {
            0,                                                // VkAttachmentDescriptionFlags     flags
            DepthFormat,                                      // VkFormat                         format
            VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits            samples
            VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp               loadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              storeOp
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp               stencilLoadOp
            VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp              stencilStoreOp
            VK_IMAGE_LAYOUT_UNDEFINED,                        // VkImageLayout                    initialLayout
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // VkImageLayout                    finalLayout
            }
        };

        VkAttachmentReference depth_attachment = {
            1,                                                // uint32_t                             attachment
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // VkImageLayout                        layout
        };

        std::vector<VulkanCookbook::SubpassParameters> subpass_parameters = {
            {
            VK_PIPELINE_BIND_POINT_GRAPHICS,              // VkPipelineBindPoint                  PipelineType
            {},                                           // std::vector<VkAttachmentReference>   InputAttachments
            {                                             // std::vector<VkAttachmentReference>   ColorAttachments
                {
                0,                                          // uint32_t                             attachment
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,   // VkImageLayout                        layout
                }
            },
            {},                                           // std::vector<VkAttachmentReference>   ResolveAttachments
            &depth_attachment,                            // VkAttachmentReference const        * DepthStencilAttachment
            {}                                            // std::vector<uint32_t>                PreserveAttachments
            }
        };

        std::vector<VkSubpassDependency> subpass_dependencies = {
            {
            VK_SUBPASS_EXTERNAL,                            // uint32_t                   srcSubpass
            0,                                              // uint32_t                   dstSubpass
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       srcStageMask
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       dstStageMask
            VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              srcAccessMask
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              dstAccessMask
            VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
            },
            {
            0,                                              // uint32_t                   srcSubpass
            VK_SUBPASS_EXTERNAL,                            // uint32_t                   dstSubpass
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // VkPipelineStageFlags       srcStageMask
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,              // VkPipelineStageFlags       dstStageMask
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,           // VkAccessFlags              srcAccessMask
            VK_ACCESS_MEMORY_READ_BIT,                      // VkAccessFlags              dstAccessMask
            VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
            }
        };

        InitVkDestroyer( logicalDevice, RenderPass );
        
        if( !CreateRenderPass( logicalDevice, attachment_descriptions, subpass_parameters, subpass_dependencies, *RenderPass ) ) {
            return false;
        }

        // Graphics pipeline

        std::vector<VkPushConstantRange> push_constant_ranges = {
            {
            VK_SHADER_STAGE_FRAGMENT_BIT,   // VkShaderStageFlags     stageFlags
            0,                              // uint32_t               offset
            sizeof( float ) * 4             // uint32_t               size
            }
        };

        InitVkDestroyer( logicalDevice, PipelineLayout );
        if( !VulkanCookbook::CreatePipelineLayout( logicalDevice, { *DescriptorSetLayout }, {}, *PipelineLayout ) ) {
            return false;
        }

        std::vector<unsigned char> vertex_shader_spirv;
        if( !VulkanCookbook::GetBinaryFileContents( "Data/Shaders/11 Lighting/01 Rendering a geometry with vertex diffuse lighting/shader.vert.spv", vertex_shader_spirv ) ) {
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
            }
        };

        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
        SpecifyPipelineShaderStages( shader_stage_params, shader_stage_create_infos );

        std::vector<VkVertexInputBindingDescription> vertex_input_binding_descriptions = 
        {
            {
            0,                            // uint32_t                     binding
            stride,                // uint32_t                     stride
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
            { // Normal vector
            1,                                                                        // uint32_t   location
            0,                                                                        // uint32_t   binding
            VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
            3 * sizeof( float )                                                       // uint32_t   offset
            },
            { // Texcoords
            2,                                                                        // uint32_t   location
            0,                                                                        // uint32_t   binding
            VK_FORMAT_R32G32_SFLOAT,                                                  // VkFormat   format
            6 * sizeof( float )                                                       // uint32_t   offset
            },
            { // Tangent vector
            3,                                                                        // uint32_t   location
            0,                                                                        // uint32_t   binding
            VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
            8 * sizeof( float )                                                       // uint32_t   offset
            },
            { // bitangent vector
            4,                                                                        // uint32_t   location
            0,                                                                        // uint32_t   binding
            VK_FORMAT_R32G32B32_SFLOAT,                                               // VkFormat   format
            11 * sizeof( float )                                                      // uint32_t   offset
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
            &dynamic_state_create_info, *PipelineLayout, *RenderPass, 0, VK_NULL_HANDLE, -1, pipeline_create_info );

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
        BeginRenderPass( CommandBuffer, *RenderPass , framebuffer, sizew, { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0 } }, VK_SUBPASS_CONTENTS_INLINE );

        VkViewport viewport = {
          0.0f,                                       // float    x
          0.0f,                                       // float    y
          static_cast<float>(sizew.extent.width),   // float    width
          static_cast<float>(sizew.extent.height),  // float    height
          0.0f,                                       // float    minDepth
          1.0f,                                       // float    maxDepth
        };
        SetViewportStateDynamically( CommandBuffer, 0, { viewport } );

        VkRect2D scissor = {
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


        BindVertexBuffers( CommandBuffer, 0, {{ vectorVertexBuffer.at(0), 0 }} );
              
        BindDescriptorSets( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 0, {DescriptorSets[0]}, { 0,0 } );
        
        std::array<float, 4> light_position = { 0.0f, 10.0f, 0.0f, 0.0f };
        ProvideDataToShadersThroughPushConstants( CommandBuffer, *PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof( float ) * 4, &light_position[0] );
            
        BindPipelineObject( CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *Pipeline );

        DrawGeometry( CommandBuffer, Model.at(0).Parts[0].VertexCount, 1, 0, 0 );
                
        EndRenderPass( CommandBuffer ); 

        
        return true;
    }
    

    bool UpdateStagingBuffer( bool force, VkDevice LogicalDevice, float width, float height, VulkanCookbook::Matrix4x4 vvv )
    {  

        if(force)
        {

          UpdateUniformBuffer = true;

            VulkanCookbook::Matrix4x4 model_view_matrix = PrepareScalingMatrix((float)glyphWidth * 1.1f, (float)glyphHeight * 1.1f, 1.0f);

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
    
    // Comparator to sort by x, then y, then z
    bool comparePoints(const glm::vec3 &a, const glm::vec3 &b) {
        if (a.x != b.x) return a.x < b.x; // Sort by x first
        if (a.y != b.y) return a.y < b.y; // Then by y
        return a.z < b.z;                 // Finally by z
    }
};