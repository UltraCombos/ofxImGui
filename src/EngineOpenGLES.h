#pragma once

#include "ofConstants.h"
#if defined(TARGET_OPENGLES) && (!defined (OF_TARGET_API_VULKAN) )

#include "BaseEngine.h"

#include "ofEvents.h"
#include "ofShader.h"
#include "imgui.h"

namespace ofxImGui
{
	class EngineOpenGLES 
		: public BaseEngine
	{
	public:
		~EngineOpenGLES()
		{
			exit();
		}

		// BaseEngine required
		void setup() override;
		void exit() override;
		void render() override;
		bool createDeviceObjects() override;
		void invalidateDeviceObjects() override;

		void onKeyReleased(ofKeyEventArgs& event) override;

#if defined(TARGET_RASPBERRY_PI)
		void onRemapMouseButton(int&) override;

#endif

		// Custom 
		static void rendererDrawLists(ImDrawData * draw_data, EngineOpenGLES* p_self);

		static ofShader g_Shader;
	private:
		typedef void(*RenderDrawListsFn)(ImDrawData*, void*);
		RenderDrawListsFn m_render_func;
	};
}

#endif