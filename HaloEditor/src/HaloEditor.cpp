#include "Halo.h"

#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

class EditorLayer : public Halo::Layer
{
public:
	EditorLayer()
		:m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
	{

	}

	void OnAttach() override 
	{
		using namespace Halo;
		m_Mesh.reset(new Mesh("assets/meshes/cerberus.fbx"));
		m_Texture.reset(Texture2D::Create("assets/textures/cerberus/cerberus_A.png",false));
		m_Shader.reset(Shader::Create("assets/shaders/shader.glsl"));
		m_LightShader.reset(Shader::Create("assets/shaders/lightCubeShader.glsl"));
		m_VertexArray.reset(VertexArray::Create());

		float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, 0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	 0.5f,  -0.5f,  0.5f,
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0,1,2,0,2,3,0,4,3,0,4,5,0,7,1,0,7,5,4,7,5,4,7,6,1,6,2,6,7,1,4,2,3,4,2,6 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_LightPos = glm::vec3(10.2f, 10.0f, 20.0f);

	}

	void OnUpdate() override
	{
		using namespace Halo;
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		Renderer::BeginScene();

		glm::mat4 viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_LightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

		m_Shader->Bind();
		UniformBufferDeclaration<sizeof(glm::mat4) + sizeof(glm::vec3) * 2 + sizeof(int), 4> phongShaderUB;
		phongShaderUB.Push("u_viewProjection", viewProjection);
		phongShaderUB.Push("u_lightPos", m_LightPos);
		phongShaderUB.Push("u_viewPos", m_Camera.GetPosition());
		phongShaderUB.Push("u_baseColor", 1);
		m_Shader->UploadUniformBuffer(phongShaderUB);
		m_Texture->Bind(1);
		m_Mesh->Render();

		m_LightShader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_LightShader)->UploadUniformMat4("u_viewProjection", viewProjection);
		std::dynamic_pointer_cast<OpenGLShader>(m_LightShader)->UploadUniformMat4("u_model", model);
		Renderer::Submit(m_VertexArray);

		Renderer::EndScene();

		m_Camera.OnUpdate();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		auto cameraPosition = m_Camera.GetPosition();
		ImGui::Text("Camera Position: %.2f, %.2f, %.2f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
		auto cameraForward = m_Camera.GetForwardDirection();
		ImGui::Text("Camera Forward: %.2f, %.2f, %.2f", cameraForward.x, cameraForward.y, cameraForward.z);
		ImGui::End();
	}

	void OnEvent(Halo::Event& event) override
	{
		if (event.GetEventType() == Halo::EventType::KeyPressed)
		{
			Halo::KeyPressedEvent& e = (Halo::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HL_KEY_TAB)
				HL_TRACE("Tab key is pressed (event)!");
			//HL_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
private:
	std::shared_ptr<Halo::Mesh> m_Mesh;
	std::shared_ptr<Halo::Shader> m_Shader;
	std::shared_ptr<Halo::Shader> m_LightShader;
	std::shared_ptr<Halo::VertexArray> m_VertexArray;
	std::shared_ptr<Halo::Texture2D> m_Texture;

	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;
	};
	Light m_Light;
	glm::vec3 m_LightPos;

	Halo::Camera m_Camera;
};

class HaloEditor : public Halo::Application
{
public:
	HaloEditor()
	{
		PushLayer(new EditorLayer());
	}

	~HaloEditor()
	{

	}

};

Halo::Application* Halo::CreateApplication()
{
	return new HaloEditor();
}