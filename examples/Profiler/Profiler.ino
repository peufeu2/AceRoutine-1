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
    delayMicroseconds(random(1000));
    COROUTINE_DELAY(10);
    digitalWrite(LED, LED_OFF);
    delayMicroseconds(random(1000));
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

// Linear histogram with 30 bins, 5000µs per bin, so 0-150 ms
ace_routine::LinearHistogramCoroutineProfiler wait_prof(30,5000);

// Log histogram to see how long it takes to blink a LED
ace_routine::Log2HistogramCoroutineProfiler run_prof(10);

void setup() {
  Serial.begin(115200);
  while (!Serial); // Leonardo/Micro
  pinMode(LED, OUTPUT);

  // Auto-register all coroutines into the scheduler.
  CoroutineScheduler::setup();

  blinkLed.setRunProfiler( &run_prof );
  blinkLed.setWaitProfiler( &wait_prof );

}

void loop() {
  CoroutineScheduler::loop();

  static unsigned next_print;
  unsigned m = micros();
  if( int(m - next_print) > 0 ) {
    next_print = m+2000000;
    CoroutineScheduler::printProfilingStats( Serial, false /* don't reset stats */ );
    Serial.printf(" cpu %d\n", getCpuFrequencyMhz() );
  }

}





