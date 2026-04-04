#!/usr/bin/env python3
"""Convert .niv level files to JSON format for the web version."""
import json
import os

LEVELS_SRC = os.path.join(os.path.dirname(__file__), '..', '..', 'niveaux')
LEVELS_DST = os.path.join(os.path.dirname(__file__), '..', 'public', 'assets', 'levels')

os.makedirs(LEVELS_DST, exist_ok=True)

converted = 0
for i in range(1, 26):
    path = os.path.join(LEVELS_SRC, f'niveau{i}.niv')
    if not os.path.exists(path):
        print(f"  Skipping niveau{i}.niv (not found)")
        continue

    with open(path) as f:
        lines = [l.strip() for l in f if l.strip()]

    objects = []
    time_limit = {"minutes": 3, "seconds": 0}

    for j in range(0, len(lines) - 2, 3):
        x, y, t = int(lines[j]), int(lines[j + 1]), int(lines[j + 2])
        if t == 1000:
            time_limit = {"minutes": x, "seconds": y}
        else:
            objects.append({"x": x, "y": y, "type": t})

    level_data = {"level": i, "timeLimit": time_limit, "objects": objects}
    out_path = os.path.join(LEVELS_DST, f'niveau{i}.json')
    with open(out_path, 'w') as f:
        json.dump(level_data, f, indent=2)

    converted += 1
    print(f"  niveau{i}.json ({len(objects)} objects)")

print(f"\nConverted {converted} levels")
