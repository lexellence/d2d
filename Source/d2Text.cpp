/**************************************************************************************\
** File: d2Text.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for classes related to text drawing
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Text.h"
namespace d2d
{
    namespace
    {
        class FontResource;
        ResourceManager<FontResource> m_fontManager;

        //+--------------------------\--------------------------------
        //|      FontResource        |
        //\--------------------------/--------------------------------
        class FontResource : public Resource
        {
        public:
            // filePaths[0]: path of the font file
            explicit FontResource(const std::vector<std::string>& filePaths)
                : Resource(filePaths)
            {
                if (filePaths.size() < 1)
                    throw InitException{"FontResource requires one filePath"s};
                m_fontPtr = new FTGLOutlineFont(filePaths[0].c_str());
                if (!m_fontPtr || m_fontPtr->Error())
                    throw InitException{"Could not load font: "s + filePaths[0]};
                //m_fontPtr->FaceSize(FONT_SIZE);
            }
            FTGLOutlineFont* GetFontPtr() const
            {
                return m_fontPtr;
            }

        private:
            FTGLOutlineFont* m_fontPtr;
        };
    }

    //+--------------------------\--------------------------------
    //|      FontReference       |
    //\--------------------------/--------------------------------
    FontReference::FontReference(const std::string &fontPath)
        : ResourceReference(m_fontManager.Load({fontPath}))
    {}
    FontReference::~FontReference()
    {
        m_fontManager.Unload(GetID());
    }
    FTGLOutlineFont* FontReference::GetFontPtr() const
    {
        return m_fontManager.GetResource(GetID()).GetFontPtr();
    }
}
