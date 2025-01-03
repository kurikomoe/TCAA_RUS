import json
from pathlib import Path
from typing import Any, Dict, List

from . import GetParazAcc, Paratranz
from . import yarn_spinner_pb2 as pb
from .case_utils import (DeductionGroup, GetSpecialCase, ParseProtoFromCase,
                         SpecialCase)


def Key(*args) -> str:
    name = args[0]
    idx = args[1]
    return f"{name}-{idx}"

def File(*args) -> Path:
    serifu_file_name = args[0]
    return Path(serifu_file_name)

case_mapping = {
    "Default (en-US)-sharedassets0.assets-144.json": "Case 1-sharedassets0.assets-154.json",
    "Default (en-US)-sharedassets0.assets-145.json": "Case 2-sharedassets0.assets-155.json",
    "Default (en-US)-sharedassets0.assets-146.json": "Case 3-sharedassets0.assets-156.json",
    "Default (en-US)-sharedassets0.assets-148.json": "Case 4-sharedassets0.assets-158.json",
    "Default (en-US)-sharedassets0.assets-143.json": "Case 5-sharedassets0.assets-153.json",
}

def SearchInDeduction(line_id: str, sp_case: SpecialCase) -> None|DeductionGroup:
    for node_name, value in sp_case.deduction.items():
        if line_id in value.finals:
            return value
    return None


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    proto_bin_path = in_root / "case"
    # ==================================================================

    ret = {}

    serifu_path = in_root / "serifu"

    for serifu_file in serifu_path.glob("Default (en-US)*.json"):
        with open(serifu_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        line_ids = data['_stringTable']['keys']['Array']
        lines = data['_stringTable']['values']['Array']
        assert len(line_ids) == len(lines)

        stem = Path(serifu_file).stem

        case_file = case_mapping[Path(serifu_file).name]
        assert case_file

        program = ParseProtoFromCase(proto_bin_path / case_file)
        sp_case: SpecialCase = GetSpecialCase(case_file, program)

        tmp = []
        for idx, (line_id, line) in enumerate(zip(line_ids, lines)):
            keywords: List[Any] = []
            if line_id in sp_case.options:
                keywords.append("选项文本")

            if line_id in sp_case.present_prompt:
                keywords.append("证据选择对象文本")
                print(f"Ignore 证据选择文本: {line}")
                continue  # directly ignores

            if (deduction := SearchInDeduction(line_id, sp_case)):
                keywords.append({
                    "类型": "Deduction组句目标文本",
                    "来源": deduction.words,
                })

            tmp.append(Paratranz(
                key=f"{stem}-{idx}",
                original=line,
                translation=None,
                context=json.dumps({
                    "id": line_id,
                    "keywords": keywords,
                }, ensure_ascii=False, indent=2),
            ))

        file = File(serifu_file.name)
        assert file not in ret
        ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    serifu_path = raw_root / "serifu"

    ret = {}

    for serifu_file in serifu_path.glob("Default (en-US)*.json"):
        with open(serifu_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        lines = data['_stringTable']['values']['Array']

        name = Path(serifu_file).stem

        paraz_file = paraz_root / File(serifu_file.name)
        paraz_acc = GetParazAcc(paraz_file)

        for idx, _ in enumerate(lines):
            def getter(tag, key=None, data=lines, name=name, idx=idx):
                key = key if key else Key(name, idx)
                assert key in paraz_acc
                paraz_data = paraz_acc[key]
                assert data[tag] == paraz_data.original, \
                    f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {serifu_file}\nTranz: {paraz_file}"
                # TODO(kuriko): add checker here
                return paraz_data.translation

            lines[idx] = getter(idx)

        file = Path("resources") / File(serifu_file.name)
        assert file not in ret
        ret[file] = data

    return ret
