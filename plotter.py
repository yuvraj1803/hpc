import subprocess
import sys
import re
import statistics
import matplotlib.pyplot as plt


def auto_scale(times):
    max_t = max(times)
    if max_t >= 1.0:
        return times, "s", 1.0
    elif max_t >= 1e-3:
        return [t * 1e3 for t in times], "ms", 1e3
    elif max_t >= 1e-6:
        return [t * 1e6 for t in times], "µs", 1e6
    else:
        return [t * 1e9 for t in times], "ns", 1e9


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


# ---- auto-scale units ----
scaled_times, unit, scale = auto_scale(times)

mean   = statistics.mean(scaled_times)
median = statistics.median(scaled_times)
stdev  = statistics.stdev(scaled_times) if len(times) > 1 else 0.0
best   = min(scaled_times)
worst  = max(scaled_times)

# ---- plot ----
plt.figure(figsize=(9, 5))
plt.plot(range(1, runs + 1), scaled_times, marker="o", label="Run time")

plt.axhline(mean,   linestyle="--", label=f"Mean:   {mean:.3f} {unit}")
plt.axhline(median, linestyle=":",  label=f"Median: {median:.3f} {unit}")
plt.axhline(best,   linestyle="-.", label=f"Best:   {best:.3f} {unit}")

stats_text = (
    f"Runs:   {runs}\n"
    f"Mean:   {mean:.3f} {unit}\n"
    f"Median: {median:.3f} {unit}\n"
    f"Stddev: {stdev:.3f} {unit}\n"
    f"Best:   {best:.3f} {unit}\n"
    f"Worst:  {worst:.3f} {unit}"
)

plt.text(
    0.02, 0.98,
    stats_text,
    transform=plt.gca().transAxes,
    verticalalignment="top",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.85)
)

plt.xlabel("Run")
plt.ylabel(f"Time ({unit})")
plt.title("Kernel performance")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
