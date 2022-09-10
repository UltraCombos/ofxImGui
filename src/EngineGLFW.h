#pragma once

#include "ofConstants.h"
#include "GLFW/glfw3.h"
#if !defined(TARGET_OPENGLES) && (!defined (OF_TARGET_API_VULKAN) )

#include "BaseEngine.h"

#include "ofEvents.h"
#include "imgui.h"

namespace ofxImGui
{
	class EngineGLFW 
		: public BaseEngine
	{
	public:
		~EngineGLFW()
		{
			exit();
		}

		// BaseEngine required
		void setup() override;
		void exit() override;
		void render() override;
		bool createDeviceObjects() override;
		void invalidateDeviceObjects() override;

		bool createFontsTexture();

		void onKeyReleased(ofKeyEventArgs& event) override;
		void onKeyPressed(ofKeyEventArgs& event) override;
		void onMousePressed(ofMouseEventArgs& event) override;

		// Custom 
		static void programmableRenderDrawLists(ImDrawData * draw_data, EngineGLFW* p_self);
		static void fixedRenderDrawLists(ImDrawData * draw_data, EngineGLFW* p_self);

		GLuint m_FontTexture;
		unsigned int m_VaoHandle;

	private:
		typedef void(*RenderDrawListsFn)(ImDrawData*, void*);
		RenderDrawListsFn m_render_func;
	};
}

#endif