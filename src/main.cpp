
int main()
{
	Application app{};

	if (!app.Initialize())
	{
		return 1;
	}

	std::wstring dirPath = L"..\\rsc\\shaders"; 
    std::wstring fileName = L"first.wgsl";

    std::atomic<bool> stopFlag(false);
	std::atomic<bool> should_recompile_shader_flag(false);
	
	std::thread watcherThread(watchFile,
							  dirPath,
							  fileName,
							  std::ref(should_recompile_shader_flag),
							  std::ref(stopFlag));

	
	while (app.IsRunning())
	{
		if(should_recompile_shader_flag.load())
		{
			// reset pipeline with changed shader
			app.MakePipeline( app.get_shadertoy_shader() );
			should_recompile_shader_flag.store(false);
		}
		app.MainLoop();
	}

	stopFlag.store(true);
	
	if (watcherThread.joinable())
	{
        watcherThread.join();
    }
	
	app.Terminate();

	return 0;
}

// void window_close_callback(GLFWwindow* window)
// {
//     // This function is called immediately when the user tries to close the window.
    
//     // Example: Print a message and prevent the window from closing (if desired)
//     printf("User attempted to close the window!\n");

//     // If you do NOT want the window to close yet, you can reset the flag:
//     // glfwSetWindowShouldClose(window, GLFW_FALSE); 
    
//     // If you DO want the window to close (default behavior), do nothing or set the flag:
//     // glfwSetWindowShouldClose(window, GLFW_TRUE); // Redundant, but explicit
// }


// glfwSetWindowCloseCallback(window, window_close_callback);
