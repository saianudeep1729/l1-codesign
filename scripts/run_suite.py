import json, time, subprocess
from pathlib import Path

BENCHES = [
    ("icache_footprint", ["benchmarks/micro/icache_footprint", "20000000"]),
    ("dcache_stride",    ["benchmarks/micro/dcache_stride"]),
    ("pointer_chase",    ["benchmarks/micro/pointer_chase"]),
    ("branchy",          ["benchmarks/micro/branchy"]),
]

def run_one(name, cmd):
    t0 = time.time()
    p = subprocess.run(cmd, text=True, capture_output=True)
    return {
        "bench": name,
        "cmd": cmd,
        "sec": time.time() - t0,
        "rc": p.returncode,
        "stdout": p.stdout[-2000:],
        "stderr": p.stderr[-2000:],
    }

def main():
    results = [run_one(n, c) for n, c in BENCHES]
    Path("out").mkdir(exist_ok=True)
    Path("out/results.json").write_text(json.dumps(results, indent=2))
    print("Wrote out/results.json")

if __name__ == "__main__":
    main()
