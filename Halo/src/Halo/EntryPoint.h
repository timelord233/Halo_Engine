#pragma once

#ifdef HL_PLATFORM_WINDOWS

extern Halo::Application* Halo::CreateApplication();

int main(int argc, char* argv)
{
	Halo::Log::Init();
	HL_CORE_ERROR("Halo Core Init");
	HL_INFO("hello");

	auto app = Halo::CreateApplication();
	app->Run();
	delete app;
}

#endif