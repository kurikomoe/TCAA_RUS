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


with open(args.input, "rb") as f2:
    data = f2.read()
    data += b"\x00\x00\x00"
    data_bin = bytearray(data)
    cur = io.BytesIO(data_bin)

# 0x05FD0D,
avail_space = cur.seek(0, io.SEEK_END)

def worker(mapping, new_pos=False):
    global cur, avail_space
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

    if not new_pos:
        assert tot_en_len >= tot_chs_len
        print(f"en_len: {hex(tot_en_len)}")
        print(f"chs_lchs: {hex(tot_chs_len)}")
        print("space left: ", tot_en_len - tot_chs_len)

    cur.seek(0x18, io.SEEK_SET)
    string_base = struct.unpack("<I", cur.read(4))[0]
    print(f"string_base: {string_base}")


    def find_all_occurrences(data, target_bytes, add_prefix=False):
        offset = 0
        offsets = []
        if add_prefix:
            target_bytes = b"\x00"+target_bytes
        while True:
            pos = data.find(target_bytes, offset)
            if pos == -1:
                break
            if add_prefix:
                offsets.append(pos+1)
            else:
                offsets.append(pos)
            offset = pos + 1  # Move past the current match to continue searching
        return offsets

    locs: List[Tuple[int, int]] = []

    last_pos = 0
    last_offset_pos = 0
    for (en, chs) in mapping:
        print(f"Finding: {en} => {chs}")
        en_b = en.encode("utf8")
        chs_b = chs.encode("utf8")

        poses = find_all_occurrences(data, en_b, True)

        if len(poses) > 1:
            print(f"try guessing the pos: {[hex(i) for i in poses]}")
            margin = 0xFFFFFFFFFF
            nearest_pos = 0
            for i in poses:
                if i < last_pos:
                    continue
                if margin > i - last_pos:
                    margin = i - last_pos
                    nearest_pos = i
            poses = [ nearest_pos ]

        assert len(poses) == 1, poses
        pos = poses[0]
        print("pos: ", hex(pos))

        last_pos = pos

        offset = pos - string_base
        offset_b = struct.pack("<I", offset)
        poses = find_all_occurrences(data, offset_b)

        if len(poses) > 1:
            print(f"try guessing the offset pos: {[hex(i) for i in poses]}")
            margin = 0xFFFFFFFFFF
            nearest_pos = 0
            for i in poses:
                if i < last_offset_pos:
                    continue
                if margin > i - last_offset_pos:
                    margin = i - last_offset_pos
                    nearest_pos = i
            poses = [ nearest_pos ]

        assert len(poses) == 1, f"more than one offset {poses}"
        offset_pos = poses[0]
        last_offset_pos = offset_pos
        print("offset_pos: ", hex(offset_pos))

        locs.append((pos, offset_pos))

    if new_pos:
        cur.seek(avail_space)
    else:
        cur.seek(locs[0][0])
    for (pos, offset_pos), (en, chs) in zip(locs, mapping):
        en_b = en.encode("utf8")
        chs_b = chs.encode("utf8")
        print(f"writing {chs}:  {str(chs_b)}")

        start_pos = cur.seek(0, io.SEEK_CUR)
        cur.write(chs_b)
        end_pos = cur.seek(0, io.SEEK_CUR)

        if new_pos:
            avail_space = end_pos

        cur.seek(offset_pos)
        cur.write(struct.pack("<I", start_pos-string_base))
        cur.seek(end_pos)


    for i in range(tot_en_len - tot_chs_len):
        cur.write(b"\x00")


mapping = [
    ("Autopsy\x00", "尸检报告\x00"),
    ("Compendium\x00", "法术汇编\x00"),
    ("Testimony\x00", "证词\x00"),
]
worker(mapping, True)

mapping = [
    ("Contract\x00", "契约\x00"),
]
worker(mapping)

mapping = []
for k, v in trans_data.items():
    mapping.append(
        (f"{k}\x00", f"{v}\x00")
    )
worker(mapping)


def map_worker():
    mapping = [
        ("Map\x00", "地图\x00"),
    ]

    cur.seek(0x18, io.SEEK_SET)
    string_base = struct.unpack("<I", cur.read(4))[0]
    print(f"string_base: {string_base}")

    def find_all_occurrences(data, target_bytes, add_prefix=False):
        offset = 0
        offsets = []
        if add_prefix:
            target_bytes = b"\x00"+target_bytes
        while True:
            pos = data.find(target_bytes, offset)
            if pos == -1:
                break
            if add_prefix:
                offsets.append(pos+1)
            else:
                offsets.append(pos)
            offset = pos + 1  # Move past the current match to continue searching
        return offsets

    locs: List[Tuple[int, int]] = []
    last_pos = 0
    last_offset_pos = 0x4e8c40
    for (en, chs) in mapping:
        print(f"Finding: {en} => {chs}")
        en_b = en.encode("utf8")
        chs_b = chs.encode("utf8")

        poses = find_all_occurrences(data, en_b, True)

        if len(poses) > 1:
            print(f"try guessing the pos: {[hex(i) for i in poses]}")
            margin = 0xFFFFFFFFFF
            nearest_pos = 0
            for i in poses:
                if i < last_pos:
                    continue
                if margin > i - last_pos:
                    margin = i - last_pos
                    nearest_pos = i
            poses = [ nearest_pos ]

        assert len(poses) == 1, poses
        pos = poses[0]
        print("pos: ", hex(pos))

        last_pos = pos

        offset = pos - string_base
        offset_b = struct.pack("<I", offset)
        poses = find_all_occurrences(data, offset_b)

        if len(poses) > 1:
            print(f"try guessing the offset pos: {[hex(i) for i in poses]}")
            margin = 0xFFFFFFFFFF
            nearest_pos = 0
            for i in poses:
                if i < last_offset_pos:
                    continue
                if margin > i - last_offset_pos:
                    margin = i - last_offset_pos
                    nearest_pos = i
            poses = [ nearest_pos ]

        assert len(poses) == 1, f"more than one offset {poses}"
        offset_pos = poses[0]
        last_offset_pos = offset_pos
        print("offset_pos: ", hex(offset_pos))

        locs.append((pos, offset_pos))

    cur.seek(avail_space)
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

map_worker()

with open(args.output, "wb") as fout:
    fout.write(cur.getvalue())
