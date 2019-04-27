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
#include "drawtext.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <Box2D/Box2D.h>
#include <array>
#include <string>
namespace d2d
{
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
		bool doubleBuffer;
		int antiAliasingSamples; 
		float fpsUpdateDelay;
		std::array<int, 4> colorChannelBits;
		Color clearColor;
		int imageExtensions;
		OpenGLSettings gl;
	};

	namespace Window
	{
		// Resource references
		class TextureReference : public ResourceReference
		{
		public:
			TextureReference(const std::string& filename);
			TextureReference(const char* filename);
			~TextureReference();
			float GetWidthToHeightRatio() const;
		private:
			float m_widthToHeightRatio;
		};

		class FontReference : public ResourceReference
		{
		public:
			FontReference(const std::string& filename);
			FontReference(const char* filename);
			~FontReference();
		};

		// OpenGL point sizes
		const unsigned NUM_POINT_SIZES{ 21 };
		const float POINT_SIZES[NUM_POINT_SIZES] =
			{ 0.5f, 0.625f, 0.75f, 0.875f, 
			  1.0f, 1.25f, 1.5f, 1.75f, 
			  2.0f, 2.25f, 2.5f, 3.0f, 3.5f, 
			  4.0f, 4.5f, 5.0f, 6.0f, 
			  7.0f, 8.0f, 9.0f, 10.0f };
		const Range<int> VALID_POINT_SIZE_RANGE{ 0, NUM_POINT_SIZES - 1 };

		// Anti-aliasing
		const Range<int> VALID_SAMPLE_RANGE{ 1, 16};
		const int WINDOW_MIN_SAMPLES_TO_ENABLE_ANTI_ALIASING{ 2 };

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
		void SetShowCursor(bool enabled);
		void SetFPSInterval(float interval);
		void SetCameraRect(const Rect& rect);
		void SetPointSize(float size);
		void SetLineWidth(float width);
		void SetColor(const Color& newColor);
		void SetColor(float red, float green, float blue, float alpha);
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
		void DrawRect(const b2Vec2& lowerBound, const b2Vec2& upperBound, bool fill=false);
		void DrawLine(const b2Vec2& p1, const b2Vec2& p2);
		void DrawLineStrip(const b2Vec2* vertices, unsigned vertexCount);
		void DrawTexture(unsigned textureID, const b2Vec2& size);
		void DrawTextureInRect(unsigned textureID, const b2Vec2& lowerBound, const b2Vec2& upperBound);
		void DrawString(const std::string &text, Alignment alignment, float fontSize, unsigned fontID);
	}
}
