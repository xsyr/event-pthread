/**
 * @file event.h
 * @brief 这个类实现了类似 Windows Event的功能。Event
 * 定义了一个事件，初始时默认为无信号状态。可以通过构造函数的参数修改这个默认
 * 行为。
 * @author xsyr
 * @version 0.1
 * @date 2013-12-30
 */

#ifndef INFRA_THREADING_EVENT_H_
#define INFRA_THREADING_EVENT_H_

#include <pthread.h>

namespace infra
{

namespace thread
{

/**
 * @brief 用法：
 * 
 * @code 
 *
 * infra::thread::Event e;
 *
 * // 等待信号
 * e.Wait();
 * ...
 * e.TimedWait(5s);
 * ...
 * e.Signal();
 * ...
 * e.Reset();
 *
 * @endcode
 */
class Event
{
 public:

  /**
   * @brief 
   *
   * @param signaled 信号状态。默认位没有信号。 true 表示有信号，false
   * 为无信号。
   */
   Event(bool signaled = false);

  /**
   * @brief 无限等待直到事件为有信号的状态，可以使用 Signal()
   * 设置为有信号的状态。如果当前已经处于有信号的状态，则会立即返回。
   */
  virtual void Wait();

  /**
   * @brief 等待直到有信号或者超时。
   *
   * @param timeout_ms 等待的时长，单位为 ms。
   *
   * @return true 表示有信号，false 表示超时返回。
   */
  virtual bool TimedWait(long timeout_ms);

  /**
   * @brief 将事件置为有信号的状态。如果当前已经时有信号的状态，则什么事都不做。
   */
  virtual void Signal();

  /**
   * @brief 将事件置为无信号的状态。如果当前已经时无信号的状态，则什么事都不做。
   * 复位时，所有之前等待的线程都收到信号。
   */
  virtual void Reset();

 protected:
  bool            signal_;
  bool            reset_request_;
  unsigned int    waiting_threads_; /// 记录当前等待的线程数是为了实现 Reset 的语义。
  pthread_mutex_t mutex_;
  pthread_cond_t  cond_;

 private:
  Event(const Event& other);
  Event& operator=(const Event& other);
};

} /// end-ns-threading

} /// end-ns-infra

#endif /* end of include guard: INFRA_EVENT_H_ */

