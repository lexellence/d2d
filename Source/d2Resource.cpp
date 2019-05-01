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
	void Resource::IncrementReferenceCount() 
	{ 
		++m_referenceCount; 
	}
	void Resource::DecrementReferenceCount() 
	{ 
		--m_referenceCount; 
	}
	unsigned long Resource::GetReferenceCount() const 
	{ 
		return m_referenceCount; 
	}

	ResourceReference::ResourceReference(const std::string& filename, unsigned id)
	  : m_filename{ filename }, m_id{ id }
	{ 			
		d2LogDebug << "ResourceReference::ResourceReference(string) handle: " << m_id << " filename: " << m_filename;
	}
	ResourceReference::~ResourceReference()
	{ }
	unsigned ResourceReference::GetID() const
	{ 
		return m_id; 
	}
	const std::string& ResourceReference::GetFilename() const 
	{ 
		return m_filename; 
	}
}