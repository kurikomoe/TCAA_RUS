import json
from pathlib import Path
from typing import Dict, List

from pydantic.json import pydantic_encoder

from . import GenParazAcc, GetParazAcc, Paratranz


def Key(*args) -> str:
    key_name = args[0]
    name = args[1]
    tag = args[2]
    return f"location-{key_name}-{name}-{tag}"

def File(*args) -> Path:
    key_name = args[0]
    return Path(key_name).with_suffix(".json")


keys = [
    "caseOneLocations",
    "caseTwoLocations",
    "caseThreeLocations",
    "caseFourLocations",
    "caseFiveLocations",
]

def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    file = in_root / "LocationLibrary-level0-604.json"

    with open(file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret = {}

    # Courtroom
    displayName = data["courtroom"]["displayName"]
    ret[File("Courtroom")] = [
        Paratranz(
            key=Key("courtroom", "Courtroom", "displayName"),
            original=displayName,
            context=json.dumps({
                "KeyName": "courtroom",
                "Name": "Courtroom",
                "Attr": "displayName",
            }, ensure_ascii=False, indent=2),
        )
    ]

    for _, key_name in enumerate(keys):
        tmp = []

        for idx, item in enumerate(data[key_name]["Array"]):
            # Key, do not translate
            name = item["name"]

            def adder(tag, key=None, data=item, name=name, idx=idx):
                nonlocal tmp
                tmp.append(Paratranz(
                    key=key if key else Key(key_name, name, tag),
                    original=data[tag],
                    context=json.dumps({
                        "KeyName": key_name,
                        "Name": name,
                        "Attr": tag,
                    }, ensure_ascii=False, indent=2),
                ))

            display_name = item["displayName"]
            adder("displayName")

        file = File(key_name)
        assert file not in ret
        ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    base_file = Path("LocationLibrary-level0-604.json")

    raw_file = raw_root / base_file

    with open(raw_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret: Dict[Path, Dict] = {}

    paraz_file = paraz_root / File("Courtroom")
    paraz_acc = GetParazAcc(paraz_file)

    key = Key("courtroom", "Courtroom", "displayName")
    assert key in paraz_acc
    data["courtroom"]["displayName"] = paraz_acc[key].translation

    for _, key_name in enumerate(keys):
        # Read corresponding Paratranz file
        paraz_file = paraz_root / File(key_name)
        paraz_acc = GetParazAcc(paraz_file)

        for idx, item in enumerate(data[key_name]["Array"]):
            # Key, do not translate
            name = item["name"]
            def getter(tag, key=None, data=item, name=name, idx=idx):
                key = key if key else Key(key_name, name, tag)
                assert key in paraz_acc
                paraz_data = paraz_acc[key]
                assert data[tag] == paraz_data.original, \
                    f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {base_file}\nTranz: {paraz_file}"
                # TODO(kuriko): add checker here
                return paraz_data.translation

            item["displayName"] = getter("displayName")

    file = "level0" / base_file
    ret[file] = data

    return ret
