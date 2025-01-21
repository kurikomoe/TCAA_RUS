import json
from pathlib import Path
from typing import Dict, List

from . import GetParazAcc, Paratranz


def Key(*args) -> str:
    name = args[0]
    tag = args[1]
    return f"Inputs-{name}-{tag}"

def File(*args) -> Path:
    name = args[0]
    return Path(name).with_suffix(".json")


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    ret = {}

    inputsfile_path = in_root / "inputs"

    tmp = []

    patterns = ["KeybindingSetting-*.json", "BindingDisplay-*.json"]
    for pat in patterns:
        for inputsfile in inputsfile_path.glob(pat):
            with open(inputsfile, "r", encoding="utf-8") as f:
                data = json.load(f)

            name = Path(inputsfile).name

            tag = None
            if "KeybindingSetting" in name:
                tag = "inputName"
            elif "BindingDisplay" in name:
                tag = "input"
            else:
                assert False, f"Unknown file: {name}"

            if data[tag] != "":
                tmp.append(Paratranz(
                    key=Key(name, tag),
                    original=data[tag],
                    translation=None,
                    context=json.dumps({
                        "file": name,
                        "tag": tag,
                    }, ensure_ascii= False, indent = 2)
                ))


    file = File("InputsFile")
    assert file not in ret
    ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    ret = {}

    inputsfile_path = raw_root / "inputs"

    paraz_file = paraz_root / File("InputsFile")
    paraz_acc = GetParazAcc(paraz_file)

    patterns = ["KeybindingSetting-*.json", "BindingDisplay-*.json"]
    for pat in patterns:
        for inputsfile_file in inputsfile_path.glob(pat):
            with open(inputsfile_file, "r", encoding="utf-8") as f:
                data = json.load(f)

            name = Path(inputsfile_file).name

            def getter(tag, key=None, data=data, name=name, idx=0):
                key = key if key else Key(name, tag)
                assert key in paraz_acc
                paraz_data = paraz_acc[key]
                assert data[tag] == paraz_data.original, \
                    f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {name}\nTranz: {paraz_file}"
                return paraz_data.translation

            tag = None
            if "KeybindingSetting" in name:
                tag = "inputName"
            elif "BindingDisplay" in name:
                tag = "input"
            else:
                assert False, f"Unknown file: {name}"

            if data[tag] != "":
                data[tag] = getter(tag)

            if "-resources" in name:
                file = Path("resources") / name
            elif "-level0" in name:
                file = Path("level0") / name
            else:
                assert False, f"Unknown file: {name}"

            assert file not in ret
            ret[file] = data

    return ret
