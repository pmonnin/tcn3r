#include "CacheManager.h"


CacheManager::CacheManager() : m_cache(), m_inverseCache()
{

}

unsigned int CacheManager::getElementIndex(const std::string& element)
{
    if (m_cache.find(element) == m_cache.end())
    {
        m_cache[element] = m_inverseCache.size();
        m_inverseCache.push_back(element);
    }

    return m_cache[element];
}

std::string CacheManager::getElementFromIndex(unsigned int index) const
{
    if (index >= m_inverseCache.size())
        return "";

    return m_inverseCache[index];
}
