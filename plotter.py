import subprocess
import sys
import re
import matplotlib.pyplot as plt

if len(sys.argv) < 4:
    print("usage: python plotter.py <generator_exe> <kernel_exe> <runs> [gen_args...]")
    sys.exit(1)

gen_exe   = sys.argv[1]
kern_exe  = sys.argv[2]
runs      = int(sys.argv[3])
gen_args  = sys.argv[4:]

times = []

for i in range(runs):
    # start generator
    gen = subprocess.Popen(
        [gen_exe] + gen_args,
        stdout=subprocess.PIPE,
        text=True
    )

    # pipe into kernel
    kern = subprocess.run(
        [kern_exe],
        stdin=gen.stdout,
        capture_output=True,
        text=True
    )

    gen.stdout.close()

    m = re.search(r"time[:=]\s*([0-9.]+)", kern.stdout)
    if not m:
        print("No time found. Output was:\n", kern.stdout)
        sys.exit(1)

    times.append(float(m.group(1)))

# ---- plot ----
plt.plot(range(1, runs + 1), times, marker="o")
plt.xlabel("Run")
plt.ylabel("Time (seconds)")
plt.title("sum0 performance")
plt.grid(True)
plt.show()
