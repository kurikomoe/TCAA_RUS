import json
from pathlib import Path
from typing import Dict, List

from pydantic.json import pydantic_encoder

from . import GenParazAcc, GetParazAcc, Paratranz


def Key(*args) -> str:
    name = args[0]
    idx = args[1]
    return f"credit-{name}-{idx}"

def File(*args) -> Path:
    key_name = args[0]
    return Path(key_name).with_suffix(".json")


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    file = in_root / "CreditLibrary-level0-651.json"

    with open(file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret = {}
    tmp = []

    for idx, item in enumerate(str(data["credits"]).split("\r\n")):
        # Key, do not translate
        def adder(tag, data=item, idx=idx):
            nonlocal tmp
            tmp.append(Paratranz(
                key=Key(tag, idx),
                original=item,
                context=json.dumps({
                    "Idx": idx,
                    "Attr": tag,
                }, ensure_ascii=False, indent=2),
            ))

        adder("credits", item)

    file = File("credits")
    assert file not in ret
    ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    base_file = Path("CreditLibrary-level0-651.json")

    raw_file = raw_root / base_file

    with open(raw_file, "r", encoding="utf-8") as f:
        data = json.load(f)

    ret: Dict[Path, Dict] = {}

    paraz_file = paraz_root / File("credits")
    paraz_acc = GetParazAcc(paraz_file)

    tmp = []
    for idx, item in enumerate(str(data["credits"]).split("\r\n")):
        # <size="80em">游戏总监：Stephen Charles|<voffset=-5em><size="80em">汉化组-主催：KurikoMoe</voffset>
        def getter(tag, data, idx=idx):
            key = Key(tag, idx)
            assert key in paraz_acc
            paraz_data = paraz_acc[key]
            assert data == paraz_data.original, \
                f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {base_file}\nTranz: {paraz_file}"
            # TODO(kuriko): add checker here
            return paraz_data.translation

        ss = getter("credits", item)

        if "：" in ss:
            orig, trans = ss.split("|")
            ss = f'<size="80em">{orig}|<voffset=-5em><size="80em">{trans}</voffset>'

        tmp.append(ss)

    final_credits = "\r\n".join(tmp)
    data["credits"] = final_credits

    file = "level0" / base_file
    ret[file] = data

    return ret
