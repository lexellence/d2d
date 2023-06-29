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
#include "d2Texture.h"
#include "d2Window.h"
#include "d2Exception.h"
namespace d2d
{
	namespace
	{
		void InvertSurface(SDL_Surface& surface);
		void GenerateGLTexture(SDL_Surface& surface, GLuint &texID, float& widthToHeightRatio);
	}
	static const TextureCoordinates FULL_TEXTURE_COORD
	{
		.lowerLeft{ 0.0f, 1.0f },
		.lowerRight{ 1.0f, 1.0f },
		.upperRight{ 1.0f, 0.0f },
		.upperLeft{ 0.0f, 0.0f }
	};

	//+----------------------\------------------------------------
	//|		  Texture        |
	//\----------------------/------------------------------------
	Texture::Texture(const std::string& filePath)
		: m_textureCoords{ FULL_TEXTURE_COORD },
		m_loadedTextureID{ false }
	{
		// Load texture from file
		SDL_Surface* surfacePtr{ IMG_Load(filePath.c_str()) };
		if(!surfacePtr)
			throw InitException{ "Failed to load texture: "s + filePath };

		// Convert to gl texture
		GenerateGLTexture(*surfacePtr, m_glTextureID, m_widthToHeightRatio);
		m_loadedTextureID = true;
		SDL_FreeSurface(surfacePtr);
	}
	Texture::Texture(GLuint glTextureID, const TextureCoordinates& textureCoords, float pixelWidthToHeightRatio)
		: m_glTextureID{ glTextureID }, 
		m_textureCoords{ textureCoords },
		m_widthToHeightRatio{ pixelWidthToHeightRatio },
		m_loadedTextureID{ false }
	{}
	Texture::~Texture()
	{
		if(m_loadedTextureID)
			glDeleteTextures(1, &m_glTextureID);
	}
	GLuint Texture::GetGLTextureID() const
	{
		return m_glTextureID;
	}
	const TextureCoordinates& Texture::GetTextureCoordinates() const
	{
		return FULL_TEXTURE_COORD;
	}
	float Texture::GetWidthToHeightRatio() const
	{
		return m_widthToHeightRatio;
	}

	//+----------------------\------------------------------------
	//|     TextureAtlas     |
	//\----------------------/------------------------------------
	TextureAtlas::TextureAtlas(const std::string& imagePath, const std::string& atlasXMLPath)
		: Texture{ imagePath }
	{
		// Load atlas data
		boost::property_tree::ptree data;
		boost::property_tree::read_xml(atlasXMLPath, data);

		int atlasWidth{ data.get<int>("TextureAtlas.<xmlattr>.width") };
		int atlasHeight{ data.get<int>("TextureAtlas.<xmlattr>.height") };
		SDL_assert_release(atlasWidth > 0);
		SDL_assert_release(atlasHeight > 0);

		for(auto const &spriteNode : data.get_child("TextureAtlas"))
		{
			if(spriteNode.first == "sprite")
			{
				// name
				std::string spriteName = spriteNode.second.get<std::string>("<xmlattr>.n");
				SDL_assert_release(spriteName.length() > 0);

				// position
				int x = spriteNode.second.get<int>("<xmlattr>.x");
				int y = spriteNode.second.get<int>("<xmlattr>.y");
				SDL_assert_release(x >= 0);
				SDL_assert_release(x < atlasWidth);
				SDL_assert_release(y >= 0);
				SDL_assert_release(y < atlasHeight);

				// size
				int w = spriteNode.second.get<int>("<xmlattr>.w");
				int h = spriteNode.second.get<int>("<xmlattr>.h");
				SDL_assert_release(w > 0);
				SDL_assert_release(w <= atlasWidth);
				SDL_assert_release(h > 0);
				SDL_assert_release(h <= atlasHeight);

				// center of mass
				//float pivotRelativeX = spriteNode.second.get<float>("<xmlattr>.pX");
				//float pivotRelativeY = spriteNode.second.get<float>("<xmlattr>.pY");
				//SDL_assert_release(pivotRelativeX >= 0.0f);
				//SDL_assert_release(pivotRelativeX <= 1.0f);
				//SDL_assert_release(pivotRelativeY >= 0.0f);
				//SDL_assert_release(pivotRelativeY <= 1.0f);

				// opposite corner
				int x2 = x + w;
				int y2 = y + h;
				SDL_assert_release(x2 <= atlasWidth);
				SDL_assert_release(y2 <= atlasHeight);

				boost::optional<const boost::property_tree::ptree&> rot = spriteNode.second.get_child_optional("<xmlattr>.r");
				bool isRotatedClockwise90Degrees = (bool)rot;

				TextureAtlasEntry& spriteEntryRef = m_spriteMap[spriteName];
				const float X1 = (float)x / (float)atlasWidth;
				const float Y1 = (float)y / (float)atlasHeight;
				const float X2 = (float)x2 / (float)atlasWidth;
				const float Y2 = (float)y2 / (float)atlasHeight;
				if(!isRotatedClockwise90Degrees)
				{
					spriteEntryRef.textureCoords.lowerLeft.Set(X1, Y2);
					spriteEntryRef.textureCoords.lowerRight.Set(X2, Y2);
					spriteEntryRef.textureCoords.upperRight.Set(X2, Y1);
					spriteEntryRef.textureCoords.upperLeft.Set(X1, Y1);
				}
				else
				{
					spriteEntryRef.textureCoords.lowerLeft.Set(X1, Y1);
					spriteEntryRef.textureCoords.lowerRight.Set(X1, Y2);
					spriteEntryRef.textureCoords.upperRight.Set(X2, Y2);
					spriteEntryRef.textureCoords.upperLeft.Set(X2, Y1);
					std::swap(w, h);
				}
				spriteEntryRef.widthToHeightRatio = (float)w / (float)h;
				//spriteEntryRef.relativeCenterOfMass.Set(pivotRelativeX, pivotRelativeY);
			}
		}
	}
	const TextureAtlasEntry& TextureAtlas::GetEntry(const std::string& spriteName) const
	{
		return m_spriteMap.at(spriteName);
	}

	//+--------------------------\--------------------------------
	//|     TextureFromAtlas     |
	//\--------------------------/--------------------------------
	TextureFromAtlas::TextureFromAtlas(const TextureAtlas& atlas, const std::string& name)
		: Texture{ atlas.GetGLTextureID(), 
		atlas.GetEntry(name).textureCoords, 
		atlas.GetEntry(name).widthToHeightRatio }
	{}

	namespace
	{
		//+----------------------\------------------------------------
		//|	   InvertSurface	 | UNUSED
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
			for(index = 0; index < height_div_2; index++)
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
		//	Register SDL_Surface as OpenGL texture,
		//	save gl id and aspect ratio in the output parameters.
		void GenerateGLTexture(SDL_Surface& surface, GLuint &texID, float& widthToHeightRatio)
		{
			if(surface.h == 0)
				widthToHeightRatio = 1.0f;
			else
				widthToHeightRatio = (float)surface.w / (float)surface.h;

			// Flip it so it complies with opengl's lower-left origin
			// UNUSED, USING TEXTURE_COORDINATES TO COMPENSATE INSTEAD
			//InvertSurface(*surface);

			// Pixel Format
			int colorMode = GL_RGB;
			if(surface.format->BytesPerPixel == 4)
				colorMode = GL_RGBA;

			// Create the OpenGL sprite
			Window::EnableTextures();
			Window::EnableBlending();

			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, colorMode, surface.w, surface.h, 0,
				colorMode, GL_UNSIGNED_BYTE, surface.pixels);

			// GL Texture filter
			const WindowDef& windowDef = Window::GetWindowDef();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, windowDef.gl.texture2DMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, windowDef.gl.texture2DMinFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, windowDef.gl.textureWrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, windowDef.gl.textureWrapT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, windowDef.gl.textureEnvMode);
		}
	}
}
