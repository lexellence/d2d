/**************************************************************************************\
** File: d2Window.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for Window and related classes
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Window.h"
#include "d2Utility.h"
#include "d2Timer.h"
#include "d2NumberManip.h"
#include <optional>
namespace d2d
{
	namespace
	{
		WindowDef m_windowDef;
		bool m_sdlImageInitialized{ false };
		SDL_Window* m_windowPtr{ nullptr };
		SDL_GLContext m_glContext;
		bool m_texturesEnabled;
		bool m_blendingEnabled;
		bool m_textureBinded;
		unsigned int m_boundGLTextureID;
		bool m_fontBinded;
		unsigned int m_boundFontID;
		//int m_bindedDTXFontSize;
		Timer m_timer;
		float m_fpsUpdateInterval;
		float m_fpsUpdateAccumulator;
		unsigned int m_frames;
		float m_fps;
	}

    //+--------------------------\--------------------------------
    //|          Window          |
    //\--------------------------/--------------------------------
    namespace Window
    {
        //+----------------------\------------------------------------
        //|	Startup and shutdown |
        //\----------------------/------------------------------------
        void Init(const WindowDef &windowDef)
        {
            // Save settings
            m_windowDef = windowDef;

            // Make sure SDL video subsystem is initialized
            if (!SDL_WasInit(SDL_INIT_VIDEO))
                throw InitException{
                    "Failed to initialize window: SDL video system not yet initialized"};

            // Get rid of previous window, if it exists.
            Close();

            // SDL_image
            {
                SDL_ClearError();
                int imageExtensionsLoaded = IMG_Init(windowDef.imageExtensions);
                if ((imageExtensionsLoaded & windowDef.imageExtensions) != windowDef.imageExtensions)
                    throw InitException{ "Failed to load all requested imageExtensions: "s + IMG_GetError() };
                m_sdlImageInitialized = true;
            }

            // OpenGL settings
            {
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, m_windowDef.gl.profileMask);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_windowDef.gl.versionMajor);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_windowDef.gl.versionMinor);
                SDL_GL_SetAttribute(SDL_GL_RED_SIZE, m_windowDef.colorChannelBits[0]);
                SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, m_windowDef.colorChannelBits[1]);
                SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, m_windowDef.colorChannelBits[2]);
                SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, m_windowDef.colorChannelBits[3]);
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_windowDef.doubleBuffer);

				int samples = d2d::GetClamped(m_windowDef.antiAliasingSamples, VALID_ANTI_ALIASING_SAMPLES);
				if(samples >= MIN_SAMPLES_TO_ENABLE_ANTI_ALIASING)
				{
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
				}
				else
					SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			}

            // Create window
            {
                Uint32 windowFlags{ SDL_WINDOW_OPENGL };
                if(m_windowDef.fullScreen)
                    windowFlags |= m_windowDef.gl.fullscreenModeFlag;
                else
                    windowFlags |= m_windowDef.gl.windowedModeFlag;

				SDL_ClearError();
				m_windowPtr = SDL_CreateWindow(m_windowDef.title.c_str(), 
					m_windowDef.x, m_windowDef.y, m_windowDef.width, m_windowDef.height, windowFlags);
				if(!m_windowPtr)
					throw InitException{ std::string{"SDL_CreateWindow failed: "} +SDL_GetError() };
			}

			// Create OpenGL context
			SDL_ClearError();
			if(!(m_glContext = SDL_GL_CreateContext(m_windowPtr)))
				throw InitException{ std::string{"SDL_GL_CreateContext failed: "} +SDL_GetError() };

			// Set vsync
			if(m_windowDef.vsync)
				SDL_GL_SetSwapInterval(m_windowDef.vsyncAllowLateSwaps ? -1 : 1);
			else
				SDL_GL_SetSwapInterval(0);

			EnableTextures();
			EnableBlending();

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// OpenGL settings
			glClearColor(COLOR_ZERO.red, COLOR_ZERO.green, COLOR_ZERO.blue, COLOR_ZERO.alpha);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);	// Face culling discards polygons facing into the screen
			glShadeModel(m_windowDef.gl.shadeModel);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, m_windowDef.gl.perspectiveCorrectionMode);

			if(m_windowDef.gl.pointSmoothing)
				glEnable(GL_POINT_SMOOTH);
			else
				glDisable(GL_POINT_SMOOTH);
			if(m_windowDef.gl.lineSmoothing)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);

			// Initialize FPS counter
			m_timer.Start();
			m_fpsUpdateInterval = m_windowDef.fpsUpdateDelay;
			m_fpsUpdateAccumulator = 0.0f;
			m_frames = 0U;
			m_fps = 0.0f;
		}
		void Close()
		{
			// Shutdown SDL_Image
			if (m_sdlImageInitialized) {
				IMG_Quit();
				m_sdlImageInitialized = false;
			}

			// If there is a window, get rid of it
			if (m_windowPtr)
			{
				SDL_DestroyWindow(m_windowPtr);
				SDL_ClearError();
				m_windowPtr = nullptr;
			}
		}
		//+-------------\---------------------------------------------
		//|	Accessors	|
		//\-------------/---------------------------------------------
		float GetXYAspectRatio()
		{
			int width, height;
			SDL_GL_GetDrawableSize(m_windowPtr, &width, &height);
			if (height == 0)
				height = 1;
			return ((float)width / (float)height);
		}
		b2Vec2 GetScreenResolution()
		{
			int width, height;
			SDL_GL_GetDrawableSize(m_windowPtr, &width, &height);
			return{ (float)width, (float)height };
		}
		float GetFPS()
		{
			return m_fps;
		}
		b2Vec2 GetMousePositionAsPercentOfWindow(Sint32 eventMouseX, Sint32 eventMouseY)
		{
			b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
			return { (float)eventMouseX / resolution.x, (1 - ((float)eventMouseY / resolution.y)) };
		}
		b2Vec2 GetMousePosition()
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			b2Vec2 resolution{ d2d::Window::GetScreenResolution() };
			return GetMousePositionAsPercentOfWindow(x, y);
		}
		const WindowDef& GetWindowDef()
		{
			return m_windowDef;
		}
		SDL_Window* GetSDLWindowPtr()
		{
			return m_windowPtr;
		}

		//+-------------\---------------------------------------------
		//|	Modifiers	|
		//\-------------/---------------------------------------------
		void Resize(int width, int height)
		{
			if(height <= 0)   
				height = 1;
			glViewport(0, 0, (GLsizei)width, (GLsizei)height);
		}
		void SetClearColor(const Color& newColor)
		{
			glClearColor(newColor.red, newColor.green, newColor.blue, newColor.alpha);
		}
		void SetShowCursor(bool enabled)
		{
			SDL_ShowCursor(enabled);
		}
		void SetFPSInterval(float interval)
		{
			if (interval < 0.0f)
				m_fpsUpdateInterval = 0.0f;
			else
				m_fpsUpdateInterval = interval;
		}
		void SetCameraRect(const Rect& rect)
		{
			// Setup a 2D viewport based on camera dimensions
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(rect.lowerBound.x,		// left
				rect.upperBound.x,		// right
				rect.lowerBound.y,		// bottom
				rect.upperBound.y);		// top
		}
		void SetColor(const d2d::Color & newColor)
		{
			glColor4f(newColor.red, newColor.green, newColor.blue, newColor.alpha);
		}
		void SetPointSize(float size)
		{
			glPointSize(size);
		}
		void SetLineWidth(float width)
		{
			// -----------------------------------
			// Support for glLineWidth is not
			// required of OpenGL implementations,
			// and probably won't work!
			// -----------------------------------
			glLineWidth(width);
		}
		void EnableTextures()
		{
			if (!m_texturesEnabled)
			{
				glEnable(GL_TEXTURE_2D);
				m_texturesEnabled = true;
			}
		}
		void DisableTextures()
		{
			if (m_texturesEnabled)
			{
				glDisable(GL_TEXTURE_2D);
				m_texturesEnabled = false;
			}
		}
		void EnableBlending()
		{
			if (!m_blendingEnabled)
			{
				// Enable blending
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				m_blendingEnabled = true;
			}
		}
		void DisableBlending()
		{
			if (m_blendingEnabled)
			{
				glDisable(GL_BLEND);
				m_blendingEnabled = false;
			}
		}
		void StartScene()
		{
			// Direct OpenGL calls to go to this window
			SDL_GL_MakeCurrent(m_windowPtr, m_glContext);

			// Clear the screen and model matrix
			glClear(GL_COLOR_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		void EndScene()
		{
			// Update FPS periodically
			++m_frames;
			m_timer.Update();
			m_fpsUpdateAccumulator += m_timer.Getdt();
			if (m_fpsUpdateAccumulator > 0.0f &&
				m_fpsUpdateAccumulator >= m_fpsUpdateInterval)
			{
				m_fps = m_frames / m_fpsUpdateAccumulator;
				m_frames = 0;
				m_fpsUpdateAccumulator = 0.0f;
			}

			// Swap buffers
			if (m_windowPtr)
				SDL_GL_SwapWindow(m_windowPtr);
		}
		void PushMatrix()
		{
			// Save transformation
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
		}
		void Translate(const b2Vec2 & position)
		{
			// Move to the local origin
			glTranslatef(position.x, position.y, 0.0f);
		}
		void Rotate(float radians)
		{
			// Rotate about the z-axis
			glRotatef(GetDegreesFromRadians(radians), 0.0f, 0.0f, 1.0f);
		}
		void PopMatrix()
		{
			// Restore transformation
			glPopMatrix();
		}

		//+-------------\---------------------------------------------
		//|	Draw		|
		//\-------------/---------------------------------------------
		void DrawPoint(const b2Vec2 & position)
		{
			glBegin(GL_POINTS);
			glVertex2f(position.x, position.y);
			glEnd();
		}
		void DrawCircle(const b2Vec2 & center, float radius, bool fill)
		{
			PushMatrix();
			Translate(center);

			// Triangle fan for filled mode, line loop for outline mode
			glBegin(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);

			// Triangle fans need to start at the center
			if (fill)
				glVertex2f(0.0f, 0.0f);

			// Make sure radius is positive
			radius = abs(radius);

			// Number of vertices is proportional to how big it is
			unsigned numVertices{ (unsigned)(16.0f + radius * 2.0f) };

			// Angle between adjacent vertex pairs
			float radiansPerVertex{ TWO_PI / (float)numVertices };

			// Specify vertices
			for (unsigned i = 0; i < numVertices; ++i)
			{
				float theta{ i * radiansPerVertex };
				glVertex2f(cosf(theta) * radius, sinf(theta) * radius);
			}

			// Only the triangle fan needs to close the loop
			if (fill)
				glVertex2f(radius, 0.0f);
			glEnd();
			PopMatrix();
		}
		void DrawPolygon(const b2Vec2 * vertices, unsigned vertexCount, bool fill)
		{
			if (!vertices || !vertexCount)
				return;

			// Polygon for filled mode, line loop for outline mode
			glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
			for (unsigned i = 0; i < vertexCount; ++i)
				glVertex2f(vertices[i].x, vertices[i].y);
			glEnd();
		}
		void DrawRect(const Rect & drawRect, bool fill)
		{
			// Quad for filled mode, line loop for outline mode
			glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
			glVertex2f(drawRect.lowerBound.x, drawRect.lowerBound.y);
			glVertex2f(drawRect.upperBound.x, drawRect.lowerBound.y);
			glVertex2f(drawRect.upperBound.x, drawRect.upperBound.y);
			glVertex2f(drawRect.lowerBound.x, drawRect.upperBound.y);
			glEnd();
		}
		void DrawLine(const b2Vec2 & p1, const b2Vec2 & p2)
		{
			glBegin(GL_LINES);
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
			glEnd();
		}
		void DrawLineStrip(const b2Vec2* vertices, unsigned vertexCount)
		{
			if (!vertices || !vertexCount)
				return;

			glBegin(GL_LINE_STRIP);
			for (unsigned i = 0; i < vertexCount; ++i)
				glVertex2f(vertices[i].x, vertices[i].y);
			glEnd();
		}
		namespace
		{
			b2Vec2 GetTextAlignmentTranslation(float width, float height, float fontHeight,
				float padding, const AlignmentAnchor& anchor)
			{
				b2Vec2 translation;
				// Alignment: dtx string alignment is left bottom of the first character of the first line,
				//	so we have to translate to that point.
				switch(anchor.x)
				{
				case AlignmentAnchorX::CENTER:
					translation.x = -0.5f * width;
					break;
				case AlignmentAnchorX::LEFT:
					translation.x = padding;
					break;
				case AlignmentAnchorX::RIGHT:
					translation.x = -(width + padding);
					break;
				}
				switch(anchor.y)
				{
				case AlignmentAnchorY::CENTER:
					translation.y = 0.5f * height - fontHeight;
					break;
				case AlignmentAnchorY::BOTTOM:
					translation.y = height + padding - fontHeight;
					break;
				case AlignmentAnchorY::TOP:
					translation.y = -(fontHeight + padding);
					break;
				}
				return translation;
			}
		}
		void DrawString(const std::string& text, float size, const FontReference* fontPtr, const AlignmentAnchor& anchor)
		{
			if(!fontPtr)
				return;

			// Bind font if not already bound
			if (!m_fontBinded || m_boundFontID != fontPtr->GetID())
			{
				// Find the loaded font with matching id, and enable it.
				m_fontBinded = true;
				m_boundFontID = fontPtr->GetID();
				dtx_use_font(fontPtr->GetDTXFontPtr(), DTX_FONT_SIZE);
			}

			// Height 
			float fontHeight;
			{
				float lineHeight{ dtx_line_height() };
				float fontHeightToLineHeightRatio{ FONT_HEIGHT_TO_LINE_HEIGHT_RATIO };
				fontHeight = fontHeightToLineHeightRatio * lineHeight;
			}

			// Padding
			float fontPadding{ FONT_PADDING };
			struct dtx_box stringDimensions;
			dtx_string_box(text.c_str(), &stringDimensions);
			b2Vec2 translation = GetTextAlignmentTranslation(stringDimensions.width, stringDimensions.height, fontHeight, fontPadding, anchor);

			// Adjust size on screen so that it looks the same at any window size
			float scale{ size / (float)DTX_FONT_SIZE };

			// Draw string
			Window::PushMatrix();
			glScalef(scale, scale, 1.0f);
			Window::Translate(translation);
			dtx_string(text.c_str());
			Window::PopMatrix();
		}
		void DrawTexture(const Texture& texture, const b2Vec2& size)
		{
			Rect drawRect;
			drawRect.SetCenter(b2Vec2_zero, size);
			DrawTextureInRect(texture, drawRect);
		}
		void DrawTextureInRect(const Texture& texture, const Rect& drawRect)
		{
			GLuint glTextureID = texture.GetGLTextureID();
			if (!m_textureBinded || m_boundGLTextureID != glTextureID)
			{
				glBindTexture(GL_TEXTURE_2D, glTextureID);
				m_boundGLTextureID = glTextureID;
			}

            TextureCoordinates textureCoords = texture.GetTextureCoordinates();
            glBegin(GL_QUADS);
            {
                glTexCoord2f(textureCoords.lowerLeft.x, textureCoords.lowerLeft.y);
                glVertex2f(drawRect.lowerBound.x, drawRect.lowerBound.y);
                glTexCoord2f(textureCoords.lowerRight.x, textureCoords.lowerRight.y);
                glVertex2f(drawRect.upperBound.x, drawRect.lowerBound.y);
                glTexCoord2f(textureCoords.upperRight.x, textureCoords.upperRight.y);
                glVertex2f(drawRect.upperBound.x, drawRect.upperBound.y);
                glTexCoord2f(textureCoords.upperLeft.x, textureCoords.upperLeft.y);
                glVertex2f(drawRect.lowerBound.x, drawRect.upperBound.y);
            }
            glEnd();
        }
		void ShowSimpleMessageBox(MessageBoxType type, const std::string& title, const std::string& message)
		{
			if(m_windowDef.fullScreen)
			{
				SDL_SetWindowFullscreen(m_windowPtr, 0);
				SDL_GL_SwapWindow(m_windowPtr);
			}
			SDL_ShowSimpleMessageBox((Uint32)type, title.c_str(), message.c_str(), m_windowPtr);
			if(m_windowDef.fullScreen)
			{
				SDL_SetWindowFullscreen(m_windowPtr, m_windowDef.gl.fullscreenModeFlag);
				SDL_GL_SwapWindow(m_windowPtr);
			}
		}
    } // namespace Window
} // namespace d2d
