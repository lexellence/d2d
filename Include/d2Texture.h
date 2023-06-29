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
namespace d2d
{
	//+----------------------\------------------------------------
	//|  TextureCoordinates  |
	//\----------------------/------------------------------------
	struct TextureCoordinates
	{
		b2Vec2 lowerLeft;
		b2Vec2 lowerRight;
		b2Vec2 upperRight;
		b2Vec2 upperLeft;
	};

	//+----------------------\------------------------------------
	//|		 ITexture        | (interface)
	//\----------------------/------------------------------------
	//class ITexture
	//{
	//public:
	//	virtual ~ITexture() {};
	//	virtual GLuint GetGLTextureID() const = 0;
	//	virtual const TextureCoordinates& GetTextureCoordinates() const = 0;
	//	virtual float GetWidthToHeightRatio() const = 0;
	//};

	//+----------------------\------------------------------------
	//|		  Texture        |
	//\----------------------/------------------------------------
	class Texture
	{
	public:
		explicit Texture(const std::string& filePath);
		Texture(GLuint glTextureID, const TextureCoordinates& textureCoords, float pixelWidthToHeightRatio);
		virtual ~Texture();
		virtual GLuint GetGLTextureID() const;
		virtual const TextureCoordinates& GetTextureCoordinates() const;
		virtual float GetWidthToHeightRatio() const;

	private:
		GLuint m_glTextureID;
		TextureCoordinates m_textureCoords;
		float m_widthToHeightRatio;
		bool m_loadedTextureID;
	};

	//+----------------------\------------------------------------
	//|  TextureAtlasEntry	 |
	//\----------------------/------------------------------------
	struct TextureAtlasEntry
	{
		TextureCoordinates textureCoords;
		float widthToHeightRatio;
		//b2Vec2 relativeCenterOfMass;
	};

	//+----------------------\------------------------------------
	//|     TextureAtlas	 |
	//\----------------------/------------------------------------
	class TextureAtlas : public Texture
	{
	public:
		explicit TextureAtlas(const std::string& imagePath, const std::string& atlasXMLPath);
		const TextureAtlasEntry& GetEntry(const std::string& spriteName) const;

	private:
		std::unordered_map<std::string, TextureAtlasEntry> m_spriteMap;
	};

	//+--------------------------\--------------------------------
	//|     TextureFromAtlas     |
	//\--------------------------/--------------------------------
	class TextureFromAtlas : public Texture
	{
	public:
		TextureFromAtlas(const TextureAtlas& atlas, const std::string& name);
		//const b2Vec2& GetRelativeCenterOfMass() const;
	};
}
