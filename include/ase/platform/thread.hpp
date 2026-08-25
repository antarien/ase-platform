#pragma once

/**
 * ASE FOUNDATION HEADER
 *
 * @file        thread.hpp
 * @brief       One OS thread, owned by an ase::platform::Thread object
 * @description The whole threading surface of Layer 0: construct with a plain
 *              function pointer and a void*, then join or detach. Move-only,
 *              because a thread has exactly one owner; copying one would leave
 *              two objects believing they may join it.
 *
 *              THE HANDLE IS AN OPAQUE INTEGER, NOT A POINTER TO A HIDDEN
 *              STRUCT. The goal is unchanged from the first version: the
 *              pthread handle must not appear in this header, so nothing above
 *              Layer 0 sees the backing thread API and no consumer rebuilds
 *              when it changes - the .cpp errors out on Windows today. A PIMPL
 *              reached that goal at the price of one heap allocation per
 *              thread; an opaque uint64_t reaches it at no price. thread.cpp
 *              static_asserts that the platform handle fits, so a platform
 *              whose handle does not is a compile error there, never a silent
 *              truncation here.
 *
 *              ThreadFunc is a raw function pointer rather than a callable:
 *              a foundation primitive cannot depend on std::function, and the
 *              void* argument is what a thread entry point can carry.
 *
 *              IT RETURNS void*, AND THAT IS WHAT REMOVED THE ADAPTER. The OS
 *              entry point returns void*; a ThreadFunc returning void differs
 *              from it in exactly that one respect, and bridging that single
 *              difference cost an adapter function with C linkage plus a
 *              heap-allocated payload to carry fn and arg across the thread
 *              boundary, plus the ownership rule for that payload. Matching
 *              the entry signature hands the caller's function to the OS
 *              directly: no adapter, no allocation, no ownership question.
 *              A caller with nothing to report returns nullptr.
 *
 * @module      ase-platform
 * @layer       0 (Foundation)
 * @category    structure/synchronization/thread
 * @created     2026-04-16
 * @modified    2026-08-20
 * @version     2.0.0
 */

#include <cstdint>

namespace ase::platform {

using ThreadFunc = void* (*)(void* arg);

class Thread {
public:
    Thread() noexcept;
    Thread(ThreadFunc fn, void* arg) noexcept;
    ~Thread() noexcept;

    Thread(const Thread&)            = delete;
    Thread& operator=(const Thread&) = delete;
    Thread(Thread&&) noexcept;
    Thread& operator=(Thread&&) noexcept;

    bool joinable() const noexcept;
    void join() noexcept;
    void detach() noexcept;

private:
    uint64_t handle_ = 0;      // Platform thread handle, meaningful only while valid_
    bool     joined_ = false;  // Joined or detached already - the thread is no longer ours
    bool     valid_  = false;  // A thread was actually started
};

}  // namespace ase::platform
