/*
 * HelloScheduler. Same as HelloCoroutine, but using the CoroutineScheduler.
 */

#include <Arduino.h>
#include <AceRoutine.h>
// using namespace ace_routine;

#ifdef LED_BUILTIN
  const int LED = LED_BUILTIN;
#else
  // Some ESP32 boards do not define LED_BUILTIN. Sometimes they have more than
  // 1. Replace this with the proper pin number.
  const int LED = 5;
#endif

const int LED_ON = HIGH;
const int LED_OFF = LOW;

/***** enable one of these three ******/
// with profiling
using Coroutine = ace_routine::CoroutineTemplate< ace_routine::Coroutine_Delay_32bit_Profiler_Impl< ace_routine::NamedCoroutine,ace_routine::ClockInterface >>;
using CoroutineScheduler = ace_routine::CoroutineSchedulerTemplate<Coroutine>;
using Profiler = ace_routine::Profiler;
Profiler *Profiler::root;

// without profiling, with 32 bit delays
// using Coroutine = ace_routine::CoroutineTemplate< ace_routine::Coroutine_Delay_32bit_Impl< ace_routine::NamedCoroutine,ace_routine::ClockInterface >>;
// using CoroutineScheduler = ace_routine::CoroutineSchedulerTemplate<Coroutine>;

// without profiling, with 16 bit delays
// using namespace ace_routine;


/**
 * Mr. Blinkie is trying to flash the LED with a short duration.
 */
COROUTINE(blinkLed) {
  COROUTINE_LOOP() {
    digitalWrite(LED, LED_ON);
    delayMicroseconds(random(10));
    COROUTINE_DELAY(10);
    digitalWrite(LED, LED_OFF);
    delayMicroseconds(random(10));
    COROUTINE_DELAY(100);
  }
}

/**
 * Mr. Serial is outputting stuff on the serial port, 
 * and doing complicated blocking things like web requests and crypto mining
 * (emulated for your convenience with a random delay())
 */
COROUTINE(printHelloWorld) {
  COROUTINE_LOOP() {
    unsigned d = 1 + random(10) * (2+random(8));
    Serial.printf( "the number is %d", d );
    delay( d );
    COROUTINE_DELAY(100);
    Serial.println();
    COROUTINE_DELAY(100);
  }
}

/**
 * Now profile them and see if the second coroutine interferes with the first.
 */

/*  Runtime of blinkLed should be 0-10µs, so for the sake of example,
    let's use a linear profiler.
    It uses microseconds as units by defaults, so let's use 
      first parameter: 30 histogram bine
      second parameter: each bin is 500µs
*/
ace_routine::LinearHistogramCoroutineProfiler run_prof( 30, 1 );

// ace_routine::Log2HistogramCoroutineProfiler run_prof(10);

/*  Wait time is measured as the difference between what was requested by
    COROUTINE_DELAY and what delay actually happened. So it won't plot
    the actual delay, rather how late it was compared to schedule.
    This makes it much more useful when a coroutine uses variable delays.

    Let's use a logarithmic histogram, which compresses a large
    dynamic range into few bins by taking the log2 of wait time.

    20 bins means it will record from 2^0 to 2^20 microseconds.
*/
ace_routine::Log2HistogramCoroutineProfiler wait_prof( 20 );


void setup() {
  Serial.begin(115200);
  while (!Serial); // Leonardo/Micro
  pinMode(LED, OUTPUT);

  // Auto-register all coroutines into the scheduler.
  CoroutineScheduler::setup();

  // give them names so they show up correctly in profiler output
  printHelloWorld.setName( "hello" );
  blinkLed.setName( "blinkLed" );

  /* set profilers manually */
  blinkLed.setRunProfiler( &run_prof );
  blinkLed.setWaitProfiler( &wait_prof );

  /* set profilers automatically for all coroutines */
  /*
  for (Coroutine** p = Coroutine::getRoot(); (*p) != nullptr; p = (*p)->getNext()) {
    (*p)->setRunProfiler( ace_routine::Log2HistogramCoroutineProfiler wait_prof( 20 ));
    (*p)->setWaitProfiler( ace_routine::Log2HistogramCoroutineProfiler wait_prof( 20 ));
  }
  */
}

void loop() {
  CoroutineScheduler::loop();

  static unsigned next_print;
  unsigned m = micros();
  if( int(m - next_print) > 0 ) {
    next_print = m+2000000;
    Profiler::printAllStats( Serial, false /* don't reset stats */ );
    Serial.printf(" cpu %d\n", getCpuFrequencyMhz() );
  }

}





