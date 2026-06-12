#include "EngineGLFW.h"

#if !defined(TARGET_OPENGLES) && !defined(OF_TARGET_API_VULKAN)

#include "ofAppGLFWWindow.h"
#include "ofGLProgrammableRenderer.h"
#include "GLFW/glfw3.h"

namespace ofxImGui
{
    //GLuint EngineGLFW::m_FontTexture = 0;
    //unsigned int EngineGLFW::m_VaoHandle = 0;

    //--------------------------------------------------------------
    void EngineGLFW::setup()
    {
        if (isSetup) return;

        pContext = ImGui::GetCurrentContext();
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

        if (ofIsGLProgrammableRenderer())
        {
            //io.RenderDrawListsFn = programmableRenderDrawLists;
            m_render_func = (RenderDrawListsFn)&programmableRenderDrawLists;
        }
        else
        {
            //io.RenderDrawListsFn = fixedRenderDrawLists;
            m_render_func = (RenderDrawListsFn)&fixedRenderDrawLists;
        }

        ImGui::GetPlatformIO().Platform_SetClipboardTextFn = &BaseEngine::setClipboardString;
        ImGui::GetPlatformIO().Platform_GetClipboardTextFn = &BaseEngine::getClipboardString;

        createDeviceObjects();

        // Override listeners
        ofAddListener(ofEvents().mousePressed, this, &EngineGLFW::onMousePressed);
        ofAddListener(ofEvents().keyReleased, this, &EngineGLFW::onKeyReleased);
        ofAddListener(ofEvents().keyPressed, this, &EngineGLFW::onKeyPressed);

        // BaseEngine listeners
        ofAddListener(ofEvents().mouseDragged, (BaseEngine*)this, &BaseEngine::onMouseDragged);
        ofAddListener(ofEvents().mouseReleased, (BaseEngine*)this, &BaseEngine::onMouseReleased);
        ofAddListener(ofEvents().mouseScrolled, (BaseEngine*)this, &BaseEngine::onMouseScrolled);
        ofAddListener(ofEvents().windowResized, (BaseEngine*)this, &BaseEngine::onWindowResized);

        isSetup = true;
    }

    //--------------------------------------------------------------
    void EngineGLFW::exit()
    {
        if (!isSetup) return;

        // Override listeners
        ofRemoveListener(ofEvents().mousePressed, this, &EngineGLFW::onMousePressed);
        ofRemoveListener(ofEvents().keyReleased, this, &EngineGLFW::onKeyReleased);
        ofRemoveListener(ofEvents().keyPressed, this, &EngineGLFW::onKeyPressed);

        // Base class listeners
        ofRemoveListener(ofEvents().mouseDragged, (BaseEngine*)this, &BaseEngine::onMouseDragged);
        ofRemoveListener(ofEvents().mouseReleased, (BaseEngine*)this, &BaseEngine::onMouseReleased);
        ofRemoveListener(ofEvents().mouseScrolled, (BaseEngine*)this, &BaseEngine::onMouseScrolled);
        ofRemoveListener(ofEvents().windowResized, (BaseEngine*)this, &BaseEngine::onWindowResized);

        invalidateDeviceObjects();

        pContext = NULL;
        isSetup = false;
    }

    void EngineGLFW::render()
    {
        if (!isSetup) return;

        m_render_func(ImGui::GetDrawData(), this);
    }


    //--------------------------------------------------------------
    void remapToGLFWConvention(int& button)
    {
        switch (button)
        {

        case 0:
            {
                break;
            }
        case 1:
            {
                button = 2;
                break;
            }
        case 2:
            {
                button = 1;
                break;
            }
        }
    }

    //--------------------------------------------------------------
    void EngineGLFW::onMousePressed(ofMouseEventArgs& event)
    {
        ImGuiContextScope scope(pContext);
        int button = event.button;
        if (button >= 0 && button < 5)
        {
            remapToGLFWConvention(button);
            mousePressed[button] = true;
            mouseReleased = false;
        }
    }

    //--------------------------------------------------------------
    void EngineGLFW::programmableRenderDrawLists(ImDrawData * draw_data, EngineGLFW* p_self)
    {
        // Process pending texture updates (new in 1.92)
        if (draw_data->Textures != nullptr)
            for (ImTextureData* tex : *draw_data->Textures)
                if (tex->Status != ImTextureStatus_OK)
                    p_self->UpdateTexture(tex);

        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width == 0 || fb_height == 0)
            return;
        draw_data->ScaleClipRects(draw_data->FramebufferScale);

        // Backup GL state
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
        GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
        GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
        GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
        GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glActiveTexture(GL_TEXTURE0);

        // Setup viewport, orthographic projection matrix
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        const float ortho_projection[4][4] =
        {
            { 2.0f / draw_data->DisplaySize.x, 0.0f,                              0.0f, 0.0f },
            { 0.0f,                  2.0f / -draw_data->DisplaySize.y, 0.0f, 0.0f },
            { 0.0f,                  0.0f,                  -1.0f, 0.0f },
            { -1.0f,                  1.0f,                   0.0f, 1.0f },
        };
        glUseProgram(g_ShaderHandle);
        glUniform1i(g_UniformLocationTex, 0);
        glUniformMatrix4fv(g_UniformLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
        glBindVertexArray(p_self->m_VaoHandle);

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const ImDrawIdx* idx_buffer_offset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, p_self->m_VboHandle);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_self->m_ElementsHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

            for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
            {
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)pcmd->GetTexID());
                    glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                idx_buffer_offset += pcmd->ElemCount;
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        // Restore modified GL state
        glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindVertexArray(last_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFunc(last_blend_src, last_blend_dst);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    }

    //--------------------------------------------------------------
    void EngineGLFW::fixedRenderDrawLists(ImDrawData * draw_data, EngineGLFW* p_self)
    {
        // Process pending texture updates (new in 1.92)
        if (draw_data->Textures != nullptr)
            for (ImTextureData* tex : *draw_data->Textures)
                if (tex->Status != ImTextureStatus_OK)
                    p_self->UpdateTexture(tex);

        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width == 0 || fb_height == 0)
            return;
        draw_data->ScaleClipRects(draw_data->FramebufferScale);

        // We are using the OpenGL fixed pipeline to make the example code simpler to read!
        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnable(GL_TEXTURE_2D);
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

        // Setup viewport, orthographic projection matrix
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f, draw_data->DisplaySize.x, draw_data->DisplaySize.y, 0.0f, -1.0f, +1.0f);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Render command lists
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
            const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
            glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
            glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
            glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)pcmd->GetTexID());
                    glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
                }
                idx_buffer += pcmd->ElemCount;
            }
        }
#undef OFFSETOF

        // Restore modified state
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glPopAttrib();
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    }

    //--------------------------------------------------------------
    static ImGuiKey glfw_key_to_imgui_key(int keycode)
    {
        switch (keycode)
        {
        case GLFW_KEY_TAB:
            {
                return ImGuiKey_Tab;
            }
        case GLFW_KEY_LEFT:
            {
                return ImGuiKey_LeftArrow;
            }
        case GLFW_KEY_RIGHT:
            {
                return ImGuiKey_RightArrow;
            }
        case GLFW_KEY_UP:
            {
                return ImGuiKey_UpArrow;
            }
        case GLFW_KEY_DOWN:
            {
                return ImGuiKey_DownArrow;
            }
        case GLFW_KEY_PAGE_UP:
            {
                return ImGuiKey_PageUp;
            }
        case GLFW_KEY_PAGE_DOWN:
            {
                return ImGuiKey_PageDown;
            }
        case GLFW_KEY_HOME:
            {
                return ImGuiKey_Home;
            }
        case GLFW_KEY_END:
            {
                return ImGuiKey_End;
            }
        case GLFW_KEY_INSERT:
            {
                return ImGuiKey_Insert;
            }
        case GLFW_KEY_DELETE:
            {
                return ImGuiKey_Delete;
            }
        case GLFW_KEY_BACKSPACE:
            {
                return ImGuiKey_Backspace;
            }
        case GLFW_KEY_SPACE:
            {
                return ImGuiKey_Space;
            }
        case GLFW_KEY_ENTER:
            {
                return ImGuiKey_Enter;
            }
        case GLFW_KEY_ESCAPE:
            {
                return ImGuiKey_Escape;
            }
        case GLFW_KEY_APOSTROPHE:
            {
                return ImGuiKey_Apostrophe;
            }
        case GLFW_KEY_COMMA:
            {
                return ImGuiKey_Comma;
            }
        case GLFW_KEY_MINUS:
            {
                return ImGuiKey_Minus;
            }
        case GLFW_KEY_PERIOD:
            {
                return ImGuiKey_Period;
            }
        case GLFW_KEY_SLASH:
            {
                return ImGuiKey_Slash;
            }
        case GLFW_KEY_SEMICOLON:
            {
                return ImGuiKey_Semicolon;
            }
        case GLFW_KEY_EQUAL:
            {
                return ImGuiKey_Equal;
            }
        case GLFW_KEY_LEFT_BRACKET:
            {
                return ImGuiKey_LeftBracket;
            }
        case GLFW_KEY_BACKSLASH:
            {
                return ImGuiKey_Backslash;
            }
        case GLFW_KEY_RIGHT_BRACKET:
            {
                return ImGuiKey_RightBracket;
            }
        case GLFW_KEY_GRAVE_ACCENT:
            {
                return ImGuiKey_GraveAccent;
            }
        case GLFW_KEY_CAPS_LOCK:
            {
                return ImGuiKey_CapsLock;
            }
        case GLFW_KEY_SCROLL_LOCK:
            {
                return ImGuiKey_ScrollLock;
            }
        case GLFW_KEY_NUM_LOCK:
            {
                return ImGuiKey_NumLock;
            }
        case GLFW_KEY_PRINT_SCREEN:
            {
                return ImGuiKey_PrintScreen;
            }
        case GLFW_KEY_PAUSE:
            {
                return ImGuiKey_Pause;
            }
        case GLFW_KEY_F1:
            {
                return ImGuiKey_F1;
            }
        case GLFW_KEY_F2:
            {
                return ImGuiKey_F2;
            }
        case GLFW_KEY_F3:
            {
                return ImGuiKey_F3;
            }
        case GLFW_KEY_F4:
            {
                return ImGuiKey_F4;
            }
        case GLFW_KEY_F5:
            {
                return ImGuiKey_F5;
            }
        case GLFW_KEY_F6:
            {
                return ImGuiKey_F6;
            }
        case GLFW_KEY_F7:
            {
                return ImGuiKey_F7;
            }
        case GLFW_KEY_F8:
            {
                return ImGuiKey_F8;
            }
        case GLFW_KEY_F9:
            {
                return ImGuiKey_F9;
            }
        case GLFW_KEY_F10:
            {
                return ImGuiKey_F10;
            }
        case GLFW_KEY_F11:
            {
                return ImGuiKey_F11;
            }
        case GLFW_KEY_F12:
            {
                return ImGuiKey_F12;
            }
        case GLFW_KEY_LEFT_SHIFT:
            {
                return ImGuiKey_LeftShift;
            }
        case GLFW_KEY_LEFT_CONTROL:
            {
                return ImGuiKey_LeftCtrl;
            }
        case GLFW_KEY_LEFT_ALT:
            {
                return ImGuiKey_LeftAlt;
            }
        case GLFW_KEY_LEFT_SUPER:
            {
                return ImGuiKey_LeftSuper;
            }
        case GLFW_KEY_RIGHT_SHIFT:
            {
                return ImGuiKey_RightShift;
            }
        case GLFW_KEY_RIGHT_CONTROL:
            {
                return ImGuiKey_RightCtrl;
            }
        case GLFW_KEY_RIGHT_ALT:
            {
                return ImGuiKey_RightAlt;
            }
        case GLFW_KEY_RIGHT_SUPER:
            {
                return ImGuiKey_RightSuper;
            }
        case GLFW_KEY_MENU:
            {
                return ImGuiKey_Menu;
            }
        case GLFW_KEY_KP_0:
            {
                return ImGuiKey_Keypad0;
            }
        case GLFW_KEY_KP_1:
            {
                return ImGuiKey_Keypad1;
            }
        case GLFW_KEY_KP_2:
            {
                return ImGuiKey_Keypad2;
            }
        case GLFW_KEY_KP_3:
            {
                return ImGuiKey_Keypad3;
            }
        case GLFW_KEY_KP_4:
            {
                return ImGuiKey_Keypad4;
            }
        case GLFW_KEY_KP_5:
            {
                return ImGuiKey_Keypad5;
            }
        case GLFW_KEY_KP_6:
            {
                return ImGuiKey_Keypad6;
            }
        case GLFW_KEY_KP_7:
            {
                return ImGuiKey_Keypad7;
            }
        case GLFW_KEY_KP_8:
            {
                return ImGuiKey_Keypad8;
            }
        case GLFW_KEY_KP_9:
            {
                return ImGuiKey_Keypad9;
            }
        case GLFW_KEY_KP_DECIMAL:
            {
                return ImGuiKey_KeypadDecimal;
            }
        case GLFW_KEY_KP_DIVIDE:
            {
                return ImGuiKey_KeypadDivide;
            }
        case GLFW_KEY_KP_MULTIPLY:
            {
                return ImGuiKey_KeypadMultiply;
            }
        case GLFW_KEY_KP_SUBTRACT:
            {
                return ImGuiKey_KeypadSubtract;
            }
        case GLFW_KEY_KP_ADD:
            {
                return ImGuiKey_KeypadAdd;
            }
        case GLFW_KEY_KP_ENTER:
            {
                return ImGuiKey_KeypadEnter;
            }
        case GLFW_KEY_KP_EQUAL:
            {
                return ImGuiKey_KeypadEqual;
            }
        case GLFW_KEY_A:
            {
                return ImGuiKey_A;
            }
        case GLFW_KEY_B:
            {
                return ImGuiKey_B;
            }
        case GLFW_KEY_C:
            {
                return ImGuiKey_C;
            }
        case GLFW_KEY_D:
            {
                return ImGuiKey_D;
            }
        case GLFW_KEY_E:
            {
                return ImGuiKey_E;
            }
        case GLFW_KEY_F:
            {
                return ImGuiKey_F;
            }
        case GLFW_KEY_G:
            {
                return ImGuiKey_G;
            }
        case GLFW_KEY_H:
            {
                return ImGuiKey_H;
            }
        case GLFW_KEY_I:
            {
                return ImGuiKey_I;
            }
        case GLFW_KEY_J:
            {
                return ImGuiKey_J;
            }
        case GLFW_KEY_K:
            {
                return ImGuiKey_K;
            }
        case GLFW_KEY_L:
            {
                return ImGuiKey_L;
            }
        case GLFW_KEY_M:
            {
                return ImGuiKey_M;
            }
        case GLFW_KEY_N:
            {
                return ImGuiKey_N;
            }
        case GLFW_KEY_O:
            {
                return ImGuiKey_O;
            }
        case GLFW_KEY_P:
            {
                return ImGuiKey_P;
            }
        case GLFW_KEY_Q:
            {
                return ImGuiKey_Q;
            }
        case GLFW_KEY_R:
            {
                return ImGuiKey_R;
            }
        case GLFW_KEY_S:
            {
                return ImGuiKey_S;
            }
        case GLFW_KEY_T:
            {
                return ImGuiKey_T;
            }
        case GLFW_KEY_U:
            {
                return ImGuiKey_U;
            }
        case GLFW_KEY_V:
            {
                return ImGuiKey_V;
            }
        case GLFW_KEY_W:
            {
                return ImGuiKey_W;
            }
        case GLFW_KEY_X:
            {
                return ImGuiKey_X;
            }
        case GLFW_KEY_Y:
            {
                return ImGuiKey_Y;
            }
        case GLFW_KEY_Z:
            {
                return ImGuiKey_Z;
            }
        case GLFW_KEY_0:
            {
                return ImGuiKey_0;
            }
        case GLFW_KEY_1:
            {
                return ImGuiKey_1;
            }
        case GLFW_KEY_2:
            {
                return ImGuiKey_2;
            }
        case GLFW_KEY_3:
            {
                return ImGuiKey_3;
            }
        case GLFW_KEY_4:
            {
                return ImGuiKey_4;
            }
        case GLFW_KEY_5:
            {
                return ImGuiKey_5;
            }
        case GLFW_KEY_6:
            {
                return ImGuiKey_6;
            }
        case GLFW_KEY_7:
            {
                return ImGuiKey_7;
            }
        case GLFW_KEY_8:
            {
                return ImGuiKey_8;
            }
        case GLFW_KEY_9:
            {
                return ImGuiKey_9;
            }
        default:
            {
                return ImGuiKey_None;
            }
        }
    }

    static void update_key_modifiers(ImGuiIO& io, GLFWwindow* window)
    {
        io.AddKeyEvent(ImGuiMod_Ctrl,  (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
        io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS));
        io.AddKeyEvent(ImGuiMod_Alt,   (glfwGetKey(window, GLFW_KEY_LEFT_ALT)     == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS));
        io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS));
    }

    //--------------------------------------------------------------
    void EngineGLFW::onKeyReleased(ofKeyEventArgs& event)
    {
        ImGuiContextScope scope(pContext);
        ImGuiIO& io = ImGui::GetIO();

        ofAppGLFWWindow* win = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr());
        if (win) update_key_modifiers(io, win->getGLFWWindow());

        ImGuiKey imgui_key = glfw_key_to_imgui_key(event.keycode);
        if (imgui_key != ImGuiKey_None)
            io.AddKeyEvent(imgui_key, false);
    }

    //--------------------------------------------------------------
    void EngineGLFW::onKeyPressed(ofKeyEventArgs& event)
    {
        ImGuiContextScope scope(pContext);
        ImGuiIO& io = ImGui::GetIO();

        ofAppGLFWWindow* win = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr());
        if (win) update_key_modifiers(io, win->getGLFWWindow());

        ImGuiKey imgui_key = glfw_key_to_imgui_key(event.keycode);
        if (imgui_key != ImGuiKey_None)
            io.AddKeyEvent(imgui_key, true);

        if (event.codepoint != 0)
            io.AddInputCharacter(event.codepoint);
    }

    //--------------------------------------------------------------
    bool EngineGLFW::createDeviceObjects()
    {
        if (ofIsGLProgrammableRenderer())
        {
            // Backup GL state
            GLint last_array_buffer, last_vertex_array;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

            const GLchar *vertex_shader =
                "#version 330\n"
                "uniform mat4 ProjMtx;\n"
                "in vec2 Position;\n"
                "in vec2 UV;\n"
                "in vec4 Color;\n"
                "out vec2 Frag_UV;\n"
                "out vec4 Frag_Color;\n"
                "void main()\n"
                "{\n"
                "	Frag_UV = UV;\n"
                "	Frag_Color = Color;\n"
                "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
                "}\n";

            const GLchar* fragment_shader =
                "#version 330\n"
                "uniform sampler2D Texture;\n"
                "in vec2 Frag_UV;\n"
                "in vec4 Frag_Color;\n"
                "out vec4 Out_Color;\n"
                "void main()\n"
                "{\n"
                "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
                "}\n";

            g_ShaderHandle = glCreateProgram();
            g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
            g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
            glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
            glCompileShader(g_VertHandle);
            glCompileShader(g_FragHandle);
            glAttachShader(g_ShaderHandle, g_VertHandle);
            glAttachShader(g_ShaderHandle, g_FragHandle);
            glLinkProgram(g_ShaderHandle);

            g_UniformLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
            g_UniformLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
            g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
            g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
            g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

            glGenBuffers(1, &m_VboHandle);
            glGenBuffers(1, &m_ElementsHandle);

            glGenVertexArrays(1, &m_VaoHandle);
            glBindVertexArray(m_VaoHandle);
            glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
            glEnableVertexAttribArray(g_AttribLocationPosition);
            glEnableVertexAttribArray(g_AttribLocationUV);
            glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
            glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
            glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
            glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

            // Font texture is now managed via UpdateTexture() / ImGuiBackendFlags_RendererHasTextures

            // Restore modified GL state
            glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
            glBindVertexArray(last_vertex_array);

            return true;
        }
        else
        {
            return true;
        }
    }

    //--------------------------------------------------------------
    bool EngineGLFW::createFontsTexture()
    {
        // Font textures are now managed via UpdateTexture() using ImGuiBackendFlags_RendererHasTextures
        return true;
    }

    //--------------------------------------------------------------
    void EngineGLFW::UpdateTexture(ImTextureData* tex)
    {
        if (tex->Status == ImTextureStatus_WantCreate)
        {
            IM_ASSERT(tex->TexID == ImTextureID_Invalid);
            GLint last_texture;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

            GLuint gl_texture;
            glGenTextures(1, &gl_texture);
            glBindTexture(GL_TEXTURE_2D, gl_texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLenum fmt = (tex->Format == ImTextureFormat_RGBA32) ? GL_RGBA : GL_RED;
            glTexImage2D(GL_TEXTURE_2D, 0, fmt, tex->Width, tex->Height, 0, fmt, GL_UNSIGNED_BYTE, tex->GetPixels());

            glBindTexture(GL_TEXTURE_2D, last_texture);

            tex->SetTexID((ImTextureID)(uintptr_t)gl_texture);
            tex->SetStatus(ImTextureStatus_OK);
            m_FontTexture = gl_texture;
        }
        else if (tex->Status == ImTextureStatus_WantUpdates)
        {
            IM_ASSERT(tex->TexID != ImTextureID_Invalid);
            GLint last_texture;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
            glBindTexture(GL_TEXTURE_2D, (GLuint)(uintptr_t)tex->TexID);

            GLenum fmt = (tex->Format == ImTextureFormat_RGBA32) ? GL_RGBA : GL_RED;
            for (ImTextureRect& r : tex->Updates)
            {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, tex->Width);
                glTexSubImage2D(GL_TEXTURE_2D, 0, r.x, r.y, r.w, r.h, fmt, GL_UNSIGNED_BYTE, tex->GetPixelsAt(r.x, r.y));
            }
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

            glBindTexture(GL_TEXTURE_2D, last_texture);
            tex->SetStatus(ImTextureStatus_OK);
        }
        else if (tex->Status == ImTextureStatus_WantDestroy)
        {
            if (tex->TexID != ImTextureID_Invalid)
            {
                GLuint gl_texture = (GLuint)(uintptr_t)tex->TexID;
                glDeleteTextures(1, &gl_texture);
                if (m_FontTexture == gl_texture)
                    m_FontTexture = 0;
            }
            tex->SetTexID(ImTextureID_Invalid);
            tex->SetStatus(ImTextureStatus_Destroyed);
        }
    }

    //--------------------------------------------------------------
    void EngineGLFW::invalidateDeviceObjects()
    {
        if (ofIsGLProgrammableRenderer())
        {
            if (m_VaoHandle) glDeleteVertexArrays(1, &m_VaoHandle);
            if (m_VboHandle) glDeleteBuffers(1, &m_VboHandle);
            if (m_ElementsHandle) glDeleteBuffers(1, &m_ElementsHandle);
            m_VaoHandle = m_VboHandle = m_ElementsHandle = 0;

            glDetachShader(g_ShaderHandle, g_VertHandle);
            glDeleteShader(g_VertHandle);
            g_VertHandle = 0;

            glDetachShader(g_ShaderHandle, g_FragHandle);
            glDeleteShader(g_FragHandle);
            g_FragHandle = 0;

            glDeleteProgram(g_ShaderHandle);
            g_ShaderHandle = 0;
        }

        if (m_FontTexture)
        {
            glDeleteTextures(1, &m_FontTexture);
            m_FontTexture = 0;
        }
    }
}

#endif
