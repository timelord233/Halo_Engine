#pragma once

#include "Halo/Renderer/RendererAPI.h"
#include "Halo/Renderer/Texture.h"

namespace Halo {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(TextureFormat format, unsigned int width, unsigned int height);
		OpenGLTexture2D(const std::string& path, bool srgb);
		virtual ~OpenGLTexture2D();

		virtual void Bind(unsigned int slot = 0) const;

		virtual TextureFormat GetFormat() const { return m_Format; }
		virtual unsigned int GetWidth() const { return m_Width; }
		virtual unsigned int GetHeight() const { return m_Height; }

		virtual const std::string& GetPath() const override { return m_FilePath; }

		virtual RendererID GetRendererID() const override { return m_RendererID; }
	private:
		RendererID m_RendererID;
		TextureFormat m_Format;
		unsigned int m_Width, m_Height;
		bool m_IsHDR = false;
		unsigned char* m_ImageData;

		std::string m_FilePath;
	};

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(const std::string& path);
		OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height);

		virtual ~OpenGLTextureCube();

		virtual void Bind(unsigned int slot = 0) const;

		virtual TextureFormat GetFormat() const { return m_Format; }
		virtual unsigned int GetWidth() const { return m_Width; }
		virtual unsigned int GetHeight() const { return m_Height; }
		virtual uint32_t GetMipLevelCount() const override;

		virtual const std::string& GetPath() const override { return m_FilePath; }

		virtual RendererID GetRendererID() const override { return m_RendererID; }
	private:
		RendererID m_RendererID;
		TextureFormat m_Format;
		unsigned int m_Width, m_Height;

		unsigned char* m_ImageData;

		std::string m_FilePath;
	};
}