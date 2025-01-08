import json
from pathlib import Path
from typing import Dict, List

from . import GetParazAcc, Paratranz


def Key(*args) -> str:
    name = args[0]
    tag = args[1]
    return f"SaveFile-{name}-{tag}"

def File(*args) -> Path:
    name = args[0]
    return Path(name).with_suffix(".json")


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    ret = {}

    savefile_path = in_root / "save"

    tmp = []

    for savefile_file in savefile_path.glob("SaveFile*.json"):
        with open(savefile_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        name = Path(savefile_file).name

        tmp.append(Paratranz(
            key=Key(name, "saveMessage"),
            original=data["saveMessage"],
            translation=None,
            context=json.dumps({
                "file": name,
                "tag": "saveMessage",
            }, ensure_ascii= False, indent = 2)
        ))

        tmp.append(Paratranz(
            key=Key(name, "loadMessage"),
            original=data["loadMessage"],
            translation=None,
            context=json.dumps({
                "file": name,
                "tag": "loadMessage",
            }, ensure_ascii= False, indent = 2)
        ))

        tmp.append(Paratranz(
            key=Key(name, "deleteMessage"),
            original=data["deleteMessage"],
            translation=None,
            context=json.dumps({
                "file": name,
                "tag": "deleteMessage",
            }, ensure_ascii= False, indent = 2)
        ))

    file = File("SaveFile")
    assert file not in ret
    ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    ret = {}

    savefile_path = raw_root / "save"

    paraz_file = paraz_root / File("SaveFile")
    paraz_acc = GetParazAcc(paraz_file)

    for savefile_file in savefile_path.glob("SaveFile*.json"):
        with open(savefile_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        name = Path(savefile_file).name

        def getter(tag, key=None, data=data, name=name, idx=0):
            key = key if key else Key(name, tag)
            assert key in paraz_acc
            paraz_data = paraz_acc[key]
            assert data[tag] == paraz_data.original, \
                f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {name}\nTranz: {paraz_file}"
            return paraz_data.translation

        data['saveMessage'] = getter('saveMessage')
        data['loadMessage'] = getter('loadMessage')
        data['deleteMessage'] = getter('deleteMessage')

        file = Path("resources") / name
        assert file not in ret
        ret[file] = data

    return ret
