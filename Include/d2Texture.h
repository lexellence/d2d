/**************************************************************************************\
** File: d2Texture.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for Texture and related classes
**
\**************************************************************************************/
#pragma once
#include "d2Resource.h"
namespace d2d
{
	struct TextureCoordinates
	{
		b2Vec2 lowerLeft;
		b2Vec2 lowerRight;
		b2Vec2 upperRight;
		b2Vec2 upperLeft;
	};
	static const TextureCoordinates NORMAL_FULL_TEXTURE_COORD
	{
		.lowerLeft{ 0.0f, 1.0f },
		.lowerRight{ 1.0f, 1.0f },
		.upperRight{ 1.0f, 0.0f },
		.upperLeft{ 0.0f, 0.0f }
	};

	class TextureAtlas : public ResourceReference
	{
	public:
		TextureAtlas(const std::string& imagePath, const std::string& atlasXMLPath);
		virtual ~TextureAtlas();
		GLuint GetGLTextureID() const;
		float GetWidthToHeightRatio(const std::string& name) const;
		const TextureCoordinates& GetTextureCoordinates(const std::string& name) const;
		const b2Vec2& GetRelativeCenterOfMass(const std::string& name) const;
	};

	class Texture
	{
	public:
		virtual ~Texture() {};
		virtual GLuint GetGLTextureID() const = 0;
		virtual const TextureCoordinates& GetTextureCoordinates() const = 0;
	};
	class TextureFromAtlas : public Texture
	{
	public:
		TextureFromAtlas(const TextureAtlas& atlas, const std::string& name);
		virtual GLuint GetGLTextureID() const;
		virtual const TextureCoordinates& GetTextureCoordinates() const;
		float GetWidthToHeightRatio() const;
		const b2Vec2& GetRelativeCenterOfMass() const;
	private:
		const TextureAtlas *const m_atlasPtr;
		std::string m_name;
	};
	class TextureStandalone : public ResourceReference, public Texture
	{
	public:
		TextureStandalone(const std::string& imagePath);
		virtual ~TextureStandalone();
		virtual GLuint GetGLTextureID() const;
		virtual const TextureCoordinates& GetTextureCoordinates() const;
		float GetWidthToHeightRatio() const;
	};

	void GenerateGLTexture(SDL_Surface& surface, GLuint& texID, float& widthToHeightRatio);
}
