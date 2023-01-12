#include "hlpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Halo {

	std::vector<SharedPtr<Shader>> Shader::s_AllShaders;

	SharedPtr<Shader> Shader::Create(const std::string& filepath)
	{
		SharedPtr<Shader> result = nullptr;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  result = std::make_shared<OpenGLShader>(filepath);
		}
		s_AllShaders.push_back(result);
		return result;
	}

	SharedPtr<Shader> Shader::CreateFromString(const std::string& source)
	{
		SharedPtr<Shader> result = nullptr;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  result = OpenGLShader::CreateFromString(source);
		}
		s_AllShaders.push_back(result);
		return result;
	}

}