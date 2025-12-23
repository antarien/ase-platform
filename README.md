# ase-platform

[![Layer](https://img.shields.io/badge/Layer-0%20Foundation-blue.svg)]()
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)]()
[![Status](https://img.shields.io/badge/Status-Planned-yellow.svg)]()

> Cross-platform abstractions for OS-specific functionality

Part of [ASE - Antares Simulation Engine](../../..)

## Status

**Not Yet Implemented** - This module is a stub for future development.

## Planned Features

- **Threading**: Abstraction over `std::thread` with affinity/priority control
- **FileSystem**: Enhanced file operations beyond `std::filesystem`
- **Time**: High-resolution timers and monotonic clocks
- **SIMD**: Portable SIMD intrinsics (SSE, AVX, NEON)
- **Endianness**: Byte order conversion utilities
- **System Info**: CPU cores, memory, cache sizes

## Motivation

While C++20 provides many platform abstractions, certain features still require platform-specific code:

- Thread affinity (bind to specific CPU cores)
- Memory-mapped files
- High-precision timing
- SIMD intrinsics (SSE, AVX, NEON)
- Page-aligned allocation

This module will provide portable abstractions across Linux, macOS, and Windows.

## Planned Usage

```cpp
// Thread affinity (not yet implemented)
#include <ase/platform/thread.hpp>

using namespace ase::platform;

Thread worker([]() {
    // Worker logic
});
worker.set_affinity(2);  // Pin to CPU core 2
worker.set_priority(ThreadPriority::High);

// High-resolution timer (not yet implemented)
#include <ase/platform/time.hpp>

Timer timer;
timer.start();
// ... work ...
double elapsed_ms = timer.elapsed_ms();

// SIMD abstraction (not yet implemented)
#include <ase/platform/simd.hpp>

void process_vectors(float* a, float* b, float* out, size_t count) {
    // Automatically uses SSE/AVX/NEON based on platform
    simd::add_vec4(a, b, out, count);
}

// System information (not yet implemented)
#include <ase/platform/system.hpp>

size_t cpu_cores = System::logical_cpu_count();
size_t l1_cache = System::l1_cache_size();
size_t page_size = System::page_size();
```

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Linux (x64) | Planned | Primary development platform |
| macOS (ARM64) | Planned | Apple Silicon support |
| Windows (x64) | Planned | MSVC and MinGW |
| Web (WASM) | Future | Emscripten/WebAssembly |

## Dependencies

### External
- C++20 standard library
- Platform-specific APIs:
  - Linux: `pthread`, `sched.h`
  - macOS: `pthread`, `mach`
  - Windows: Win32 API

### Internal
- None (Layer 0 - Foundation)

## Design Principles

### Zero-Cost Abstraction
Platform abstractions should compile to the same code as direct platform API calls.

### Header-Only Where Possible
Prefer header-only implementations for simple wrappers.

### Fallback to STL
If platform-specific feature is unavailable, fall back to `std::` equivalent gracefully.

### Compile-Time Detection
Use `#ifdef` for platform detection, not runtime checks.

## References

- [Folly (Facebook)](https://github.com/facebook/folly) - Platform abstractions
- [abseil (Google)](https://github.com/abseil/abseil-cpp) - Time/thread utilities
- [EASTL Platform](https://github.com/electronicarts/EASTL) - Game engine platform layer

## Contributing

This module is planned but not yet implemented. If you need platform abstractions:
1. Use STL/Boost as temporary solution
2. Implement abstractions in this module following Layer 0 guidelines
3. Test on all target platforms (Linux, macOS, Windows)
4. Document platform-specific behavior

## License

Proprietary - ASE Engine

---

**Layer 0 Foundation** | No ASE dependencies | Planned
