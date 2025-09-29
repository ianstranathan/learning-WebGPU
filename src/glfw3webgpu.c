#include <webgpu/webgpu.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <iostream>
#include <Windows.h> // Include standard Windows API headers

/*
  This is a small wrapper to get a valid windows surface for glfw
  Based on https://github.com/eliemichel/glfw3webgpu/blob/main/glfw3webgpu.c

  I wasn't able to get this wrapper to work as is (the macro dilineating OS types wasn't defined)
  so I just cut out the window's stuff and refactored according to the changed webgpu standard
 */
  

WGPUSurface glfwCreateWindowWGPUSurface(WGPUInstance instance, GLFWwindow* window)
{
	HWND hwnd = glfwGetWin32Window(window);
	HINSTANCE hinstance = GetModuleHandle(NULL);

	/* typedef struct WGPUChainedStruct { */
	/* 	struct WGPUChainedStruct const * next; */
	/* 	WGPUSType sType; */
	/* } WGPUChainedStruct WGPU_STRUCTURE_ATTRIBUTE; */

	
	/* typedef struct WGPUSurfaceSourceWindowsHWND { */
	/* 	WGPUChainedStruct chain; */
	/* 	/\** */
	/* 	 * The [`HINSTANCE`](https://learn.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point) for this application. */
	/* 	 * Most commonly `GetModuleHandle(nullptr)`. */
	/* 	 *\/ */
	/* 	void * hinstance; */
	/* 	/\** */
	/* 	 * The [`HWND`](https://learn.microsoft.com/en-us/windows/apps/develop/ui-input/retrieve-hwnd) that will be wrapped by the @ref WGPUSurface. */
	/* 	 *\/ */
	/* 	void * hwnd; */
	/* } WGPUSurfaceSourceWindowsHWND WGPU_STRUCTURE_ATTRIBUTE; */

	
	WGPUSurfaceSourceWindowsHWND fromWindowsHWND;
	fromWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
	fromWindowsHWND.chain.next = NULL;
	fromWindowsHWND.hinstance = hinstance;
	fromWindowsHWND.hwnd = hwnd;

	WGPUSurfaceDescriptor surfaceDescriptor = {0};

	/* typedef struct WGPUSurfaceDescriptor { */
	/* 	WGPUChainedStruct const * nextInChain; */
	/* 	/\** */
	/* 	 * Label used to refer to the object. */
	/* 	 * */
	/* 	 * This is a \ref NonNullInputString. */
	/* 	 *\/ */
	/* 	WGPUStringView label; */
	/* } WGPUSurfaceDescriptor WGPU_STRUCTURE_ATTRIBUTE; */

	
	surfaceDescriptor.nextInChain = &fromWindowsHWND.chain;
	surfaceDescriptor.label = label_maker("SOMETHING SOMETHING SURFACE");

	return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
}
