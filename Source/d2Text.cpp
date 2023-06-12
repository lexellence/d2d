/**************************************************************************************\
** File: d2Text.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for classes related to text drawing
**
\**************************************************************************************/
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
            explicit FontResource(const std::vector<std::string> &filePaths)
                : Resource(filePaths)
            {
                if (filePaths.size() < 1)
                    throw InitException{"FontResource requires one filePath"s};

                m_dtxFontPtr = dtx_open_font(filePaths[0].c_str(), DTX_FONT_SIZE);
                if (!m_dtxFontPtr)
                    throw InitException{"Failed to open font: "s + filePaths[0]};
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
            struct dtx_font *m_dtxFontPtr{nullptr};
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
    dtx_font* FontReference::GetDTXFontPtr() const
    {
        return m_fontManager.GetResource(GetID()).GetDTXFontPtr();
    }
}
