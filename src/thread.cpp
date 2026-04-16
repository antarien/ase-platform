#include <ase/platform/thread.hpp>

#if defined(_WIN32)
#  error "ase::platform::Thread: Windows support pending (pthread-only impl currently)"
#endif

#include <pthread.h>

namespace ase::platform {

struct Thread::Impl {
    pthread_t handle;
    bool      joined  = false;
    bool      started = false;

    ThreadFunc fn   = nullptr;
    void*      arg  = nullptr;
};

namespace {

extern "C" void* trampoline(void* raw) {
    auto* impl = static_cast<Thread::Impl*>(raw);
    if (impl && impl->fn) {
        impl->fn(impl->arg);
    }
    return nullptr;
}

}  // namespace

Thread::Thread() noexcept = default;

Thread::Thread(ThreadFunc fn, void* arg) noexcept : impl_(new Impl{}) {
    impl_->fn  = fn;
    impl_->arg = arg;
    if (pthread_create(&impl_->handle, nullptr, &trampoline, impl_) == 0) {
        impl_->started = true;
    } else {
        delete impl_;
        impl_ = nullptr;
    }
}

Thread::~Thread() noexcept {
    if (impl_) {
        if (impl_->started && !impl_->joined) {
            pthread_detach(impl_->handle);
        }
        delete impl_;
        impl_ = nullptr;
    }
}

Thread::Thread(Thread&& other) noexcept : impl_(other.impl_) {
    other.impl_ = nullptr;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    if (this != &other) {
        if (impl_) {
            if (impl_->started && !impl_->joined) {
                pthread_detach(impl_->handle);
            }
            delete impl_;
        }
        impl_       = other.impl_;
        other.impl_ = nullptr;
    }
    return *this;
}

bool Thread::joinable() const noexcept {
    return impl_ && impl_->started && !impl_->joined;
}

void Thread::join() noexcept {
    if (joinable()) {
        pthread_join(impl_->handle, nullptr);
        impl_->joined = true;
    }
}

void Thread::detach() noexcept {
    if (joinable()) {
        pthread_detach(impl_->handle);
        impl_->joined = true;
    }
}

}  // namespace ase::platform
