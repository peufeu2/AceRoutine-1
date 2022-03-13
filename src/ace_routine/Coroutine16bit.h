
/**
 *    16-bit delay implementation (uses less RAM)
 */
template <typename T_BASE, typename T_CLOCK>
class Coroutine_Delay_16bit_Impl : public T_BASE {
  public:

    unsigned long coroutineMillis()  const {   return T_CLOCK::millis();    }
    unsigned long coroutineMicros()  const {   return T_CLOCK::micros();    }
    unsigned long coroutineSeconds() const {   return T_CLOCK::seconds();   }

    /** Check if delay millis time is over. */
    bool isDelayExpired() const {
      uint16_t nowMillis = coroutineMillis();
      uint16_t elapsed = nowMillis - mDelayStart;
      return elapsed >= mDelayDuration;
    }

    /** Check if delay micros time is over. */
    bool isDelayMicrosExpired() const {
      uint16_t nowMicros = coroutineMicros();
      uint16_t elapsed = nowMicros - mDelayStart;
      return elapsed >= mDelayDuration;
    }

    /** Check if delay seconds time is over. */
    bool isDelaySecondsExpired() const {
      uint16_t nowSeconds = coroutineSeconds();
      uint16_t elapsed = nowSeconds - mDelayStart;
      return elapsed >= mDelayDuration;
    }

    /**
     * Configure the delay timer for delayMillis.
     *
     * The maximum duration is set to (UINT16_MAX / 2) (i.e. 32767
     * milliseconds) if given a larger value. This makes the longest allowable
     * time between two successive calls to isDelayExpired() for a given
     * coroutine to be 32767 (UINT16_MAX - UINT16_MAX / 2 - 1) milliseconds,
     * which should be long enough for all practical use-cases. (The '- 1'
     * comes from an edge case where isDelayExpired() evaluates to be true in
     * the CoroutineScheduler::runCoroutine() but becomes to be false in the
     * COROUTINE_DELAY() macro inside Coroutine::runCoroutine()) because the
     * clock increments by 1 millisecond.)
     */
    void setDelayMillis(uint16_t delayMillis) {
      mDelayStart = coroutineMillis();

      // If delayMillis is a compile-time constant, the compiler seems to
      // completely optimize away this bounds checking code.
      mDelayDuration = (delayMillis >= UINT16_MAX / 2)
          ? UINT16_MAX / 2
          : delayMillis;
    }

    /**
     * Configure the delay timer for delayMicros. Similar to seDelayMillis(),
     * the maximum delay is 32767 micros.
     */
    void setDelayMicros(uint16_t delayMicros) {
      mDelayStart = coroutineMicros();

      // If delayMicros is a compile-time constant, the compiler seems to
      // completely optimize away this bounds checking code.
      mDelayDuration = (delayMicros >= UINT16_MAX / 2)
          ? UINT16_MAX / 2
          : delayMicros;
    }

    /**
     * Configure the delay timer for delaySeconds. Similar to seDelayMillis(),
     * the maximum delay is 32767 seconds.
     */
    void setDelaySeconds(uint16_t delaySeconds) {
      mDelayStart = coroutineSeconds();

      // If delaySeconds is a compile-time constant, the compiler seems to
      // completely optimize away this bounds checking code.
      mDelayDuration = (delaySeconds >= UINT16_MAX / 2)
          ? UINT16_MAX / 2
          : delaySeconds;
    }

    /**
     * Configures zero delay.
     * This seems useless, but if the profiler is enabled, this will record
     * the current timestamp.
     */
    void setDelayZero() {}

    /**
     * ...and if the profiler is enabled, this will look at the timestamp
     * recorded by setDelay() and thus the profiler will know how long we
     * waited in that YIELD.
     */
    void profileEnterZero() {}
    void profileEnterMillis () {}
    void profileEnterMicros () {}
    void profileEnterSeconds() {}
    void profileExit( uint16_t old_mDelayStart ) {}

  protected:
    /**
     * Start time provided by COROUTINE_DELAY(), COROUTINE_DELAY_MICROS(), or
     * COROUTINE_DELAY_SECONDS(). The unit of this number is context dependent,
     * milliseconds, microseconds, or seconds.
     */
    uint16_t mDelayStart;

    /**
     * Delay time specified by COROUTINE_DELAY(), COROUTINE_DELAY_MICROS() or,
     * COROUTINE_DELAY_SECONDS(). The unit of this number is context dependent,
     * milliseconds, microseconds, or seconds.
     */
    uint16_t mDelayDuration;
};

