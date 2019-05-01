/**************************************************************************************\
** File: d2Resource.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Resource class
**
\**************************************************************************************/
#pragma once
#include "d2Utility.h"
namespace d2d
{
	class Resource
	{
	public:
		void IncrementReferenceCount();
		void DecrementReferenceCount();
		unsigned long GetReferenceCount() const;

	private:
		unsigned long m_referenceCount{ 1 };
	};

	class ResourceReference
	{
	public:
		ResourceReference() = delete;
		ResourceReference(const std::string& filename, unsigned id);
		virtual ~ResourceReference() = 0;
		unsigned GetID() const;
		const std::string& GetFilename() const;

	private:
		std::string m_filename;
		unsigned m_id;
	};

	template<class ResourceType>
	class ResourceManager
	{
	public:
		~ResourceManager()
		{
			for(auto &resourcePtr : m_resourcePtrList)
			{
				delete resourcePtr;
				resourcePtr = nullptr;
			}
		}

		unsigned Load(const std::string& filename)
		{
			auto indexIterator{ m_filenameIndexMap.find(filename) };
			unsigned handle;

			// If no index with the same filename
			if(indexIterator == m_filenameIndexMap.end())
			{
				// If there are no unused numSlots
				if(m_availableIndexList.empty())
				{
					// Load and add as a new element
					m_resourcePtrList.push_back(new ResourceType(filename));
					handle = m_resourcePtrList.size() - 1;
				}
				else
				{
					// Load and put into an unused slot.
					handle = m_availableIndexList.top();
					m_availableIndexList.pop();
					m_resourcePtrList.at(handle) = new ResourceType(filename);
				}
				// Map the resource's index to its filename and bail.
				m_filenameIndexMap[filename] = handle;
			}
			else
			{
				// We found an index with the same filename, so increment the   
				// reference count for the resource that already exists at that index.
				handle = indexIterator->second;
				SDL_assert(m_resourcePtrList[handle] != nullptr);
				m_resourcePtrList.at(handle)->IncrementReferenceCount();
			}
			return handle;
		}
		void Unload(unsigned handle)
		{
			if(handle >= m_resourcePtrList.size() || m_resourcePtrList[handle] == nullptr)
			{
				d2LogWarning << "Tried to unload resource with unrecognized handle";
				return;
			}

			m_resourcePtrList[handle]->DecrementReferenceCount();
			if(m_resourcePtrList[handle]->GetReferenceCount() == 0)
			{
				// Free resource
				delete m_resourcePtrList[handle];
				m_resourcePtrList[handle] = nullptr;
				m_availableIndexList.push(handle);

				// Remove filename to index map entry
				auto indexMapIterator = m_filenameIndexMap.begin();
				while(indexMapIterator != m_filenameIndexMap.end())
				{
					if((*indexMapIterator).second == handle)
					{
						indexMapIterator = m_filenameIndexMap.erase(indexMapIterator);
					}
					else
					{
						++indexMapIterator;
					}
				}
			}
		}
		// Throws exception if handle not found
		const ResourceType& GetResource(unsigned handle) const
		{
			SDL_assert_release(handle < m_resourcePtrList.size());
			return *m_resourcePtrList[handle];
		}

	private:
		std::stack<unsigned> m_availableIndexList;
		std::vector<ResourceType*> m_resourcePtrList;
		std::map<std::string, unsigned> m_filenameIndexMap;
	};
}
