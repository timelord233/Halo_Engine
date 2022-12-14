#pragma once

#include "Halo/Renderer/RendererAPI.h"

namespace Halo {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray,bool depthTest) override;
	};


}