import json, sys

def main(path):
    with open(path) as f:
        rows = json.load(f)
    sorted_rows = sorted(rows, key=lambda r: r["sec"])
    # Simple table: bench, sec, rc
    for r in sorted_rows:
        print(f"{r['bench']:20}  {r['sec']:.6f}  {r['rc']}")

if __name__ == "__main__":
    main(sys.argv[1])
