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
		Resource() = delete;
		explicit Resource(const std::vector<std::string>& filePaths);
		virtual ~Resource() = 0;
		void IncrementReferenceCount();
		void DecrementReferenceCount();
		unsigned long GetReferenceCount() const;
		const std::vector<std::string>& GetFilePaths() const;

	private:
		size_t m_referenceCount{ 1 };
		std::vector<std::string> m_filePaths;
	};

	using ResourceID = size_t;
	class ResourceReference
	{
	public:
		ResourceReference() = delete;
		explicit ResourceReference(ResourceID id);
		virtual ~ResourceReference() = 0;
		ResourceID GetID() const;

	private:
		ResourceID m_id;
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

		size_t Load(const std::vector<std::string>& filePaths)
		{
			if (filePaths.size() < 1)
				throw InitException{ "ResourceManager::Load requires one filePath"s };

			auto indexIterator{ m_filenameIndexMap.find(filePaths[0]) };
			size_t id;

			// If no index with the same filename
			if(indexIterator == m_filenameIndexMap.end())
			{
				// If there are no unused numSlots
				if(m_availableIndexList.empty())
				{
					// Load and add as a new element
					m_resourcePtrList.push_back(new ResourceType(filePaths));
					id = m_resourcePtrList.size() - 1;
				}
				else
				{
					// Load and put into an unused slot.
					id = m_availableIndexList.top();
					m_availableIndexList.pop();
					m_resourcePtrList.at(id) = new ResourceType(filePaths);
				}
				// Map the resource's index to its filename and bail.
				m_filenameIndexMap[filePaths[0]] = id;
			}
			else
			{
				// We found an index with the same filename, so increment the
				// reference count for the resource that already exists at that index.
				id = indexIterator->second;
				SDL_assert(m_resourcePtrList.at(id) != nullptr);
				m_resourcePtrList.at(id)->IncrementReferenceCount();
			}
			return id;
		}
		void Unload(size_t id)
		{
			if(id >= m_resourcePtrList.size() || m_resourcePtrList[id] == nullptr)
			{
				d2LogWarning << "Tried to unload resource with unrecognized id";
				return;
			}

			m_resourcePtrList[id]->DecrementReferenceCount();
			if(m_resourcePtrList[id]->GetReferenceCount() == 0)
			{
				// Free resource
				delete m_resourcePtrList[id];
				m_resourcePtrList[id] = nullptr;
				m_availableIndexList.push(id);

				// Remove filename to index map entry
				auto indexMapIterator = m_filenameIndexMap.begin();
				while(indexMapIterator != m_filenameIndexMap.end())
				{
					if((*indexMapIterator).second == id)
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
		// Throws exception if id not found
		const ResourceType& GetResource(unsigned id) const
		{
			SDL_assert_release(id < m_resourcePtrList.size());
			return *m_resourcePtrList[id];
		}

	private:
		std::stack<unsigned> m_availableIndexList;
		std::vector<ResourceType*> m_resourcePtrList;
		std::map<std::string, unsigned> m_filenameIndexMap;
	};
}
