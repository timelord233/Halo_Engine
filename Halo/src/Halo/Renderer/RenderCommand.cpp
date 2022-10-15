#include "hlpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Halo {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}