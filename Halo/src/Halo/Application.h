#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Halo/LayerStack.h"
#include "Halo/ImGui/ImGuiLayer.h"
#include "Halo/Events/ApplicationEvent.h"
#include "Halo/Renderer/Shader.h"
#include "Halo/Renderer/Buffer.h"

namespace Halo {
	class Halo_API Application
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

		unsigned int m_VertexArray;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};

	//To be define in CLIENT
	Application* CreateApplication();
}
