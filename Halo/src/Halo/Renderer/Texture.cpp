#include "hlpch.h"
#include "Texture.h"

#include "Halo/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Halo {

	Texture2D* Texture2D::Create(TextureFormat format, unsigned int width, unsigned int height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLTexture2D(format, width, height);
		}
		return nullptr;
	}

	Texture2D* Texture2D::Create(const std::string& path, bool srgb)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLTexture2D(path, srgb);
		}
		return nullptr;
	}

	TextureCube* TextureCube::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return new OpenGLTextureCube(path);
		}
		return nullptr;
	}

	std::shared_ptr<Halo::TextureCube> TextureCube::Create(TextureFormat format, uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTextureCube>(format, width, height);
		}
		return nullptr;
	}

	uint32_t Texture::CalculateMipMapCount(uint32_t width, uint32_t height)
	{
		uint32_t levels = 1;
		while ((width | height) >> levels)
			levels++;

		return levels;
	}
}