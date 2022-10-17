#include "Halo.h"

#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
		m_Mesh.reset(new Halo::Mesh("assets/meshes/cerberus.fbx"));
		std::string vertexSrc = R"(
			#version 430

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Color;

			uniform mat4 u_viewProjection;

			out vec3 v_Normal;

			void main()
			{
				gl_Position = u_viewProjection * vec4(a_Position, 1.0);
				v_Normal = a_Color;
			}
		)";
		std::string fragmentSrc = R"(
			#version 430

			layout(location = 0) out vec4 finalColor;

			in vec3 v_Normal;

			void main()
			{
				finalColor = vec4((v_Normal * 0.5 + 0.5), 1.0);
			}
		)";

		m_Shader.reset(new Halo::Shader(vertexSrc, fragmentSrc));

		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}

	void OnUpdate() override
	{
		using namespace Halo;
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		Renderer::BeginScene();

		m_Shader->Bind();
		glm::mat4 viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

		/*		
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::translate(view, glm::vec3(-10.0f, 10.0f, 10.0f));
		projection = glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f);
		viewProjection = projection * view * model;
		*/

		m_Shader->UploadUniformMat4("u_viewProjection", viewProjection);
		Renderer::Submit(m_VertexArray);

		m_Mesh->Render();

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
	std::unique_ptr<Halo::Mesh> m_Mesh;
	std::unique_ptr<Halo::Shader> m_Shader;
	std::shared_ptr<Halo::VertexArray> m_VertexArray;
	Halo::Camera m_Camera;
};

class SandBox : public Halo::Application
{
public:
	SandBox()
	{
		PushLayer(new EditorLayer());
	}

	~SandBox()
	{

	}

};

Halo::Application* Halo::CreateApplication()
{
	return new SandBox();
}