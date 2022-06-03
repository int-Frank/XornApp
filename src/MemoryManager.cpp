
#include <cstdlib>
#include <cstring>

#include "MemoryManager.h"

void *SafeMemoryManager::Malloc(size_t sze, bool zero)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  if (zero)
    return malloc(sze);
  else
    return calloc(sze, 1);
}

void SafeMemoryManager::Free(void *pMem)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  free(pMem);
}

void SafeMemoryManager::Memcpy(void *pDest, void *pSrc, size_t size)
{
  std::lock_guard<std::mutex> lock(m_mutex);
  memcpy(pDest, pSrc, size);
}
