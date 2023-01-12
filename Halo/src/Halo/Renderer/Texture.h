#pragma once

#include "Halo/Core/Base.h"
#include "RendererAPI.h"

namespace Halo {

	enum class TextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2,
		Float16 = 3
	};

	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual RendererID GetRendererID() const = 0;
		virtual void Bind(uint32_t slot = 0) const = 0;

		static uint32_t CalculateMipMapCount(uint32_t width, uint32_t height);
	};

	class Texture2D : public Texture
	{
	public:
		static Texture2D* Create(TextureFormat format, unsigned int width, unsigned int height);
		static Texture2D* Create(const std::string& path, bool srgb = false);

		virtual TextureFormat GetFormat() const = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual const std::string& GetPath() const = 0;
	};

	class TextureCube : public Texture
	{
	public:
		static TextureCube* Create(const std::string& path);
		static std::shared_ptr<TextureCube> Create(TextureFormat format, uint32_t width, uint32_t height);

		virtual TextureFormat GetFormat() const = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;

		virtual const std::string& GetPath() const = 0;
	};

}