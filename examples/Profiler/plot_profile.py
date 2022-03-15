import json, pprint
import numpy as np
from matplotlib import pyplot as plt
import urllib.request

FIGSIZE = (20,16)
FIGSIZE = (10,8)

def nicegrid():
    plt.grid(b=True, which='major', color='k', linestyle='-')
    plt.grid(b=True, which='minor', color='#606060', linestyle='-', alpha=0.2)
    plt.minorticks_on()

# geab json profile data from ESP32
if 0:
    print("req")
    jsondata = urllib.request.urlopen("http://192.168.0.16/coro", timeout=2).read().decode("utf-8")
    print( jsondata )
else:   # use example data
    jsondata = """[
{"name":"blinkLed", "type":"wait", "hist":"log", "exp":2, "hz": 1000000, "runtime_ms": 352565, "data":[2069, 2487, 1052, 3, 0, 0, 0, 1, 0, 20, 26, 17, 92, 124, 161, 132, 3, 0, 0, 0]},
{"name":"blinkLed", "type":"run", "hist":"lin", "div":1, "hz": 1000000, "runtime_ms": 352573, "data":[0, 0, 398, 728, 593, 605, 657, 628, 512, 634, 734, 389, 306, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]}
]"""

# deserialize
data = json.loads(jsondata)

# format data and group by coroutine
# this is easier to do here than in C++
coros = {}
for d in data:
    coros.setdefault( d["name"], {} ) [d["type"]] = d

n_coroutines = len(coros)
print( "%d coroutines" % n_coroutines )

plt.figure( figsize=FIGSIZE )

plots_per_row = 4
nplots = n_coroutines
columns = 1+nplots//plots_per_row
rows    = nplots if nplots <= plots_per_row else plots_per_row
xlim    = 1e-4, 1e3
ylim    = 1e-6, 1

mode_cumsum = True
mode_probability = True

for coroutine_index, (coroutine_name, profile_data) in enumerate( sorted( coros.items(), key=lambda p:p[0].lower() )):
    # if coroutine_name not in ("ow","ows"):
        # continue
    for wait_run, profile in profile_data.items():

        cycle_time = 1 / profile["hz"]
        runtime = profile["runtime_ms"] * 1e-3  # in seconds
        histo   = profile["data"]
        hist_type = profile["hist"]

        if hist_type == "log":
            times = profile["exp"]**np.arange( len( histo )) * cycle_time
        elif hist_type == "lin":
            times = np.arange( len( histo )) * cycle_time * profile["div"]

        hsum    = np.cumsum( histo )
        print( coroutine_name, wait_run, hsum[-1], " runs" )
        runs_per_sec = hsum[-1]/runtime
        print( hsum )
        print( times )

        # each bin does not contain one time, but a range of times.
        # make a plot that reflects that, instead of just a line.
        if hist_type == "log":
            x = []
            sumy = []
            histoy = []
        elif hist_type == "lin":
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

        x = 1e3 * np.array( x )         # format time in ms
        histoy = np.array( histoy )     # histogram of times
        sumy = np.array( sumy )
        sumy = sumy[-1]-sumy            # cumulative sum

        if mode_probability:
            sumy = sumy/sumy[0]
            histoy = histoy / histoy.sum()
            ylabel = "Probability of"
        else:
            ylabel = "Number of runs with"

        plt.subplot( rows, columns, coroutine_index+1 )

        if wait_run == "wait":
            color = "#FF8000"
        else:
            color = "#0080FF"

        if mode_cumsum:
            y = sumy
            plt.ylabel( ylabel + " time < X axis" )
        else:
            y = histoy
            plt.ylabel("Histogram of %s time on X axis" % ylabel )

        plt.title( "%s: %.01f Hz" % (coroutine_name, runs_per_sec))
        nicegrid()

        plt.loglog( x, y,   linewidth=3, label=wait_run, color=color )
        if coroutine_index == 0:
            plt.legend()

        plt.xlabel("Time (ms)")
        plt.legend()

# plt.subplot( 2,2,1 )
# plt.title("Histogram runCoroutine() time")
# plt.xlabel( "Time (ms)")

# plt.subplot( 2,2,3 )
# nicegrid()
# plt.title("Probability of runCoroutine() longer than time on x-axis")
# plt.xlabel( "Time (ms)")

# plt.subplot( 2,2,2 )
# nicegrid()
# plt.title("Histogram DELAY() lateness")
# plt.xlabel( "Time (ms)")

# plt.subplot( 2,2,4 )
# nicegrid()
# plt.title("Probability of DELAY() being late by time on x-axis")
# plt.xlabel( "Time (ms)")

plt.tight_layout()
plt.show()
