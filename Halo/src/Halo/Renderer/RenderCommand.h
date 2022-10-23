#pragma once

#include "RendererAPI.h"

namespace Halo {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, bool depthTest = true)
		{
			s_RendererAPI->DrawIndexed(vertexArray, depthTest);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}