#ifndef ACE_ROUTINE_COROUTINE_32BIT_H
#define ACE_ROUTINE_COROUTINE_32BIT_H

namespace ace_routine {

/**
 *    This is one possible base class for Coroutine.
 * 
 *    32-bit delay implementation (uses 4 bytes more RAM)
 *    More flexible if the delay is calculated in the coroutine
 *    as setDelayMillis() is no longer limited to 32767 milliseconds, 
 *    instead being limited to 2^31 us, or 2147 seconds. 
 *    Basically this avoids if()'s in the coroutine to call DELAY_SECONDS
 *    if the value is too large to fit in the 16-bit millisecond delay.
 */
template <typename T_BASE, typename T_CLOCK>
class Coroutine_Delay_32bit_Impl : public T_BASE  {
  public:
    unsigned long coroutineMillis()  const {   return T_CLOCK::millis();    }
    unsigned long coroutineMicros()  const {   return T_CLOCK::micros();    }
    unsigned long coroutineSeconds() const {   return T_CLOCK::seconds();   }

    /**
     *    All functions store delays as 32 bit micros.
     */
    bool isDelayExpired() const {
      uint32_t nowMicros = coroutineMicros();
      uint32_t elapsed = nowMicros - mDelayStart;
      return elapsed >= mDelayDuration;
    }
    bool isDelayMicrosExpired() const {
      uint32_t nowMicros = coroutineMicros();
      uint32_t elapsed = nowMicros - mDelayStart;
      return elapsed >= mDelayDuration;
    }
    bool isDelaySecondsExpired() const {
      uint32_t nowMicros = coroutineMicros();
      uint32_t elapsed = nowMicros - mDelayStart;
      return elapsed >= mDelayDuration;
    }

    /**
     *    All functions store delays as 32 bit micros.
     */
    void setDelayMillis(uint16_t delayMillis) {
      mDelayStart = coroutineMicros();
      mDelayDuration = (delayMillis >= UINT32_MAX / 2000)
                      ? UINT32_MAX / 2
                      : delayMillis*1000;
    }
    void setDelayMicros(uint32_t delayMicros) {
      mDelayStart = coroutineMicros();
      mDelayDuration = (delayMicros >= UINT32_MAX / 2)
          ? UINT32_MAX / 2
          : delayMicros;
    }
    void setDelaySeconds(uint16_t delaySeconds) {
      mDelayStart = coroutineMicros();
      mDelayDuration = (delaySeconds >= UINT32_MAX / 2000000)
                      ? UINT32_MAX / 2
                      : delaySeconds*1000000;
    }

    /**
     *    Profiler functions are in the next class.
     */
    void setDelayZero() {}
    void profileEnterMillis ( ) {}
    void profileEnterMicros ( ) {}
    void profileEnterSeconds( ) {}
    void profileEnterZero() {}
    void profileExit( ) {}

  protected:
    uint32_t mDelayStart;
    uint32_t mDelayDuration;
};

/**
 *    Base class for the Profiler. This is just the interface.
 *    Implementation in Profiler.h
 */
class CoroutineProfilerBase {
  public:
    /**
     * Called at the end of a delay, as the coroutine switches 
     * from Run to Yield/Delay state.
     * 
     * Parameters are the actual delay that occured,
     * and the delay that was requested by the coroutine. 
     * 
     * The profiler can do whatever it wants with that, for 
     * example make statistics about the difference between
     * the two.  
     */
    virtual void profileWait( unsigned long wait_micros, unsigned long expected_wait_micros ) =0;

    /**
     * Called as the coroutine switches from Yield/Delay state to Run.
     * 
     * Parameters are the time spent running this iteration of the
     * coroutine. These come from ClockInterface::cycles, so it can be 
     * CPU cycles, microseconds, etc.
     */
    virtual void profileRun ( unsigned long run_cycles  ) =0;

    /**
     * Clears accumulated profiling statistics.
     */
    virtual void clear() =0;

    /**
     * Prints accumulated profiling statistics.
     */
    virtual void print( Print& printer, unsigned long cycles_per_second ) =0;
};

/**
 *    This is one possible base class for Coroutine.
 * 
 *    Same 32-bit delay implementation above.
 * 
 *    This adds Profiling features.
 * 
 */
template <typename T_BASE, typename T_CLOCK>
class Coroutine_Delay_32bit_Profiler_Impl: public Coroutine_Delay_32bit_Impl<T_BASE,T_CLOCK> {
  protected:

    /**
     * A profiler instance accumulates statistics about time intervals, so
     * we can use two instances to make statistics about
     * the run time and the wait time of the same coroutine.
     * 
     * If these pointers are null, then no profiling occurs on that
     * coroutine.
     */
    CoroutineProfilerBase *mWaitProfiler = nullptr;
    CoroutineProfilerBase *mRunProfiler = nullptr;

  public:

    void setWaitProfiler( CoroutineProfilerBase *profiler ) { mWaitProfiler = profiler; }
    void setRunProfiler ( CoroutineProfilerBase *profiler ) { mRunProfiler  = profiler; }

    CoroutineProfilerBase* getWaitProfiler( ) { return mWaitProfiler; }
    CoroutineProfilerBase* getRunProfiler ( ) { return mRunProfiler; }

    bool printProfilingStats( Print& printer ) { 
      printer.print( "\"" );
      if( this->getName() )
        printer.print( this->getName() );
      else
        printer.printf( "%x", this );
      printer.print( "\":{");
      if( this->mRunProfiler ) {
        printer.print("\"run\":" );
        this->mRunProfiler->print( printer, T_CLOCK::cycles_per_second() );
        if( this->mWaitProfiler )
          printer.print(",");
      }
      if( this->mWaitProfiler ) {
        printer.print("\"wait\":" );
        this->mWaitProfiler->print( printer, 1000000 );
      }
      printer.print( "}");
      return true;
    }
    void clearProfilingStats( ) { 
      if( this->mRunProfiler )
        this->mRunProfiler->clear();
      if( this->mWaitProfiler )
        this->mWaitProfiler->clear();
    }


    /**
     * Configures zero delay.
     * This seems useless, but if the profiler is enabled, this will record
     * the current timestamp.
     */
    void setDelayZero() {
      this->mDelayStart = this->coroutineMicros();
    }

    /**
     * ...and if the profiler is enabled, this will look at the timestamp
     * recorded by setDelay() and thus the profiler will know how long we
     * waited in that YIELD.
     */
    void profileEnterMicros ( ) { profileEnterZero(); }
    void profileEnterMillis ( ) { profileEnterZero(); }
    void profileEnterSeconds( ) { profileEnterZero(); }

    void profileEnterZero() { 
      if( mWaitProfiler )
        mWaitProfiler->profileWait( this->coroutineMicros()-this->mDelayStart, this->mDelayDuration );

      /** 
       * mDelayStart is only used when coroutine is in Delay/Yield state. 
       * Reuse it in Run state to store timestamp when it entered Run state
       * And take another sample of micros() here to avoid counting the time spent
       * in the profiler function, which may print stuff to serial
       */
      this->mDelayStart = T_CLOCK::cycles();
    }

    /**
     *  When the coroutine entered Run state, the macros above stored the timestamp in
     *  mDelayStart. Then, when it is about to enter Delay/Yield state, the macros move
     *  this value to old_mDelayStart, setup the delay and the current timestamp into
     *  mDelayStart. So, we don't need to call micros() again.
     */
    void profileExit( ) {
      unsigned long ticks = T_CLOCK::cycles();
      if( mRunProfiler )
        mRunProfiler->profileRun( ticks - this->mDelayStart );
    }
};

}

#endif
