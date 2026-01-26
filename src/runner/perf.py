import subprocess
from dataclasses import dataclass
from typing import List, Dict

@dataclass(frozen=True)
class PerfConfig:
    events: List[str]

def perf_stat(cmd: List[str], perf: PerfConfig) -> Dict[str, float]:
    """
    Runs perf stat and parses CSV output.
    """
    full = ["perf", "stat", "-x,", "-e", ",".join(perf.events)] + cmd
    p = subprocess.run(full, text=True, capture_output=True)
    if p.returncode != 0:
        raise RuntimeError(p.stderr)

    out = {}
    for ln in p.stderr.splitlines():  # perf writes to stderr
        parts = [x.strip() for x in ln.split(",")]
        if len(parts) < 3:
            continue
        val, _, ev = parts[0], parts[1], parts[2]
        try:
            out[ev] = float(val.replace("<not counted>", "nan"))
        except ValueError:
            pass
    return out
