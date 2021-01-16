#include <Windows.h>
#include <exception>
#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <cts/renderer.h>
#include <cts/typedefs/win32_surface.h>

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

static bool sQuitRequested = false;

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
        : mWindow(NULL)
        , mInstance(CTS_NULL_HANDLE)
        , mPhysicalDevice(CTS_NULL_HANDLE)
        , mQueueFamilyIndex(UINT32_MAX)
        , mDevice(CTS_NULL_HANDLE)
        , mGraphicsQueue(CTS_NULL_HANDLE)
        , mPresentQueue(CTS_NULL_HANDLE)
        , mSurface(CTS_NULL_HANDLE)
        , mAllocator(NULL)
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
        const int width = 800;
        const int height = 800;
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
            width,
            height,

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

        uint32_t imageCount;
        ctsGetSwapchainImages(mDevice, mSwapChain, &imageCount, nullptr);
        mSwapChainImages.resize(imageCount);

        ctsGetSwapchainImages(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());
        mSwapChainImageFormat = surfaceFormat.format;
        mSwapChainExtent = extent;
    }

    void createImageViews() {
        mSwapChainImageViews.resize(mSwapChainImages.size());

        for (size_t i = 0; i < mSwapChainImages.size(); i++) {
            CtsImageViewCreateInfo createInfo{};
            createInfo.sType = CTS_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = mSwapChainImages[i];
            createInfo.viewType = CTS_IMAGE_VIEW_TYPE_2D;
            createInfo.format = mSwapChainImageFormat;
            createInfo.components.r = CTS_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = CTS_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = CTS_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = CTS_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = CTS_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (ctsCreateImageView(mDevice, &createInfo, mAllocator, &mSwapChainImageViews[i]) != CTS_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void mainLoop() {
        while (!sQuitRequested) {
            pollEvents();
        }
    }

    void cleanup() {
        for (auto imageView : mSwapChainImageViews) {
            ctsDestroyImageView(mDevice, imageView, mAllocator);
        }

        ctsDestroySwapchain(mDevice, mSwapChain, mAllocator);
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
        ctsGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<CtsQueueFamilyProperties> queueFamilies(queueFamilyCount);
        ctsGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

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

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        CtsExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

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

    HWND mWindow;

    CtsInstance mInstance;
    CtsPhysicalDevice mPhysicalDevice;
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

    const CtsAllocationCallbacks* mAllocator;
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
