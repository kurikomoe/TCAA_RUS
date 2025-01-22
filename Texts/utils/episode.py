import json
from pathlib import Path
from typing import Dict, List

from . import GenParazAcc, GetParazAcc, Paratranz


def Key(*args) -> str:
    name = args[0]
    tag = args[1]
    return f"EpisodeLibrary-{name}-{tag}"


def File(*args) -> Path:
    name = args[0]
    file = Path(f"{name}.json")
    return file


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    file = in_root / "EpisodeLibrary-level0-607.json"

    with open(file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret = {}
    tmp = []

    for idx, episode in enumerate(data['episodes']["Array"]):
        # Key, do not translate
        name = episode["episodeName"]
        if name == "Debugger":
            continue

        def adder(tag, key=None, data=episode, name=name, idx=idx):
            nonlocal tmp
            tmp.append(Paratranz(
                key=key if key else Key(name, tag),
                original=data[tag],
                context=json.dumps({
                    "Name": name,
                    "Attr": tag,
                }, ensure_ascii=False, indent=2),
            ))

        adder("episodeName")

        for override in episode["initState"]["occupationOverrides"]["Array"]:
            adder("value",
                  key=f"{name}-occupationOverrides-{override['key']}",
                  data=override)

    file = File("EpisodeLibrary")
    assert file not in ret
    ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    base_file = Path("EpisodeLibrary-level0-607.json")

    raw_file = raw_root / base_file

    with open(raw_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    # Read corresponding Paratranz file
    paraz_file = paraz_root / File("EpisodeLibrary")
    paraz_acc = GetParazAcc(paraz_file)

    ret: Dict[Path, Dict] = {}
    for idx, episode in enumerate(data['episodes']["Array"]):
        # Key, do not translate
        name = episode["episodeName"]
        if name == "Debugger":
            continue

        def getter(tag, key=None, data=episode, name=name, idx=idx):
            key = key if key else Key(name, tag)
            assert key in paraz_acc
            paraz_data = paraz_acc[key]
            assert data[tag] == paraz_data.original, \
                f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {base_file}\nTranz: {paraz_file}"
            # TODO(kuriko): add checker here
            return paraz_data.translation

        episode["episodeName"] = getter("episodeName")

        for override in episode["initState"]["occupationOverrides"]["Array"]:
            override["value"] = getter(
                "value",
                key=f"{name}-occupationOverrides-{override['key']}",
                data=override,
            )

    file = "level0" / base_file
    ret[file] = data

    return ret
