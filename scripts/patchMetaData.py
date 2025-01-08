import argparse
import io
import json
import struct
from pprint import pprint
from typing import Dict, List, Tuple

parser = argparse.ArgumentParser()
parser.add_argument("-t", "--translation", type=str, help="input translation file")
parser.add_argument("-i", "--input", type=str, help="input global-metadata.dat file")
parser.add_argument("-o", "--output", type=str, help="output global-metadata.dat file")

args = parser.parse_args()

with open(args.translation, "r", encoding="utf8") as f:
    trans_data: Dict[str, str] = json.load(f)

mapping = [
    # ("Evocation\x00", "塑能术\x00"),
    # ("Transmutation\x00", "形变术\x00"),
    # ("Conjuration\x00", "召唤术\x00"),
    # ("Divination\x00", "占卜术\x00"),
    # ("Illusion\x00", "幻象术\x00"),
    # ("Abjuration\x00", "防护术\x00"),
    # ("Necromancy\x00", "死灵术\x00"),
]

for k, v in trans_data.items():
    mapping.append(
        (f"{k}\x00", f"{v}\x00")
    )

pprint(mapping)

tot_en_len = 0
tot_chs_len = 0
for (en, chs) in mapping:
    en_len = len(en.encode("utf8"))
    chs_len = len(chs.encode("utf8"))
    tot_en_len += en_len
    tot_chs_len += chs_len
    print(en, en_len)
    print(chs, chs_len)

assert tot_en_len >= tot_chs_len
print(f"en_len: {hex(tot_en_len)}")
print(f"chs_lchs: {hex(tot_chs_len)}")
print("space left: ", tot_en_len - tot_chs_len)

with open(args.input, "rb") as f2:
    data = f2.read()
    data_bin = bytearray(data)
    cur = io.BytesIO(data_bin)

cur.seek(0x18, io.SEEK_SET)
string_base = struct.unpack("<I", cur.read(4))[0]
print(f"string_base: {string_base}")


def find_all_occurrences(data, target_bytes):
    offset = 0
    offsets = []
    while True:
        pos = data.find(target_bytes, offset)
        if pos == -1:
            break
        offsets.append(pos)
        offset = pos + 1  # Move past the current match to continue searching
    return offsets

locs: List[Tuple[int, int]] = []
for (en, chs) in mapping:
    print(f"Finding: {en} => {chs}")
    en_b = en.encode("utf8")
    chs_b = chs.encode("utf8")

    poses = find_all_occurrences(data, en_b)
    assert len(poses) == 1, poses
    pos = poses[0]

    offset = pos - string_base
    offset_b = struct.pack("<I", offset)
    poses = find_all_occurrences(data, offset_b)
    assert len(poses) == 1, f"more than one offset"
    offset_pos = poses[0]

    print(hex(pos))
    locs.append((pos, offset_pos))

cur.seek(locs[0][0])
for (pos, offset_pos), (en, chs) in zip(locs, mapping):
    en_b = en.encode("utf8")
    chs_b = chs.encode("utf8")
    print(f"writing {chs}:  {str(chs_b)}")

    start_pos = cur.seek(0, io.SEEK_CUR)
    cur.write(chs_b)
    end_pos = cur.seek(0, io.SEEK_CUR)

    cur.seek(offset_pos)
    cur.write(struct.pack("<I", start_pos-string_base))
    cur.seek(end_pos)


for i in range(tot_en_len - tot_chs_len):
    cur.write(b"\x00")


with open(args.output, "wb") as fout:
    fout.write(cur.getvalue())
