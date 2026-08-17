#pragma once

#include <cstdint>

/**
 * @brief Host-provided mutual exclusion for an SPI bus shared with other peripherals.
 *
 * On boards where the display and/or SD card sit on the same SPI bus as something the
 * host drives itself - a LoRa radio, say - the host owns the arbitration. Installing an
 * implementation here lets the UI take that lock around its own bus traffic, so the host
 * does not have to serialize whole UI cycles to stay safe.
 *
 * Hold time is what matters: the UI takes the lock only around actual transfers, never
 * across rendering, so the bus stays free during the CPU-only majority of a redraw.
 *
 * @note Implementations MUST be reentrant. UI call sites nest (a tile draw takes the
 * lock, and the SD read that feeds it takes it again), so a plain non-recursive mutex
 * will self-deadlock. Wrap one if that is all the host has - see the firmware's
 * tftSetup.cpp for a task-tracking example.
 *
 * When no implementation is installed the guards below are no-ops, which is correct for
 * hosts that do not share the bus.
 */
class ISpiLock
{
  public:
    virtual ~ISpiLock() = default;
    virtual void lock(void) = 0;
    virtual void lock(uint32_t timeout) {} // dummy will be removed by pr#314
    virtual void unlock(void) = 0;

    /**
     * Let a waiter in without giving up the enclosing critical section.
     *
     * Provided for an async-DMA flush: once the display driver can signal completion
     * asynchronously, the UI can release the bus while the transfer runs. Nothing calls
     * it yet, so the default - release and immediately re-take - is sufficient.
     */
    virtual void yield(void)
    {
        unlock();
        lock();
    }

    /**
     * Normally set through DeviceScreen::create(). Kept as a static holder because two
     * classes of call site cannot reach an instance member: the LVGL flush and touch
     * callbacks are static and have no `this`, and the SD layer (`sdCard`, `SDFs`) is
     * file-scope globals with no route to a DeviceGUI.
     */
    static void install(ISpiLock *lock);
    static ISpiLock *installed(void);

    /// RAII: hold the bus for the enclosing scope. Safe when nothing is installed.
    class Guard
    {
      public:
        Guard(void) : held(ISpiLock::installed())
        {
            if (held)
                held->lock();
        }
        ~Guard()
        {
            if (held)
                held->unlock();
        }
        Guard(const Guard &) = delete;
        Guard &operator=(const Guard &) = delete;

      private:
        ISpiLock *held;
    };
};
