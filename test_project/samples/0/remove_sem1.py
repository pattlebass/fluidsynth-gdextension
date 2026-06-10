import os

input_dir = "."
MTHD_SIZE = 14  # MThd is always exactly 14 bytes (4 marker + 4 length + 6 data)

for filename in os.listdir(input_dir):
    if not filename.lower().endswith((".mid", ".midi")):
        continue

    path = os.path.join(input_dir, filename)

    with open(path, "rb") as f:
        data = f.read()

    if not data.startswith(b"MThd"):
        print(f"{filename}: no MThd, skipping")
        continue

    mtrk_start = data.find(b"MTrk", MTHD_SIZE)

    if mtrk_start == -1:
        print(f"{filename}: no MTrk found, skipping")
        continue

    sem1_size = mtrk_start - MTHD_SIZE
    if sem1_size == 0:
        print(f"{filename}: no SEM1 chunk present, skipping")
        continue

    stripped = data[:MTHD_SIZE] + data[mtrk_start:]

    with open(path, "wb") as f:
        f.write(stripped)

    print(f"{filename}: removed {sem1_size} byte SEM1 chunk")
