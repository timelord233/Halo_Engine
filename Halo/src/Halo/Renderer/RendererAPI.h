#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Halo {

	using RendererID = uint32_t;

	struct RenderAPICapabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples;
		float MaxAnisotropy;
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray,bool depthTest) = 0;

		static RenderAPICapabilities& GetCapabilities()
		{
			static RenderAPICapabilities capabilities;
			return capabilities;
		}

		inline static API GetAPI() { return s_API; }

		static void Init();
	private:
		static API s_API;
	};

}