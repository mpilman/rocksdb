//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.


#pragma once

namespace rocksdb { namespace port {

class MutexBase {
public:
  virtual ~MutexBase();
  virtual void Lock() = 0;
  virtual void Unlock() = 0;
  virtual void AssertHeld() = 0;
};

class RWMutexBase {
public:
  virtual ~RWMutexBase();
  virtual void ReadLock() = 0;
  virtual void WriteLock() = 0;
  virtual void ReadUnlock() = 0;
  virtual void WriteUnlock() = 0;
  virtual void AssertHeld() { }
};

class CondVarBase {
public:
    virtual ~CondVarBase();
    virtual void Wait() = 0;
    virtual bool TimedWait(uint64_t abs_time_us) = 0;
    virtual void Signal() = 0;
    virtual void SignalAll() = 0;
};

class Mutex;
extern MutexBase* (*createMutex)(bool);
extern RWMutexBase* (*createRWMutex)();
extern CondVarBase* (*createCondVarBase)(Mutex*);

class Mutex {
public:
#ifdef ROCKSDB_DEFAULT_TO_ADAPTIVE_MUTEX
  explicit Mutex(bool adaptive = true)
    : impl(createMutex(adaptive))
  {}
#else
  explicit Mutex(bool adaptive = false)
    : impl((*createMutex)(adaptive))
  {}
#endif
  ~Mutex() { delete impl; }
  void Lock() { impl->Lock(); }
  void Unlock() { impl->Unlock(); }
  void AssertHeld() { impl->AssertHeld(); }
  MutexBase* GetImpl() { return impl; }
private:
  MutexBase* impl;
};

class RWMutex {
public:
  RWMutex()
      : impl((*createRWMutex)())
  {}
  ~RWMutex() { delete impl; }
  RWMutexBase* GetImpl() { return impl; }
  void ReadLock() { impl->ReadLock(); }
  void WriteLock() { impl->WriteLock(); }
  void ReadUnlock() { impl->ReadUnlock(); }
  void WriteUnlock() { impl->WriteUnlock(); }
  void AssertHeld() { impl->AssertHeld(); }
private:
    RWMutexBase* impl;
};

class CondVar {
public:
  explicit CondVar(Mutex* mu)
      : impl(createCondVarBase(mu))
  {}
  ~CondVar() { delete impl; }

  void Wait() { impl->Wait(); }
  // Timed condition wait.  Returns true if timeout occurred.
  bool TimedWait(uint64_t abs_time_us) { return impl->TimedWait(abs_time_us); }
  void Signal() { impl->Signal(); }
  void SignalAll() { impl->SignalAll(); }
private:
    CondVarBase* impl;
};

}} // namespace port, rocksdb

