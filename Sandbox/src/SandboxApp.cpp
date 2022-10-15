#include "Halo.h"

#include "imgui/imgui.h"

class ExampleLayer : public Halo::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Halo::Input::IsKeyPressed(HL_KEY_TAB))
			HL_TRACE("Tab key is pressed (poll)!");
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

};

class SandBox : public Halo::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
	}

	~SandBox()
	{

	}

};

Halo::Application* Halo::CreateApplication()
{
	return new SandBox();
}