#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <mutex>

#include "xnIMemoryManager.h"

class SafeMemoryManager : public xn::IMemoryManager
{
public:

  void *Malloc(size_t, bool zero = true) override;
  void Free(void *) override;
  void Memcpy(void *pDest, void *pSrc, size_t size) override;

private:

  std::mutex m_mutex;
};

#endif