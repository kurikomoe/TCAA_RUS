import json
from pathlib import Path
from typing import Dict, List

from pydantic.json import pydantic_encoder

from . import GenParazAcc, GetParazAcc, Paratranz


def Key(*args) -> str:
    name = args[0]
    name2 = args[1]
    tag = args[2]
    return f"courtroom-{name}-{name2}-{tag}"

def File(*args) -> Path:
    key_name = args[0]
    return Path(key_name).with_suffix(".json")


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    file = in_root / "CourtroomLoader-level0-671.json"

    with open(file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret = {}
    tmp = []

    # Courtroom
    for idx, item in enumerate(data["courtroomList"]["Array"]):
        # Key, do not translate
        name = item["name"]
        locationData = item["locationData"]
        name2 = locationData["name"]

        def adder(tag, key=None, data=locationData, name=name, idx=idx):
            nonlocal tmp
            tmp.append(Paratranz(
                key=key if key else Key(name, name2, tag),
                original=data[tag],
                context=json.dumps({
                    "Name": name,
                    "Name2": name2,
                    "Attr": tag,
                }, ensure_ascii=False, indent=2),
            ))

        displayName = locationData["displayName"]
        adder("displayName")

    file = File("courtroom")
    assert file not in ret
    ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    base_file = Path("CourtroomLoader-level0-671.json")

    raw_file = raw_root / base_file

    with open(raw_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret: Dict[Path, Dict] = {}

    paraz_file = paraz_root / File("courtroom")
    paraz_acc = GetParazAcc(paraz_file)

    for idx, item in enumerate(data["courtroomList"]["Array"]):
        name = item["name"]
        locationData = item["locationData"]
        name2 = locationData["name"]

        def getter(tag, key=None, data=locationData, name=name, idx=idx):
            key = key if key else Key(name, name2, tag)
            assert key in paraz_acc
            paraz_data = paraz_acc[key]
            assert data[tag] == paraz_data.original, \
                f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {base_file}\nTranz: {paraz_file}"
            # TODO(kuriko): add checker here
            return paraz_data.translation

        locationData["displayName"] = getter("displayName")

    file = "level0" / base_file
    ret[file] = data

    return ret
