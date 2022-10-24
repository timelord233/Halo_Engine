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
		m_CheckerboardTex.reset(Texture2D::Create("assets/editor/Checkerboard.tga"));
		CreateEnvironmentMap("assets/env/birchwood_4k.hdr");

		m_Shader.reset(Shader::Create("assets/shaders/shader.glsl"));
		m_LightShader.reset(Shader::Create("assets/shaders/lightCubeShader.glsl"));
		m_pbrShader.reset(Shader::Create("assets/shaders/pbrShader.glsl"));
		m_SkyboxShader.reset(Shader::Create("assets/shaders/skybox.glsl"));

		// Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.1f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.1f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.1f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.1f);
		data[3].TexCoord = glm::vec2(0, 1);

		m_FullscreenQuadVertexArray.reset(VertexArray::Create());
		auto quadVB = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
		quadVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});

		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
		auto quadIB = IndexBuffer::Create(indices, sizeof(indices));

		m_FullscreenQuadVertexArray->AddVertexBuffer(quadVB);
		m_FullscreenQuadVertexArray->SetIndexBuffer(quadIB);

		/*m_VertexArray.reset(VertexArray::Create());

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

		std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices1[] = { 0,1,2,0,2,3,0,4,3,0,4,5,0,7,1,0,7,5,4,7,5,4,7,6,1,6,2,6,7,1,4,2,3,4,2,6 };
		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices1, sizeof(indices1));
		m_VertexArray->SetIndexBuffer(indexBuffer);*/

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

		m_SkyboxShader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_SkyboxShader)->UploadUniformMat4("u_InverseVP", glm::inverse(viewProjection));
		std::dynamic_pointer_cast<OpenGLShader>(m_SkyboxShader)->UploadUniformInt("u_Texture",0);
		m_CubeMap->Bind(0);
		Renderer::SubmitFullscreenQuad(m_FullscreenQuadVertexArray);

		//m_Shader->Bind();
		//UniformBufferDeclaration<sizeof(glm::mat4) + sizeof(glm::vec3) * 2 + sizeof(int), 4> phongShaderUB;
		//phongShaderUB.Push("u_viewProjection", viewProjection);
		//phongShaderUB.Push("u_lightPos", m_LightPos);
		//phongShaderUB.Push("u_viewPos", m_Camera.GetPosition());
		//phongShaderUB.Push("u_baseColor", 1);
		//m_Shader->UploadUniformBuffer(phongShaderUB);
		//m_Texture->Bind(1);

		m_pbrShader->Bind();
		UniformBufferDeclaration<sizeof(glm::mat4) + sizeof(glm::vec3) * 3 + sizeof(float) * 7 + sizeof(int) * 4, 15> pbrShaderUB;
		pbrShaderUB.Push("u_viewProjection", viewProjection);
		pbrShaderUB.Push("u_lightPos", m_LightPos);
		pbrShaderUB.Push("u_viewPos", m_Camera.GetPosition());
		pbrShaderUB.Push("u_AlbedoColor", m_AlbedoInput.Color);
		pbrShaderUB.Push("u_Metalness", m_MetalnessInput.Value);
		pbrShaderUB.Push("u_Roughness", m_RoughnessInput.Value);
		pbrShaderUB.Push("u_AlbedoTexture", 1);
		pbrShaderUB.Push("u_NormalTexture", 2);
		pbrShaderUB.Push("u_MetalnessTexture", 3);
		pbrShaderUB.Push("u_RoughnessTexture", 4);
		pbrShaderUB.Push("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
		pbrShaderUB.Push("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
		pbrShaderUB.Push("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
		pbrShaderUB.Push("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
		pbrShaderUB.Push("u_ao", 1.0f);
		m_pbrShader->UploadUniformBuffer(pbrShaderUB);

		if (m_AlbedoInput.TextureMap)
			m_AlbedoInput.TextureMap->Bind(1);
		if (m_NormalInput.TextureMap)
			m_NormalInput.TextureMap->Bind(2);
		if (m_MetalnessInput.TextureMap)
			m_MetalnessInput.TextureMap->Bind(3);
		if (m_RoughnessInput.TextureMap)
			m_RoughnessInput.TextureMap->Bind(4);
		m_Mesh->Render();

		//m_LightShader->Bind();
		//std::dynamic_pointer_cast<OpenGLShader>(m_LightShader)->UploadUniformMat4("u_viewProjection", viewProjection);
		//std::dynamic_pointer_cast<OpenGLShader>(m_LightShader)->UploadUniformMat4("u_model", model);
		//Renderer::Submit(m_VertexArray);

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

		ImGui::Begin("Settings");

		ImGui::Separator();
		{
			ImGui::Text("Mesh");
			std::string fullpath = m_Mesh ? m_Mesh->GetFilePath() : "None";
			size_t found = fullpath.find_last_of("/\\");
			std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
			ImGui::Text(path.c_str()); ImGui::SameLine();
			if (ImGui::Button("...##Mesh"))
			{
				std::string filename = Halo::Application::Get().OpenFile("");
				if (filename != "")
					m_Mesh.reset(new Halo::Mesh(filename));
			}
		}
		ImGui::Separator();
		// Textures ------------------------------------------------------------------------------
		{
			// Albedo
			if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(m_AlbedoInput.TextureMap ? (void*)m_AlbedoInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_AlbedoInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_AlbedoInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)m_AlbedoInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Halo::Application::Get().OpenFile("");
						if (filename != "")
							m_AlbedoInput.TextureMap.reset(Halo::Texture2D::Create(filename, m_AlbedoInput.SRGB));
					}
				}
				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Checkbox("Use##AlbedoMap", &m_AlbedoInput.UseTexture);
				if (ImGui::Checkbox("sRGB##AlbedoMap", &m_AlbedoInput.SRGB))
				{
					if (m_AlbedoInput.TextureMap)
						m_AlbedoInput.TextureMap.reset(Halo::Texture2D::Create(m_AlbedoInput.TextureMap->GetPath(), m_AlbedoInput.SRGB));
				}
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(m_AlbedoInput.Color), ImGuiColorEditFlags_NoInputs);
			}
		}
		{
			// Normals
			if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(m_NormalInput.TextureMap ? (void*)m_NormalInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_NormalInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_NormalInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)m_NormalInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Halo::Application::Get().OpenFile("");
						if (filename != "")
							m_NormalInput.TextureMap.reset(Halo::Texture2D::Create(filename));
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##NormalMap", &m_NormalInput.UseTexture);
			}
		}
		{
			// Metalness
			if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(m_MetalnessInput.TextureMap ? (void*)m_MetalnessInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_MetalnessInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_MetalnessInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)m_MetalnessInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Halo::Application::Get().OpenFile("");
						if (filename != "")
							m_MetalnessInput.TextureMap.reset(Halo::Texture2D::Create(filename));
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##MetalnessMap", &m_MetalnessInput.UseTexture);
				ImGui::SameLine();
				ImGui::SliderFloat("Value##MetalnessInput", &m_MetalnessInput.Value, 0.0f, 1.0f);
			}
		}
		{
			// Roughness
			if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(m_RoughnessInput.TextureMap ? (void*)m_RoughnessInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (m_RoughnessInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(m_RoughnessInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)m_RoughnessInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Halo::Application::Get().OpenFile("");
						if (filename != "")
							m_RoughnessInput.TextureMap.reset(Halo::Texture2D::Create(filename));
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##RoughnessMap", &m_RoughnessInput.UseTexture);
				ImGui::SameLine();
				ImGui::SliderFloat("Value##RoughnessInput", &m_RoughnessInput.Value, 0.0f, 1.0f);
			}
		}

		ImGui::Separator();
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
	std::shared_ptr<Halo::Shader> m_pbrShader;
	std::shared_ptr<Halo::VertexArray> m_VertexArray;
	std::shared_ptr<Halo::Texture2D> m_Texture;

	std::shared_ptr<Halo::TextureCube> m_CubeMap;
	std::shared_ptr<Halo::TextureCube> m_IrradianceMap;
	std::shared_ptr<Halo::Shader> m_SkyboxShader;
	std::shared_ptr<Halo::VertexArray> m_FullscreenQuadVertexArray;

	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;
	};
	Light m_Light;
	glm::vec3 m_LightPos;

	struct AlbedoInput
	{
		glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; // Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		std::unique_ptr<Halo::Texture2D> TextureMap;
		bool SRGB = true;
		bool UseTexture = false;
	};
	AlbedoInput m_AlbedoInput;

	struct MetalnessInput
	{
		float Value = 1.0f;
		std::unique_ptr<Halo::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	MetalnessInput m_MetalnessInput;

	struct NormalInput
	{
		std::unique_ptr<Halo::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	NormalInput m_NormalInput;

	struct RoughnessInput
	{
		float Value = 0.5f;
		std::unique_ptr<Halo::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	RoughnessInput m_RoughnessInput;

	Halo::Camera m_Camera;
	// Editor resources
	std::unique_ptr<Halo::Texture2D> m_CheckerboardTex;

	void CreateEnvironmentMap(const std::string& filepath)
	{
		using namespace Halo;
		const uint32_t cubemapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		std::shared_ptr<TextureCube> envUnfiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
		std::shared_ptr<Shader> equirectangularConversionShader;
		equirectangularConversionShader.reset(Shader::Create("assets/shaders/EquirectangularToCubeMap.glsl"));
		std::shared_ptr<Texture2D> envEquirect;
		envEquirect.reset(Texture2D::Create(filepath));
		HL_CORE_ASSERT(envEquirect->GetFormat() == TextureFormat::Float16, "Texture is not HDR!");

		equirectangularConversionShader->Bind();
		envEquirect->Bind();

		glBindImageTexture(0, envUnfiltered->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
		glGenerateTextureMipmap(envUnfiltered->GetRendererID());

		std::shared_ptr<Shader>	envFilteringShader;
		envFilteringShader.reset(Shader::Create("assets/shaders/EnvironmentMipFilter.glsl"));

		std::shared_ptr<TextureCube> envFiltered = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);

		glCopyImageSubData(envUnfiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			envFiltered->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			envFiltered->GetWidth(), envFiltered->GetHeight(), 6);

		envFilteringShader->Bind();
		envUnfiltered->Bind();

		const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
		for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
		{
			const GLuint numGroups = glm::max(1, size / 32);
			glBindImageTexture(0, envFiltered->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
			glDispatchCompute(numGroups, numGroups, 6);
		}

		std::shared_ptr<Shader> envIrradianceShader;
		envIrradianceShader.reset(Shader::Create("assets/shaders/EnvironmentIrradiance.glsl"));

		std::shared_ptr<TextureCube> irradianceMap = TextureCube::Create(TextureFormat::Float16, irradianceMapSize, irradianceMapSize);
		envIrradianceShader->Bind();
		envFiltered->Bind();

		glBindImageTexture(0, irradianceMap->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
		glGenerateTextureMipmap(irradianceMap->GetRendererID());

		m_CubeMap = envUnfiltered;
		m_IrradianceMap = irradianceMap;
	}
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