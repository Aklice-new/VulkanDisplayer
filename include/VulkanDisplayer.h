#ifndef _VULKANDISPLAYER_H_
#define _VULKANDISPLAYER_H_

#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <optional>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;
static const int MAX_FRAMES_IN_FLIGHT = 3;

#define VK_CHECK(x)                                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult err = x;                                                                                              \
        if (err)                                                                                                       \
        {                                                                                                              \
            printf("Detected Vulkan error: %d", err);                                                                  \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

/*Uniform Bufer Object*/
struct UniformObject
{
    // glm::mat4 model;
    // glm::mat4 view;
    // glm::mat4 proj;
    glm::mat4 mvp;
};

// Debug Setting
#ifdef NDEBUG // If the program is run in DEBUG mode.
const bool enableValidationLayers = false;
#else // If the program is run in release mode
const bool enableValidationLayers = true;
#endif

/*The struct which will query the device to return which families of queues are supported*/
struct QueueFamilyIndices
{
    /*Initial value of -1 represents "Not found" or "Not available"*/
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    /*
    Once the neccessary queries to the device have been processed,
    we would either remain with -1 for one or either of the queues,
    or both would now have a positive index ( or 0).
    */
    bool isComplete()
    {
        /*If this function returns true, then both type of queue famileis are supported on our device*/
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

/*Structure which carries information about our swapchain mechanism that would be platform specific*/
struct SwapChainSupportDetails // comment this
{

    VkSurfaceCapabilitiesKHR capabilities; // Swap chain capabilities. Dimensions of images. Maximum and minimum images
                                           // that can be held within our swap chain.
    std::vector<VkSurfaceFormatKHR>
        formats; // Format of the pixels in memory, and the type of color spaces that are supported (SRGB for example).
    std::vector<VkPresentModeKHR>
        presentModes; // A list of supported presentation modes. In a sense, this represents different methods, for
                      // presenting looping the images in the swap chain.
};

class VulkanDisplayer
{

public:
    VulkanDisplayer(const std::vector<Vertex>& vertices_, const std::vector<uint32_t>& indices_)
    {
        vertices = vertices_;
        indices = indices_;
    }
    ~VulkanDisplayer() {}

private:
    /* contains the triangles info*/
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    /************************ Data *************************/

    GLFWwindow* window; // GLFW window object

    /************************* Vulkans *************************/
    VkInstance instance;  // The Vulkan instance represents the connection between OUR application the Vulkan API.
    VkSurfaceKHR surface; // The surface is an interface between the NATIVE windowing API and the Vulkan API. Note, this
                          // is platform specific always.

    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // The physical device represents the actual hardware.
    VkDevice device;

    VkQueue graphicsQueue; // The graphics queue is used to submit command buffers that render images.
    VkQueue presentQueue;  // A set of commands that execture presentation commands

    VkSwapchainKHR swapChain; // The swap chain is essentially a queue of images that are waiting to be presented to the
                              // screen.
    VkFormat swapChainImageFormat; // The chosen surface format will be stored in this variable.
    VkExtent2D swapChainExtent;    // A handle representing the resolution of the images inside the swap chain.
    // VkExtent2D displaySizeIdentity;

    std::vector<VkImage> swapChainImages;         // The images in the swap chain.
    std::vector<VkImageView> swapChainImageViews; // The image views are used to represent the images in the swap chain.

    VkRenderPass renderPass; // Denotes the number and type of formats used in the rendering pass.

    VkDescriptorSetLayout descriptorSetLayout; // The descriptor set layout is used to define the layout of the
                                               // descriptor sets that will be used in the pipeline.
    VkPipelineLayout pipelineLayout; // The pipeline layout is used to define the uniform values in the shader.
    VkPipeline graphicsPipeline;     // The graphics pipeline is used to define the pipeline that will be used to render
                                     // the graphics.
    std::vector<VkFramebuffer> swapChainFramebuffers; // An array of valid render targets which can be rendered to
                                                      // and then submitted to the Queue to execute on the device.

    VkCommandPool commandPool; // The command pool is used to allocate command buffers that will be submitted to the
    std::vector<VkCommandBuffer> commandBuffers; // The command buffers are used to record commands that will be
                                                 // submitted to the device.

    /*Semaphores are used to synchronize the application on a global level as it otherwise does not exist by default to
     * ensure maximum performance. (Explained better in the cpp file)*/
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    std::vector<VkBuffer> vertexBuffer;
    std::vector<VkDeviceMemory> vertexBufferMemory;

    std::vector<VkBuffer> indexBuffer;
    std::vector<VkDeviceMemory> indexBufferMemory;

    std::vector<VkBuffer> uniformBuffer;
    std::vector<VkDeviceMemory> uniformBufferMemory;

    VkDescriptorPool descriptorPool; // The descriptor pool which contains the descriptor sets

    std::vector<VkDescriptorSet>
        descriptorSets; // The descriptor set that will contain all ... ubos? ResourceS? Something?!
    /*
    TODO: I'm not sure whether to use texture here, becauce we dont use texture.
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
    */
    UniformObject ubo;

    /*The coordinate frame's vectors*/
    glm::vec3 cameraForwardVector = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

    // TODO: use this for android
    // bool orientationChanged = false;
public:
    void run();
    void initVulkan();
    void render();
    void main_loop();
    bool is_initialized = false;
    int currentFrame = 0;

private:
    /*initializing GLFW window (only on linux/windows)*/
    void initWindow();
    /* some reset funs */
    void cleanup();
    void cleanupSwapChain();
    void recreateSwapChain();
    /*initializing vulkan passes */
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    void createInstance(); // step 1
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void setupDebugCallback(); // step 2
    void createSurface();      // step 3
    void pickPhysicalDevice(); // step 4
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void createLogicalDevice(); // step 5
    // void establishDisplaySizeIdentity();
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    void createSwapChain();           // step 6
    void createImageViews();          // step 7
    void createRenderPass();          // step 8
    void createDescriptorSetLayout(); // step 9 设置shader中的uniform数据的分布
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& data);
    void createGraphicsPipeline(); // step 10
    void createFramebuffers();     // step 11
    void createCommandPool();      // step 12
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
        VkDeviceMemory& bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();                                  // 用于创建提交command
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);                  //用于完成提交command
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size); // 内存拷贝
    void createVertexBuffer();                                                  // step 13
    void createIndexBuffer();                                                   // step 14
    void createUniformBuffer();                                                 // step 15
    void createDescriptorPool();                                                // step 16
    void createDescriptorSets();                                                // step 17
    void createCommandBuffers();                                                // step 18
    void createSemaphores();                                                    // step 19
    // void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    /* rendering passes */
    void updateUniformBuffer(uint32_t currentImage);
    void updateVertexBuffer(uint32_t currentImage);
    // TODO: we dont need this, because our indices dont change.
    // void updateIndexBuffer(uint32_t currentImage);
};

#endif // _VULKANDISPLAYER_H_