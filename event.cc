
#include "event.h"

#include <time.h>
#include <errno.h>

namespace infra
{
namespace thread
{

Event::Event(bool signaled /* = false */)
 : signal_(signaled),
   reset_request_(false),
   waiting_threads_(0),
   mutex_(PTHREAD_MUTEX_INITIALIZER),
   cond_(PTHREAD_COND_INITIALIZER) {

}

void Event::Wait() {
  pthread_mutex_lock(&this->mutex_);

  /// 这里记录等待的线程数是为了在有信号的时候所有的之前等待的线程都会收到信号
  /// 之后才会复位。否则会发生其中的一个线程收到信号之后就复位了，其他等待的线程
  /// 就没有收到信号，这不符合事件的语义。
  ++this->waiting_threads_;
  while(!this->signal_) pthread_cond_wait(&this->cond_, &this->mutex_);
  --this->waiting_threads_;
  if(0 == this->waiting_threads_ && this->reset_request_) {
    this->signal_ = false;
    this->reset_request_ = false;
  }
  pthread_mutex_unlock(&this->mutex_);
}

bool Event::TimedWait(long timeout_ms) {
  int what = 0;
  pthread_mutex_lock(&this->mutex_);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec  += timeout_ms / 1000;
  ts.tv_nsec += (timeout_ms % 1000) * 1000 * 1000;
  ts.tv_sec  += ts.tv_nsec / (1000 * 1000 * 1000);
  ts.tv_nsec  = ts.tv_nsec % (1000 * 1000 * 1000);

  ++this->waiting_threads_;
  while(!this->signal_) {
    what = pthread_cond_timedwait(&this->cond_, &this->mutex_, &ts);
    if(ETIMEDOUT == what) break;
  }
  --this->waiting_threads_;

  /// what == 0 表示已经有信号了。
  if(0 == what &&
     0 == this->waiting_threads_ &&
     this->reset_request_) {
    this->signal_ = false;
    this->reset_request_ = false;
  }

  pthread_mutex_unlock(&this->mutex_);

  return 0 == what;
}

void Event::Signal() {
  pthread_mutex_lock(&this->mutex_);
  this->signal_ = true;
  pthread_cond_broadcast(&this->cond_);
  pthread_mutex_unlock(&this->mutex_);
}

void Event::Reset() {
  pthread_mutex_lock(&this->mutex_);
  if(0 == this->waiting_threads_) {
    this->signal_ = false;
    this->reset_request_ = false;
  } else {
    this->reset_request_ = true;
  }
  pthread_mutex_unlock(&this->mutex_);
}

}
}
