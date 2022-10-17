#include "Halo.h"

#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class EditorLayer : public Halo::Layer
{
public:
	EditorLayer()
		:m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
	{

	}

	void OnAttach() override 
	{
		m_Mesh.reset(new Halo::Mesh("assets/meshes/cerberus.fbx"));
		std::string vertexSrc = R"(
			#version 430

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec3 a_Tangent;
			layout(location = 3) in vec3 a_Bitangent;
			layout(location = 4) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;

			out vec3 v_Normal;

			void main()
			{
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				v_Normal = a_Normal;
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
	}

	void OnUpdate() override
	{
		glm::mat4 viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

		m_Shader->Bind();
		m_Shader->UploadUniformMat4("u_ViewProjection", viewProjection);

		m_Mesh->Render();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Halo::Event& event) override
	{
		if (event.GetEventType() == Halo::EventType::KeyPressed)
		{
			Halo::KeyPressedEvent& e = (Halo::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HL_KEY_TAB)
				HL_TRACE("Tab key is pressed (event)!");
			HL_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
private:
	std::unique_ptr<Halo::Mesh> m_Mesh;
	std::unique_ptr<Halo::Shader> m_Shader;
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