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
		m_Mesh.reset(new Mesh("assets/meshes/cerberus.fbx"));
		m_Texture.reset(Texture2D::Create("assets/textures/cerberus/cerberus_A.png",true));
		std::string vertexSrc = R"(
			#version 430

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec3 a_Tangent;
			layout(location = 3) in vec3 a_Bitangent;
			layout(location = 4) in vec2 a_TexCoord;

			uniform mat4 u_viewProjection;

			out vec3 v_Normal;
			out vec2 v_TexCoord;

			void main()
			{
				gl_Position =  u_viewProjection * vec4(a_Position, 1.0);
				v_Normal = a_Normal;
				v_TexCoord = a_TexCoord;
			}
		)";
		std::string fragmentSrc = R"(
			#version 430

			layout(location = 0) out vec4 finalColor;

			in vec3 v_Normal;	
			in vec2 v_TexCoord;

			uniform sampler2D u_baseColor;

			void main()
			{
				finalColor = texture(u_baseColor,v_TexCoord);
			}
		)";

		m_Shader.reset(new Halo::Shader(vertexSrc, fragmentSrc));

		m_VertexArray.reset(VertexArray::Create());

		float vertices[8*4] = {
			// positions          // colors           // texture coords
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Color" },
			{ ShaderDataType::Float2, "a_Texcoord"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = { 0, 1, 2 ,2 , 3, 0};
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

		glm::mat4 viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();
		m_Shader->UploadUniformMat4("u_viewProjection", viewProjection);
		m_Shader->UploadUniformInt("u_baseColor", 1);
		m_Shader->Bind();
		m_Texture->Bind(1);
		//Renderer::Submit(m_VertexArray);

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
	std::unique_ptr<Halo::Texture2D> m_Texture;

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