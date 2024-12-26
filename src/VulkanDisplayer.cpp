#include "VulkanDisplayer.h"

#include <cstdint>
#include <vulkan/vulkan.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include <set>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

/*Initializing GLFW window passes*/
void VulkanDisplayer::initWindow()
{ /*Initializes the GLFW library*/
    glfwInit();

    /*GLFW was originally designed for OpenGL. We thereofre provide a hint to the library to not create the GL */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    /*Provides a hint to the window to handle resize events*/
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Coursework 2", nullptr, nullptr);

    glfwSetWindowUserPointer(
        window, this); // Set an arbitrary pointer to our window object that we can pass to functions that require it
    // glfwSetWindowSizeCallback(window,
    //     VulkanDisplayer::onWindowResized); // Used to specify a callback whenver a singal is issued for a resize
    //     event}
}

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"}; // The validation layer we will be using. It is the one provided by the LunarG SDK
const std::vector<const char*> deviceExtensions
    = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}; // The device extensions we will be using. It is the one provided by the Vulkan
                                         // SDK
const char* toStringMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT s)
{
    switch (s)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return "VERBOSE";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return "ERROR";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return "WARNING";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return "INFO";
    default: return "UNKNOWN";
    }
}
const char* toStringMessageType(VkDebugUtilsMessageTypeFlagsEXT s)
{
    if (s
        == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "General | Validation | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "Validation | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "General | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT))
        return "General | Validation";
    if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        return "Validation";
    if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        return "General";
    return "Unknown";
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /* pUserData */)
{
    auto ms = toStringMessageSeverity(messageSeverity);
    auto mt = toStringMessageType(messageType);
    printf("[%s: %s]\n%s\n", ms, mt, pCallbackData->pMessage);

    return VK_FALSE;
}
void VulkanDisplayer::run()
{
    initWindow();
    initVulkan();
    main_loop();
    cleanup();
}

void VulkanDisplayer::main_loop()
{
    /*The function checks repeatedly at the start of the loop if glfw has been instructed to stop*/
    while (!glfwWindowShouldClose(window))
    {
        /*Checks continously for any changes that have been made and submits them immmedietely*/
        glfwPollEvents();

        // processKeyboardInput(window, ubo, cameraForwardVector, cameraUpVector);

        /*Displays the triangle to the screen*/
        render();
        // std::cout << "Rendering : " << currentFrame << std::endl;
        // currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    /*This functions ensures all resources have been successfuly deallocated before continuing*/
    vkDeviceWaitIdle(device);
}

void VulkanDisplayer::updateVertexBuffer(uint32_t currentFrame)
{
    // NOT IMPLEMENTED
}
void VulkanDisplayer::updateUniformBuffer(uint32_t currentFrame)
{
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    UniformObject ubo{};
    float ratio = (float) swapChainExtent.width / (float) swapChainExtent.height;
    // getPrerotationMatrix(capabilities, pretransformFlag, ubo.mvp, ratio);
    // mat is initialized to the identity matrix
    ubo.mvp = glm::mat4(1.0f);

    // scale by screen ratio
    ubo.mvp = glm::scale(ubo.mvp, glm::vec3(1.0f, ratio, 1.0f));

    // rotate 1 degree every function call.
    static float currentAngleDegrees = 0.0f;
    currentAngleDegrees += 1.0f;
    ubo.mvp = glm::rotate(ubo.mvp, glm::radians(currentAngleDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    void* data;
    vkMapMemory(device, uniformBufferMemory[currentFrame], 0, sizeof(ubo), 0, &data);
    memcpy(data, glm::value_ptr(ubo.mvp), sizeof(glm::mat4));
    vkUnmapMemory(device, uniformBufferMemory[currentFrame]);
    std::cout << "Update Uniform Buffer : " << currentFrame << std::endl;
}

void VulkanDisplayer::initVulkan()
{
    createInstance();
    setupDebugCallback();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    // establishDisplaySizeIdentity();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffer();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSemaphores();
    is_initialized = true;
}

void VulkanDisplayer::render()
{
    if (!is_initialized)
    {
        return;
    }
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    std::cout << "imageIndex : " << imageIndex << std::endl;
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR); // failed to acquire swap chain image
    updateUniformBuffer(currentFrame);                           // update uniform buffer
    // TODO: updateVertexBuffer(currentFrame);
    vkResetFences(device, 1, &inFlightFences[currentFrame]);
    // vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    // recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]));

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    // if (result == VK_SUBOPTIMAL_KHR)
    // {
    //     orientationChanged = true;
    // }
    // else
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        recreateSwapChain();
    }
    else
    {
        assert(result == VK_SUCCESS); // failed to present swap chain image!
    }
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanDisplayer::recreateSwapChain()
{
    vkDeviceWaitIdle(device);
    cleanupSwapChain();
    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void VulkanDisplayer::cleanupSwapChain()
{
    for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
    }
    vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        vkDestroyImageView(device, swapChainImageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

bool VulkanDisplayer::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    /*Prepare a vector capable of storing the string names.*/
    std::vector<VkLayerProperties> availableLayers(layerCount);
    /*Store the names of the available validation layers inside our vector */
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const char* layer : validationLayers)
    {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layer, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
        {
            return false;
        }
    }
    return true;
}

/* here extentions are supported by glfw */
std::vector<const char*> VulkanDisplayer::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

void VulkanDisplayer::createInstance()
{
    /*Check if validation layers are available by the SDK in use and we have found a suitable validation layer for our
     * applicaiton*/
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not supported");
    }

    /*A struct providing information to the Vulkan library with information about our application*/
    VkApplicationInfo appInfo = {};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;    // The type of strucure we are passing information of.
    appInfo.pApplicationName = "Vulkan Coursework 1";      // The title of our application.
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // The API version of Vulkan we are targetting
    appInfo.pEngineName = "No Engine"; // Indicates we have not used a specific engine for the creation of our
                                       // application. May have just been an nullptr to indicicate that.
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // The version number of the engine used to create the application
    appInfo.apiVersion = VK_API_VERSION_1_0; // This most likely is designed for forward compatibility. It specifies a
                                             // maximum API version that our application can be used on.

    /*Tells vulkan which global extensions we wish to use and validation layers as well*/
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo
        = &appInfo; // Helps the implementation recognize behavoiour which is inherent to classes of applications (??)

    /*Get the rquired extensions from glfw for running our application*/
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size()); // The amount of extensions required
    createInfo.ppEnabledExtensionNames
        = extensions.data(); // Pointer to the address where the list of extensions is stored

    /*
    If the validation layers are available with the SDK
    and we are running in debug mode.
    */
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount
            = static_cast<uint32_t>(validationLayers.size()); // Pass in the amount of the validation layers we will be
                                                              // using. We have only one. The LunarG SDK one.
        createInfo.ppEnabledLayerNames = validationLayers.data(); // Pass in the string representation of the layer. It
                                                                  // is a pointer to the first element in our case.
    }
    else
    {
        /*Do not use validation layers, as they are not enabled*/
        createInfo.enabledExtensionCount = 0;
    }
    // Create the instance
    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
}

VkResult VulkanDisplayer::CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
/* set some debug callback */
void VulkanDisplayer::setupDebugCallback()
{
    if (!enableValidationLayers)
        return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
}

/*create surface by glfw*/
void VulkanDisplayer::createSurface()
{
    VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));
}

QueueFamilyIndices VulkanDisplayer::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }
    return indices;
}
bool VulkanDisplayer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}
SwapChainSupportDetails VulkanDisplayer::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    /*Checks the surface and the device to see the supported capabilities for the swap chain creation*/
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    /*Queries the supported surface formats. Surface formats are mainly the way data is presented*/
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    /*Queries the supported presentation modes for the swap chain mechanism. I.e. the "algorithm" for swapping over
     * images*/
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool VulkanDisplayer::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void VulkanDisplayer::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            physicalDevice = device;
            break;
        }
    }
    if (physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

void VulkanDisplayer::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

/*
If a valid swap chain is found, we now must specify the required swap chain properties
we would like to run for our program.

This is important as each program may have an ideal value in mind for the application,
but must query if that property is supported by the swap chain instance we were provided with.
*/
VkSurfaceFormatKHR VulkanDisplayer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{

    // Choices of format is important as it represents the the way data is stored. I.e. the amount of channels, the
    // memory requirements for each channel etc. Tests if SRGB is available to use or not

    /*
    If Vulkan has no preffered surface format.
    */
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        // Choose the first one which we want
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    // If however, we are not allowed to choose our own format, we would traverse the list and see a possible
    // combination
    for (const auto& availableFormat : availableFormats)
    {
        // Try and search for our required combination
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    // If this also fails, then we can just settle with the first option provided
    // Take note that we could in fact rank the combinations to find a secondary best, but for the purpose of the
    // tutorial that seems extensive.
    return availableFormats[0];
}

/*
This setting handles the way we would present our iamges to the screen.
*/
VkPresentModeKHR VulkanDisplayer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    // Our preffered option
    VkPresentModeKHR bestMode
        = VK_PRESENT_MODE_FIFO_KHR; // Implemented as a queue. The swap chain presents the first image in that queue,
                                    // while loading a rendered image in queue at the back. This mode is guarranteed to
                                    // be available.

    for (const auto& availablePresentMode : availablePresentModes)
    {

        if (availablePresentMode
            == VK_PRESENT_MODE_MAILBOX_KHR) // Good for performance. Non-bloocking. Whenever the queu becomes full, it
                                            // should by convention block incoming images. This, instead, updates the
                                            // queue with the newer images.
        {
            return availablePresentMode;
        }
        else if (availablePresentMode
            == VK_PRESENT_MODE_IMMEDIATE_KHR) // Images would be flushed to the screen immediately upon submission. May
                                              // result in tearing.
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode; // Since the FIFO_KHR mode is guarranteed to be available, if none of our ideal modes are
                     // available, we settle with this one.
}

/*
The swap extennt represents the resolution which best matches window configuraiton
we have.

This value would suually always match the screen width and height, but isnot required to.
It may differ. Therefore we can provide int the maxium value of uint32_t if we wish that to be the case.
*/
VkExtent2D VulkanDisplayer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    /*If we are not required to match the window resolution, simply return the current width and height of the sruface*/
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    /*Else make the surface match the resolution of our window*/
    else
    {
        /*Our window dimensions*/
        int width, height;

        /*Query glfw to return these values and store them in width and height*/
        glfwGetWindowSize(window, &width, &height);

        /*Set the extent to whatever the window resolution values are*/
        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        /*Our actual extent that would be made our current extent would end up choosing a value between the minimum and
         * maximum extent values.*/
        /*That extent would end up matching the window resolution exactly if it is within the range, otherwise it would
         * get clamped*/
        actualExtent.width = std::max(
            capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

void VulkanDisplayer::createSwapChain()
{
    /*Retirieve swap chain requirements*/
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    /*Choose our specified swap chain properties*/
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    /*The amount of images that can be used in our swap chain "queue". */
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount
        + 1; // Try to have the minimum amount + 1. The minimum amount would usualy be a single image (1) to implement
             // double-buffering we would therefore required 2.

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities
                         .maxImageCount; // maxImageCount with a value of 0 woudl only indicate that we are bound only
                                         // by memory to the amount of images in the queue.
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain));

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);

    std::cout << "########################################" << std::endl;
    std::cout << "imageCount : " << imageCount << std::endl;
    std::cout << "########################################" << std::endl;

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void VulkanDisplayer::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]));
    }
}

void VulkanDisplayer::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
}

void VulkanDisplayer::createDescriptorSetLayout()
{
    // 因为我们这里uniform中只有一个ubo，不需要其他的数据，如纹理等，所以只需要一个binding
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {uboLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout));
}

std::vector<char> VulkanDisplayer::readFile(const std::string& filename) // Pass in the file path
{
    /*Read the file as binary data, and begin reading it from the back*/
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    /*If a link to the file has not been successfully associated with as stream*/
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file!");
    }

    /*The returned position at the end of the file and be used to set the file size*/
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize); // And a buffer capable of holding the binary data

    file.seekg(0);                      // Extracts the beginning of the file
    file.read(buffer.data(), fileSize); // Read in filesize bytes from position pointed to by buffer.data() This would
                                        // point to the start of the file

    file.close(); // Close the file for reading

    return buffer;
}

VkShaderModule VulkanDisplayer::createShaderModule(const std::vector<char>& code) // Pass in the bytecode
{
    VkShaderModuleCreateInfo createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();                                 // Size in bytes
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // Pointer to the data

    /*Create the vk shader module*/
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Shader Module!");
    }

    // The buffer witht he code can be free immediately after this, like in OpenGL;

    return shaderModule;
}
void VulkanDisplayer::createGraphicsPipeline()
{
    auto vertShaderCode = readFile("shaders/shader.vert.spv");
    auto fragShaderCode = readFile("shaders/shader.frag.spv");
    // VkShaderModule 是一个代表可编程着色器的 Vulkan
    // 对象。着色器用于对图形数据执行各种操作，例如转换顶点、给像素着色和计算全局效果。
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    /*Gets the binding descriptions which we have created. It recieves information about the layout of the bindings ( if
     * there are more than one) and the layout of the attributes contained in the bound array*/
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    // 设置 vertex shader中 in 中的各个项
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType
        = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; // Specify it is per vertex instead of per instance
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions
        = &bindingDescription; // Most likely passing information of the binding to be used, such as the index in teh
                               // array, the elements it is build from(Vertex) and if it is per vertex or per instance
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
        attributeDescriptions.size()); // The amount of attributes in a single instance ( Vertex, in our case)
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    /*
    Describes the type of geometry which will be drawn
    and if primitve restart should be enabled.
    设置图元装配的格式
    */
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Triangle from every 3 vertices without reuse
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /*
    The viewport is the region of the framebuffer we
    will be rendering to. This would most often
    be the entirety of our screen.
    */
    VkViewport viewport = {};
    viewport.x = 0.0f; // From the top left corner.
    viewport.y = 0.0f;
    viewport.width
        = (float) swapChainExtent
              .width; // The width we have for our images in the swap chain. Done to match the window resolutions.
    viewport.height = (float) swapChainExtent.height; // The height we have for our images in the swap chain
    viewport.minDepth = 0.0f; // The min depth and max depth should stick to 0,0 and 1,0 if we are not doing anything
                              // that requires depth buffering.
    viewport.maxDepth = 1.0f;

    /*The scissor recatangle defines which pixels of the image will be stoed in the framebuffer*/
    VkRect2D scissor = {};

    /*In this case we want one which renders the entire framebuffer so we specify no offsets*/
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;

    /*Combines the viewport and scissor rectangle into a viewport state*/
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    /*
    The rasterizer will take all of the non-clipped vertices
    and transform them into valid pixels, i.e. fragments.
    */
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable
        = VK_FALSE; // Fragments beyond the near and far plane will be discarded instead of clamped

    rasterizer.rasterizerDiscardEnable = VK_FALSE; // If set to true disables any output to the the framebuffer

    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // Determines how fragments are generated for geometry
    rasterizer.lineWidth = 1.0f;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Type of face culling used
    rasterizer.frontFace
        = VK_FRONT_FACE_COUNTER_CLOCKWISE; // The order direction for vertices to be ackoledged as front-facing

    // Usefull for shadow mapping. Can be used for shadow mapping to bias them ( Removes shadow acne).
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    /*
    Configure multisampling. Basically it's used to sample multiple polygons
    would rasterize to same pixel. It is used to fix artifacts along the edges
    where the boundary seems like a rought transition rather than a smooth
    one. Known as antialiasing.
    */
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    /*
    Usually cells in the framebuffer would already have a
    value previously stored in them. It is therefore important
    to set up a concention on whole we wish to do update the cell
    with the new value.

    A couple of options exist.
    Mix the old value with the new one.
    Combine them using a bitwise operation
    */
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask
        = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    /*
    Specifies a structure specifying parameters of a newly created pipeline color blend state
    */
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Maybe mention that some parts of the pipeline here can actually be changed dynamically??

    /*
    You can use uniform values in shaders, which are globals similar to dynamic state variables that can be changed
    at drawing time to alter the behavior of your shaders without having to recreate them.
    They are commonly used to pass the transformation matrix to the vertex shader,
    or to create texture samplers in the fragment shader.
    */
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount
        = 1; // This specifies the amount of descriptor layouts the pipeline will make use of.
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = 0;

    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

    /*
    The graphics pipeline now combines all information
    about shader stages, fixed-function state,
    pipeline layout for uniform data and push constants,
    render pass information.
    */
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr;

    pipelineInfo.layout = pipelineLayout;

    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0; // Specifies the subpass index where the graphics pipeline will be used

    /*Optimization step from Vulkan, pipeline derivatives are pipelines derived by some base pipeline*/
    /*Saves time as it would have most of it's functionality to be similar and just copies it in*/
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    /*The VK_NULL_HANDLE is for a pipeline cache*/
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline));

    /*Once the data has been passed along the graphics pipeline, we don't really require the buffers anymore hence free
     * their memory*/
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}
/*
A framebuffer is a set of valid
VkImage that we use using the framebuffer.
*/
void VulkanDisplayer::createFramebuffers()
{
    /*Resize the buffer to accomodate all framebuffers*/
    swapChainFramebuffers.resize(swapChainImageViews.size());
    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        /*Framebuffers should have the same resoltuions as window image width and height*/
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]));
    }
}

/*
Individaul draw calls cannot be accessed or used directly.
Instead, Vulkan requires them to be placed inside command
buffers.

The advantage of this is that all of the hard work of setting up the drawing commands can be done in advance and in
multiple threads. After that, you just have to tell Vulkan to execute the commands in the main loop.

*/
void VulkanDisplayer::createCommandPool()
{
    /*Command pools will have commandbuffers of the same type only, so we must have information about which type of
     * queues are supported*/
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    /*Command buffers are executed by submitting them on one of the device queues, like the graphics and presentation
    queues we retrieved. Each command pool can only allocate command buffers that are submitted on a single type of
    queue. We're going to record commands for drawing, which is why we've chosen the graphics queue family.*/

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(); // draw commands
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Hints how new commands are being recoreded. If
                                                                      // they often change or they persist.

    VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
}

void VulkanDisplayer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory));

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

uint32_t VulkanDisplayer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    assert(false); // failed to find suitable memory type!
    return -1;
}

VkCommandBuffer VulkanDisplayer::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; // Creates and sets up the command buffer
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool; // Tells to which command pool the buffer belongs to
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; // Weqather commands will be associated with graphics
                                                                   // commands etc
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanDisplayer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

/*Copies over data from a buffer specifed as source to one specifed as destination. It must pass the amount of data to
 * be transferred*/
void VulkanDisplayer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    /*Memory transfer operations are executed using command Buffers*/
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(); // REcords a command buffer

    VkBufferCopy copyRegion = {};
    copyRegion.size = size; // Size of the region we want to copy
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer); // Once it's done, we do not need it anymore so free memory associated with it
}

void VulkanDisplayer::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    vertexBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    vertexBufferMemory.resize(MAX_FRAMES_IN_FLIGHT);
    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);
        // TODO: 关于usage，可能需要修改，因为这里的顶点我们需要每次都去修改
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer[i], vertexBufferMemory[i]);

        copyBuffer(stagingBuffer, vertexBuffer[i], bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
}

void VulkanDisplayer::createIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    indexBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    indexBufferMemory.resize(MAX_FRAMES_IN_FLIGHT);
    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer[i], indexBufferMemory[i]);

        copyBuffer(stagingBuffer, indexBuffer[i], bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
}

void VulkanDisplayer::createUniformBuffer()
{
    VkDeviceSize bufferSize = sizeof(UniformObject); // The buffer size would be the size of the struct
    uniformBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBufferMemory.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer[i],
            uniformBufferMemory[i]);
    }
}

void VulkanDisplayer::createDescriptorPool()
{
    VkDescriptorPoolSize poolSizes[2];
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool));
}

void VulkanDisplayer::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()));

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffer[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformObject);
        // only uniform buffer here
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}

void VulkanDisplayer::createCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = commandBuffers.size();

    VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

    /*Begin recording command buffers*/
    for (size_t i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags
            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // How we're going to use the command buffer. Can be
                                                            // resubmitted while pending execution, in this case.
        beginInfo.pInheritanceInfo = nullptr;

        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

        /*Bind the correct framebuffer for each image, and reuse the same renderpass as we only have one we're
         * interested in*/
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];

        /*Keep the rendering area to the same dimensions as the whole window*/
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        /*When the framebuffer is reset, update the values to black*/
        VkClearValue clearColor = {0.2f, 0.2f, 0.2f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE); // Execute the command buffers with only the primary command buffer itself is
                                         // provided and no secondary command buffers are there.

        vkCmdBindPipeline(
            commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); // Bind the GRAPHICS pipeline

        /*
        The number are as follows

        3 - vertices in the triangle
        1 - Triangle in the scene
        0 - Offset is 0 as data is tightly packed
        0 - Offset between instances is 0, we only have one
        */

        VkBuffer vertexBuffers[] = {vertexBuffer[i]}; // We only have one vertex buffer

        VkDeviceSize offsets[]
            = {0}; // This array specifies a one-to-one mapping between the ammount of vertex buffers and the offsets of
                   // each buffer, i.e from where to start reading vertex data from.

        vkCmdBindVertexBuffers(
            commandBuffers[i], 0, 1, vertexBuffers, offsets); // This call is used to bind vertex buffers to bindings.

        vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer[i], 0,
            VK_INDEX_TYPE_UINT32); // You can only have one idnex buffer, apparently

        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
            &descriptorSets[i], 0,
            nullptr); // They are not unique to graphics pipelines. Hence we specify the bind point to be graphics,

        // vkCmdDraw(commandBuffers[i], 3, 1, 0, 0); /**DRAW THE TRIANGLE***/

        vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]); // End render pass

        VK_CHECK(vkEndCommandBuffer(commandBuffers[i]));
    }
}

/*
Creates two semaphores for global syncrhonization.
One semaphore to signal that an image has been acquired and can be rendered.

One semaphore will signal that rendering has finished and can be passed to the
swap chain to be presented.
*/
void VulkanDisplayer::createSemaphores()
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));

        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));

        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]));
    }
}

/*
Vulkan operates on giving as much power to the
programmer as possible. One of the capabilities this
encompasses is the ability for us to manage and set up
our own allocation systems. Custom allocators etc.

As a result, however, this means that we also manage
the destruction of these objects and must ensure all
resources are returned to the OS at the end of our
program's life cycle.
*/
void VulkanDisplayer::cleanup()
{

    vkDeviceWaitIdle(device);
    cleanupSwapChain();
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(device, uniformBuffer[i], nullptr);
        vkFreeMemory(device, uniformBufferMemory[i], nullptr);
    }
    // vkDestroyBuffer(device, stagingBuffer, nullptr);
    // vkFreeMemory(device, stagingMemory, nullptr);
    // vkFreeMemory(device, textureImageMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroyDevice(device, nullptr);
    // if (enableValidationLayers)
    // {
    //     DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    // }
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    is_initialized = false;
    /*
    This function will destroy the window and
    it's context upon recieving a valid value
    of GLFW's flag for closing a window
    */
    glfwDestroyWindow(window);

    /*
    This function must be called before terminating
    the application. It frees all remianing windows, curosrs,
    and any other allocated resources.
    */
    glfwTerminate();
}
