/**
 * ASE FOUNDATION IMPLEMENTATION
 *
 * @file        thread.cpp
 * @brief       pthread backing for ase::platform::Thread
 * @description Holds everything the header refuses to show: the pthread handle
 *              and the joined/valid bookkeeping.
 *
 *              THE TWO RULE CONFLICTS THIS FILE CARRIED UNTIL 2026-08-20 ARE
 *              GONE, AND NEITHER NEEDED AN EXCEPTION. They were named here as
 *              standing on purpose - an adapter with C linkage, and four raw
 *              allocation sites - with the reasoning that a C entry point
 *              cannot take a C++ interface and that ase::alloc is not reachable
 *              from a threading primitive. The first reason was true of the
 *              adapter but not of the file: the adapter existed only to bridge
 *              a return type, and matching the entry signature in ThreadFunc
 *              removed the adapter, and with it its linkage and the payload it
 *              carried. The second reason was true in its conclusion and wrong
 *              in its ground: ase::alloc is a bump arena whose memory is freed
 *              all at once by reset(), so it could not own a per-thread payload
 *              whatever the build files say - and that mattered not at all once
 *              the remaining two allocations, the PIMPL, were replaced by an
 *              opaque uint64_t in the header. Nothing is allocated here now.
 *
 *              WHAT A READER SHOULD CHECK BEFORE TRUSTING THE HANDLE CAST: the
 *              static_assert below. pthread_t is an integer type on glibc; on a
 *              platform where it is a pointer the static_cast fails to compile
 *              rather than truncating a handle at runtime, which is the failure
 *              direction this file wants.
 *
 *              Windows is an #error, not a stub: a threading primitive that
 *              silently does nothing is worse than one that refuses to build.
 *
 * @module      ase-platform
 * @layer       0 (Foundation)
 * @category    structure/synchronization/thread
 * @created     2026-04-16
 * @modified    2026-08-20
 * @version     2.0.0
 */

#include <ase/platform/thread.hpp>

#if defined(_WIN32)
#  error "ase::platform::Thread: Windows support pending (pthread-only impl currently)"
#endif

#include <pthread.h>

namespace ase::platform {

static_assert(sizeof(pthread_t) <= sizeof(uint64_t),
              "ase::platform::Thread keeps the platform thread handle in a uint64_t; this "
              "platform's pthread_t does not fit and the header needs a wider field");

Thread::Thread() noexcept = default;

Thread::Thread(ThreadFunc fn, void* arg) noexcept {
    if (fn == nullptr) { return; }

    pthread_t tid{};
    if (pthread_create(&tid, nullptr, fn, arg) == 0) {
        handle_ = static_cast<uint64_t>(tid);
        valid_  = true;
    }
}

Thread::~Thread() noexcept {
    if (valid_ && !joined_) {
        pthread_detach(static_cast<pthread_t>(handle_));
    }
}

Thread::Thread(Thread&& other) noexcept
    : handle_(other.handle_), joined_(other.joined_), valid_(other.valid_) {
    other.handle_ = 0;
    other.joined_ = false;
    other.valid_  = false;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    if (this != &other) {
        if (valid_ && !joined_) {
            pthread_detach(static_cast<pthread_t>(handle_));
        }
        handle_ = other.handle_;
        joined_ = other.joined_;
        valid_  = other.valid_;

        other.handle_ = 0;
        other.joined_ = false;
        other.valid_  = false;
    }
    return *this;
}

bool Thread::joinable() const noexcept {
    return valid_ && !joined_;
}

void Thread::join() noexcept {
    if (joinable()) {
        pthread_join(static_cast<pthread_t>(handle_), nullptr);
        joined_ = true;
    }
}

void Thread::detach() noexcept {
    if (joinable()) {
        pthread_detach(static_cast<pthread_t>(handle_));
        joined_ = true;
    }
}

}  // namespace ase::platform
