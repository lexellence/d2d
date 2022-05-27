/**************************************************************************************\
** File: d2Window.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Window class
**
\**************************************************************************************/
#pragma once
#include "d2Resource.h"
#include "d2Color.h"
#include "d2Rect.h"
namespace d2d
{
	//+------------------\----------------------------------------
	//|	   Textures 	 |
	//\------------------/----------------------------------------
	class TextureAtlasReference : public ResourceReference
	{
	public:
		TextureAtlasReference(const std::string& imagePath, const std::string& atlasXMLPath);
		virtual ~TextureAtlasReference();
	};
	class TextureReference : public ResourceReference
	{
	public:
		TextureReference(const std::string& imagePath);
		TextureReference(unsigned textureAtlasID, const std::string& name);
		virtual ~TextureReference();
		float GetWidthToHeightRatio() const;
		void Draw(const b2Vec2& size) const;
		void DrawInRect(const Rect& drawRect) const;

	private:
		bool m_referencesAtlas;
		Rect m_textureCoordinates;
		float m_widthToHeightRatio;

		GLuint GetGLTextureID() const;
		const Rect& GetTextureCoordinates() const;
	};

	//+------------------\----------------------------------------
	//|	     Text 		 |
	//\------------------/----------------------------------------
	class FontReference : public ResourceReference
	{
	public:
		FontReference(const std::string& fontPath);
		virtual ~FontReference();
	};
	struct TextStyle
	{
		FontReference& font;
		Color color;
		float size;
	};

	//+------------------\----------------------------------------
	//|	   Animations	 |
	//\------------------/----------------------------------------
	class AnimationFrame
	{
	public:
		AnimationFrame(const d2d::TextureReference* textureRefPtr = nullptr, 
			float frameTime = 0.0f, const d2d::Color& tintColor = d2d::WHITE_OPAQUE, 
			const b2Vec2& relativeSize = { 1.0f, 1.0f },
			const b2Vec2& relativePosition = b2Vec2_zero, float relativeAngle = 0.0f);
		void Draw(const b2Vec2& animationSize, const d2d::Color& animationColor) const;
		float GetFrameTime() const;

	private:
		const d2d::TextureReference* m_textureRefPtr{ nullptr };
		float m_frameTime;
		d2d::Color m_tintColor;
		b2Vec2 m_relativeSize;
		b2Vec2 m_relativePosition;
		float m_relativeAngle;
	};
	enum class AnimationType
	{
		STATIC, SINGLE_PASS, LOOP, PENDULUM
	};
	const unsigned ANIMATION_MAX_FRAMES{ 10u };
	class AnimationDef
	{
	public:
		AnimationDef() = delete;
		AnimationDef(const AnimationFrame& frame);
		AnimationDef(const std::vector<AnimationFrame>& frameList,
			AnimationType type = AnimationType::STATIC,
			unsigned firstFrame = 0u, bool forward = true);
		friend class Animation;

	private:
		std::array<AnimationFrame, ANIMATION_MAX_FRAMES> m_frameList;
		unsigned m_numFrames;
		AnimationType m_type;
		unsigned m_firstFrame{ 0u };
		bool m_forward{ true };
	};
	class Animation
	{
	public:
		void Init(const AnimationDef* animationDefPtr, 
			const b2Vec2& relativeSize = { 1.0f, 1.0f },
			const b2Vec2& relativePosition = b2Vec2_zero, float relativeAngle = 0.0f,
			const d2d::Color& tint = d2d::WHITE_OPAQUE);
		void SetFlip(bool flipX, bool flipY);
		void Update(float dt);
		void Draw(const b2Vec2& entitySize) const;
		bool IsFinished() const;
		void Restart();

	private:
		std::array<AnimationFrame, ANIMATION_MAX_FRAMES> m_frameList;
		unsigned m_numFrames;
		AnimationType m_type;
		bool m_finished;
		unsigned m_currentFrame;
		unsigned m_firstFrame;
		bool m_forward;
		bool m_startForward;
		float m_frameTimeAccumulator;
		bool m_flipX;
		bool m_flipY;
		b2Vec2 m_relativeSize;
		b2Vec2 m_relativePosition;
		float m_relativeAngle;
		d2d::Color m_tintColor;
	};

	//+------------------\----------------------------------------
	//|	    Window	     |
	//\------------------/----------------------------------------
	struct OpenGLSettings
	{
		// profileMask: 
		//		SDL_GL_CONTEXT_PROFILE_CORE
		//		SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
		//		SDL_GL_CONTEXT_PROFILE_ES
		int profileMask{ SDL_GL_CONTEXT_PROFILE_CORE };
		int versionMajor{ 2 };
		int versionMinor{ 1 };
		GLenum shadeModel{ GL_SMOOTH };
		GLenum perspectiveCorrectionMode{ GL_NICEST };

		// texture2DMinFilter: 
		//		GL_NEAREST_MIPMAP_NEAREST
		//		GL_LINEAR_MIPMAP_NEAREST
		//		GL_NEAREST_MIPMAP_LINEAR
		//		GL_LINEAR_MIPMAP_LINEAR
		//		GL_NEAREST
		//		GL_LINEAR
		GLint texture2DMinFilter{ GL_LINEAR };

		// texture2DMagFilter: 
		//		GL_NEAREST
		//		GL_LINEAR 
		GLint texture2DMagFilter{ GL_LINEAR };

		// textureWrapS: 
		//		GL_CLAMP_TO_EDGE
		//		GL_CLAMP_TO_BORDER,
		//		GL_MIRRORED_REPEAT
		//		GL_REPEAT (gl default)
		//		GL_MIRROR_CLAMP_TO_EDGE
		GLint textureWrapS{ GL_CLAMP_TO_EDGE };
		GLint textureWrapT{ GL_CLAMP_TO_EDGE };
		GLint textureEnvMode{ GL_MODULATE };
		bool pointSmoothing{ false };	//Implementation dependent
		bool lineSmoothing{ false };	//Implementation dependent?
	};
	struct WindowDef
	{
		std::string title;
		bool fullScreen;
		std::array<int, 2> position;
		std::array<int, 2> size;
		bool vsync;
		bool vsyncAllowLateSwaps;
		bool doubleBuffer;
		int antiAliasingSamples;
		float fpsUpdateDelay;
		std::array<int, 4> colorChannelBits;
		int imageExtensions;
		OpenGLSettings gl;
	};
	namespace Window
	{
		// OpenGL point sizes
		const unsigned NUM_POINT_SIZES{ 21 };
		const float POINT_SIZES[NUM_POINT_SIZES] =
			{ 0.5f, 0.625f, 0.75f, 0.875f, 
			  1.0f, 1.25f, 1.5f, 1.75f, 
			  2.0f, 2.25f, 2.5f, 3.0f, 3.5f, 
			  4.0f, 4.5f, 5.0f, 6.0f, 
			  7.0f, 8.0f, 9.0f, 10.0f };
		const Range<int> VALID_POINT_SIZES{ 0, NUM_POINT_SIZES - 1 };

		// Anti-aliasing
		const Range<int> VALID_ANTI_ALIASING_SAMPLES{ 1, 16};
		const int MIN_SAMPLES_TO_ENABLE_ANTI_ALIASING{ 2 };

		// Open/close window
		void Init(const WindowDef& windowDef);
		void Close();

		// Accessors
		float GetXYAspectRatio();
		b2Vec2 GetScreenResolution();
		float GetFPS();
		b2Vec2 GetMousePositionAsPercentOfWindow(Sint32 eventMouseX, Sint32 eventMouseY);
		b2Vec2 GetMousePosition();

		// Modifiers
		void SetClearColor(const Color& newColor);
		void SetShowCursor(bool enabled);
		void SetFPSInterval(float interval);
		void SetCameraRect(const Rect& rect);
		void SetPointSize(float size);
		void SetLineWidth(float width);
		void SetColor(const Color& newColor);
		void EnableTextures();
		void DisableTextures();
		void EnableBlending();
		void DisableBlending();

		// Scene
		void StartScene();
		void PushMatrix();
		void Translate(const b2Vec2& position);
		void Rotate(float radians);
		void PopMatrix();
		void EndScene();

		// Draw
		void DrawPoint(const b2Vec2& position = b2Vec2_zero);
		void DrawCircle(const b2Vec2& center, float radius, bool fill=false);
		void DrawPolygon(const b2Vec2* vertices, unsigned vertexCount, bool fill = false);
		void DrawRect(const Rect& drawRect, bool fill=false);
		void DrawLine(const b2Vec2& p1, const b2Vec2& p2);
		void DrawLineStrip(const b2Vec2* vertices, unsigned vertexCount);
		void DrawString(const std::string &text, Alignment alignment, float size, const FontReference& font);
	}
}
