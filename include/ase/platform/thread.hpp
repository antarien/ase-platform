#pragma once

namespace ase::platform {

using ThreadFunc = void (*)(void* arg);

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
    struct Impl;
    Impl* impl_ = nullptr;
};

}  // namespace ase::platform
