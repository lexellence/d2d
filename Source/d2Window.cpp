/**************************************************************************************\
** File: d2Window.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Window class
**
\**************************************************************************************/
#include "d2Window.h"
#include "d2Utility.h"
#include "d2Resource.h"
#include "d2Timer.h"
#include "d2NumberManip.h"
#include "drawtext.h"
#include <GL/GLU.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <Box2D/Box2D.h>
#include <string>
namespace d2d
{
	namespace Window
	{
		namespace
		{
			class TextureResource;
			class FontResource;

			WindowDef m_windowDef;
			bool m_sdlImageInitialized{ false };
			SDL_Window* m_windowPtr{ nullptr };
			SDL_GLContext m_glContext;
			ResourceManager<TextureResource> m_textureManager;
			ResourceManager<FontResource> m_fontManager;
			bool m_texturesEnabled;
			bool m_blendingEnabled;
			bool m_textureBinded;
			unsigned int m_bindedTextureID;
			bool m_fontBinded;
			unsigned int m_bindedFontID;
			int m_bindedDTXFontSize;
			bool m_firstPresent;
			Timer m_timer;
			float m_fpsUpdateInterval;
			float m_fpsUpdateAccumulator;
			unsigned int m_frames;
			float m_fps;

			void InvertSurface(SDL_Surface& surface)
			{
				int pitch = surface.pitch;
				int height = surface.h;
				void* imagePixels = surface.pixels;

				int index;
				void* temp_row;
				int height_div_2;

				temp_row = (void*)malloc(pitch);
				SDL_assert_release(temp_row && "Out of memory.");

				// If height is odd, no need to swap middle row
				height_div_2 = (int)(height * .5);
				for (index = 0; index < height_div_2; index++)
				{
					//uses string.h
					memcpy((Uint8*)temp_row,
						(Uint8*)(imagePixels)+
						pitch * index,
						pitch);
					memcpy(
						(Uint8*)(imagePixels)+
						pitch * index,
						(Uint8*)(imagePixels)+
						pitch * (height - index - 1),
						pitch);
					memcpy(
						(Uint8*)(imagePixels)+
						pitch * (height - index - 1),
						temp_row,
						pitch);
				}
				free(temp_row);
			}
			void GenerateGLTexture(const std::string& filename, GLuint& texID, float& widthToHeightRatio)
			{
				// Load surface from texture file
				SDL_Surface* surface{ IMG_Load(filename.c_str()) };
				if (!surface)
					throw InitException{ "SDL_image failed to load file " + filename };

				if (surface->h == 0)
					widthToHeightRatio = 1.0f;
				else
					widthToHeightRatio = (float)surface->w / (float)surface->h;

				// Flip it so it complies with opengl's lower-left origin
				InvertSurface(*surface);

				// Does it use the alpha channel?
				int colorMode = GL_RGB;
				if (surface->format->BytesPerPixel == 4)
					colorMode = GL_RGBA;

				// Create the OpenGL texture
				EnableTextures();
				EnableBlending();

				glGenTextures(1, &texID);
				glBindTexture(GL_TEXTURE_2D, texID);
				glTexImage2D(GL_TEXTURE_2D, 0, colorMode, surface->w, surface->h, 0,
					colorMode, GL_UNSIGNED_BYTE, surface->pixels);

				// Texture filter
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_windowDef.gl.texture2DMagFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_windowDef.gl.texture2DMinFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_windowDef.gl.textureWrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_windowDef.gl.textureWrapT);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_windowDef.gl.textureEnvMode);

				// We don't need the surface anymore
				SDL_FreeSurface(surface);
			}

			// Resources
			class TextureResource : public Resource
			{
			public:
				TextureResource(const std::string& filename)
				{
					GenerateGLTexture(filename, m_glTextureID, m_widthToHeightRatio);
				}
				~TextureResource()
				{
					glDeleteTextures(1, &m_glTextureID);
				}
				GLuint GetGLTextureID() const
				{
					return m_glTextureID;
				}
				float GetWidthToHeightRatio() const
				{
					return m_widthToHeightRatio;
				}

			private:
				GLuint m_glTextureID{ 0 };
				float m_widthToHeightRatio{ 1.0f };
			};
			class FontResource : public Resource
			{
			public:
				static const int dtxFontSize{ 192 };

			public:
				FontResource(const std::string& filename)
					: m_dtxFontPtr{ dtx_open_font(filename.c_str(), dtxFontSize) }
				{
					if (!m_dtxFontPtr)
						throw InitException{ "Failed to open font: " + filename };
				}
				~FontResource()
				{
					if (m_dtxFontPtr)
						dtx_close_font(m_dtxFontPtr);
				}
				dtx_font* GetDTXFontPtr() const
				{
					return m_dtxFontPtr;
				}

			private:
				struct dtx_font* m_dtxFontPtr{};
			};
		}

		//+----------------------\------------------------------------
		//|	Resource References  |
		//\----------------------/------------------------------------
		TextureReference::TextureReference(const std::string& filename)
			: ResourceReference(filename, m_textureManager.Load(filename))
		{
			d2LogDebug << "TextureReference::Load handle: " << GetID();
			m_widthToHeightRatio = m_textureManager.GetResource(GetID()).GetWidthToHeightRatio();
		}
		TextureReference::TextureReference(const char* filename)
			: TextureReference((std::string)filename)
		{}
		TextureReference::~TextureReference()
		{
			m_textureManager.Unload(GetID());
		}
		float TextureReference::GetWidthToHeightRatio() const
		{
			return m_widthToHeightRatio;
		}

		FontReference::FontReference(const std::string& filename)
			: ResourceReference(filename, m_fontManager.Load(filename))
		{
			d2LogDebug << "FontReference::Load handle: " << GetID();
		}
		FontReference::FontReference(const char* filename)
			: FontReference((std::string)filename)
		{}
		FontReference::~FontReference()
		{
			m_fontManager.Unload(GetID());
		}

		//+----------------------\------------------------------------
		//|	Startup and shutdown |
		//\----------------------/------------------------------------
		void Init(const WindowDef& windowDef)
		{
			// Save settings
			m_windowDef = windowDef;

			// Make sure SDL video subsystem is initialized
			if(!SDL_WasInit(SDL_INIT_VIDEO))
				throw InitException{ "Tried to create window before SDL was initialized." };

			// Get rid of previous window, if it exists.
			Close();

			// SDL_image
			m_sdlImageInitialized = IMG_Init(windowDef.imageExtensions) & windowDef.imageExtensions;
			SDL_assert_release(m_sdlImageInitialized);

			// OpenGL settings
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, m_windowDef.gl.profileMask);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_windowDef.gl.versionMajor);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_windowDef.gl.versionMinor);
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, m_windowDef.colorChannelBits[0]);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, m_windowDef.colorChannelBits[1]);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, m_windowDef.colorChannelBits[2]);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, m_windowDef.colorChannelBits[3]);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_windowDef.doubleBuffer);

			int samples{ d2d::GetClamped(m_windowDef.antiAliasingSamples, VALID_SAMPLE_RANGE) };
			if(samples >= WINDOW_MIN_SAMPLES_TO_ENABLE_ANTI_ALIASING)
			{
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
			}
			else
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			
			// Create window
			Uint32 windowFlags{ SDL_WINDOW_OPENGL };
			int width{ m_windowDef.size.at(0) }, height{ m_windowDef.size.at(1) };
			int x{ m_windowDef.position.at(0) }, y{ m_windowDef.position.at(1) };
			if(m_windowDef.fullScreen)
			{
				if(width < 1 || height < 1)
				{
					x = y = width = height = 0;
					windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				}
				else
					windowFlags |= SDL_WINDOW_FULLSCREEN;
			}
			
			m_windowPtr = SDL_CreateWindow(m_windowDef.title.c_str(), x, y, width, height, windowFlags);
			if(!m_windowPtr)
				throw InitException{ "SDL_CreateWindow failed." };
			
			// Create OpenGL context
			if(!(m_glContext = SDL_GL_CreateContext(m_windowPtr)))
				throw InitException{ "SDL_GL_CreateContext failed." };

			// Set vsync
			//SDL_GL_SetSwapInterval(m_windowDef.vsync ? -1 : 0); // -1 allows late swaps
			SDL_GL_SetSwapInterval(m_windowDef.vsync ? 1 : 0);

			EnableTextures();
			EnableBlending();

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// OpenGL settings
			glClearColor(m_windowDef.clearColor.red, m_windowDef.clearColor.green, m_windowDef.clearColor.blue, m_windowDef.clearColor.alpha);
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
			if(m_sdlImageInitialized)
			{
				IMG_Quit();
				m_sdlImageInitialized = false;
			}

			// If there is a window, get rid of it
			if(m_windowPtr)
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
			if(height == 0)
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
		//+-------------\---------------------------------------------
		//|	Modifiers	|
		//\-------------/---------------------------------------------
		void SetShowCursor(bool enabled)
		{
			SDL_ShowCursor(enabled);
		}
		void SetFPSInterval(float interval)
		{
			if(interval < 0.0f)
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
		void SetColor(const d2d::Color& newColor)
		{
			glColor4f(newColor.red, newColor.green, newColor.blue, newColor.alpha);
		}
		void SetColor(float red, float green, float blue, float alpha)
		{
			glColor4f(red, green, blue, alpha);
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
			if(!m_texturesEnabled)
			{
				glEnable(GL_TEXTURE_2D);
				m_texturesEnabled = true;
			}
		}
		void DisableTextures()
		{
			if(m_texturesEnabled)
			{
				glDisable(GL_TEXTURE_2D);
				m_texturesEnabled = false;
			}
		}
		void EnableBlending()
		{
			if(!m_blendingEnabled)
			{
				// Enable blending
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				m_blendingEnabled = true;
			}
		}
		void DisableBlending()
		{
			if(m_blendingEnabled)
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
			if(m_fpsUpdateAccumulator > 0.0f &&
				m_fpsUpdateAccumulator >= m_fpsUpdateInterval)
			{
				m_fps = m_frames / m_fpsUpdateAccumulator;
				m_frames = 0;
				m_fpsUpdateAccumulator = 0.0f;
			}

			// Swap buffers
			if(m_windowPtr)
				SDL_GL_SwapWindow(m_windowPtr);
		}
		void PushMatrix()
		{
			// Save transformation
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
		}
		void Translate(const b2Vec2& position)
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
		void DrawPoint(const b2Vec2& position)
		{
			glBegin(GL_POINTS);
			glVertex2f(position.x, position.y);
			glEnd();
		}
		void DrawCircle(const b2Vec2& center, float radius, bool fill)
		{
			PushMatrix();
			Translate(center);

			// Triangle fan for filled mode, line loop for outline mode
			glBegin(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);

			// Triangle fans need to start at the center
			if(fill)	
				glVertex2f(0.0f, 0.0f);
				
			// Make sure radius is positive
			radius = abs(radius);

			// Number of vertices is proportional to how big it is
			unsigned numVertices{ (unsigned)(16.0f + radius * 2.0f) };

			// Angle between adjacent vertex pairs
			float radiansPerVertex{ TWO_PI / (float)numVertices };

			// Specify vertices
			for(unsigned i = 0; i < numVertices; ++i)
			{
				float theta{ i * radiansPerVertex };
				glVertex2f(cosf(theta) * radius, sinf(theta) * radius);
			}

			// Only the triangle fan needs to close the loop
			if(fill)	
				glVertex2f(radius, 0.0f);
			glEnd();
			PopMatrix();
		}
		void DrawPolygon(const b2Vec2* vertices, unsigned vertexCount, bool fill)
		{
			if(!vertices || !vertexCount)
				return;

			// Polygon for filled mode, line loop for outline mode
			glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);
			for(unsigned i = 0; i < vertexCount; ++i)
				glVertex2f(vertices[i].x, vertices[i].y);
			glEnd();
		}
		void DrawRect(const b2Vec2& lowerBound, const b2Vec2& upperBound, bool fill)
		{
			// Quad for filled mode, line loop for outline mode
			glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
			glVertex2f(lowerBound.x, lowerBound.y);
			glVertex2f(upperBound.x, lowerBound.y);
			glVertex2f(upperBound.x, upperBound.y);
			glVertex2f(lowerBound.x, upperBound.y);
			glEnd();
		}
		void DrawLine(const b2Vec2& p1, const b2Vec2& p2)
		{
			glBegin(GL_LINES);
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
			glEnd();
		}
		void DrawLineStrip(const b2Vec2* vertices, unsigned vertexCount)
		{
			if(!vertices || !vertexCount)
				return;

			glBegin(GL_LINE_STRIP);
			for(unsigned i = 0; i < vertexCount; ++i)
				glVertex2f(vertices[i].x, vertices[i].y);
			glEnd();
		}
		void DrawTexture(unsigned textureID, const b2Vec2& size)
		{
			b2Vec2 halfSize{ 0.5f * size };
			DrawTextureInRect(textureID, { -halfSize.x, -halfSize.y }, halfSize);
		}
		void DrawTextureInRect(unsigned textureID, const b2Vec2& lowerBound, const b2Vec2& upperBound)
		{
			// Bind texture
			if(!m_textureBinded || m_bindedTextureID != textureID)
			{
				glBindTexture(GL_TEXTURE_2D, m_textureManager.GetResource(textureID).GetGLTextureID());
				m_bindedTextureID = textureID;
			}

			// Draw a textured quad
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(lowerBound.x, lowerBound.y);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(upperBound.x, lowerBound.y);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(upperBound.x, upperBound.y);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(lowerBound.x, upperBound.y);
			glEnd();
		}
		void DrawString(const std::string &text, Alignment alignment,
			float fontSize, unsigned fontID)
		{
			if(!m_fontBinded || m_bindedFontID != fontID)
			{
				// Find the loaded font with matching id, and enable it.
				const FontResource& font{ m_fontManager.GetResource(fontID) };
				m_fontBinded = true;
				m_bindedDTXFontSize = FontResource::dtxFontSize;
				m_bindedFontID = fontID;

				dtx_use_font(font.GetDTXFontPtr(), m_bindedDTXFontSize);
			}
			if(m_bindedDTXFontSize < 0)
				return;

			float scale{ fontSize / (float)m_bindedDTXFontSize };
			glScalef(scale, scale, 1.0f);

			// Alignment: dtx string alignment is left bottom of the first character of the first line,
			//	so we have to translate to that point.
			float fontHeight;
			{
				float lineHeight{ dtx_line_height() };
				float fontHeightToLineHeightRatio{ 0.6f };
				fontHeight = fontHeightToLineHeightRatio * lineHeight;
			}
			float fontPadding{ 0.0f };
			struct dtx_box stringDimensions;
			dtx_string_box(text.c_str(), &stringDimensions);
			b2Vec2 translation;
			switch(alignment)
			{
			case Alignment::LEFT_TOP:
				translation.x = fontPadding;
				translation.y = -(fontHeight + fontPadding);
				break;
			case Alignment::CENTER_TOP:
				translation.x = -0.5f * stringDimensions.width;
				translation.y = -(fontHeight + fontPadding);
				break;
			case Alignment::RIGHT_TOP:
				translation.x = -(stringDimensions.width + fontPadding);
				translation.y = -(fontHeight + fontPadding);
				break;
			case Alignment::LEFT_CENTER:
				translation.x = fontPadding;
				translation.y = 0.5f * stringDimensions.height - fontHeight;
				break;
			case Alignment::CENTERED:
				translation.x = -0.5f * stringDimensions.width;
				translation.y = 0.5f * stringDimensions.height - fontHeight;
				break;
			case Alignment::RIGHT_CENTER:
				translation.x = -(stringDimensions.width + fontPadding);
				translation.y = 0.5f * stringDimensions.height - fontHeight;
				break;
			case Alignment::LEFT_BOTTOM:  // dtx origin 
			default:
				translation.x = fontPadding;
				translation.y = stringDimensions.height + fontPadding - fontHeight;
				break;
			case Alignment::CENTER_BOTTOM:
				translation.x = -0.5f * stringDimensions.width;
				translation.y = (stringDimensions.height + fontPadding) - fontHeight;
				break;
			case Alignment::RIGHT_BOTTOM:
				translation.x = -(stringDimensions.width + fontPadding);
				translation.y = stringDimensions.height + fontPadding - fontHeight;
				break;
			}
			
			// Draw string
			PushMatrix();
			Translate(translation);
			dtx_string(text.c_str());
			PopMatrix();
		}
		/*void LoadTextureList(const std::vector<std::string>& filenames, std::vector<unsigned>& textureIDs)
		{
			textureIDs.clear();
			for(auto textureFilename : filenames)
			{
				unsigned textureID{};
				LoadTexture(textureFilename, textureID);
				textureIDs.push_back(textureID);
			}
		}
		void LoadFontList(const std::vector<std::string>& filenames, std::vector<unsigned>& fontIDs)
		{
			fontIDs.clear();
			for(auto fontFilename : filenames)
			{
				unsigned fontID{};
				LoadFont(fontFilename, fontID);
				fontIDs.push_back(fontID);
			}
		}*/
		/*void UnloadTextureList(std::vector<unsigned>& m_textureIDList)
		{
			for(auto textureID : m_textureIDList)
			{
				UnloadTexture(textureID);
			}
			m_textureIDList.clear();
		}
		void UnloadFontList(std::vector<unsigned>& m_fontIDList)
		{
			for(auto fontID : m_fontIDList)
			{
				UnloadFont(fontID);
			}
			m_fontIDList.clear();
		}*/
	}
}