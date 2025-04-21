import json
from pathlib import Path
from typing import Any, Dict, List

from . import GetParazAcc, Paratranz
from . import yarn_spinner_pb2 as pb
from .case_utils import (DeductionGroup, GetSpecialCase, ParseProtoFromCase,
                         RunCommandInfo, SpecialCase)
from .item import GetItems


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

non_translation_prefix = [
    "Player: ",
]

def SearchPrefix(line: str) -> bool:
    for pat in non_translation_prefix:
        if line.startswith(pat):
            return True
    return False

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

    item_list = GetItems(in_root)

    for serifu_file in serifu_path.glob("Default (en-US)*.json"):
        with open(serifu_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        line_ids = data['_stringTable']['keys']['Array']
        lines = data['_stringTable']['values']['Array']
        assert len(line_ids) == len(lines)

        stem = Path(serifu_file).stem

        case_file = case_mapping[Path(serifu_file).name]
        assert case_file

        program, _ = ParseProtoFromCase(proto_bin_path / case_file)
        sp_case: SpecialCase = GetSpecialCase(case_file, program)

        tmp = []
        for idx, (line_id, line) in enumerate(zip(line_ids, lines)):
            keywords: List[Any] = []

            if line_id in sp_case.options:
                keywords.append("选项文本")

            try:
                if (idx2 := sp_case.run_command_option.index(line_id)):
                    info: RunCommandInfo = sp_case.run_command_option_type[idx2]
                    if not info.ignored:
                        # NOTE(Kuriko): Backward commpatitable
                        if info.cmd == "LoadTalk":
                            keywords.append("对话选项")

                        keywords.append({
                            "类型": info.cmd,
                            "指令": info.inst,
                        })
                    else:
                        print(f"Ignore {info.cmd}: {line}")
                        continue

                    for item in item_list:
                        if item in line:
                            keywords.append(f"疑似物品： {item}")
            except ValueError:
                pass

            if (deduction := SearchInDeduction(line_id, sp_case)):
                keywords.append({
                    "类型": "Deduction组句目标文本",
                    "来源": deduction.words,
                })

            if SearchPrefix(line):
                keywords.append("特殊前缀指令")

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
    checks = {
        "marks": False,
        "speaker": True,
        "underline": True,
        "tags": True,
        #  "punctuations": True,
        "punctuations": False,  # Turn off for rus
        "pangu": True,
    }

    proto_bin_path = raw_root / "case"
    # ==================================================================

    serifu_path = raw_root / "serifu"

    ret = {}

    for serifu_file in serifu_path.glob("Default (en-US)*.json"):
        with open(serifu_file, "r", encoding="utf-8") as f:
            data = json.load(f)

        lines = data['_stringTable']['values']['Array']

        line_ids = data['_stringTable']['keys']['Array']
        lines = data['_stringTable']['values']['Array']
        assert len(line_ids) == len(lines)

        stem = Path(serifu_file).stem

        case_file = case_mapping[Path(serifu_file).name]
        assert case_file

        program, _ = ParseProtoFromCase(proto_bin_path / case_file)
        sp_case: SpecialCase = GetSpecialCase(case_file, program)

        paraz_file = paraz_root / File(serifu_file.name)
        paraz_acc = GetParazAcc(paraz_file, checks=checks)

        for idx, (line_id, line) in enumerate(zip(line_ids, lines)):
            try:
                if (idx2 := sp_case.run_command_option.index(line_id)):
                    info: RunCommandInfo = sp_case.run_command_option_type[idx2]
                    if info.ignored:
                        print(f"Ignore {info.cmd}: {line}")
                        continue
            except ValueError:
                pass

            def getter(tag, key=None, data=lines, name=stem, idx=idx):
                key = key if key else Key(name, idx)
                assert key in paraz_acc, (key, data[tag])
                paraz_data = paraz_acc[key]
                assert data[tag] == paraz_data.original, \
                    f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {serifu_file}\nTranz: {paraz_file}"
                # TODO(kuriko): add checker here
                return paraz_data.translation

            lines[idx] = getter(idx)

        file = Path("sharedassets0") / File(serifu_file.name)
        assert file not in ret
        ret[file] = data

    return ret
