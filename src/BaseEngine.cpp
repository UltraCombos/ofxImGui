#include "BaseEngine.h"

#include "ofAppRunner.h"
#include "ofAppBaseWindow.h"
#include "imgui.h"

namespace ofxImGui
{
	int BaseEngine::g_ShaderHandle = 0;
	int BaseEngine::g_VertHandle = 0;
	int BaseEngine::g_FragHandle = 0;

	int BaseEngine::g_UniformLocationTex = 0;
	int BaseEngine::g_UniformLocationProjMtx = 0;
	int BaseEngine::g_AttribLocationUV = 0;

	int BaseEngine::g_AttribLocationPosition = 0;
	int BaseEngine::g_AttribLocationColor = 0;

	unsigned int BaseEngine::g_VboHandle = 0;
	unsigned int BaseEngine::g_ElementsHandle = 0;

	char BaseEngine::g_BufferClipboard [2048];

	//--------------------------------------------------------------
	void BaseEngine::onKeyPressed(ofKeyEventArgs& event)
	{
		int key = event.keycode;
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[key] = true;
		//io->AddInputCharacter((unsigned short)event.codepoint);
	}

	//--------------------------------------------------------------
	void BaseEngine::onMouseDragged(ofMouseEventArgs& event)
	{
		mouseReleased = false;
	}

	//--------------------------------------------------------------
	void BaseEngine::onMousePressed(ofMouseEventArgs& event)
	{
		int imgui_btn = event.button;
		if (imgui_btn >= 0 && imgui_btn < MaxMouseButtonNum)
		{
			onRemapMouseButton(imgui_btn);

			mousePressed[imgui_btn] = true;
			mouseReleased = false;
		}
	}

	//--------------------------------------------------------------
	void BaseEngine::onMouseReleased(ofMouseEventArgs& event)
	{
		mouseReleased = true;
	}

	//--------------------------------------------------------------
	void BaseEngine::onMouseScrolled(ofMouseEventArgs& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel = event.scrollY;
	}

	//--------------------------------------------------------------
	void BaseEngine::onWindowResized(ofResizeEventArgs& window)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)window.width, (float)window.height);
	}

	//--------------------------------------------------------------
	const char* BaseEngine::getClipboardString(void* user_data)
	{
		std::string str = ofGetWindowPtr()->getClipboardString();
		strncpy(g_BufferClipboard, str.c_str(), sizeof(g_BufferClipboard));
		g_BufferClipboard[sizeof(g_BufferClipboard) - 1] = '\0';
		return g_BufferClipboard;
	}

	//--------------------------------------------------------------
	void BaseEngine::setClipboardString(void* user_data, const char * text)
	{
		strncpy(g_BufferClipboard, text, sizeof(g_BufferClipboard));
		g_BufferClipboard[sizeof(g_BufferClipboard) - 1] = '\0';
		ofGetWindowPtr()->setClipboardString(g_BufferClipboard);
	}

	//--------------------------------------------------------------
	GLuint BaseEngine::loadTextureImage2D(unsigned char * pixels, int width, int height, int gl_internal_format)
	{
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

		GLuint new_texture;
		glGenTextures(1, &new_texture);
		glBindTexture(GL_TEXTURE_2D, new_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			gl_internal_format,
			width, height,
			0,
			gl_internal_format,
			GL_UNSIGNED_BYTE,
			pixels
		);

		glBindTexture(GL_TEXTURE_2D, last_texture);

		return new_texture;
	};
}
