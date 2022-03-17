namespace ace_routine {

/**
 *    Base class for the Profiler. This is just the interface.
 */
class Profiler {
  protected:
    /**
     * To profile stuff that isn't coroutines, this class also has a name,
     * a root singleton, and can enumerate all profilers in the system.
     */

    Profiler *next = nullptr;
    static Profiler *root;
    const char *name = nullptr;	// name of coroutine, if associated with one
    const char *type = nullptr;	// "run" or "wait"
    unsigned cycles_per_second = 1000000;

  public:
    Profiler() {
  		next = root;
  		root = this;
    }

    static Profiler* getRoot() { return root; }
    Profiler* getNext() { return next; }

    const char *getName() const { return name; }
    const char *getType() const { return type; }
    void begin( const char *_name, const char *_type, unsigned _cycles_per_second ) { 
    	name = _name; 
    	type = _type; 
    	cycles_per_second = _cycles_per_second;
    }

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
     * Prints accumulated profiling statistics for this profiler.
     */
    virtual void print( Print& printer ) =0;


    /**
     * Prints accumulated profiling statistics for all profilers.
     */
  	static void printAllStats( Print& printer, bool reset ) {
  		printer.print("[\n");
  		for (Profiler* p = getRoot(); p; ) {
  			p->printProfilingStats( printer, reset );
  			p = p->getNext();
  			if( p )
  			 	printer.print( "," );
  			printer.print( "\n" );
  		}
  		printer.print( "]\n" );
  	}        

    /**
     * Prints  for this profiler.
     */
    bool printProfilingStats( Print& printer, bool reset ) { 
  		if( name )
  			printer.printf( "{\"name\":\"%s\"", name );
  		else
  			printer.printf( "{\"name\":\"%x\"", this );

  		printer.printf( ", \"type\":\"%s\", ", (type ? type : "null") );

  		print( printer );
  		if( reset )
  			clear();

  		printer.print( "}");
  	}
};

/**
 * 	This Profiler class makes a histogram of measured intervals.
 * 
 * 	It is a virtual base class that will be derived into Log and Linear histograms.
 * 
 */
class HistogramCoroutineProfiler : public ace_routine::Profiler {
  protected:
  	unsigned nbins;		// number of bins in histogram
  	uint32_t *histo;	// number of times an interval was measured in bin histo[n]
  	uint32_t clear_time;

  	/**
  	 * Add one interval of length t.
  	 * This class doesn't know anything about the units of this number.
  	 */
  	virtual void add( uint32_t t ) =0;

  	/**
  	 * Reset histogram.
  	 */
  	void clear( ) {
  		for( unsigned i=0; i<nbins; i++ )
  			histo[i] = 0;
  		clear_time = millis();
  	}

  	/**
  	 * prints  histogram array in json format
  	 */
  	void print_hist( Print& printer ) {
  		printer.print( "[");
  		for( unsigned i=0; i<nbins; i++ ) {
  			if( i ) printer.print( ", ");
  			printer.print( histo[i] );
  		}
  		printer.print( "]");
  	}

  public:
  	/**
  	 * Allocates memory for nbins.
  	 */
  	HistogramCoroutineProfiler( unsigned _nbins ) {
  		nbins = _nbins;
  		histo = new uint32_t[nbins];
  		clear();
  	}

  	~HistogramCoroutineProfiler() { 
  		delete[] histo;
  	}

  	/**
  	 * Called by the coroutine itself after it has finished waiting, to report:
  	 * 	wait_micros: how long it waited
  	 * expected_wait_micros: how long it was supposed to wait
  	 * 
  	 * If variable delays are used, it would be pointless to plot how long it waited.
  	 * It is more interesting to plot the difference between the two above variables,
  	 * which represent how late it was versus its expected schedule.
  	 */
  	virtual void profileWait( unsigned long wait_micros, unsigned long expected_wait_micros ) {
  		add( wait_micros - expected_wait_micros );	// log the DIFFERENCE between requested delay and what we got
  	}

  	/**
  	 * Called by the coroutine itself after it has finished running, to report:
  	 * how many cycles it spend running the coroutine.
  	 * 
  	 * The duration of a cycle depends on what ClockInterfac class was used.
  	 */
  	virtual void profileRun ( unsigned long run_cycles  ) {
  		add( run_cycles );
  	}
};

/**
 * This is a simple linear histogram.
 * Each bin represents a number of microseconds.
 * 
 * setDivider sets the bin size, so for example if it is 1000, then the first
 * bin in histo[0] covers all time intervals between 0µs and 999 µs included, 
 * then... etc.
 */
class LinearHistogramCoroutineProfiler : public HistogramCoroutineProfiler {
public:
	// sets bin size
	void setDivider( unsigned div ) { 
		divider = div;
		clear(); 
	}

	LinearHistogramCoroutineProfiler( unsigned _nbins, unsigned _divider=1 ) 
		: HistogramCoroutineProfiler( _nbins ) {
			setDivider( _divider );
	}

protected:

	unsigned divider = 1;

	// increment a bin
	virtual void add( uint32_t t ) {
		t = t / divider;
		if( t < nbins )
			histo[t] ++;
		else
			histo[nbins-1] ++;
	}

	/**
	 * Output JSON
	 * {
	 * 	type: 	"lin",	for linear histogram
	 * 	div:	int,	divider, or bin size in cycles
	 * 	hz:		int,	cycle frequency
	 * 	runtime_ms:		long since it was last cleared, that can be used to calculate
	 * 					how many times the coroutine runs per second
	 * 	data: [...]		array of ints for histogram bins.
	 * 
	 * 
	 * 	data[n] is the histogram.
	 * 	Bin edges are:
	 * 	histo[0] = 0..(div-1) cycles
	 * 	histo[1] = div .. (2*div-1) cycles
	 *  etc
	 */
	virtual void print( Print& printer ) {
		printer.printf("\"hist\":\"lin\", \"div\":%d, \"hz\": %d, \"runtime_ms\": %d, \"data\":", divider, cycles_per_second, millis()-clear_time );
		print_hist( printer );
	}
};

/**
 * This is a log2 histogram.
 * Each bin represents twice the number of microseconds as the previous one.
 * This allows a wide dynamic range without using too many bins.
 */
class Log2HistogramCoroutineProfiler : public HistogramCoroutineProfiler {
public:
	Log2HistogramCoroutineProfiler( unsigned _nbins ) : HistogramCoroutineProfiler( _nbins ) {}
protected:

	// increment bin
	virtual void add( uint32_t t ) {
		for( unsigned i=0; i<nbins; i++ ) {		// compute log2
			t >>= 1;
			if( !t ) {
				if( histo[i] < UINT32_MAX ) 
					histo[i]++;
				return;
			} 
		}
		histo[nbins-1]++;
	}

	/**
	 * Output JSON
	 * {
	 * 	type: 	"log",	for linear histogram
	 * 	hz:		int,
	 * 	runtime_ms:		long since it was last cleared, that can be used to calculate
	 * 					how many times the coroutine runs per second
	 * 	data: [...]		array of ints for histogram bins.
	 * 
	 * 
	 * 	data[n] is the histogram.
	 * 	Bin edges are:
	 * 	histo[0] = 0-1 cycles
	 * 	histo[1] = 2-3 cycles
	 * 	histo[2] = 4-7 cycles
	 * 	histo[3] = 8-16 cycles
	 */
	virtual void print( Print& printer ) {
		printer.printf("\"hist\":\"log\", \"exp\":2, \"hz\": %d, \"runtime_ms\": %d, \"data\":", cycles_per_second, millis()-clear_time );
		print_hist( printer );
	}
};


#include <cmath>
/**
 * This is a log2 histogram.
 * Each bin represents twice the number of microseconds as the previous one.
 * This allows a wide dynamic range without using too many bins.
 */
class LogHistogramCoroutineProfiler : public HistogramCoroutineProfiler {
public:
  LogHistogramCoroutineProfiler( unsigned _nbins, float _exponent ) : HistogramCoroutineProfiler( _nbins ) { logexponent = 1.0/log( _exponent ); }
protected:
  float logexponent;
  // increment bin
  virtual void add( uint32_t t ) {
    int index = logexponent * log( 1+t );
    if( index < nbins )
      histo[index]++;
    else
      histo[nbins-1]++;
  }

  /**
   * Output JSON
   * {
   *  type:   "log",  for linear histogram
   *  hz:   int,
   *  runtime_ms:   long since it was last cleared, that can be used to calculate
   *          how many times the coroutine runs per second
   *  data: [...]   array of ints for histogram bins.
   * 
   * 
   *  data[n] is the histogram.
   *  Bin edges are:
   *  histo[0] = 0-1 cycles
   *  histo[1] = 2-3 cycles
   *  histo[2] = 4-7 cycles
   *  histo[3] = 8-16 cycles
   */
  virtual void print( Print& printer ) {
    printer.printf("\"hist\":\"log\", \"exp\":%f, \"hz\": %d, \"runtime_ms\": %d, \"data\":", exp(1.0/logexponent), cycles_per_second, millis()-clear_time );
    print_hist( printer );
  }
};


}