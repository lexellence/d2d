/**************************************************************************************\
** File: d2Resource.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Resource, ResourceReference classes
**
\**************************************************************************************/
#include "d2pch.h"
#include "d2Resource.h"
#include "d2Utility.h"

namespace d2d
{
	Resource::Resource(const std::vector<std::string>& filePaths)
		: m_filePaths{ filePaths }
	{ }
	Resource::~Resource()
	{ }
	void Resource::IncrementReferenceCount()
	{
		++m_referenceCount;
	}
	void Resource::DecrementReferenceCount()
	{
		if(m_referenceCount > 0)
			--m_referenceCount;
	}
	unsigned long Resource::GetReferenceCount() const
	{
		return m_referenceCount;
	}
	const std::vector<std::string>& Resource::GetFilePaths() const
	{
		return m_filePaths;
	}

	ResourceReference::ResourceReference(ResourceID id)
	  : m_id{ id }
	{ }
	ResourceReference::~ResourceReference()
	{ }
	ResourceID ResourceReference::GetID() const
	{
		return m_id;
	}
}
