/**************************************************************************************\
** File: d2Window.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Window class
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Window.h"
#include "d2Utility.h"
#include "d2Resource.h"
#include "d2Timer.h"
#include "d2NumberManip.h"
#include "d2StringManip.h"
#include "d2Rect.h"
namespace d2d
{
	//+--------------------------------\--------------------------------------
	//|			 Animation	    	   |
	//\--------------------------------/--------------------------------------
	AnimationFrame::AnimationFrame(const d2d::TextureReference& texture,
		float frameTime, const d2d::Color& tintColor,
		const b2Vec2& relativeSize, const b2Vec2& relativePosition, float relativeAngle)
		: m_texturePtr{ &texture },
		m_frameTime{ frameTime },
		m_tintColor{ tintColor },
		m_relativeSize{ relativeSize },
		m_relativePosition{ relativePosition },
		m_relativeAngle{ relativeAngle }
	{

	}
	void AnimationFrame::Draw(const b2Vec2& animationSize, const d2d::Color& animationColor) const
	{
		d2d::Window::PushMatrix();
		d2d::Window::Translate(m_relativePosition);
		d2d::Window::Rotate(m_relativeAngle);
		d2d::Window::SetColor(animationColor * m_tintColor);
		m_texturePtr->Draw(animationSize * m_relativeSize);
		d2d::Window::PopMatrix();
	}
	float AnimationFrame::GetFrameTime() const
	{
		return m_frameTime;
	}
	AnimationDef::AnimationDef()
	{
		m_frameList.clear();
	}
	AnimationDef::AnimationDef(const AnimationFrame& frame)
		: m_type{ AnimationType::STATIC },
		m_firstFrame{ 0u }
	{
		m_frameList.push_back(frame);
	}
	AnimationDef::AnimationDef(const std::vector<AnimationFrame>& frameList,
		AnimationType type, unsigned firstFrame, bool startForward)
		: m_frameList{ frameList },
		m_type{ type },
		m_firstFrame{ firstFrame },
		m_startForward{ startForward }
	{
		d2Assert(m_firstFrame < m_frameList.size());
	}
	void Animation::Init(AnimationDef const* animationDefPtr,
		const b2Vec2& relativeSize, const b2Vec2& relativePosition, 
		float relativeAngle, const d2d::Color& tintColor)
	{
		if(!animationDefPtr) 
		{
			m_enabled = false;
			return;
		}
		m_def = *animationDefPtr;
		m_enabled = true;
		m_currentFrame = m_def.m_firstFrame;
		m_forward = m_def.m_startForward;
		m_frameTimeAccumulator = 0.0f;
		m_flipX = false;
		m_flipY = false;
		m_relativeSize = relativeSize;
		m_relativePosition = relativePosition;
		m_relativeAngle = relativeAngle;
		m_tintColor = tintColor;
	}
	void Animation::FlipX()
	{
		m_flipX = !m_flipX;
	}
	void Animation::FlipY()
	{
		m_flipY = !m_flipY;
	}
	void Animation::Update(float dt)
	{
		if (IsEnabled() && 
			m_def.m_type != AnimationType::STATIC &&
			m_currentFrame < m_def.m_frameList.size())
		{
			m_frameTimeAccumulator += dt;
			if(m_frameTimeAccumulator >= m_def.m_frameList[m_currentFrame].GetFrameTime())
			{
				// Go to next frame
				m_frameTimeAccumulator -= m_def.m_frameList[m_currentFrame].GetFrameTime();
				bool reachedEnd{ m_forward && (m_currentFrame == m_def.m_frameList.size() - 1) };
				bool reachedBeginning{ !m_forward && (m_currentFrame == 0) };
				bool stillNeedsToChangeFrame{ true };
				switch(m_def.m_type)
				{
				case AnimationType::SINGLE_PASS:
					if(reachedEnd || reachedBeginning)
					{
						m_enabled = true;
						return;
					}
					break;
				case AnimationType::LOOP:
					if(reachedEnd || reachedBeginning)
						stillNeedsToChangeFrame = false;
					if(reachedEnd)
						m_currentFrame = 0;
					else if(reachedBeginning)
						m_currentFrame = m_def.m_frameList.size() - 1;
					break;
				case AnimationType::PENDULUM:
				default:
					if(reachedEnd)
						m_forward = false;
					else if(reachedBeginning)
						m_forward = true;
					break;
				}

				if(stillNeedsToChangeFrame)
				{
					// Go to next frame
					if(m_forward)
						++m_currentFrame;
					else
						--m_currentFrame;
				}
			}
		}
	}
	void Animation::Draw(const b2Vec2& entitySize) const
	{
		if (IsEnabled() &&
			m_currentFrame < m_def.m_frameList.size())
		{
			d2d::Window::PushMatrix();
			d2d::Window::Translate(m_relativePosition);
			d2d::Window::Rotate(m_relativeAngle);
			m_def.m_frameList[m_currentFrame].Draw(entitySize * m_relativeSize, m_tintColor);
			d2d::Window::PopMatrix();
		}
	}
	bool Animation::IsEnabled() const
	{
		return m_enabled;
	}
	bool Animation::IsAnimated() const
	{
		return m_def.m_type != AnimationType::STATIC;
	}
	void Animation::Restart()
	{
		m_enabled = true;
		m_currentFrame = m_def.m_firstFrame;
		m_forward = m_def.m_startForward;
		m_frameTimeAccumulator = 0.0f;
	}
	void Animation::SetTint(const d2d::Color& newTintColor) {
		m_tintColor = newTintColor;
	}
	namespace
	{
		class SpriteResource;
		class SpriteAtlasResource;
		class FontResource;

		WindowDef m_windowDef;
		bool m_sdlImageInitialized{ false };
		SDL_Window* m_windowPtr{ nullptr };
		SDL_GLContext m_glContext;
		ResourceManager<SpriteResource> m_spriteManager;
		ResourceManager<SpriteAtlasResource> m_spriteAtlasManager;
		ResourceManager<FontResource> m_fontManager;
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

		//+----------------------\------------------------------------
		//|	   InvertSurface	 |
		//\----------------------/------------------------------------
		//	Flip sprite so it complies with opengl's lower-left origin.
		//	Called by GenerateGLTexture.
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

		//+----------------------\------------------------------------
		//|	  GenerateGLTexture  |
		//\----------------------/------------------------------------
		//	Load sprite data from file, register it with OpenGL, and
		//	save its ID and aspect ratio in the output parameters.
		void GenerateGLTexture(const std::string & filename, GLuint & texID, float& widthToHeightRatio)
		{
			// Load surface from sprite file
			SDL_Surface* surface{ IMG_Load(filename.c_str()) };
			if (!surface)
				throw InitException{ "SDL_image failed to load file "s + filename };

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

			// Create the OpenGL sprite
			Window::EnableTextures();
			Window::EnableBlending();

			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, colorMode, surface->w, surface->h, 0,
				colorMode, GL_UNSIGNED_BYTE, surface->pixels);

			// Sprite filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_windowDef.gl.texture2DMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_windowDef.gl.texture2DMinFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_windowDef.gl.textureWrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_windowDef.gl.textureWrapT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_windowDef.gl.textureEnvMode);

			// We don't need the surface anymore
			SDL_FreeSurface(surface);
		}

		//+----------------------\------------------------------------
		//|		Resources		 |
		//\----------------------/------------------------------------
		class SpriteResource : public Resource
		{
		public:
			// filePaths[0]: path of the image file
			explicit SpriteResource(const std::vector<std::string>& filePaths)
				: Resource(filePaths)
			{
				if (filePaths.size() < 1)
					throw InitException{ "SpriteResource requires one filePath"s };

				// Load sprite into OpenGL
				GenerateGLTexture(filePaths[0], m_glTextureID, m_pixelWidthToHeightRatio);
			}
			~SpriteResource()
			{
				// Unload sprite from OpenGL
				glDeleteTextures(1, &m_glTextureID);
			}
			GLuint GetGLTextureID() const
			{
				return m_glTextureID;
			}
			float GetPixelWidthToHeightRatio() const
			{
				return m_pixelWidthToHeightRatio;
			}
		private:
			GLuint m_glTextureID;
			float m_pixelWidthToHeightRatio;
		};

		class SpriteAtlasResource : public SpriteResource
		{
		public:
			// filePaths[0]: path of the image file
			// filePaths[1]: path of the xml file with atlas data
			explicit SpriteAtlasResource(const std::vector<std::string>& filePaths)
				: SpriteResource(filePaths)
			{
				if (filePaths.size() < 2)
					throw InitException{ "SpriteAtlasResource requires two filePaths"s };

				// Load atlas data
				boost::property_tree::ptree data;
				boost::property_tree::read_xml(filePaths[1], data);

				int atlasWidth{ data.get<int>("TextureAtlas.<xmlattr>.width") };
				int atlasHeight{ data.get<int>("TextureAtlas.<xmlattr>.height") };
				SDL_assert_release(atlasWidth > 0);
				SDL_assert_release(atlasHeight > 0);

				for (auto const& spriteNode : data.get_child("TextureAtlas"))
				{
					if (spriteNode.first == "sprite")
					{
						// name
						std::string spriteName{ spriteNode.second.get<std::string>("<xmlattr>.name") };
						SDL_assert_release(spriteName.length() > 0);

						int x{ spriteNode.second.get<int>("<xmlattr>.x") };
						int y{ spriteNode.second.get<int>("<xmlattr>.y") };
						SDL_assert_release(x >= 0);
						SDL_assert_release(x < atlasWidth);
						SDL_assert_release(y >= 0);
						SDL_assert_release(y < atlasHeight);

						int w{ spriteNode.second.get<int>("<xmlattr>.w") };
						int h{ spriteNode.second.get<int>("<xmlattr>.h") };
						SDL_assert_release(w > 0);
						SDL_assert_release(w <= atlasWidth);
						SDL_assert_release(h > 0);
						SDL_assert_release(h <= atlasHeight);

						int x2{ x + w };
						int y2{ y + h };
						SDL_assert_release(x2 <= atlasWidth);
						SDL_assert_release(y2 <= atlasHeight);

						b2Vec2 lowerTextureCoordinate{ (float)x / (float)atlasWidth, (float)y / (float)atlasHeight };
						b2Vec2 upperTextureCoordinate{ (float)x2 / (float)atlasWidth, (float)y2 / (float)atlasHeight };
						m_textureCoordinatesMap[spriteName].SetBounds(lowerTextureCoordinate, upperTextureCoordinate);
						m_pixelWidthToHeightRatioMap[spriteName] = (float)w / (float)h;
					}
				}
			}
			const d2d::Rect & GetTextureCoordinates(const std::string & spriteName) const
			{
				return m_textureCoordinatesMap.at(spriteName);
			}
			float GetPixelWidthToHeightRatio(const std::string & spriteName) const
			{
				return m_pixelWidthToHeightRatioMap.at(spriteName);
			}

		private:
			std::unordered_map<std::string, d2d::Rect> m_textureCoordinatesMap;
			std::unordered_map<std::string, float> m_pixelWidthToHeightRatioMap;
		};

		class FontResource : public Resource
		{
		public:
			static const int dtxFontSize{ 192 };

		public:
			// filePaths[0]: path of the font file
			explicit FontResource(const std::vector<std::string>& filePaths)
				: Resource(filePaths)
			{
				if (filePaths.size() < 1)
					throw InitException{ "FontResource requires one filePath"s };

				m_dtxFontPtr = dtx_open_font(filePaths[0].c_str(), dtxFontSize);
				if (!m_dtxFontPtr)
					throw InitException{ "Failed to open font: "s + filePaths[0] };
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
			struct dtx_font* m_dtxFontPtr{ nullptr };
		};
	}

	//+----------------------\------------------------------------
	//|	 Resource References |
	//\----------------------/------------------------------------
	TextureAtlasReference::TextureAtlasReference(const std::string & imagePath, const std::string & atlasXMLPath)
		: ResourceReference(m_spriteAtlasManager.Load({ imagePath, atlasXMLPath }))
	{ }
	TextureAtlasReference::~TextureAtlasReference()
	{
		m_spriteAtlasManager.Unload(GetID());
	}

	TextureReference::TextureReference(const std::string & imagePath)
		: ResourceReference(m_spriteManager.Load({ imagePath }))
	{
		m_referencesAtlas = false;
		m_textureCoordinates.SetBounds(b2Vec2_zero, { 1.0f, 1.0f });
		m_widthToHeightRatio = m_spriteManager.GetResource(GetID()).GetPixelWidthToHeightRatio();
	}
	TextureReference::TextureReference(unsigned textureAtlasID, const std::string & name)
		: ResourceReference(textureAtlasID)
	{
		m_referencesAtlas = true;
		m_textureCoordinates = m_spriteAtlasManager.GetResource(GetID()).GetTextureCoordinates(name);
		m_widthToHeightRatio = m_spriteAtlasManager.GetResource(GetID()).GetPixelWidthToHeightRatio(name);
	}
	TextureReference::~TextureReference()
	{
		if (m_referencesAtlas)
			m_spriteAtlasManager.Unload(GetID());
		else
			m_spriteManager.Unload(GetID());
	}
	float TextureReference::GetWidthToHeightRatio() const
	{
		return m_widthToHeightRatio;
	}
	GLuint TextureReference::GetGLTextureID() const
	{
		if (m_referencesAtlas)
			return m_spriteAtlasManager.GetResource(GetID()).GetGLTextureID();
		else
			return m_spriteManager.GetResource(GetID()).GetGLTextureID();
	}
	const Rect& TextureReference::GetTextureCoordinates() const
	{
		return m_textureCoordinates;
	}
	void TextureReference::Draw(const b2Vec2& size) const
	{
		Rect drawRect;
		drawRect.SetCenter(b2Vec2_zero, size);
		DrawInRect(drawRect);
	}
	void TextureReference::DrawInRect(const Rect& drawRect) const
	{
		// Bind sprite
		GLuint glTextureID{ GetGLTextureID() };
		if (!m_textureBinded || m_boundGLTextureID != glTextureID)
		{
			glBindTexture(GL_TEXTURE_2D, glTextureID);
			m_boundGLTextureID = glTextureID;
		}

		const Rect& texCoords{ GetTextureCoordinates() };
		glBegin(GL_QUADS);
		glTexCoord2f(texCoords.lowerBound.x, texCoords.lowerBound.y);
		glVertex2f(drawRect.lowerBound.x, drawRect.lowerBound.y);
		glTexCoord2f(texCoords.upperBound.x, texCoords.lowerBound.y);
		glVertex2f(drawRect.upperBound.x, drawRect.lowerBound.y);
		glTexCoord2f(texCoords.upperBound.x, texCoords.upperBound.y);
		glVertex2f(drawRect.upperBound.x, drawRect.upperBound.y);
		glTexCoord2f(texCoords.lowerBound.x, texCoords.upperBound.y);
		glVertex2f(drawRect.lowerBound.x, drawRect.upperBound.y);
		glEnd();
	}

	FontReference::FontReference(const std::string & fontPath)
		: ResourceReference(m_fontManager.Load({ fontPath }))
	{ }
	FontReference::~FontReference()
	{
		m_fontManager.Unload(GetID());
	}

	namespace Window
	{
		//+----------------------\------------------------------------
		//|	Startup and shutdown |
		//\----------------------/------------------------------------
		void Init(const WindowDef& windowDef)
		{
			// Save settings
			m_windowDef = windowDef;

			// Make sure SDL video subsystem is initialized
			if(!SDL_WasInit(SDL_INIT_VIDEO))
				throw InitException{ "Failed to initialize window: SDL video system not yet initialized" };

			// Get rid of previous window, if it exists.
			Close();

			// SDL_image
			{
				IMG_SetError("");
				int imageExtensionsLoaded = IMG_Init(windowDef.imageExtensions);
				if((imageExtensionsLoaded & windowDef.imageExtensions) != windowDef.imageExtensions)
					throw InitException{ std::string{"Failed to load all requested imageExtensions: "} +IMG_GetError() };
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
				int width = m_windowDef.size.at(0);
				int height = m_windowDef.size.at(1);
				int x = m_windowDef.position.at(0);
				int y = m_windowDef.position.at(1);
				if(m_windowDef.fullScreen)
					windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				else
					windowFlags |= SDL_WINDOW_RESIZABLE;

				SDL_SetError("");
				m_windowPtr = SDL_CreateWindow(m_windowDef.title.c_str(), x, y, width, height, windowFlags);
				if(!m_windowPtr)
					throw InitException{ std::string{"SDL_CreateWindow failed: "} +SDL_GetError() };
			}

			// Create OpenGL context
			SDL_SetError("");
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
			if (m_sdlImageInitialized)
			{
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
		//+-------------\---------------------------------------------
		//|	Modifiers	|
		//\-------------/---------------------------------------------
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
		void SetCameraRect(const Rect & rect)
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
		void DrawString(const std::string& text, Alignment alignment, float size, const FontReference& font)
		{
			if (!m_fontBinded || m_boundFontID != font.GetID())
			{
				// Find the loaded font with matching id, and enable it.
				m_fontBinded = true;
				//m_bindedDTXFontSize = FontResource::dtxFontSize;
				m_boundFontID = font.GetID();

				//dtx_use_font(m_fontManager.GetResource(fontID).GetDTXFontPtr(), m_bindedDTXFontSize);
				dtx_use_font(m_fontManager.GetResource(font.GetID()).GetDTXFontPtr(), FontResource::dtxFontSize);
			}
			//if (m_bindedDTXFontSize < 0)
			//	return;



			// ***********************************************************************************************************
			// TODO: THIS IS HACKED TOGETHER... MAKE A PROPER TEXT SIZING MECHANISM, OR SWITCH TO ANOTHER GL TEXT LIB
			// ***********************************************************************************************************
			//float scale{ fontSize / (float)m_bindedDTXFontSize };
			//b2Vec2 resolution{ GetScreenResolution() };
			//size *= 0.5f * (resolution.x + resolution.y);
			float scale{ size / (float)FontResource::dtxFontSize };
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
			switch (alignment)
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
			Window::PushMatrix();
			Window::Translate(translation);
			dtx_string(text.c_str());
			Window::PopMatrix();
		}
		void DrawSprite(ResourceID spriteID, const b2Vec2& size)
		{
			Rect drawRect;
			drawRect.SetCenter(b2Vec2_zero, size);
			DrawSpriteInRect(spriteID, drawRect);
		}
		void DrawSpriteInRect(ResourceID spriteID, const Rect& drawRect)
		{
			// Get resource
			const d2d::SpriteResource& spriteResource = m_spriteManager.GetResource(spriteID);

			// Bind sprite
			GLuint glTextureID{ spriteResource.GetGLTextureID()};
			
			if (!m_textureBinded || m_boundGLTextureID != glTextureID)
			{
				glBindTexture(GL_TEXTURE_2D, glTextureID);
				m_boundGLTextureID = glTextureID;
			}

			//const Rect& texCoords{ spriteResource.GetTextureCoordinates() };
			const Rect& texCoords{ b2Vec2_zero, b2Vec2{1.0f, 1.0f} };
			glBegin(GL_QUADS);
			glTexCoord2f(texCoords.lowerBound.x, texCoords.lowerBound.y);
			glVertex2f(drawRect.lowerBound.x, drawRect.lowerBound.y);
			glTexCoord2f(texCoords.upperBound.x, texCoords.lowerBound.y);
			glVertex2f(drawRect.upperBound.x, drawRect.lowerBound.y);
			glTexCoord2f(texCoords.upperBound.x, texCoords.upperBound.y);
			glVertex2f(drawRect.upperBound.x, drawRect.upperBound.y);
			glTexCoord2f(texCoords.lowerBound.x, texCoords.upperBound.y);
			glVertex2f(drawRect.lowerBound.x, drawRect.upperBound.y);
			glEnd();
		}
	}
}