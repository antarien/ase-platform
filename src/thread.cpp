#include <ase/platform/thread.hpp>

#if defined(_WIN32)
#  error "ase::platform::Thread: Windows support pending (pthread-only impl currently)"
#endif

#include <pthread.h>

namespace ase::platform {

struct Thread::Impl {
    pthread_t handle{};
    bool      joined{false};
    bool      valid{false};
};

namespace {

// Trampoline payload is private to thread.cpp — Thread::Impl stays opaque
// behind the PIMPL, and the pthread-facing function only needs the
// captured ThreadFunc + arg.  Ownership: the trampoline frees the payload
// after fn returns; if pthread_create fails the constructor frees it.
struct TrampolinePayload {
    ThreadFunc fn  = nullptr;
    void*      arg = nullptr;
};

extern "C" void* pthread_trampoline(void* raw) {
    auto* payload = static_cast<TrampolinePayload*>(raw);
    if (payload && payload->fn) {
        payload->fn(payload->arg);
    }
    delete payload;
    return nullptr;
}

}  // namespace

Thread::Thread() noexcept = default;

Thread::Thread(ThreadFunc fn, void* arg) noexcept : impl_(new Impl{}) {
    auto* payload = new TrampolinePayload{fn, arg};
    if (pthread_create(&impl_->handle, nullptr, &pthread_trampoline, payload) == 0) {
        impl_->valid = true;
    } else {
        delete payload;
    }
}

Thread::~Thread() noexcept {
    if (impl_) {
        if (impl_->valid && !impl_->joined) {
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
            if (impl_->valid && !impl_->joined) {
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
    return impl_ && impl_->valid && !impl_->joined;
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
