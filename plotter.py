import subprocess
import sys
import re
import statistics
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
    gen = subprocess.Popen(
        [gen_exe] + gen_args,
        stdout=subprocess.PIPE,
        text=True
    )

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

# ---- statistics ----
mean   = statistics.mean(times)
median = statistics.median(times)
stdev  = statistics.stdev(times) if len(times) > 1 else 0.0
best   = min(times)
worst  = max(times)

# ---- plot ----
plt.figure(figsize=(9, 5))
plt.plot(range(1, runs + 1), times, marker="o", label="Run time")

# reference lines
plt.axhline(mean,   linestyle="--", label=f"Mean:   {mean:.6f}s")
plt.axhline(median, linestyle=":",  label=f"Median: {median:.6f}s")
plt.axhline(best,   linestyle="-.", label=f"Best:   {best:.6f}s")

# text box
stats_text = (
    f"Runs:   {runs}\n"
    f"Mean:   {mean:.6f}s\n"
    f"Median: {median:.6f}s\n"
    f"Stddev: {stdev:.6f}s\n"
    f"Best:   {best:.6f}s\n"
    f"Worst:  {worst:.6f}s"
)

plt.text(
    0.02, 0.98,
    stats_text,
    transform=plt.gca().transAxes,
    verticalalignment="top",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
)

plt.xlabel("Run")
plt.ylabel("Time (seconds)")
plt.title("sum0 performance")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
