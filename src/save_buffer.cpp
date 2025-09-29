// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// -- My original Stuff
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// Add a callback to monitor the moment queued work finished
void onQueueWorkDone(WGPUQueueWorkDoneStatus status, void*)
{
	std::cout << "Queued work finished with status: " << status << std::endl;
};


// int main()
// {
// 	// Instance --------------------------------------------------
	
// 	WGPUInstanceDescriptor desc = {0};
// 	WGPUInstance instance = wgpuCreateInstance(&desc);

// 	if (!instance)
// 	{
// 		std::cerr << "Could not initialize WebGPU!" << std::endl;
// 		return 1;
// 	}
// 	std::cout << "WGPU instance: " << instance << std::endl;

// 	// Adapter --------------------------------------------------
// 	std::cout << "Requesting adapter..." << std::endl;
// 	WGPUAdapter adapter = requestAdapterSync(instance);
// 	std::cout << "Got adapter: " << adapter << std::endl;
	
// 	// We no longer need to use the instance once we have the adapter
// 	wgpuInstanceRelease(instance);

// 	// Device --------------------------------------------------
// 	std::cout << "Requesting device..." << std::endl;
// 	WGPUDeviceDescriptor device_desc = make_device_descriptor("My Device");
// 	WGPUDevice device = requestDeviceSync(adapter, &device_desc);
// 	std::cout << "Got device: " << device << std::endl;

// 	// Queue --------------------------------------------------

// 	WGPUQueue queue = wgpuDeviceGetQueue(device);
// 	WGPUQueueWorkDoneCallbackInfo queue_work_done_callback_info = {0};
// 	queue_work_done_callback_info.callback = (WGPUQueueWorkDoneCallback)onQueueWorkDone;
// 	wgpuQueueOnSubmittedWorkDone(queue, queue_work_done_callback_info);

// 	// -- econder descriptor; has a next in chain and a WGPUStringView label
// 	WGPUCommandEncoderDescriptor encoderDesc = {0};
// 	encoderDesc.label = label_maker( "Dummy Queue Label");

// 	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

// 	wgpuCommandEncoderInsertDebugMarker(encoder, label_maker("Do one thing"));
// 	wgpuCommandEncoderInsertDebugMarker(encoder, label_maker("Do another thing"));

// 	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
// 	cmdBufferDescriptor.nextInChain = nullptr;
// 	cmdBufferDescriptor.label = label_maker("Command buffer");
// 	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	
// 	wgpuCommandEncoderRelease(encoder); // release encoder after it's finished

// 	// Finally submit the command queue
// 	std::cout << "Submitting command..." << std::endl;
// 	wgpuQueueSubmit(queue, 1, &command);
// 	wgpuCommandBufferRelease(command);
// 	std::cout << "Command submitted." << std::endl;


// 	// Release resources --------------------------------------------------
// 	wgpuQueueRelease(queue);
// 	wgpuAdapterRelease(adapter);
// 	wgpuDeviceRelease(device);

// 	return 0;
// }

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// -- Combining the GLFW Test & the webgpu initial code
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------

// Add a callback to monitor the moment queued work finished
void onQueueWorkDone(WGPUQueueWorkDoneStatus status, void*)
{
	std::cout << "Queued work finished with status: " << status << std::endl;
};


int main()
{
	// Instance --------------------------------------------------
	
	WGPUInstanceDescriptor desc = {0};
	WGPUInstance instance = wgpuCreateInstance(&desc);

	if (!instance)
	{
		std::cerr << "Could not initialize WebGPU!" << std::endl;
		return 1;
	}
	std::cout << "WGPU instance: " << instance << std::endl;


	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);

	// Here we create our WebGPU surface from the window!
	WGPUSurface surface = glfwCreateWindowWGPUSurface(instance, window);
	
	// Adapter --------------------------------------------------
	std::cout << "Requesting adapter..." << std::endl;
	WGPUAdapter adapter = requestAdapterSync(instance, surface);
	std::cout << "Got adapter: " << adapter << std::endl;
	
	// We no longer need to use the instance once we have the adapter
	wgpuInstanceRelease(instance);

	// Device --------------------------------------------------
	std::cout << "Requesting device..." << std::endl;
	WGPUDeviceDescriptor device_desc = make_device_descriptor("My Device");
	WGPUDevice device = requestDeviceSync(adapter, &device_desc);
	std::cout << "Got device: " << device << std::endl;

	// Queue --------------------------------------------------

	WGPUQueue queue = wgpuDeviceGetQueue(device);
	WGPUQueueWorkDoneCallbackInfo queue_work_done_callback_info = {0};
	queue_work_done_callback_info.callback = (WGPUQueueWorkDoneCallback)onQueueWorkDone;
	wgpuQueueOnSubmittedWorkDone(queue, queue_work_done_callback_info);

	// -- econder descriptor; has a next in chain and a WGPUStringView label
	WGPUCommandEncoderDescriptor encoderDesc = {0};
	encoderDesc.label = label_maker( "Dummy Queue Label");

	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

	wgpuCommandEncoderInsertDebugMarker(encoder, label_maker("Do one thing"));
	wgpuCommandEncoderInsertDebugMarker(encoder, label_maker("Do another thing"));

	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.nextInChain = nullptr;
	cmdBufferDescriptor.label = label_maker("Command buffer");
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	
	wgpuCommandEncoderRelease(encoder); // release encoder after it's finished

	// Finally submit the command queue
	std::cout << "Submitting command..." << std::endl;
	wgpuQueueSubmit(queue, 1, &command);
	wgpuCommandBufferRelease(command);
	std::cout << "Command submitted." << std::endl;

	// ------------------------------------------------------------------------------------

	
	printf("surface = %p", surface);

	// Terminate GLFW
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	
	// Release resources --------------------------------------------------
	wgpuQueueRelease(queue);
	wgpuAdapterRelease(adapter);
	wgpuDeviceRelease(device);

	return 0;
}


// // Testing for GLFW --------------------------------------------------
// int main(int argc, char* argv[])
// {
// 	// Init WebGPU
// 	WGPUInstanceDescriptor desc;
// 	desc.nextInChain = NULL;
// 	WGPUInstance instance = wgpuCreateInstance(&desc);

// 	// Init GLFW
// 	glfwInit();
// 	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
// 	GLFWwindow* window = glfwCreateWindow(640, 480, "Learn WebGPU", NULL, NULL);

// 	// Here we create our WebGPU surface from the window!
// 	WGPUSurface surface = glfwCreateWindowWGPUSurface(instance, window);
// 	printf("surface = %p", surface);

// 	// Terminate GLFW
// 	while (!glfwWindowShouldClose(window)) glfwPollEvents();
// 	glfwDestroyWindow(window);
// 	glfwTerminate();

// 	return 0;
// }


