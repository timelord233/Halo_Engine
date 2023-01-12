#pragma once
#include "Core/Base.h"
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

		std::string OpenFile(const std::string& filter) const;

	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		UniquePtr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;

	};

	//To be define in CLIENT
	Application* CreateApplication();
}
