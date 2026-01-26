from dataclasses import dataclass
from typing import List, Tuple

@dataclass(frozen=True)
class CacheConfig:
    size_bytes: int
    line_bytes: int
    assoc: int

    @property
    def sets(self) -> int:
        lines = self.size_bytes // self.line_bytes
        return max(1, lines // self.assoc)

class SetAssocCacheLRU:
    """Simple set-associative cache with LRU replacement (educational)."""
    def __init__(self, cfg: CacheConfig, name: str):
        self.cfg = cfg
        self.name = name
        self.tags: List[List[int]] = [[] for _ in range(cfg.sets)]  # LRU at 0, MRU at end
        self.hits = 0
        self.misses = 0

    def _index_tag(self, addr: int) -> Tuple[int, int]:
        line = addr // self.cfg.line_bytes
        idx = line % self.cfg.sets
        tag = line // self.cfg.sets
        return idx, tag

    def access(self, addr: int) -> bool:
        idx, tag = self._index_tag(addr)
        way = self.tags[idx]
        if tag in way:
            self.hits += 1
            way.remove(tag)
            way.append(tag)
            return True
        self.misses += 1
        if len(way) >= self.cfg.assoc:
            way.pop(0)
        way.append(tag)
        return False

    def stats(self) -> dict:
        total = self.hits + self.misses
        return {
            "name": self.name,
            "hits": self.hits,
            "misses": self.misses,
            "miss_rate": (self.misses / total) if total else 0.0,
        }
