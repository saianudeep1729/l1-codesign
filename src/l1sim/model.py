from dataclasses import dataclass
from .cache import CacheConfig, SetAssocCacheLRU
from .trace import TraceEvent

@dataclass(frozen=True)
class L1Design:
    ic: CacheConfig
    dc: CacheConfig
    base_cpi: float = 1.0
    i_miss_penalty: float = 12.0
    d_miss_penalty: float = 12.0
    overlap: float = 0.7  # 0=fully hidden, 1=no overlap

class L1SystemModel:
    """
    Research scaffold:
    - Feed I/R/W events into I$ and D$
    - Estimate CPI from miss rates + penalty model
    """
    def __init__(self, design: L1Design):
        self.design = design
        self.icache = SetAssocCacheLRU(design.ic, "L1I")
        self.dcache = SetAssocCacheLRU(design.dc, "L1D")
        self.count = {"I": 0, "R": 0, "W": 0}

    def feed(self, ev: TraceEvent) -> None:
        self.count[ev.kind] += 1
        if ev.kind == "I":
            self.icache.access(ev.addr)
        elif ev.kind in ("R", "W"):
            self.dcache.access(ev.addr)

    def estimate_cpi(self) -> float:
        i_mr = self.icache.stats()["miss_rate"]
        d_mr = self.dcache.stats()["miss_rate"]
        return (
            self.design.base_cpi
            + i_mr * self.design.i_miss_penalty
            + d_mr * self.design.d_miss_penalty * self.design.overlap
        )

    def report(self) -> dict:
        return {
            "counts": self.count,
            "L1I": self.icache.stats(),
            "L1D": self.dcache.stats(),
            "cpi_est": self.estimate_cpi(),
        }
