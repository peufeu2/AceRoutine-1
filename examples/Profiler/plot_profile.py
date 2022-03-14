import json, pprint
import numpy as np
from matplotlib import pyplot as plt
import urllib.request

FIGSIZE = (20,16)

def nicegrid():
    plt.grid(b=True, which='major', color='k', linestyle='-')
    plt.grid(b=True, which='minor', color='#606060', linestyle='-', alpha=0.2)
    plt.minorticks_on()

jsondata = """{
{"name":"leds", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 112095788, "data":[0, 0, 1, 0, 1, 0, 3, 5, 9, 16, 421, 634, 1, 0, 0, 0, 0, 0, 1, 0]},
}"""

# geab json profile data from ESP32
if 1:
    print("req")
    jsondata = urllib.request.urlopen("http://192.168.0.16/coro", timeout=2).read().decode("utf-8")
    print( jsondata )
else:   # use example data
    jsondata = """[
{"name":"leds", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6866936, "data":[0, 0, 0, 0, 0, 0, 0, 1, 5, 11, 19, 26, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"leds", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6853720, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 34, 9, 0, 5, 0, 0, 0, 0]},
{"name":"wifi", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6840654, "data":[0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"wifi", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6827609, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"onewire", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6813849, "data":[0, 1, 0, 0, 2, 2, 13, 8, 55, 49, 25, 24, 2, 2, 0, 0, 0, 0, 1, 0]},
{"name":"onewire", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6800087, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 174, 0, 0, 10, 0, 0]},
{"name":"relays", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6788051, "data":[0, 2, 1, 2, 5, 8, 20, 23, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0]},
{"name":"relays", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6774679, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 22, 0, 0, 0, 0, 0, 0, 0]},
{"name":"fault", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6761501, "data":[0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"fault", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6748371, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0]},
{"name":"demande", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6735043, "data":[0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"demande", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6721738, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"idle", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6708672, "data":[0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"idle", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6696024, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"et_bureau", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6682549, "data":[0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"et_bureau", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6669046, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"pcbt_pc", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6655721, "data":[0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"pcbt_pc", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6642391, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"pcbt_pf", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6629087, "data":[0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0]},
{"name":"pcbt_pf", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6615759, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0]},
{"name":"pv", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6603659, "data":[0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"pv", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6590764, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 3, 0, 0, 0, 1, 0, 1, 1]},
{"name":"autoconso", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6577264, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"autoconso", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6563786, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"ui", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6550805, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 1, 0, 0, 0, 0, 0, 0, 5]},
{"name":"ui", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6537909, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 2, 3, 10, 5, 0, 0, 0, 0, 0]},
{"name":"keyb", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6524961, "data":[1, 1, 0, 3, 8, 49, 104, 105, 154, 55, 19, 79, 1, 5, 0, 0, 0, 0, 1, 5]},
{"name":"keyb", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6511723, "data":[0, 0, 0, 0, 5, 0, 585, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"bench", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6497267, "data":[1573, 259, 0, 1, 1, 0, 0, 30462, 678, 23, 11, 0, 1, 6, 1, 0, 0, 0, 1, 0]},
{"name":"bench", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6483704, "data":[0, 0, 0, 0, 32917, 82, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"GrugHTTP", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6470033, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1828]},
{"name":"GrugHTTP", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6456097, "data":[0, 0, 0, 0, 245, 8, 4, 1, 0, 0, 0, 1505, 59, 0, 0, 6, 0, 0, 0, 0]},
{"name":"GrugRequest", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6442252, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 254, 1, 1, 0, 0, 0, 0, 1, 5]},
{"name":"GrugRequest", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6428580, "data":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 242, 11, 0, 0, 2, 0, 0, 0, 6]},
{"name":"loop", "type":"run", "hist":"log", "hz": 80000000, "runtime_ms": 6414995, "data":[0, 0, 0, 30649, 525, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0]},
{"name":"loop", "type":"wait", "hist":"log", "hz": 1000000, "runtime_ms": 6401062, "data":[0, 0, 0, 0, 0, 0, 0, 30454, 689, 19, 11, 0, 1, 6, 1, 0, 0, 0, 1, 0]}
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


for coroutine_index, (coroutine_name, profile_data) in enumerate( sorted( coros.items(), key=lambda p:p[0].lower() )):
    for wait_run, profile in profile_data.items():

        cycle_time = 1 / profile["hz"]
        runtime = profile["runtime_ms"] * 1e-3
        histo   = profile["data"]
        hist_type = profile["hist"]

        if hist_type == "log":
            times = profile["exp"]**np.arange( len( histo )) * cycle_time
        elif hist_type == "lin":
            times = np.arange( len( histo )) * cycle_time * profile["div"]

        hsum    = np.cumsum( histo )
        runs_per_sec = hsum[-1]/runtime

        # plot according to bin edges
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

        x = np.array( x )
        x = 1000*x
        histoy = np.array( histoy )
        sumy = np.array( sumy )

        sumy = sumy[-1]-sumy

        # sumy = 1.0 - sumy/sumy[-1]
        # histoy = histoy / histoy.sum()

        plt.subplot( rows, columns, coroutine_index+1 )

        if hist_type == "log":
            if wait_run == "wait":
                color = "#FF8000"
            else:
                color = "#0080FF"
            plt.loglog( x, sumy,   linewidth=3, label=wait_run, color=color )
            plt.title( "%s: %.01f Hz" % (coroutine_name, runs_per_sec))
            plt.xlim( xlim )
            # plt.ylim( ylim )
            if coroutine_index == 0:
                plt.legend()
            nicegrid()
            # plt.subplot( 2,2,3 )
            # plt.loglog( x, sumy,   linewidth=3, label=coroutine_name )
        # elif hist_type == "lin":
            # plt.subplot( 2,2,2 )
            # plt.semilogy( x, histoy,   linewidth=3, label=coroutine_name )
            # plt.subplot( 2,2,4 )
            # plt.semilogy( x, sumy,   linewidth=3, label=coroutine_name )




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
