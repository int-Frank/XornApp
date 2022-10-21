#include <stdint.h>
#include <mutex>

#include "MessageBus.h"
#include "DgDoublyLinkedList.h"

template<typename T>
class SafeQueue
{
public:

  bool Empty()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }

  T PopFront()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    T x = m_queue.front();
    m_queue.pop_front();
    return x;
  }

  void PushBack(const T &x)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(x);
  }

private:

  std::mutex m_mutex;
  Dg::DoublyLinkedList<T> m_queue;
};

class MessageBus::PIMPL
{
public:

  SafeQueue<Message *>   messageQueue;
};

MessageBus::MessageBus()
  : m_pimpl(new PIMPL())
{

}

MessageBus::~MessageBus()
{
  delete m_pimpl;
}

void MessageBus::Post(Message *pMsg)
{
  m_pimpl->messageQueue.PushBack(pMsg);
}

Message *MessageBus::PopMessage()
{
  if (m_pimpl->messageQueue.Empty())
    return nullptr;

  return m_pimpl->messageQueue.PopFront();
}