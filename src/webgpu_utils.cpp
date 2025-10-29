
#include <iostream>
#include <vector>
#include <cassert>
#include "webgpu/webgpu.h"


struct UserData
{
	WGPUAdapter adapter = nullptr;
	WGPUDevice device = nullptr;
	bool requestEnded = false;
};
void onAdapterRequestEnded (WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData)
{
	
	UserData& userData = *reinterpret_cast<UserData*>(pUserData);
	if (status == WGPURequestAdapterStatus_Success) {
		userData.adapter = adapter;
	} else {
		std::cout << "Could not get WebGPU adapter: " << message << std::endl;
	}
	userData.requestEnded = true;
};


WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options)
{
	// A simple structure holding the local information shared with the
	// onAdapterRequestEnded callback.
	// struct UserData {
	// 	WGPUAdapter adapter = nullptr;
	// 	bool requestEnded = false;
	// };
	UserData userData;

	// Callback called by wgpuInstanceRequestAdapter when the request returns
	// This is a C++ lambda function, but could be any function defined in the
	// global scope. It must be non-capturing (the brackets [] are empty) so
	// that it behaves like a regular C function pointer, which is what
	// wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
	// is to convey what we want to capture through the pUserData pointer,
	// provided as the last argument of wgpuInstanceRequestAdapter and received
	// by the callback as its last argument.
	// auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
	// 	UserData& userData = *reinterpret_cast<UserData*>(pUserData);
	// 	if (status == WGPURequestAdapterStatus_Success) {
	// 		userData.adapter = adapter;
	// 	} else {
	// 		std::cout << "Could not get WebGPU adapter: " << message << std::endl;
	// 	}
	// 	userData.requestEnded = true;
	// };

	// Call to the WebGPU request adapter procedure
	// typedef struct WGPURequestAdapterCallbackInfo {
	// 	WGPUChainedStruct const * nextInChain;
	// 	WGPUCallbackMode mode;
	// 	WGPURequestAdapterCallback callback;
	// 	WGPU_NULLABLE void* userdata1;
	// 	WGPU_NULLABLE void* userdata2;
	// } WGPURequestAdapterCallbackInfo WGPU_STRUCTURE_ATTRIBUTE;

	WGPURequestAdapterCallbackInfo callbackinfo = {0};
	callbackinfo.callback = (WGPURequestAdapterCallback)onAdapterRequestEnded;
	callbackinfo.userdata1 = &userData;
	wgpuInstanceRequestAdapter(
		instance /* equivalent of navigator.gpu */,
		options,
		callbackinfo);
		// onAdapterRequestEnded,
		// (void*)&userData


	// We wait until userData.requestEnded gets true
	assert(userData.requestEnded);

	return userData.adapter;
}

void inspectAdapter(WGPUAdapter adapter)
{
    // wgpuAdapterGetLimits(WGPUAdapter adapter, WGPULimits * limits)
		
	// WGPUSupportedLimits supportedLimits = {};
	WGPULimits supportedLimits = {0};
	supportedLimits.nextInChain = nullptr;
	bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);
	if (success) {
		std::cout << "Adapter limits:" << std::endl;
		std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
		std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
		std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
		std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
	}

	// std::vector<WGPUFeatureName> features;
	
	WGPUSupportedFeatures available_features = {0};

	// First call to get the count
	wgpuAdapterGetFeatures(adapter, &available_features);

	// Allocate the vector to hold the available_features
	std::vector<WGPUFeatureName> features(available_features.featureCount);

	// Point the struct to the vector's internal buffer
	available_features.features = features.data();

	// Second call to actually populate the list
	wgpuAdapterGetFeatures(adapter, &available_features);
	
	// Call the function a first time with a null return address, just to get
	// the entry count.
	// size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);

	// // Allocate memory (could be a new, or a malloc() if this were a C program)
	// features.resize(featureCount);

	// // Call the function a second time, with a non-null return address
	// wgpuAdapterEnumerateFeatures(adapter, features.data());

	// std::cout << "Adapter features:" << std::endl;
	// std::cout << std::hex; // Write integers as hexadecimal to ease comparison with webgpu.h literals
	// for (auto f : features) {
	// 	std::cout << " - 0x" << f << std::endl;
	// }
	// std::cout << std::dec; // Restore decimal numbers
	// WGPUAdapterProperties properties = {};
	// properties.nextInChain = nullptr;
	// wgpuAdapterGetProperties(adapter, &properties);
	// std::cout << "Adapter properties:" << std::endl;
	// std::cout << " - vendorID: " << properties.vendorID << std::endl;
	// if (properties.vendorName) {
	// 	std::cout << " - vendorName: " << properties.vendorName << std::endl;
	// }
	// if (properties.architecture) {
	// 	std::cout << " - architecture: " << properties.architecture << std::endl;
	// }
	// std::cout << " - deviceID: " << properties.deviceID << std::endl;
	// if (properties.name) {
	// 	std::cout << " - name: " << properties.name << std::endl;
	// }
	// if (properties.driverDescription) {
	// 	std::cout << " - driverDescription: " << properties.driverDescription << std::endl;
	// }
	// std::cout << std::hex;
	// std::cout << " - adapterType: 0x" << properties.adapterType << std::endl;
	// std::cout << " - backendType: 0x" << properties.backendType << std::endl;
	// std::cout << std::dec; // Restore decimal numbers
	WGPUAdapterInfo info = {};
	info.nextInChain = nullptr;  // No extension chains

	wgpuAdapterGetInfo(adapter, &info);

	std::cout << "Adapter info:" << std::endl;
	std::cout << " - vendorID: " << info.vendorID << std::endl;

	if (info.vendor.data)
	{
		std::cout << " - vendor: " << info.vendor.data << std::endl;
	}
	if (info.architecture.data)
	{
		std::cout << " - architecture: " << info.architecture.data << std::endl;
	}

	std::cout << " - deviceID: " << info.deviceID << std::endl;

	if (info.device.data) {
		std::cout << " - device name: " << info.device.data << std::endl;
	}
	if (info.description.data) {
		std::cout << " - driver description: " << info.description.data << std::endl;
	}

	std::cout << std::hex;
	std::cout << " - adapterType: 0x" << info.adapterType << std::endl;
	std::cout << " - backendType: 0x" << info.backendType << std::endl;
	std::cout << std::dec;
}

void onDeviceRequestEnded (WGPURequestDeviceStatus status,
						   WGPUDevice device,
						   char const * message, void * pUserData)
{
	UserData& userData = *reinterpret_cast<UserData*>(pUserData);
	if (status == WGPURequestDeviceStatus_Success) {
		userData.device = device;
	} else {
		std::cout << "Could not get WebGPU device: " << message << std::endl;
	}
	userData.requestEnded = true;
};

WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor) {
	// struct UserData {
	// 	WGPUDevice device = nullptr;
	// 	bool requestEnded = false;
	// };
	UserData userData;

	// auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * pUserData) {
	// 	UserData& userData = *reinterpret_cast<UserData*>(pUserData);
	// 	if (status == WGPURequestDeviceStatus_Success) {
	// 		userData.device = device;
	// 	} else {
	// 		std::cout << "Could not get WebGPU device: " << message << std::endl;
	// 	}
	// 	userData.requestEnded = true;
	// };


	// ---------- Correct signatures from webgpu.h:
	// WGPU_EXPORT WGPUFuture wgpuAdapterRequestDevice(WGPUAdapter adapter,
	// 												WGPU_NULLABLE WGPUDeviceDescriptor const * descriptor,
	// 												WGPURequestDeviceCallbackInfo callbackInfo);

	// WGPURequestDeviceCallbackInfo callback_info = {0};
	// typedef struct WGPURequestDeviceCallbackInfo
	// {
	// 	WGPUChainedStruct const * nextInChain;
	// 	WGPUCallbackMode mode;
	// 	WGPURequestDeviceCallback callback;
	// 	WGPU_NULLABLE void* userdata1;
	// 	WGPU_NULLABLE void* userdata2;
	// } WGPURequestDeviceCallbackInfo WGPU_STRUCTURE_ATTRIBUTE;

	WGPURequestDeviceCallbackInfo callback_info = {0};
	callback_info.callback = (WGPURequestDeviceCallback)onDeviceRequestEnded;
	callback_info.userdata1 = &userData;
	
	wgpuAdapterRequestDevice(
		adapter,
		descriptor,
		callback_info);
		// onDeviceRequestEnded,
		// (void*)&userData

	assert(userData.requestEnded);

	return userData.device;
}

void inspectDevice(WGPUDevice device) {
	// typedef struct WGPUSupportedFeatures {
	// 	size_t featureCount;
	// 	WGPUFeatureName const * features;
	// } WGPUSupportedFeatures WGPU_STRUCTURE_ATTRIBUTE;

	// : initialize struct, make 2 calls (get count → allocate → fill list).
	// call 1
	WGPUSupportedFeatures supportedFeatures = {0};
	wgpuDeviceGetFeatures(device, &supportedFeatures);
	// allocate
	std::vector<WGPUFeatureName> features(supportedFeatures.featureCount);
	// Step 2: 
	supportedFeatures.features = features.data(); // set structs feature ptr to the vectors
	// fill list
	wgpuDeviceGetFeatures(device, &supportedFeatures);
	// wgpuDeviceEnumerateFeatures(device, features.data());

	std::cout << "Device features:" << std::endl;
	std::cout << std::hex;
	for (auto f : features) {
		std::cout << " - 0x" << f << std::endl;
	}
	std::cout << std::dec;

	WGPULimits limits = {0};
	bool success = wgpuDeviceGetLimits(device, &limits);
	
	if (success) {
		std::cout << "Device limits:" << std::endl;
		std::cout << " - maxTextureDimension1D: " << limits.maxTextureDimension1D << std::endl;
		std::cout << " - maxTextureDimension2D: " << limits.maxTextureDimension2D << std::endl;
		std::cout << " - maxTextureDimension3D: " << limits.maxTextureDimension3D << std::endl;
		std::cout << " - maxTextureArrayLayers: " << limits.maxTextureArrayLayers << std::endl;
		std::cout << " - maxBindGroups: " << limits.maxBindGroups << std::endl;
		std::cout << " - maxDynamicUniformBuffersPerPipelineLayout: " << limits.maxDynamicUniformBuffersPerPipelineLayout << std::endl;
		std::cout << " - maxDynamicStorageBuffersPerPipelineLayout: " << limits.maxDynamicStorageBuffersPerPipelineLayout << std::endl;
		std::cout << " - maxSampledTexturesPerShaderStage: " << limits.maxSampledTexturesPerShaderStage << std::endl;
		std::cout << " - maxSamplersPerShaderStage: " << limits.maxSamplersPerShaderStage << std::endl;
		std::cout << " - maxStorageBuffersPerShaderStage: " << limits.maxStorageBuffersPerShaderStage << std::endl;
		std::cout << " - maxStorageTexturesPerShaderStage: " << limits.maxStorageTexturesPerShaderStage << std::endl;
		std::cout << " - maxUniformBuffersPerShaderStage: " << limits.maxUniformBuffersPerShaderStage << std::endl;
		std::cout << " - maxUniformBufferBindingSize: " << limits.maxUniformBufferBindingSize << std::endl;
		std::cout << " - maxStorageBufferBindingSize: " << limits.maxStorageBufferBindingSize << std::endl;
		std::cout << " - minUniformBufferOffsetAlignment: " << limits.minUniformBufferOffsetAlignment << std::endl;
		std::cout << " - minStorageBufferOffsetAlignment: " << limits.minStorageBufferOffsetAlignment << std::endl;
		std::cout << " - maxVertexBuffers: " << limits.maxVertexBuffers << std::endl;
		std::cout << " - maxVertexAttributes: " << limits.maxVertexAttributes << std::endl;
		std::cout << " - maxVertexBufferArrayStride: " << limits.maxVertexBufferArrayStride << std::endl;
		std::cout << " - maxInterStageShaderVariables: " << limits.maxInterStageShaderVariables << std::endl;
		std::cout << " - maxComputeWorkgroupStorageSize: " << limits.maxComputeWorkgroupStorageSize << std::endl;
		std::cout << " - maxComputeInvocationsPerWorkgroup: " << limits.maxComputeInvocationsPerWorkgroup << std::endl;
		std::cout << " - maxComputeWorkgroupSizeX: " << limits.maxComputeWorkgroupSizeX << std::endl;
		std::cout << " - maxComputeWorkgroupSizeY: " << limits.maxComputeWorkgroupSizeY << std::endl;
		std::cout << " - maxComputeWorkgroupSizeZ: " << limits.maxComputeWorkgroupSizeZ << std::endl;
		std::cout << " - maxComputeWorkgroupsPerDimension: " << limits.maxComputeWorkgroupsPerDimension << std::endl;
	}
}

WGPUStringView label_maker(const char* _label)
{
	WGPUStringView ret = {.data = _label,
						  .length = std::strlen(_label)};
	return ret;
}

// ====================================================================================================

void device_lost_callback(WGPUDeviceLostReason reason, char const* message, void* /* pUserData */)
{
	std::cout << "Device lost: reason " << reason;
	if (message) std::cout << " (" << message << ")";
	std::cout << std::endl;
}

void on_device_error (WGPUErrorType type, char const* message, void* /* pUserData */)
{
	std::cout << "Uncaptured device error: type " << type;
	if (message) std::cout << " (" << message << ")";
	std::cout << std::endl;
}

// Add a callback to monitor the moment queued work finished
void onQueueWorkDone(WGPUQueueWorkDoneStatus status, void*)
{
	std::cout << "Queued work finished with status: " << status << std::endl;
}

void setDefault(WGPULimits &limits)
{
	limits.maxTextureDimension1D = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxTextureDimension2D = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxTextureDimension3D = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxTextureArrayLayers = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBindGroups = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBindGroupsPlusVertexBuffers = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBindingsPerBindGroup = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxDynamicUniformBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxDynamicStorageBuffersPerPipelineLayout = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxSampledTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxSamplersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxStorageBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxStorageTexturesPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxUniformBuffersPerShaderStage = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxUniformBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED;
	limits.maxStorageBufferBindingSize = WGPU_LIMIT_U64_UNDEFINED;
	limits.minUniformBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED;
	limits.minStorageBufferOffsetAlignment = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxVertexBuffers = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxBufferSize = WGPU_LIMIT_U64_UNDEFINED;
	limits.maxVertexAttributes = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxVertexBufferArrayStride = WGPU_LIMIT_U32_UNDEFINED;
	// limits.maxInterStageShaderComponents = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxInterStageShaderVariables = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxColorAttachments = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxColorAttachmentBytesPerSample = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupStorageSize = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeInvocationsPerWorkgroup = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupSizeX = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupSizeY = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupSizeZ = WGPU_LIMIT_U32_UNDEFINED;
	limits.maxComputeWorkgroupsPerDimension = WGPU_LIMIT_U32_UNDEFINED;
}

void setDefault(WGPUBindGroupLayoutEntry& bindingLayout)
{
	// ---------- webgpu-native reference:

	// 	typedef struct WGPUBindGroupLayoutEntry {
	//     WGPUChainedStruct const * nextInChain;
	//     uint32_t binding;
	//     WGPUShaderStage visibility;
	//     WGPUBufferBindingLayout buffer;
	//     WGPUSamplerBindingLayout sampler;
	//     WGPUTextureBindingLayout texture;
	//     WGPUStorageTextureBindingLayout storageTexture;
	// } WGPUBindGroupLayoutEntry WGPU_STRUCTURE_ATTRIBUTE;


	// typedef struct WGPUBufferBindingLayout {
	//     WGPUChainedStruct const * nextInChain;
	//     WGPUBufferBindingType type;
	//     WGPUBool hasDynamicOffset;
	//     uint64_t minBindingSize;
	// } WGPUBufferBindingLayout WGPU_STRUCTURE_ATTRIBUTE;

	// typedef struct WGPUSamplerBindingLayout {
	//     WGPUChainedStruct const * nextInChain;
	//     WGPUSamplerBindingType type;
	// } WGPUSamplerBindingLayout WGPU_STRUCTURE_ATTRIBUTE;


	// typedef struct WGPUTextureBindingLayout {
	//     WGPUChainedStruct const * nextInChain;
	//     WGPUTextureSampleType sampleType;
	//     WGPUTextureViewDimension viewDimension;
	//     WGPUBool multisampled;
	// } WGPUTextureBindingLayout WGPU_STRUCTURE_ATTRIBUTE;


	// typedef struct WGPUStorageTextureBindingLayout {
	//     WGPUChainedStruct const * nextInChain;
	//     WGPUStorageTextureAccess access;
	//     WGPUTextureFormat format;
	//     WGPUTextureViewDimension viewDimension;
	// } WGPUStorageTextureBindingLayout WGPU_STRUCTURE_ATTRIBUTE;
	// --------------------------------------------------
	
	bindingLayout.buffer.nextInChain = nullptr;
	bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
	bindingLayout.buffer.hasDynamicOffset = false;

	bindingLayout.sampler.nextInChain = nullptr;
	bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

	bindingLayout.storageTexture.nextInChain = nullptr;
	bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
	bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
	bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

	bindingLayout.texture.nextInChain = nullptr;
	bindingLayout.texture.multisampled = false;
	bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
	bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
}

// ----------------------------------------------------------------------------------------------------
// -- Shader utils

void compilationCallback(WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo* info, void* userData)
{

	// typedef struct WGPUCompilationInfo {
	// 	WGPUChainedStruct const * nextInChain;
	// 	size_t messageCount;
	// 	WGPUCompilationMessage const * messages;
	// } WGPUCompilationInfo WGPU_STRUCTURE_ATTRIBUTE;

	
    using namespace std;

    if (status != WGPUCompilationInfoRequestStatus_Success) {
        cerr << "Failed to retrieve shader compilation info (status: " << status << ")" << endl;
        return;
    }

    if (info->messageCount == 0)
	{
        cout << "Shader compilation completed, but no messages were returned." << endl;
        return;
    }

    bool hasError = false;
    cout << "Shader Compilation Info:" << endl;

    for (size_t i = 0; i < info->messageCount; ++i) {
        const WGPUCompilationMessage& msg = info->messages[i];

        string typeStr;
        std::ostream& out = (msg.type == WGPUCompilationMessageType_Error) ? cerr : cout;

        switch (msg.type) {
            case WGPUCompilationMessageType_Error:
                typeStr = "Error";
                hasError = true;
                break;
            case WGPUCompilationMessageType_Warning:
                typeStr = "Warning";
                break;
            case WGPUCompilationMessageType_Info:
                typeStr = "Info";
                break;
            default:
                typeStr = "Unknown";
        }

        out << "  [" << typeStr << "] Line " << msg.lineNum
			<< ", Pos " << msg.linePos << ": "
			<< msg.message.data
			<< std::endl;
    }

    if (hasError)
	{
        cerr << "Shader compilation failed due to errors above." << endl;
    } else {
        cout << "Shader compiled successfully (no errors)." << endl;
    }
}
