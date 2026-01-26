# l1-codesign
# L1 Cache Microarchitecture & Compiler/OS Co-Design 


**Focus:** Microarchitecture-first analysis with compiler and OS co-design

This repo studies how **L1 instruction/data cache design** plus **compiler + OS/runtime**
choices affect end-to-end CPU performance when the rest of the CPU is fixed.

> Fixed: OoO engine, branch predictor, L2/L3, prefetchers, memory controller, ISA  
> Tunable: L1I/L1D params + compiler + OS/runtime

---

## Quickstart

### 1) Install Python deps
```bash
pip install -r requirements.txt
