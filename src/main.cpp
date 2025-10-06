#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <conio.h>  // For _kbhit() and _getch()


#ifndef DEBUG
// ====================================================================================================
// ====================================================================================================


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

/*
  TODO:
  Specifying the byte stride of a vertex is approaching the limit of a magic number
   See: definition of vertexCount & vertexBufferLayout.arrayStride
 */

class Application {	
public:
	// Initialize everything and return true if it went all right
	bool Initialize();

	// Uninitialize everything that was initialized
	void Terminate();

	// Draw a frame and handle events
	void MainLoop();

	// Return true as long as the main loop should keep on running
	bool IsRunning();

private:
	WGPUTextureView GetNextSurfaceTextureView();
	void InitializePipeline();
	WGPULimits GetRequiredLimits(WGPUAdapter adapter) const;
	void InitializeBuffers();
	//WGPUShaderModule make_shader_module( const char*  shader_src);
	WGPUShaderModule make_shader_module( const std::string shader_src);
	// callback function from file watcher
	//void on_wgsl_toy_shader_changed( const char* shader_src);
private:
	// We put here all the variables that are shared between init and main loop
	GLFWwindow* window;
	WGPUDevice device;
	WGPUQueue queue;
	WGPUSurface surface;
	WGPUTextureFormat surfaceFormat = WGPUTextureFormat_Undefined;
	WGPURenderPipeline pipeline;
	WGPUBuffer vertexBuffer;
    WGPUBuffer indexBuffer;
    uint32_t indexCount;
};

int main()
{
	Application app;

	if (!app.Initialize())
	{
		return 1;
	}

	while (app.IsRunning())
	{
		app.MainLoop();
	}

	app.Terminate();

	return 0;
}


bool Application::Initialize() {
	WGPUInstance instance = wgpuCreateInstance(nullptr);
	
	// Open window
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(640, 480, "Learn WebGPU", nullptr, nullptr);
	
	
	std::cout << "Requesting adapter..." << std::endl;


	// Here we create our WebGPU surface from the window!
	surface = glfwCreateWindowWGPUSurface(instance, window);
	if (!surface)
	{
		std::cerr << "Failed to create surface!" << std::endl;
	}
	WGPURequestAdapterOptions adapterOpts = {};
	adapterOpts.nextInChain = nullptr;
	adapterOpts.compatibleSurface = surface;
	WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
	std::cout << "Got adapter: " << adapter << std::endl;
	
	wgpuInstanceRelease(instance);
	
	std::cout << "Requesting device..." << std::endl;
	WGPUDeviceDescriptor deviceDesc = {};
	deviceDesc.nextInChain = nullptr;
	deviceDesc.label = label_maker("My Device");
	deviceDesc.requiredFeatureCount = 0;
	deviceDesc.requiredLimits = nullptr;
	deviceDesc.defaultQueue.nextInChain = nullptr;
	deviceDesc.defaultQueue.label = label_maker("The default queue");

	WGPUDeviceLostCallbackInfo deviceLostCallbackInfo = {0};
	deviceLostCallbackInfo.callback = (WGPUDeviceLostCallback)device_lost_callback;

	deviceDesc.deviceLostCallbackInfo = deviceLostCallbackInfo;

	WGPUUncapturedErrorCallbackInfo uncapturedErrorCallbackInfo = {0};
	uncapturedErrorCallbackInfo.callback = (WGPUUncapturedErrorCallback)on_device_error;

	deviceDesc.uncapturedErrorCallbackInfo = uncapturedErrorCallbackInfo;
	
	device = requestDeviceSync(adapter, &deviceDesc);
	std::cout << "Got device: " << device << std::endl;
	
	//wgpuDeviceSetUncapturedErrorCallback(device, on_device_error, nullptr /* pUserData */);
	
	queue = wgpuDeviceGetQueue(device);

	// Configure the surface
	WGPUSurfaceConfiguration config = {};
	config.nextInChain = nullptr;

	// Configuration of the textures created for the underlying swap chain
	config.width = 640;
	config.height = 480;
	config.usage = WGPUTextureUsage_RenderAttachment;

	WGPUSurfaceCapabilities capabilities = {0};
	WGPUStatus ok = wgpuSurfaceGetCapabilities( surface, adapter, &capabilities );
	if (!ok || capabilities.formatCount == 0)
	{
		std::cerr << "Surface has no supported formats!" << std::endl;
	}

	surfaceFormat = capabilities.formats[0];
	config.format = surfaceFormat;
	wgpuSurfaceCapabilitiesFreeMembers( capabilities );
	
	// WGPUTextureFormat surfaceFormat = wgpuSurfaceGetPreferredFormat(surface, adapter);
	// config.format = surfaceFormat;

	// And we do not need any particular view format:
	config.viewFormatCount = 0;
	config.viewFormats = nullptr;
	config.device = device;
	config.presentMode = WGPUPresentMode_Fifo;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;

	wgpuSurfaceConfigure(surface, &config);

	// Release the adapter only after it has been fully utilized
	wgpuAdapterRelease(adapter);

	// this shouldn't fail silently
	// NOTE: Invalid Handle on Failure returns a nullptr
	InitializePipeline(); 
	InitializeBuffers();
	return true;
}

void Application::Terminate()
{
	wgpuBufferRelease(vertexBuffer);
	wgpuBufferRelease(indexBuffer);
	wgpuRenderPipelineRelease(pipeline);
	wgpuSurfaceUnconfigure(surface);
	wgpuQueueRelease(queue);
	wgpuSurfaceRelease(surface);
	wgpuDeviceRelease(device);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::MainLoop()
{
	glfwPollEvents();

	// Get the next target texture view
	WGPUTextureView targetView = GetNextSurfaceTextureView();
	if (!targetView)
	{
		return;
	}

	// Create a command encoder for the draw call
	WGPUCommandEncoderDescriptor encoderDesc = {};
	encoderDesc.nextInChain = nullptr;
	encoderDesc.label = label_maker("My command encoder");
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

	// Create the render pass that clears the screen with our color
	WGPURenderPassDescriptor renderPassDesc = {0};
	
	// The attachment part of the render pass descriptor describes the target texture of the pass
	WGPURenderPassColorAttachment renderPassColorAttachment = {0};
	renderPassColorAttachment.view = targetView;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = WGPUColor{ 0.2, 0.2, 0.2, 1.0 };
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;
	
	// Create the render pass and end it immediately (we only clear the screen but do not draw anything)
	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder,
																		 &renderPassDesc);
	// Select which render pipeline to use
	wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);

	// -------------------- set vertex buffer from webgpu
	// // Set vertex buffer while encoding the render pass
	// void wgpuRenderPassEncoderSetVertexBuffer(WGPURenderPassEncoder renderPassEncoder,
	// 										  uint32_t slot,
	// 										  WGPUBuffer buffer,
	// 										  uint64_t offset,
	// 										  uint64_t size)

	wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, wgpuBufferGetSize(vertexBuffer));

	// wgpuRenderPassEncoderDraw(renderPass, vertexCount, 1, 0, 0);
	
	// -------------------- set index buffer from webgpu
	// void wgpuRenderPassEncoderSetIndexBuffer(WGPURenderPassEncoder renderPassEncoder,
	// 										 WGPUBuffer buffer,
	// 										 WGPUIndexFormat format,
	// 										 uint64_t offset,
	// 										 uint64_t size)
	wgpuRenderPassEncoderSetIndexBuffer( renderPass,
										 indexBuffer,
										 WGPUIndexFormat_Uint16, // notice this doesn't match his choice
										 0,
										 wgpuBufferGetSize( indexBuffer));

	// void wgpuRenderPassEncoderDrawIndexed(WGPURenderPassEncoder renderPassEncoder,
	// 									  uint32_t indexCount,
	// 									  uint32_t instanceCount,
	// 									  uint32_t firstIndex,
	// 									  int32_t baseVertex,
	// 									  uint32_t firstInstance)

	wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);
	// --------------------------------------------------
	
	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);

	// Finally encode and submit the render pass
	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.nextInChain = nullptr;
	cmdBufferDescriptor.label = label_maker("Command buffer");
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	wgpuCommandEncoderRelease(encoder);

	//std::cout << "Submitting command..." << std::endl;
	wgpuQueueSubmit(queue, 1, &command);
	wgpuCommandBufferRelease(command);
	//std::cout << "Command submitted." << std::endl;

	// At the end of the frame
	wgpuTextureViewRelease(targetView);
	wgpuSurfacePresent(surface);
}

bool Application::IsRunning() {
	return !glfwWindowShouldClose(window);
}

WGPUTextureView Application::GetNextSurfaceTextureView()
{
	// Get the surface texture
	WGPUSurfaceTexture surfaceTexture;
	wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
	if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
	{
		return nullptr;
	}

	// Create a view for this surface texture
	WGPUTextureViewDescriptor viewDescriptor = {0};
	//viewDescriptor.nextInChain = nullptr;
	viewDescriptor.label = label_maker("Surface texture view");
	viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
	viewDescriptor.dimension = WGPUTextureViewDimension_2D;
	//viewDescriptor.baseMipLevel = 0;
	viewDescriptor.mipLevelCount = 1;
	//viewDescriptor.baseArrayLayer = 0;
	viewDescriptor.arrayLayerCount = 1;
	viewDescriptor.aspect = WGPUTextureAspect_All;
	WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture,
													   &viewDescriptor);

	// #ifndef WEBGPU_BACKEND_WGPU
	// We no longer need the texture, only its view
	// (NB: with wgpu-native, surface textures must not be manually released)
	//wgpuTextureRelease(surfaceTexture.texture);
	// #endif // WEBGPU_BACKEND_WGPU

	return targetView;
}



const std::string shader_code_str = load_shader_from_file( "../rsc/shaders/first.wgsl" );


//WGPUShaderModule Application:: make_shader_module( const char*  shader_src)
WGPUShaderModule Application:: make_shader_module( const std::string shader_src)
{
	// --------------------------------------------------
	// Reference from webgpu.h
	// --------------------------------------------------
	// typedef struct WGPUShaderModuleDescriptor
	// {
	// 	WGPUChainedStruct const * nextInChain;
	// 	WGPUStringView label;
	// }

	// typedef struct WGPUShaderSourceWGSL
	// {
	// 	WGPUChainedStruct chain;
	// 	WGPUStringView code;
	// }

    // typedef struct WGPUChainedStruct
	// {
	// 	struct WGPUChainedStruct const * next;
	// 	WGPUSType sType;
	// }
	// --------------------------------------------------
	
	assert(device && "Must have logical handle to a device to make a shader");
	// Load the shader module
	WGPUShaderSourceWGSL wgsl_desc{};
	// wgsl_desc.chain = {0};
	wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
	//wgsl_desc.code = label_maker( shader_src );
	wgsl_desc.code = label_maker( shader_src.c_str() );

	// We use the extension mechanism to specify the WGSL part
	// of the shader module descriptor
	WGPUShaderModuleDescriptor shaderDesc =
		{
			//nullptr;//(const WGPUChainedStruct*)&wgsl_desc,
			.nextInChain = &(wgsl_desc.chain),
			.label = label_maker("First WGSL Shader")
		};
	
	// Connect the chain
	return wgpuDeviceCreateShaderModule(device, &shaderDesc);;
}


void Application::InitializePipeline()
{
	// --------------------------------------------------
	// Reference from webgpu.h

	// Note: it looks like all entryPoint members are of type
	//       WGPUStringView
	// --------------------------------------------------
   
	// typedef struct WGPURenderPipelineDescriptor {
	// 	WGPUChainedStruct const * nextInChain;
	// 	/**
	// 	 * This is a \ref NonNullInputString.
	// 	 */
	// 	WGPUStringView label;
	// 	WGPU_NULLABLE WGPUPipelineLayout layout;
	// 	WGPUVertexState vertex;
	// 	WGPUPrimitiveState primitive;
	// 	WGPU_NULLABLE WGPUDepthStencilState const * depthStencil;
	// 	WGPUMultisampleState multisample;
	// 	WGPU_NULLABLE WGPUFragmentState const * fragment;
	// } WGPURenderPipelineDescriptor WGPU_STRUCTURE_ATTRIBUTE;


	// typedef struct WGPUVertexState {
	// 	WGPUChainedStruct const * nextInChain;
	// 	WGPUShaderModule module;
	// 	/**
	// 	 * This is a \ref NullableInputString.
	// 	 */
	// 	WGPUStringView entryPoint;
	// 	size_t constantCount;
	// 	WGPUConstantEntry const * constants;
	// 	size_t bufferCount;
	// 	WGPUVertexBufferLayout const * buffers;
	// } WGPUVertexState WGPU_STRUCTURE_ATTRIBUTE;

	// ------------------------- Create the render pipeline
	WGPURenderPipelineDescriptor pipelineDesc{};

	WGPUShaderModule shaderModule = make_shader_module( shader_code_str );
	
	// ------------------------- Configure the vertex pipeline
	// We use one vertex buffer
	WGPUVertexBufferLayout vertexBufferLayout{};
	std::vector<WGPUVertexAttribute> vertexAttribs(3);

	// ------------------------- position attrib
	vertexAttribs[0].shaderLocation = 0; // @location(0)
	vertexAttribs[0].format = WGPUVertexFormat_Float32x2; // vec2 in shader
	vertexAttribs[0].offset = 0;

	// ------------------------- color attrib
	vertexAttribs[1].shaderLocation = 1; // @location(1)
	vertexAttribs[1].format = WGPUVertexFormat_Float32x3; // vec3 in shader
	vertexAttribs[1].offset = 2 * sizeof(float); // offset past the vertex data

	// ------------------------- uv attrib
	vertexAttribs[2].shaderLocation = 2; // @location(1)
	vertexAttribs[2].format = WGPUVertexFormat_Float32x2; // vec2 in shader
	vertexAttribs[2].offset = 5 * sizeof(float); // offset past the color data

	// 
	vertexBufferLayout.attributeCount = static_cast<u32>(vertexAttribs.size());
	vertexBufferLayout.attributes = vertexAttribs.data();

	vertexBufferLayout.arrayStride = 7 * sizeof(float);
	vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
	
	pipelineDesc.vertex.bufferCount = 1;
	pipelineDesc.vertex.buffers = &vertexBufferLayout;
	
	pipelineDesc.vertex.module = shaderModule;
	pipelineDesc.vertex.entryPoint = label_maker("vs_main");
	pipelineDesc.vertex.constantCount = 0;
	pipelineDesc.vertex.constants = nullptr;

	// Each sequence of 3 vertices is considered as a triangle
	pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	
	// We'll see later how to specify the order in which vertices should be
	// connected. When not specified, vertices are considered sequentially.
	pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
	
	// The face orientation is defined by assuming that when looking
	// from the front of the face, its corner vertices are enumerated
	// in the counter-clockwise (CCW) order.
	pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
	
	// But the face orientation does not matter much because we do not
	// cull (i.e. "hide") the faces pointing away from us (which is often
	// used for optimization).
	pipelineDesc.primitive.cullMode = WGPUCullMode_None;

	// We tell that the programmable fragment shader stage is described
	// by the function called 'fs_main' in the shader module.
	WGPUFragmentState fragmentState{};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = label_maker("fs_main");
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;

	WGPUBlendState blendState{};
	blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation_Add;
	
	WGPUColorTargetState colorTarget{};
	colorTarget.format = surfaceFormat;
	colorTarget.blend = &blendState;
	colorTarget.writeMask = WGPUColorWriteMask_All; // We could write to only some of the color channels.
	
	// We have only one target because our render pass has only one output color
	// attachment.
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;
	pipelineDesc.fragment = &fragmentState;

	// We do not use stencil/depth testing for now
	pipelineDesc.depthStencil = nullptr;

	// Samples per pixel
	pipelineDesc.multisample.count = 1;

	// Default value for the mask, meaning "all bits on"
	pipelineDesc.multisample.mask = ~0u;

	// Default value as well (irrelevant for count = 1 anyways)
	pipelineDesc.multisample.alphaToCoverageEnabled = false;

	pipelineDesc.layout = nullptr;

	pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

	// We no longer need to access the shader module
	wgpuShaderModuleRelease(shaderModule);
}

// void Application::on_wgsl_toy_shader_changed( const char* shader_src)
// {
// 	// --------------------
// 	// make new shader module
// 	WGPUShaderModule shaderModule = make_shader_module( shader_src );

// 	// --------------------
// 	// update the cached pipeline descripter fragment and vertex states
// 	pipelineDesc.vertex.module   = shaderModule;
// 	pipelineDesc.fragment.module = shaderModule;
	
// 	if (pipeline != nullptr)
// 	{
// 		wgpuRenderPipelineRelease(pipeline);
// 		pipeline = nullptr; // Set to nullptr for safety
// 	}
	
// 	pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);
// 	wgpuShaderModuleRelease(shaderModule);
// }
	
WGPULimits Application::GetRequiredLimits(WGPUAdapter adapter) const
{
	// Get adapter supported limits, in case we need them
	WGPULimits supportedLimits{};
	bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);
	

	// WGPURequiredLimits requiredLimits{};
	WGPULimits requiredLimits{};
	setDefault(requiredLimits);

	
	requiredLimits.maxVertexAttributes = 2;
	requiredLimits.maxVertexBuffers = 1;
	requiredLimits.maxBufferSize = 6 * 5 * sizeof(float);          // 6 vertices of 5 float each
	requiredLimits.maxVertexBufferArrayStride = 5 * sizeof(float); // byte stride between 2 consecutive vertices

	// These two limits are different because they are "minimum" limits,
	// they are the only ones we are may forward from the adapter's supported
	// requiredLimits.
	requiredLimits.minUniformBufferOffsetAlignment = supportedLimits.minUniformBufferOffsetAlignment;
	requiredLimits.minStorageBufferOffsetAlignment = supportedLimits.minStorageBufferOffsetAlignment;

	return requiredLimits;
}

void Application::InitializeBuffers() {
	// Define point data
	// The de-duplicated list of point positions
	std::vector<float> pointData = {
		// x,   y,     r,   g,   b       u,   v
		-0.5, -0.5,   1.0, 0.0, 0.0,    0.0, 0.0, // Point #0
		+0.5, -0.5,   0.0, 1.0, 0.0,    1.0, 0.0, // Point #1
		+0.5, +0.5,   0.0, 0.0, 1.0,    1.0, 1.0, // Point #2
		-0.5, +0.5,   1.0, 1.0, 0.0,    0.0, 1.0, // Point #3
	};

	// Define index data
	// This is a list of indices referencing positions in the pointData
	std::vector<uint16_t> indexData = {
		0, 1, 2, // Triangle #0 connects points #0, #1 and #2
		0, 2, 3  // Triangle #1 connects points #0, #2 and #3
	};

	// We now store the index count rather than the vertex count
    indexCount = static_cast<uint32_t>(indexData.size());
	
	// Create point buffer
	WGPUBufferDescriptor bufferDesc{};
	bufferDesc.nextInChain = nullptr;
	bufferDesc.size = pointData.size() * sizeof(float);
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex; // Vertex usage here!
	bufferDesc.mappedAtCreation = false;
	vertexBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);
	
	// Upload geometry data to the buffer
	wgpuQueueWriteBuffer(queue, vertexBuffer, 0, pointData.data(), bufferDesc.size);

	// Create index buffer
	// (we reuse the bufferDesc initialized for the vertexBuffer)
	bufferDesc.size = indexData.size() * sizeof(uint16_t);
	bufferDesc.size = (bufferDesc.size + 3) & ~3; // round up to the next multiple of 4
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;;
	indexBuffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

	wgpuQueueWriteBuffer(queue, indexBuffer, 0, indexData.data(), bufferDesc.size);
}

#endif // end debug block
