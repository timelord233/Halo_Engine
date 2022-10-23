#include "hlpch.h"
#include "Renderer.h"

namespace Halo {


	void Renderer::BeginScene()
	{

	}


	void Renderer::EndScene()
	{

	}


	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}



	void Renderer::SubmitFullscreenQuad(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray,false);
	}

}