#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char* argv)
{
	Hazel::Log::Init();
	HZ_CORE_ERROR("Hazel Core Init");
	HZ_INFO("hello");

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif