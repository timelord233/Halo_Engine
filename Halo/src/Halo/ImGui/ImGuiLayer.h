#pragma once

#include "Halo/Layer.h"
#include "Halo/Events/ApplicationEvent.h"
#include "Halo/Events/KeyEvent.h"
#include "Halo/Events/MouseEvent.h"

namespace Halo {

	class HALO_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}