#include "hlpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Halo {

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, bool depthTest = true)
	{
		if (depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}