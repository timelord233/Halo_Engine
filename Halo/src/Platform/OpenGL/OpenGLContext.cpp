#include "hlpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Halo {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		HL_CORE_ASSERT(windowHandle, "Window Handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HL_CORE_ASSERT(status, "Failed to initialize Glad!");

		HL_CORE_INFO("OpenGL Info:");
		HL_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		HL_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		HL_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{

		glfwSwapBuffers(m_WindowHandle);
	}

}