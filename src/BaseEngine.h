#pragma once

#include "ofEvents.h"
#include "ofAppBaseWindow.h"

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

struct ImGuiContext;
namespace ofxImGui
{
	class ImGuiContextScope
	{
	public:
		ImGuiContextScope(ImGuiContext* pContext);
		~ImGuiContextScope();

	private:
		ImGuiContext* m_pContext;
	};

	class BaseEngine
	{
	public:
		enum { MaxMouseButtonNum = 5 };

		BaseEngine()
			: isSetup(false)
		{}

		virtual ~BaseEngine()
		{}

		virtual void setup() = 0;
		virtual void exit() = 0;
		virtual void render() = 0;
		virtual bool createDeviceObjects() = 0;
		virtual void invalidateDeviceObjects() = 0;

		virtual void onRemapMouseButton(int&) {}

		virtual void onMouseDragged(ofMouseEventArgs& event);
		virtual void onMousePressed(ofMouseEventArgs& event);
		virtual void onMouseReleased(ofMouseEventArgs& event);
		virtual void onMouseScrolled(ofMouseEventArgs& event);
		virtual void onKeyPressed(ofKeyEventArgs& event);
		virtual void onKeyReleased(ofKeyEventArgs& event) = 0;
		virtual void onWindowResized(ofResizeEventArgs& window);

		virtual GLuint loadTextureImage2D(unsigned char * pixels, int width, int height, int gl_internal_format = GL_RGBA);

		static const char* getClipboardString(ImGuiContext* ctx);
		static void setClipboardString(ImGuiContext* ctx, const char* text);

		static int g_ShaderHandle;
		static int g_VertHandle;
		static int g_FragHandle;

		static int g_UniformLocationTex;
		static int g_UniformLocationProjMtx;
		static int g_AttribLocationPosition;
		static int g_AttribLocationUV;
		static int g_AttribLocationColor;

		unsigned int m_VboHandle;
		unsigned int m_ElementsHandle;

		static char g_BufferClipboard [2048];

		bool mousePressed[5] = { false };
		bool mouseReleased = true;

	protected:
		bool isSetup;
		ImGuiContext* pContext;
	};
}

