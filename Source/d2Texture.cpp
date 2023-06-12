/**************************************************************************************\
** File: d2Window.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Window class
**
\**************************************************************************************/
#include "d2Texture.h"
#include "d2Window.h"
namespace d2d
{
    namespace
    {
        class TextureResource;
        class TextureAtlasResource;
        ResourceManager<TextureResource> m_spriteManager;
        ResourceManager<TextureAtlasResource> m_spriteAtlasManager;

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
			//InvertSurface(*surface);

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

			// GL Texture filter
			const WindowDef& windowDef = Window::GetWindowDef();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, windowDef.gl.texture2DMagFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, windowDef.gl.texture2DMinFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, windowDef.gl.textureWrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, windowDef.gl.textureWrapT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, windowDef.gl.textureEnvMode);

            // We don't need the surface anymore
            SDL_FreeSurface(surface);
        }

        //+----------------------\------------------------------------
        //|   TextureResource    |
        //\----------------------/------------------------------------
        class TextureResource : public Resource
        {
        public:
            // filePaths[0]: path of the image file
            explicit TextureResource(const std::vector<std::string>& filePaths)
                : Resource(filePaths)
            {
                if (filePaths.size() < 1)
                    throw InitException{ "SpriteResource requires one filePath"s };

                // Load sprite into OpenGL
                GenerateGLTexture(filePaths[0], m_glTextureID, m_pixelWidthToHeightRatio);
            }
            ~TextureResource()
            {
                // Unload sprite from OpenGL
                glDeleteTextures(1, &m_glTextureID);
            }
            GLuint GetGLTextureID() const { return m_glTextureID; }
            float GetPixelWidthToHeightRatio() const { return m_pixelWidthToHeightRatio; }

        private:
            GLuint m_glTextureID;
            float m_pixelWidthToHeightRatio;
        };

        //+----------------------\------------------------------------
        //| TextureAtlasResource |
        //\----------------------/------------------------------------
        struct TextureAtlasValue
        {
            TextureCoordinates textureCoords;
            float pixelWidthToHeightRatio;
            b2Vec2 relativeCenterOfMass;
        };
        class TextureAtlasResource : public TextureResource
        {
        public:
            // filePaths[0]: path of the image file
            // filePaths[1]: path of the xml file with atlas data
            explicit TextureAtlasResource(const std::vector<std::string> &filePaths)
                : TextureResource(filePaths)
            {
                if (filePaths.size() < 2)
                    throw InitException{ "TextureAtlasResource requires two filePaths"s };

                // Load atlas data
                boost::property_tree::ptree data;
                boost::property_tree::read_xml(filePaths[1], data);

                int atlasWidth{data.get<int>("TextureAtlas.<xmlattr>.width")};
                int atlasHeight{data.get<int>("TextureAtlas.<xmlattr>.height")};
                SDL_assert_release(atlasWidth > 0);
                SDL_assert_release(atlasHeight > 0);

                for (auto const &spriteNode : data.get_child("TextureAtlas"))
                {
                    if (spriteNode.first == "sprite")
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
                        float pivotRelativeX = spriteNode.second.get<float>("<xmlattr>.pX");
                        float pivotRelativeY = spriteNode.second.get<float>("<xmlattr>.pY");
                        SDL_assert_release(pivotRelativeX >= 0.0f);
                        SDL_assert_release(pivotRelativeX <= 1.0f);
                        SDL_assert_release(pivotRelativeY >= 0.0f);
                        SDL_assert_release(pivotRelativeY <= 1.0f);

                        // opposite corner
                        int x2 = x + w;
                        int y2 = y + h;
                        SDL_assert_release(x2 <= atlasWidth);
                        SDL_assert_release(y2 <= atlasHeight);

                        boost::optional<const boost::property_tree::ptree&> rot = spriteNode.second.get_child_optional( "<xmlattr>.r" );
                        bool isRotatedClockwise90Degrees = (bool)rot;

                        TextureAtlasValue& spriteEntryRef = m_spriteMap[spriteName];
                        const float X1 = (float) x / (float) atlasWidth;
                        const float Y1 = (float) y / (float) atlasHeight;
                        const float X2 = (float) x2 / (float) atlasWidth;
                        const float Y2 = (float) y2 / (float) atlasHeight;
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
                        spriteEntryRef.pixelWidthToHeightRatio = (float) w / (float) h;
                        spriteEntryRef.relativeCenterOfMass.Set(pivotRelativeX, pivotRelativeY);
                    }
                }
            }
            const TextureAtlasValue& GetValue(const std::string& spriteName) const
            {
                return m_spriteMap.at(spriteName);
            }

        private:
            std::unordered_map<std::string, TextureAtlasValue> m_spriteMap;
        };
    }

    //+--------------------------\--------------------------------
    //|	        Texture          |
    //\--------------------------/--------------------------------
    TextureStandalone::TextureStandalone(const std::string& imagePath)
        : ResourceReference(m_spriteManager.Load({imagePath}))
    {}
    TextureStandalone::~TextureStandalone()
    {
        m_spriteManager.Unload(GetID());
    }
    float TextureStandalone::GetWidthToHeightRatio() const
    {
        return m_spriteManager.GetResource(GetID()).GetPixelWidthToHeightRatio();
    }
    GLuint TextureStandalone::GetGLTextureID() const
    {
        return m_spriteManager.GetResource(GetID()).GetGLTextureID();
    }
    const TextureCoordinates& TextureStandalone::GetTextureCoordinates() const
    {
        static const TextureCoordinates NORMAL_FULL_TEXTURE_COORD{
            .lowerLeft{ 0.0f, 1.0f },
            .lowerRight{ 1.0f, 1.0f },
            .upperRight{ 1.0f, 0.0f },
            .upperLeft{ 0.0f, 0.0f }
        };
        return NORMAL_FULL_TEXTURE_COORD;
    }

    //+--------------------------\--------------------------------
    //|	      TextureAtlas       |
    //\--------------------------/--------------------------------
    TextureAtlas::TextureAtlas(const std::string& imagePath, const std::string& atlasXMLPath)
        : ResourceReference(m_spriteAtlasManager.Load({imagePath, atlasXMLPath}))
    {}
    TextureAtlas::~TextureAtlas()
    {
        m_spriteAtlasManager.Unload(GetID());
    }
    GLuint TextureAtlas::GetGLTextureID() const
    {
        return m_spriteAtlasManager.GetResource(GetID()).GetGLTextureID();
    }
    float TextureAtlas::GetWidthToHeightRatio(const std::string& name) const
    {
        return m_spriteAtlasManager.GetResource(GetID()).GetValue(name).pixelWidthToHeightRatio;
    }
    const TextureCoordinates& TextureAtlas::GetTextureCoordinates(const std::string& name) const
    {
        return m_spriteAtlasManager.GetResource(GetID()).GetValue(name).textureCoords;
    }
    const b2Vec2& TextureAtlas::GetRelativeCenterOfMass(const std::string& name) const
    {
        return m_spriteAtlasManager.GetResource(GetID()).GetValue(name).relativeCenterOfMass;
    }

    //+--------------------------\--------------------------------
    //|     TextureFromAtlas     |
    //\--------------------------/--------------------------------
    TextureFromAtlas::TextureFromAtlas(const TextureAtlas& atlas, const std::string& name)
        : m_atlasPtr{ &atlas },
          m_name{ name }
    {}
    float TextureFromAtlas::GetWidthToHeightRatio() const
    {
        return m_atlasPtr->GetWidthToHeightRatio(m_name);
    }
    GLuint TextureFromAtlas::GetGLTextureID() const
    {
        return m_atlasPtr->GetGLTextureID();
    }
    const TextureCoordinates& TextureFromAtlas::GetTextureCoordinates() const
    {
        return m_atlasPtr->GetTextureCoordinates(m_name);
    }
    const b2Vec2& TextureFromAtlas::GetRelativeCenterOfMass() const
    {
        return m_atlasPtr->GetRelativeCenterOfMass(m_name);
    }
}
