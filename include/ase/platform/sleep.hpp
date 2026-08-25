#pragma once

/**
 * ASE FOUNDATION HEADER
 *
 * @file        sleep.hpp
 * @brief       Suspend the calling thread - for a span, or until a monotonic deadline
 * @description The whole surface is two functions, and they are two OPERATIONS, not two
 *              spellings: sleep_nanos suspends for a SPAN, sleep_until_nanos suspends until
 *              an absolute CLOCK_MONOTONIC DEADLINE. Sleeping is an operation on a THREAD,
 *              not a reading of a clock, which is why both live here beside Thread and not
 *              in ase-utils next to the clock sources - a caller that sleeps does not learn
 *              what time it is.
 *
 *              NANOSECONDS, NAMED DIVISORS AT THE CALL SITE. There is deliberately no
 *              sleep_millis or sleep_seconds beside these: functions that differ only by a
 *              constant factor are that many places to get the factor wrong, and the caller
 *              already has ase::utils::NANOS_PER_MICRO / _MILLI / _SECOND to say which unit
 *              it means. `sleep_nanos(20 * NANOS_PER_MILLI)` reads as what it is;
 *              `sleep_millis(20)` hides the unit in the name, where no compiler checks it.
 *              The deadline form is NOT such a factor-variant: a relative sleep computed as
 *              `deadline - now` oversleeps by the scheduler latency on EVERY call and the
 *              error accumulates; the absolute form hands the kernel the deadline itself,
 *              so each frame's oversleep is corrected by the next one. A frame pacer that
 *              must hold its rate over hours uses the deadline form, never the span form.
 *
 *              WHAT IT REPLACES, measured 2026-08-20: std::this_thread::sleep_for with a
 *              std::chrono duration, which the rule set forbids, at three sites in
 *              core/ase-ecs - plus a fourth shape, an anonymous-namespace copy of the same
 *              nanosleep call in each of the three server main.cpp files. The primitive
 *              existed four times over and belonged in none of those places.
 *
 *              IT IS NOT A TIMER AND NOT A SCHEDULE. A system that wants to run every N
 *              milliseconds asks the scheduler for that, it does not sleep in its tick -
 *              the rule that forbids sleep() in ECS code means exactly this, and this
 *              header does not weaken it. The legitimate callers are the ones outside the
 *              tick: a boot sequence pacing its own output, a shutdown giving a thread time
 *              to end.
 *
 * @module      ase-platform
 * @layer       0 (Foundation)
 * @category    structure/synchronization/thread
 * @created     2026-08-20
 * @modified    2026-08-20
 * @version     1.0.0
 */

#include <cstdint>
#include <ctime>

namespace ase::platform {

/**
 * @brief Nanoseconds in a second, for splitting a span into the two timespec fields.
 *
 * ase::utils::NANOS_PER_SECOND carries the same value, and this is deliberately NOT taken
 * from there: it would make ase-platform bind a sibling Layer 0 module for one number,
 * and the number here is a property of the POSIX timespec split - tv_sec plus tv_nsec -
 * not of anybody's clock. Callers that want to SAY a unit use the ase-utils constants.
 */
constexpr uint64_t SLEEP_NANOS_PER_SECOND = 1000000000ULL;

/**
 * @brief Suspend the calling thread for at least `nanos` nanoseconds.
 * @param nanos How long to sleep. Zero returns immediately without a syscall.
 *
 * AT LEAST, never exactly: the kernel returns when it next schedules this thread, so the
 * actual span is the requested one plus whatever the scheduler adds. Anything that needs a
 * true deadline measures it with ase::utils::monotonic_nanos() instead of trusting this.
 *
 * A signal can cut the sleep short. This does not restart it - a caller that must not be
 * woken early has a requirement this primitive does not serve, and should say so rather
 * than looping here in silence.
 */
inline void sleep_nanos(uint64_t nanos) {
    if (nanos == 0) { return; }

    struct timespec ts {};
    ts.tv_sec  = static_cast<time_t>(nanos / SLEEP_NANOS_PER_SECOND);
    ts.tv_nsec = static_cast<long>(nanos % SLEEP_NANOS_PER_SECOND);
    ::nanosleep(&ts, nullptr);
}

/**
 * @brief Suspend the calling thread until an absolute CLOCK_MONOTONIC deadline.
 * @param deadline_nanos The deadline, in nanoseconds on CLOCK_MONOTONIC - the same clock
 *                       ase::utils::monotonic_nanos() reads, so its return values are valid
 *                       deadlines as-is. A deadline already in the past returns immediately.
 *
 * THIS IS THE FRAME-PACER FORM. A pacer that sleeps the RELATIVE remainder
 * (deadline - now) inherits the scheduler's wake-up latency as a fresh error on every
 * frame, and the errors add up - the loop drifts below its nominal rate for as long as it
 * runs. Handing the kernel the ABSOLUTE deadline makes each frame's oversleep shrink the
 * next frame's sleep by exactly that amount: the long-run rate is the nominal rate.
 *
 * A signal can cut the sleep short, exactly as with sleep_nanos above, and this does not
 * restart it either - the caller re-enters its loop, and a pacer's next iteration sleeps
 * to the SAME deadline family, so nothing is lost.
 */
inline void sleep_until_nanos(uint64_t deadline_nanos) {
    struct timespec ts {};
    ts.tv_sec  = static_cast<time_t>(deadline_nanos / SLEEP_NANOS_PER_SECOND);
    ts.tv_nsec = static_cast<long>(deadline_nanos % SLEEP_NANOS_PER_SECOND);
    ::clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, nullptr);
}

}  // namespace ase::platform
