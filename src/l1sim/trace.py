from dataclasses import dataclass
from typing import Iterator, TextIO

@dataclass(frozen=True)
class TraceEvent:
    kind: str  # "I", "R", "W"
    addr: int

def parse_trace(f: TextIO) -> Iterator[TraceEvent]:
    """
    Trace format:
      I 0x400123
      R 0x7fffabcd
      W 0x7fffabce
    """
    for ln in f:
        ln = ln.strip()
        if not ln or ln.startswith("#"):
            continue
        k, a = ln.split()
        yield TraceEvent(kind=k, addr=int(a, 16))
