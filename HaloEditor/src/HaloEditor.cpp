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
		m_Texture.reset(Texture2D::Create("assets/textures/cerberus/cerberus_A.png",false));

		std::string lightVSrc = R"(
			#version 430
			layout(location = 0) in vec3 a_Position;
			
			uniform mat4 u_viewProjection;
			uniform mat4 u_model;
			out vec3 v_pos;

			void main()
			{
				gl_Position =  u_viewProjection * u_model * vec4(a_Position, 1.0);
				v_pos = a_Position;
			}
		)";
		std::string lightFSrc = R"(
			#version 430

			layout(location = 0) out vec4 finalColor;

			void main()
			{
				finalColor = vec4(1.0);
			}
		)";
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
			out vec3 v_Position;

			void main()
			{
				gl_Position =  u_viewProjection * vec4(a_Position, 1.0);
				v_Normal = a_Normal;
				v_TexCoord = a_TexCoord;
				v_Position = a_Position;
			}
		)";
		std::string fragmentSrc = R"(
			#version 430

			layout(location = 0) out vec4 finalColor;

			in vec3 v_Normal;	
			in vec2 v_TexCoord;
			in vec3 v_Position;

			uniform vec3 u_lightPos;
			uniform vec3 u_viewPos;
			uniform sampler2D u_baseColor;

			void main()
			{
				vec3 lightColor = vec3(1.0,1.0,1.0);
				float ambientStrength = 0.1;
				vec3 ambient = ambientStrength * lightColor;
  	
				// diffuse 
				vec3 norm = normalize(v_Normal);
				vec3 lightDir = normalize(u_lightPos - v_Position);
				float diff = max(dot(norm, lightDir), 0.0);
				vec3 diffuse = diff * lightColor;

				// specular
				float specularStrength = 0.5;
				vec3 viewDir = normalize(u_viewPos - v_Position);
				vec3 reflectDir = reflect(-lightDir, norm);  
				float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
				vec3 specular = specularStrength * spec * lightColor;  
            
				finalColor = vec4((ambient + diffuse + specular),1.0) * texture(u_baseColor,v_TexCoord);
			}
		)";

		m_Shader.reset(new Halo::Shader(vertexSrc, fragmentSrc));
		m_LightShader.reset(new Halo::Shader(lightVSrc, lightFSrc));
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
		m_Shader->UploadUniformMat4("u_viewProjection", viewProjection);
		m_Shader->UploadUniformFloat3("u_lightPos", m_LightPos);
		m_Shader->UploadUniformFloat3("u_viewPos", m_Camera.GetPosition());
		m_Shader->UploadUniformInt("u_baseColor", 1);
		m_Texture->Bind(1);
		m_Mesh->Render();

		m_LightShader->Bind();
		m_LightShader->UploadUniformMat4("u_viewProjection", viewProjection);
		m_LightShader->UploadUniformMat4("u_model", model);
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
	std::unique_ptr<Halo::Mesh> m_Mesh;
	std::unique_ptr<Halo::Shader> m_Shader;
	std::unique_ptr<Halo::Shader> m_LightShader;
	std::shared_ptr<Halo::VertexArray> m_VertexArray;
	std::unique_ptr<Halo::Texture2D> m_Texture;

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