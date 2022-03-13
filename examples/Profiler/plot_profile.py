import json
import numpy as np
from matplotlib import pyplot as plt

FIGSIZE = (20,16)

def nicegrid():
    plt.grid(b=True, which='major', color='k', linestyle='-')
    plt.grid(b=True, which='minor', color='#606060', linestyle='-', alpha=0.2)
    plt.minorticks_on()


jsondata = """{
"3ffbfe9c":{"run":{"type":"log", "hz": 1000000, "runtime_us": 2066350926, "data":[0, 656, 1448, 2918, 5886, 11571, 13926, 0, 0, 1]},"wait":{"type":"lin", "div":5000, "hz": 1000000, "runtime_us": 2066353251, "data":[33820, 494, 377, 460, 280, 276, 180, 132, 170, 47, 89, 20, 50, 1, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]}}
}"""

data = json.loads(jsondata)

plt.figure( figsize=FIGSIZE )

print( "%d coroutines" % len(data))
for coroutine_name, profile_data in data.items():
    if not profile_data:
        continue
    for wait_run, profile in profile_data.items():

        print( profile )
        cycle_time = 1 / profile["hz"]
        runtime = profile["runtime_us"] * 1e-6
        histo   = profile["data"]

        if profile["type"] == "log":
            times = 2**np.arange( len( histo )) * cycle_time
        elif profile["type"] == "lin":
            times = np.arange( len( histo )) * cycle_time * profile["div"]

        hsum    = np.cumsum( histo )
        print( "coroutine %s runs %f times/s" % ( coroutine_name, hsum[-1] ))

        # plot according to bin edges

        if profile["type"] == "log":
            x = []
            sumy = []
            histoy = []
        elif profile["type"] == "lin":
            x = [0]
            sumy = [0]
            histoy = [0]

        for n in range( len( times )-1 ):
            x.append(times[n])
            sumy.append(hsum[n])
            histoy.append(histo[n])
            x.append(times[n+1])
            sumy.append(hsum[n])
            histoy.append(histo[n])

        x = np.array( x )
        x = 1000*x

        sumy = np.array( sumy )
        sumy = 1.0 - sumy/sumy[-1]

        histoy = np.array( histoy )
        histoy = histoy / histoy.sum()


        if profile["type"] == "log":
            plt.subplot( 2,2,1 )
            plt.loglog( x, histoy,   linewidth=3, label=coroutine_name )
            plt.subplot( 2,2,3 )
            plt.loglog( x, sumy,   linewidth=3, label=coroutine_name )
        elif profile["type"] == "lin":
            plt.subplot( 2,2,2 )
            plt.semilogy( x, histoy,   linewidth=3, label=coroutine_name )
            plt.subplot( 2,2,4 )
            plt.semilogy( x, sumy,   linewidth=3, label=coroutine_name )


plt.subplot( 2,2,1 )
nicegrid()
plt.title("Histogram runCoroutine() time")
plt.xlabel( "Time (ms)")

plt.subplot( 2,2,3 )
nicegrid()
plt.title("Probability of runCoroutine() longer than time on x-axis")
plt.xlabel( "Time (ms)")

plt.subplot( 2,2,2 )
nicegrid()
plt.title("Histogram DELAY() lateness")
plt.xlabel( "Time (ms)")

plt.subplot( 2,2,4 )
nicegrid()
plt.title("Probability of DELAY() being late by time on x-axis")
plt.xlabel( "Time (ms)")

plt.tight_layout()
plt.show()
