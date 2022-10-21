#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Halo/LayerStack.h"
#include "Halo/ImGui/ImGuiLayer.h"
#include "Halo/Events/ApplicationEvent.h"
#include "Halo/Renderer/Shader.h"
#include "Halo/Renderer/Buffer.h"
#include "Halo/Renderer/VertexArray.h"

namespace Halo {
	class HALO_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;

		LayerStack m_LayerStack;
		static Application* s_Instance;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;


	};

	//To be define in CLIENT
	Application* CreateApplication();
}
