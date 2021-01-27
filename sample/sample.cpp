#define NOMINMAX
#include <Windows.h>
#include <array>
#include <algorithm>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <chrono>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <cts/renderer.h>
#include <cts/typedefs/win32_surface.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

struct QueueFamilyIndices {
    enum { INVALID_QUEUE_FAMILY = UINT32_MAX };

    uint32_t graphicsFamily;
    uint32_t presentFamily;

    bool isComplete() {
        return (
            graphicsFamily != INVALID_QUEUE_FAMILY && 
            presentFamily != INVALID_QUEUE_FAMILY
        );
    }
};

struct SwapChainSupportDetails {
    CtsSurfaceCapabilities capabilities;
    std::vector<CtsSurfaceFormat> formats;
    std::vector<CtsPresentMode> presentModes;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static CtsVertexInputBindingDescription getBindingDescription() {
        CtsVertexInputBindingDescription bindingDescription{};
        
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = CTS_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<CtsVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<CtsVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = CTS_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = CTS_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = CTS_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";

static volatile bool sQuitRequested = false;
static const int MAX_FRAMES_IN_FLIGHT = 2;

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { 
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* createStruct = (CREATESTRUCT*)(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(createStruct->lpCreateParams));
        return TRUE;
    } else if (msg == WM_DESTROY) {
        sQuitRequested = true;
        return TRUE;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam); 
}

void pollEvents(void) {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

class HelloTriangleApplication {
public:
    HelloTriangleApplication()
        : mAllocator(NULL)
        , mWindow(NULL)
        , mCurrentFrame(0)
    {
    }

    void run() {
        initWindow();
        initRenderer();
        mainLoop();
        cleanup();
    }

private:
    void initWindow() {
        const char pClassName[] = "Example";
        const char pTitle[] = "Example";

        HMODULE instance = GetModuleHandle(nullptr);

        WNDCLASS wnd = {};
        wnd.cbClsExtra = 0;
        wnd.cbWndExtra = 0;
        wnd.hCursor = LoadCursor(0, IDC_ARROW);
        wnd.hIcon = LoadIcon(0, IDI_WINLOGO);
        wnd.lpszMenuName = 0;
        wnd.style = 0;
        wnd.hbrBackground = 0;
        wnd.lpfnWndProc = windowProc;
        wnd.hInstance = instance;
        wnd.lpszClassName = pClassName;
        RegisterClass(&wnd);

        mWindow = CreateWindowEx(
            0,
            pClassName,
            pTitle,

            // Window style
            WS_OVERLAPPEDWINDOW,            

            // Size and position
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            WIDTH,
            HEIGHT,

            NULL,
            NULL,
            instance,

            // lpCreateParams
            this
        );

        ShowWindow(mWindow, SW_SHOW);
    }

    void initRenderer() {
        createInstance();
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
        createTextureImage();
        createTextureImageView();
        createTextureSampler();
        loadModel();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
        createSyncObjects();
    }

    void createInstance() {
        CtsApplicationInfo appInfo{};
        appInfo.sType = CTS_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = CTS_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = CTS_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = CTS_API_VERSION_1_0;

        CtsInstanceCreateInfo createInfo{};
        createInfo.sType = CTS_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = 0;
        createInfo.pEnabledExtensionNames = nullptr;
        createInfo.enabledLayerCount = 0;
        createInfo.pEnabledLayerNames = nullptr;

        if (ctsCreateInstance(&createInfo, mAllocator, &mInstance) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void createSurface() {
        CtsWin32SurfaceCreateInfo createInfo{};
        createInfo.sType = CTS_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO;
        createInfo.hwnd = mWindow;
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (ctsCreateWin32Surface(mInstance, &createInfo, mAllocator, &mSurface) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount  = 0;
        ctsEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find supported GPUs!");
        }

        std::vector<CtsPhysicalDevice> devices(deviceCount);
        ctsEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                mPhysicalDevice = device;
                mMsaaSamples = getMaxUsableSampleCount();
                break;
            }
        }

        if (mPhysicalDevice == CTS_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

        std::vector<CtsDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            CtsDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = CTS_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
  
        CtsDeviceCreateInfo createInfo{};
        createInfo.sType = CTS_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        
        createInfo.enabledExtensionCount = 0;
        createInfo.pEnabledExtensionNames = nullptr;

        CtsPhysicalDeviceFeatures deviceFeatures{};
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = 0;
        createInfo.pEnabledLayerNames = nullptr;
        
        if (ctsCreateDevice(mPhysicalDevice, &createInfo, mAllocator, &mDevice) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        ctsGetDeviceQueue(mDevice, indices.graphicsFamily, 0, &mGraphicsQueue);
        ctsGetDeviceQueue(mDevice, indices.presentFamily, 0, &mPresentQueue);
    }

    void recreateSwapChain() {
        ctsQueueWaitIdle(mPresentQueue);
        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createColorResources();
        createDepthResources();
        createFramebuffers();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
    }

    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);

        CtsSurfaceFormat surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        CtsPresentMode presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        CtsExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        CtsSwapchainCreateInfo createInfo{};
        createInfo.sType = CTS_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO;
        createInfo.surface = mSurface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = CTS_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = CTS_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = CTS_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        //createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        //createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = CTS_TRUE;
        createInfo.oldSwapchain = CTS_NULL_HANDLE;

        if (ctsCreateSwapchain(mDevice, &createInfo, mAllocator, &mSwapChain) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        //uint32_t imageCount;
        ctsGetSwapchainImages(mDevice, mSwapChain, &imageCount, nullptr);
        mSwapChainImages.resize(imageCount);

        ctsGetSwapchainImages(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());
        mSwapChainImageFormat = surfaceFormat.format;
        mSwapChainExtent = extent;
    }

    void createImageViews() {
        mSwapChainImageViews.resize(mSwapChainImages.size());

        for (size_t i = 0; i < mSwapChainImages.size(); i++) {
            mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainImageFormat, CTS_IMAGE_ASPECT_COLOR_BIT, 1);
        }
    }

    void createRenderPass() {
        CtsAttachmentDescription colorAttachment{};
        colorAttachment.format = mSwapChainImageFormat;
        colorAttachment.samples = mMsaaSamples;
        colorAttachment.loadOp = CTS_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = CTS_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = CTS_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = CTS_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = CTS_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = CTS_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        CtsAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = mMsaaSamples;
        depthAttachment.loadOp = CTS_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = CTS_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = CTS_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = CTS_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = CTS_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = CTS_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        CtsAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = mSwapChainImageFormat;
        colorAttachmentResolve.samples = CTS_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = CTS_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = CTS_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = CTS_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = CTS_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = CTS_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = CTS_IMAGE_LAYOUT_PRESENT_SRC;

        CtsAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = CTS_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        CtsAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = CTS_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        CtsAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = CTS_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        CtsSubpassDescription subpass{};
        subpass.pipelineBindPoint = CTS_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        CtsSubpassDependency dependency{};
        dependency.srcSubpass = CTS_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = CTS_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = CTS_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = CTS_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<CtsAttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
        CtsRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = CTS_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (ctsCreateRenderPass(mDevice, &renderPassInfo, mAllocator, &mRenderPass) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void createDescriptorSetLayout() {
        CtsDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = CTS_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        CtsDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorType = CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.stageFlags = CTS_SHADER_STAGE_FRAGMENT_BIT;
        samplerLayoutBinding.pImmutableSamplers = nullptr; // Optional 

        std::array<CtsDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
        CtsDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = CTS_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (ctsCreateDescriptorSetLayout(mDevice, &layoutInfo, mAllocator, &mDescriptorSetLayout) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createGraphicsPipeline() {
        auto vertShaderCode = readFile("shaders/shader.vert");
        auto fragShaderCode = readFile("shaders/shader.frag"); 

        CtsShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        CtsShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        CtsPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = CTS_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = CTS_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        CtsPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = CTS_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = CTS_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        CtsPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        CtsPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = CTS_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        CtsPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = CTS_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = CTS_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = CTS_FALSE;

        CtsViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) mSwapChainExtent.width;
        viewport.height = (float) mSwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        CtsRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = mSwapChainExtent;

        CtsPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = CTS_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        CtsPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = CTS_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = CTS_FALSE;
        rasterizer.rasterizerDiscardEnable = CTS_FALSE;
        rasterizer.polygonMode = CTS_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = CTS_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = CTS_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = CTS_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        CtsPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = CTS_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = CTS_TRUE; // enable sample shading in the pipeline
        multisampling.minSampleShading = .2f;
        multisampling.rasterizationSamples = mMsaaSamples;
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = CTS_FALSE; // Optional
        multisampling.alphaToOneEnable = CTS_FALSE; // Optional

        CtsPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = CTS_COLOR_COMPONENT_R_BIT | CTS_COLOR_COMPONENT_G_BIT | CTS_COLOR_COMPONENT_B_BIT | CTS_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = CTS_FALSE;
        colorBlendAttachment.srcColorBlendFactor = CTS_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = CTS_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = CTS_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = CTS_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = CTS_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = CTS_BLEND_OP_ADD; // Optional

        CtsPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = CTS_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = CTS_FALSE;
        colorBlending.logicOp = CTS_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        CtsPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = CTS_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = CTS_TRUE;
        depthStencil.depthWriteEnable = CTS_TRUE;
        depthStencil.depthCompareOp = CTS_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = CTS_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = CTS_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        CtsPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = CTS_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (ctsCreatePipelineLayout(mDevice, &pipelineLayoutInfo, mAllocator, &mPipelineLayout) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        CtsGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = CTS_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.layout = mPipelineLayout;
        pipelineInfo.renderPass = mRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = CTS_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (ctsCreateGraphicsPipelines(mDevice, CTS_NULL_HANDLE, 1, &pipelineInfo, mAllocator, &mGraphicsPipeline) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        ctsDestroyShaderModule(mDevice, vertShaderModule, mAllocator);
        ctsDestroyShaderModule(mDevice, fragShaderModule, mAllocator);
    }

    void createFramebuffers() {
        mSwapChainFramebuffers.resize(mSwapChainImages.size());

        for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
            std::array<CtsImageView, 3> attachments;
            attachments[0] = mColorImageView;
            attachments[1] = mDepthImageView;
            attachments[2] = mSwapChainImageViews[i];

            CtsFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = CTS_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = mRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = mSwapChainExtent.width;
            framebufferInfo.height = mSwapChainExtent.height;
            framebufferInfo.layers = 1;

            if (ctsCreateFramebuffer(mDevice, &framebufferInfo, mAllocator, &mSwapChainFramebuffers[i]) != CTS_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);

        CtsCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = CTS_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = 0; // Optional

        if (ctsCreateCommandPool(mDevice, &poolInfo, mAllocator, &mCommandPool) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void createColorResources() {
        CtsFormat colorFormat = mSwapChainImageFormat;

        createImage(
            mSwapChainExtent.width, 
            mSwapChainExtent.height, 
            1, 
            mMsaaSamples, 
            colorFormat, 
            CTS_IMAGE_TILING_OPTIMAL, 
            CTS_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | CTS_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
            CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            mColorImage, 
            mColorImageMemory
        );

        mColorImageView = createImageView(mColorImage, colorFormat, CTS_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void createDepthResources() {
        CtsFormat depthFormat = findDepthFormat();

        createImage(
            mSwapChainExtent.width,
            mSwapChainExtent.height,
            1,
            mMsaaSamples,
            depthFormat,
            CTS_IMAGE_TILING_OPTIMAL,
            CTS_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mDepthImage,
            mDepthImageMemory
        );

        mDepthImageView = createImageView(mDepthImage, depthFormat, CTS_IMAGE_ASPECT_DEPTH_BIT, 1);

        transitionImageLayout(mDepthImage, depthFormat, CTS_IMAGE_LAYOUT_UNDEFINED, CTS_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

    void createTextureImage() {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
        CtsDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        CtsBuffer stagingBuffer;
        CtsDeviceMemory stagingBufferMemory;

        createBuffer(
            imageSize,
            CTS_BUFFER_USAGE_TRANSFER_SRC_BIT,
            CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT | CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        void* data;
        ctsMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        ctsUnmapMemory(mDevice, stagingBufferMemory);

        stbi_image_free(pixels);

        createImage(
            texWidth,
            texHeight,
            mMipLevels,
            CTS_SAMPLE_COUNT_1_BIT,
            CTS_FORMAT_R8G8B8A8_SRGB,
            CTS_IMAGE_TILING_OPTIMAL, 
            CTS_IMAGE_USAGE_TRANSFER_DST_BIT | CTS_IMAGE_USAGE_SAMPLED_BIT, 
            CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mTextureImage, 
            mTextureImageMemory
        );

        transitionImageLayout(mTextureImage, CTS_FORMAT_R8G8B8A8_SRGB, CTS_IMAGE_LAYOUT_UNDEFINED, CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mMipLevels);
        copyBufferToImage(stagingBuffer, mTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        generateMipmaps(mTextureImage, texWidth, texHeight, mMipLevels);
        
        ctsDestroyBuffer(mDevice, stagingBuffer, mAllocator);
        ctsFreeMemory(mDevice, stagingBufferMemory, mAllocator);
    }

    void createTextureImageView() {
        mTextureImageView = createImageView(mTextureImage, CTS_FORMAT_R8G8B8A8_SRGB, CTS_IMAGE_ASPECT_COLOR_BIT, mMipLevels);
    }

    void createTextureSampler() {
        CtsSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = CTS_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = CTS_FILTER_LINEAR;
        samplerInfo.minFilter = CTS_FILTER_LINEAR;
        samplerInfo.addressModeU = CTS_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = CTS_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = CTS_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = CTS_FALSE;
        samplerInfo.maxAnisotropy = 0.0f;
        samplerInfo.borderColor = CTS_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = CTS_FALSE;
        samplerInfo.compareEnable = CTS_FALSE;
        samplerInfo.compareOp = CTS_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = CTS_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mMipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (ctsCreateSampler(mDevice, &samplerInfo, mAllocator, &mTextureSampler) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void loadModel() {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
                    mVertices.push_back(vertex);
                }

                mIndices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void createVertexBuffer() {
        CtsDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

        CtsBuffer stagingBuffer;
        CtsDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, CTS_BUFFER_USAGE_TRANSFER_SRC_BIT, CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT | CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data = NULL;
        ctsMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, mVertices.data(), (size_t) bufferSize);
        ctsUnmapMemory(mDevice, stagingBufferMemory);
        
        createBuffer(bufferSize, CTS_BUFFER_USAGE_TRANSFER_SRC_BIT | CTS_BUFFER_USAGE_VERTEX_BUFFER_BIT, CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);
        copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

        ctsDestroyBuffer(mDevice, stagingBuffer, mAllocator);
        ctsFreeMemory(mDevice, stagingBufferMemory, mAllocator);
    }

    void createIndexBuffer() {
        CtsDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

        CtsBuffer stagingBuffer;
        CtsDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, CTS_BUFFER_USAGE_TRANSFER_SRC_BIT, CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT | CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data = NULL;
        ctsMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, mIndices.data(), (size_t) bufferSize);
        ctsUnmapMemory(mDevice, stagingBufferMemory);
        
        createBuffer(bufferSize, CTS_BUFFER_USAGE_TRANSFER_SRC_BIT | CTS_BUFFER_USAGE_INDEX_BUFFER_BIT, CTS_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);
        copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

        ctsDestroyBuffer(mDevice, stagingBuffer, mAllocator);
        ctsFreeMemory(mDevice, stagingBufferMemory, mAllocator);
    }

    void createUniformBuffers() {
        CtsDeviceSize bufferSize = sizeof(UniformBufferObject);

        mUniformBuffers.resize(mSwapChainImages.size());
        mUniformBuffersMemory.resize(mSwapChainImages.size());
        mUniformBuffersData.resize(mSwapChainImages.size());

        for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
            createBuffer(bufferSize, CTS_BUFFER_USAGE_UNIFORM_BUFFER_BIT, CTS_MEMORY_PROPERTY_HOST_VISIBLE_BIT | CTS_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers[i], mUniformBuffersMemory[i]);
            ctsMapMemory(mDevice, mUniformBuffersMemory[i], 0, bufferSize, 0, &mUniformBuffersData[i]);
        }
    }

    void createDescriptorPool() {
        std::array<CtsDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(mSwapChainImages.size());
        poolSizes[1].type = CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(mSwapChainImages.size());

        CtsDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = CTS_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(mSwapChainImages.size());

        if (ctsCreateDescriptorPool(mDevice, &poolInfo, mAllocator, &mDescriptorPool) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets() {
        std::vector<CtsDescriptorSetLayout> layouts(mSwapChainImages.size(), mDescriptorSetLayout);
        
        CtsDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = CTS_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = mDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(mSwapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        mDescriptorSets.resize(mSwapChainImages.size());
        if (ctsAllocateDescriptorSets(mDevice, &allocInfo, mDescriptorSets.data()) != CTS_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < mSwapChainImages.size(); i++) {
            CtsDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = mUniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            CtsDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = CTS_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = mTextureImageView;
            imageInfo.sampler = mTextureSampler;

            std::array<CtsWriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].sType = CTS_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = mDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = CTS_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = CTS_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = mDescriptorSets[i]; 
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = CTS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = nullptr; // Optional
            descriptorWrites[1].pImageInfo = &imageInfo;
            descriptorWrites[1].pTexelBufferView = nullptr; // Optional

            ctsUpdateDescriptorSets(mDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void createCommandBuffers() {
        mCommandBuffers.resize(mSwapChainFramebuffers.size());

        CtsCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = CTS_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mCommandPool;
        allocInfo.level = CTS_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();

        if (ctsAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != CTS_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < mCommandBuffers.size(); i++) {
            CtsCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = CTS_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (ctsBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != CTS_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            std::array<CtsClearValue, 2> clearValues{};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            CtsRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = CTS_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = mRenderPass;
            renderPassInfo.framebuffer = mSwapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = mSwapChainExtent;
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            ctsCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, CTS_SUBPASS_CONTENTS_INLINE);
            ctsCmdBindPipeline(mCommandBuffers[i], CTS_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

            CtsBuffer vertexBuffers[] = { mVertexBuffer };
            CtsDeviceSize offsets[] = { 0 };
            ctsCmdBindVertexBuffers(mCommandBuffers[i], 0, 1, vertexBuffers, offsets);

            ctsCmdBindIndexBuffer(mCommandBuffers[i], mIndexBuffer, 0, CTS_INDEX_TYPE_UINT32);
            ctsCmdBindDescriptorSets(mCommandBuffers[i], CTS_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[i], 0, nullptr);

            ctsCmdDrawIndexed(mCommandBuffers[i], static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
            ctsCmdEndRenderPass(mCommandBuffers[i]);

            if (ctsEndCommandBuffer(mCommandBuffers[i]) != CTS_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void createSyncObjects() {
        mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        mImagesInFlight.resize(mSwapChainImages.size(), CTS_NULL_HANDLE);

        CtsSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = CTS_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        CtsFenceCreateInfo fenceInfo{};
        fenceInfo.sType = CTS_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = CTS_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (ctsCreateSemaphore(mDevice, &semaphoreInfo, mAllocator, &mImageAvailableSemaphores[i]) != CTS_SUCCESS ||
                ctsCreateSemaphore(mDevice, &semaphoreInfo, mAllocator, &mRenderFinishedSemaphores[i]) != CTS_SUCCESS ||
                ctsCreateFence(mDevice, &fenceInfo, mAllocator, &mInFlightFences[i]) != CTS_SUCCESS
            ) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void mainLoop() {
        while (!sQuitRequested) {
            pollEvents();
            drawFrame();
        }
    }

    void drawFrame() {
        ctsWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], CTS_TRUE, UINT64_MAX);

        uint32_t imageIndex = 0;
        CtsResult result = ctsAcquireNextImage(mDevice, mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], CTS_NULL_HANDLE, &imageIndex);

        if (result == CTS_ERROR_OUT_OF_DATE) {
            recreateSwapChain();
            return;
        } else if (result != CTS_SUCCESS && result != CTS_SUBOPTIMAL) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        if (mImagesInFlight[imageIndex] != CTS_NULL_HANDLE) {
            ctsWaitForFences(mDevice, 1, &mImagesInFlight[imageIndex], CTS_TRUE, UINT64_MAX);
        }

        mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];

        updateUniformBuffer(imageIndex);

        CtsSubmitInfo submitInfo{};
        submitInfo.sType = CTS_STRUCTURE_TYPE_SUBMIT_INFO;

        CtsSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
        CtsSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };

        CtsPipelineStageFlags waitStages[] = { CTS_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        ctsResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

        if (ctsQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != CTS_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        CtsPresentInfo presentInfo{};
        presentInfo.sType = CTS_STRUCTURE_TYPE_PRESENT_INFO;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        CtsSwapchain swapChains[] = {mSwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        result = ctsQueuePresent(mPresentQueue, &presentInfo);
        if (result == CTS_ERROR_OUT_OF_DATE || result == CTS_SUBOPTIMAL) {
            recreateSwapChain();
        } else if (result != CTS_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void cleanupSwapChain()
    {
        ctsDestroyImageView(mDevice, mColorImageView, mAllocator);
        ctsDestroyImage(mDevice, mColorImage, mAllocator);
        ctsFreeMemory(mDevice, mColorImageMemory, mAllocator);
        
        ctsDestroyImageView(mDevice, mDepthImageView, mAllocator);
        ctsDestroyImage(mDevice, mDepthImage, mAllocator);
        ctsFreeMemory(mDevice, mDepthImageMemory, mAllocator);

        for (auto framebuffer : mSwapChainFramebuffers) {
            ctsDestroyFramebuffer(mDevice, framebuffer, mAllocator);
        }

        ctsFreeCommandBuffers(mDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

        ctsDestroyPipeline(mDevice, mGraphicsPipeline, mAllocator);
        ctsDestroyPipelineLayout(mDevice, mPipelineLayout, mAllocator);
        ctsDestroyRenderPass(mDevice, mRenderPass, mAllocator);

        for (auto imageView : mSwapChainImageViews) {
            ctsDestroyImageView(mDevice, imageView, mAllocator);
        }

        ctsDestroySwapchain(mDevice, mSwapChain, mAllocator);

        for (size_t i = 0; i < mSwapChainImages.size(); ++i) {
            ctsUnmapMemory(mDevice, mUniformBuffersMemory[i]);
            ctsDestroyBuffer(mDevice, mUniformBuffers[i], mAllocator);
            ctsFreeMemory(mDevice, mUniformBuffersMemory[i], mAllocator);
        }

        ctsDestroyDescriptorPool(mDevice, mDescriptorPool, mAllocator);
    }

    void cleanup() {
        cleanupSwapChain();

        ctsDestroySampler(mDevice, mTextureSampler, mAllocator);
        ctsDestroyImageView(mDevice, mTextureImageView, mAllocator);
        ctsDestroyImage(mDevice, mTextureImage, mAllocator);
        ctsFreeMemory(mDevice, mTextureImageMemory, mAllocator);

        ctsDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, mAllocator);
        ctsDestroyBuffer(mDevice, mIndexBuffer, mAllocator);
        ctsFreeMemory(mDevice, mIndexBufferMemory, mAllocator);

        ctsDestroyBuffer(mDevice, mVertexBuffer, mAllocator);
        ctsFreeMemory(mDevice, mVertexBufferMemory, mAllocator);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            ctsDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], mAllocator);
            ctsDestroySemaphore(mDevice, mImageAvailableSemaphores[i], mAllocator);
            ctsDestroyFence(mDevice, mInFlightFences[i], mAllocator);
        }
        
        ctsDestroyCommandPool(mDevice, mCommandPool, mAllocator);


        ctsDestroyDevice(mDevice, mAllocator);
        ctsDestroyWin32Surface(mSurface, mAllocator);
        ctsDestroyInstance(mInstance, mAllocator);
        DestroyWindow(mWindow);
    }

    QueueFamilyIndices findQueueFamilies(CtsPhysicalDevice physicalDevice) {
        QueueFamilyIndices indices {
            QueueFamilyIndices::INVALID_QUEUE_FAMILY,
            QueueFamilyIndices::INVALID_QUEUE_FAMILY
        };
        
        uint32_t queueFamilyCount = 0;
        ctsGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<CtsQueueFamilyProperties> queueFamilies(queueFamilyCount);
        ctsGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        uint32_t queueFamilyIndex = 0;
        CtsBool32 presentSupport = false;

        for (const auto& queueFamily : queueFamilies) {
            ctsGetPhysicalDeviceSurfaceSupport(physicalDevice, queueFamilyIndex, mSurface, &presentSupport);

            if (queueFamily.queueFlags & CTS_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = queueFamilyIndex;
            }

            if (presentSupport) {
                indices.presentFamily = queueFamilyIndex;
            }

            if (indices.isComplete()) {
                break;
            }

            ++queueFamilyIndex;
        }

        return indices;
    }

    SwapChainSupportDetails querySwapChainSupport(CtsPhysicalDevice physicalDevice) {
        SwapChainSupportDetails details;
        ctsGetPhysicalDeviceSurfaceCapabilities(physicalDevice, mSurface, &details.capabilities);

        uint32_t formatCount;
        ctsGetPhysicalDeviceSurfaceFormats(physicalDevice, mSurface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            ctsGetPhysicalDeviceSurfaceFormats(physicalDevice, mSurface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        ctsGetPhysicalDeviceSurfacePresentModes(physicalDevice, mSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            ctsGetPhysicalDeviceSurfacePresentModes(physicalDevice, mSurface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    CtsSurfaceFormat chooseSwapSurfaceFormat(const std::vector<CtsSurfaceFormat>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == CTS_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == CTS_COLOR_SPACE_SRGB_NONLINEAR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    CtsPresentMode chooseSwapPresentMode(const std::vector<CtsPresentMode>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == CTS_PRESENT_MODE_MAILBOX) {
                return availablePresentMode;
            }
        }

        return CTS_PRESENT_MODE_FIFO;
    }

    CtsExtent2D chooseSwapExtent(const CtsSurfaceCapabilities& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } 
        
        RECT rect;
        if (!GetWindowRect(mWindow, &rect)) {
            return CtsExtent2D{0, 0};
        }

        CtsExtent2D actualExtent;
        actualExtent.width  = (uint32_t)(rect.right - rect.left);
        actualExtent.height = (uint32_t)(rect.bottom - rect.top);

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }

    bool checkDeviceExtensionSupport(CtsPhysicalDevice physicalDevice) {
        static const std::vector<const char*> deviceExtensions = {
            CTS_SWAPCHAIN_EXTENSION_NAME
        };

        uint32_t extensionCount;
        ctsEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<CtsExtensionProperties> availableExtensions(extensionCount);
        ctsEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool isDeviceSuitable(CtsPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    CtsShaderModule createShaderModule(const std::vector<char>& code) {
        CtsShaderModuleCreateInfo createInfo{};
        createInfo.sType = CTS_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        CtsShaderModule shaderModule;
        if (ctsCreateShaderModule(mDevice, &createInfo, mAllocator, &shaderModule) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    std::vector<char> readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    uint32_t findMemoryType(uint32_t typeFilter, CtsMemoryPropertyFlags properties) {
        CtsPhysicalDeviceMemoryProperties memProperties;
        ctsGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void createBuffer(CtsDeviceSize size, CtsBufferUsageFlags usage, CtsMemoryPropertyFlags properties, CtsBuffer& buffer, CtsDeviceMemory& bufferMemory) {
        CtsBufferCreateInfo bufferInfo{};
        bufferInfo.sType = CTS_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = CTS_SHARING_MODE_EXCLUSIVE;

        if (ctsCreateBuffer(mDevice, &bufferInfo, mAllocator, &buffer) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        CtsMemoryRequirements memRequirements;
        ctsGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

        CtsMemoryAllocateInfo allocInfo{};
        allocInfo.sType = CTS_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (ctsAllocateMemory(mDevice, &allocInfo, mAllocator, &bufferMemory) != CTS_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        ctsBindBufferMemory(mDevice, buffer, bufferMemory, 0);
    }

    void copyBuffer(CtsBuffer srcBuffer, CtsBuffer dstBuffer, CtsDeviceSize size) {
        CtsCommandBuffer commandBuffer = beginSingleTimeCommands();

        CtsBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        ctsCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void copyBufferToImage(CtsBuffer buffer, CtsImage image, uint32_t width, uint32_t height) {
        CtsCommandBuffer commandBuffer = beginSingleTimeCommands();

        CtsBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        ctsCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        endSingleTimeCommands(commandBuffer);
    }

    void transitionImageLayout(CtsImage image, CtsFormat format, CtsImageLayout oldLayout, CtsImageLayout newLayout, uint32_t mipLevels) {
        CtsCommandBuffer commandBuffer = beginSingleTimeCommands();

        CtsImageMemoryBarrier barrier{};
        barrier.sType = CTS_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = CTS_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = CTS_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        CtsPipelineStageFlags sourceStage;
        CtsPipelineStageFlags destinationStage;

        if (newLayout == CTS_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = CTS_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= CTS_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
        }

        if (oldLayout == CTS_IMAGE_LAYOUT_UNDEFINED && newLayout == CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = CTS_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = CTS_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = CTS_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == CTS_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = CTS_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = CTS_ACCESS_SHADER_READ_BIT;

            sourceStage = CTS_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = CTS_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == CTS_IMAGE_LAYOUT_UNDEFINED && newLayout == CTS_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = CTS_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | CTS_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = CTS_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = CTS_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        ctsCmdPipelineBarrier(
            commandBuffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), mSwapChainExtent.width / (float) mSwapChainExtent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(mUniformBuffersData[currentImage], &ubo, sizeof(ubo));
    }

    void createImage(
        uint32_t width, 
        uint32_t height, 
        uint32_t mipLevels,
        CtsSampleCountFlagBits numSamples,
        CtsFormat format, 
        CtsImageTiling tiling, 
        CtsImageUsageFlags usage, 
        CtsMemoryPropertyFlags properties, 
        CtsImage& image, 
        CtsDeviceMemory& imageMemory
    ) {
        CtsImageCreateInfo imageInfo{};
        imageInfo.sType = CTS_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = CTS_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(width);
        imageInfo.extent.height = static_cast<uint32_t>(height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = CTS_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = CTS_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0; // Optional

        if (ctsCreateImage(mDevice, &imageInfo, mAllocator, &image) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        CtsMemoryRequirements memRequirements;
        ctsGetImageMemoryRequirements(mDevice, image, &memRequirements);

        CtsMemoryAllocateInfo allocInfo{};
        allocInfo.sType = CTS_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (ctsAllocateMemory(mDevice, &allocInfo, mAllocator, &imageMemory) != CTS_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        ctsBindImageMemory(mDevice, image, imageMemory, 0);
    }

    CtsImageView createImageView(CtsImage image, CtsFormat format, CtsImageAspectFlags aspectFlags, uint32_t mipLevels) {
        CtsImageViewCreateInfo viewInfo{};
        viewInfo.sType = CTS_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = CTS_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.components.r = CTS_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = CTS_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = CTS_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = CTS_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        CtsImageView imageView;
        if (ctsCreateImageView(mDevice, &viewInfo, mAllocator, &imageView) != CTS_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void generateMipmaps(CtsImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
        CtsCommandBuffer commandBuffer = beginSingleTimeCommands();

        CtsImageMemoryBarrier barrier{};
        barrier.sType = CTS_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = CTS_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = CTS_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = CTS_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = CTS_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = CTS_ACCESS_TRANSFER_READ_BIT;

            ctsCmdPipelineBarrier(
                commandBuffer,
                CTS_PIPELINE_STAGE_TRANSFER_BIT,
                CTS_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier
            );

            CtsImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            ctsCmdBlitImage(
                commandBuffer,
                image,
                CTS_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image,
                CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &blit,
                CTS_FILTER_LINEAR
            );

            barrier.oldLayout = CTS_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = CTS_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = CTS_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = CTS_ACCESS_SHADER_READ_BIT;

            ctsCmdPipelineBarrier(
                commandBuffer,
                CTS_PIPELINE_STAGE_TRANSFER_BIT,
                CTS_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = CTS_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = CTS_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = CTS_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = CTS_ACCESS_SHADER_READ_BIT;

        ctsCmdPipelineBarrier(
            commandBuffer,
            CTS_PIPELINE_STAGE_TRANSFER_BIT,
            CTS_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        endSingleTimeCommands(commandBuffer);
    }

    CtsCommandBuffer beginSingleTimeCommands() {
        CtsCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = CTS_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = CTS_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = mCommandPool;
        allocInfo.commandBufferCount = 1;

        CtsCommandBuffer commandBuffer;
        ctsAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

        CtsCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = CTS_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = CTS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        ctsBeginCommandBuffer(commandBuffer, &beginInfo); 

        return commandBuffer;
    }

    void endSingleTimeCommands(CtsCommandBuffer commandBuffer) {
        ctsEndCommandBuffer(commandBuffer);

        CtsSubmitInfo submitInfo{};
        submitInfo.sType = CTS_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        ctsQueueSubmit(mGraphicsQueue, 1, &submitInfo, CTS_NULL_HANDLE);
        ctsQueueWaitIdle(mGraphicsQueue);

        ctsFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
    }

    CtsFormat findSupportedFormat(const std::vector<CtsFormat>& candidates, CtsImageTiling tiling, CtsFormatFeatureFlags features) {
        for (CtsFormat format : candidates) {
            CtsFormatProperties props;
            ctsGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

            if (tiling == CTS_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == CTS_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    CtsFormat findDepthFormat() {
        return findSupportedFormat(
            {CTS_FORMAT_D32_SFLOAT, CTS_FORMAT_D32_SFLOAT_S8_UINT, CTS_FORMAT_D24_UNORM_S8_UINT},
            CTS_IMAGE_TILING_OPTIMAL,
            CTS_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    bool hasStencilComponent(CtsFormat format) {
        return format == CTS_FORMAT_D32_SFLOAT_S8_UINT || format == CTS_FORMAT_D24_UNORM_S8_UINT;
    }

    CtsSampleCountFlagBits getMaxUsableSampleCount() {
        CtsPhysicalDeviceProperties physicalDeviceProperties;
        ctsGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

        CtsSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & CTS_SAMPLE_COUNT_64_BIT) { return CTS_SAMPLE_COUNT_64_BIT; }
        if (counts & CTS_SAMPLE_COUNT_32_BIT) { return CTS_SAMPLE_COUNT_32_BIT; }
        if (counts & CTS_SAMPLE_COUNT_16_BIT) { return CTS_SAMPLE_COUNT_16_BIT; }
        if (counts & CTS_SAMPLE_COUNT_8_BIT) { return CTS_SAMPLE_COUNT_8_BIT; }
        if (counts & CTS_SAMPLE_COUNT_4_BIT) { return CTS_SAMPLE_COUNT_4_BIT; }
        if (counts & CTS_SAMPLE_COUNT_2_BIT) { return CTS_SAMPLE_COUNT_2_BIT; }

        return CTS_SAMPLE_COUNT_1_BIT;
    }

    const CtsAllocationCallbacks* mAllocator;
    HWND mWindow;
    CtsInstance mInstance;
    CtsPhysicalDevice mPhysicalDevice;
    CtsSampleCountFlagBits mMsaaSamples;
    uint32_t mQueueFamilyIndex;
    CtsDevice mDevice;
    CtsQueue mGraphicsQueue;
    CtsQueue mPresentQueue;
    CtsSurface mSurface;
    CtsSwapchain mSwapChain;
    std::vector<CtsImage> mSwapChainImages;
    std::vector<CtsImageView> mSwapChainImageViews;
    CtsFormat mSwapChainImageFormat;
    CtsExtent2D mSwapChainExtent;
    CtsRenderPass mRenderPass;
    CtsDescriptorSetLayout mDescriptorSetLayout;
    CtsPipelineLayout mPipelineLayout;
    CtsPipeline mGraphicsPipeline;
    std::vector<CtsFramebuffer> mSwapChainFramebuffers;
    CtsCommandPool mCommandPool;
    
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;
    
    CtsBuffer mVertexBuffer;
    CtsDeviceMemory mVertexBufferMemory;
    CtsBuffer mIndexBuffer;
    CtsDeviceMemory mIndexBufferMemory;

    CtsImage mColorImage;
    CtsDeviceMemory mColorImageMemory;
    CtsImageView mColorImageView;

    CtsImage mDepthImage;
    CtsDeviceMemory mDepthImageMemory;
    CtsImageView mDepthImageView;

    CtsImage mTextureImage;
    CtsDeviceMemory mTextureImageMemory;
    CtsImageView mTextureImageView;
    CtsSampler mTextureSampler;
    uint32_t mMipLevels;

    std::vector<CtsBuffer> mUniformBuffers;
    std::vector<CtsDeviceMemory> mUniformBuffersMemory;
    std::vector<void*> mUniformBuffersData;

    CtsDescriptorPool mDescriptorPool;
    std::vector<CtsDescriptorSet> mDescriptorSets;

    std::vector<CtsCommandBuffer> mCommandBuffers;
    std::vector<CtsSemaphore> mImageAvailableSemaphores;
    std::vector<CtsSemaphore> mRenderFinishedSemaphores;
    std::vector<CtsFence> mInFlightFences;
    std::vector<CtsFence> mImagesInFlight;
    size_t mCurrentFrame;
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
