# mypy: disable-error-code="name-defined, attr-defined"
import json
import shlex
from pathlib import Path
from typing import Dict, List

from . import GetParazAcc, Paratranz, kquote
from . import yarn_spinner_pb2 as pb
from .case_utils import Key, ParseProtoFromCase, commands, extractor, importer


def File(*args) -> Path:
    case_name = args[0]
    return Path(case_name).with_suffix(".json")


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    proto_bin_path = in_root / "case"

    ret = {}
    for proto_json in proto_bin_path.glob("Case *.json"):
        case_name = proto_json.name

        program = ParseProtoFromCase(proto_json)

        tmp: List[Paratranz] = []
        for node_name, node in program.nodes.items():
            for inst_idx, inst in enumerate(node.instructions):
                if inst.opcode != pb.Instruction.RUN_COMMAND:
                    continue

                for op in inst.operands:
                    if op.HasField("string_value"):
                        cmd = shlex.split(op.string_value)
                        if cmd[0] not in commands:
                            continue

                        keywords = []
                        if cmd[0] == "SetDeductionField":
                            keywords.append(f"Deduction组句单词 - {cmd[1]}")

                        strings = extractor(cmd, commands[cmd[0]])

                        for idx, ss in enumerate(strings):
                            tmp.append(Paratranz(
                                key=Key(case_name, node_name, inst_idx, idx),
                                original=ss,
                                context=json.dumps({
                                    "node_name": node_name,
                                    "cmd": cmd,
                                    "keywords": keywords,
                                }, ensure_ascii=False, indent=2),
                            ))

        file = File(case_name)
        assert file not in ret
        ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    proto_bin_path = raw_root / "case"

    ret = {}

    for proto_json in proto_bin_path.glob("Case *.json"):
        print(f"="*80)
        print(f"Case: {proto_json}")

        case_name = proto_json.name

        with open(proto_json, "r", encoding="utf8") as f:
            data = json.load(f)
            proto_bin_json_data = data["compiledYarnProgram"]["Array"]
            proto_bin = bytearray(proto_bin_json_data)

        tmp: List[Paratranz] = []

        program = pb.Program() #type: ignore[attr-defined]
        program.ParseFromString(proto_bin)

        # Read corresponding Paratranz file
        paraz_file = paraz_root / File(case_name)
        paraz_acc = GetParazAcc(paraz_file)

        for node_name, node in program.nodes.items():
            for inst_idx, inst in enumerate(node.instructions):
                if inst.opcode != pb.Instruction.RUN_COMMAND:  #type: ignore[attr-defined]
                    continue

                for op in inst.operands:
                    if op.HasField("string_value"):
                        cmd = shlex.split(op.string_value)
                        if cmd[0] not in commands:
                            continue

                        strings = extractor(cmd, commands[cmd[0]])

                        texts_to_import = []
                        for idx, ss in enumerate(strings):
                            def getter(tag, key=None, data=strings, name=case_name, idx=idx):
                                key = key if key else Key(case_name, node_name, inst_idx, idx)
                                assert key in paraz_acc, key
                                paraz_data = paraz_acc[key]
                                assert data[tag] == paraz_data.original, \
                                    f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {case_name}\nTranz: {paraz_file}"
                                # TODO(kuriko): add checker here
                                return paraz_data.translation

                            texts_to_import.append(getter(idx))

                        importer(cmd, commands[cmd[0]], texts=texts_to_import)
                        new_cmd = " ".join([kquote(s, idx) for idx, s in enumerate(cmd)])

                        print("-"*40)
                        print(op.string_value)
                        print(new_cmd)

                        op.string_value = new_cmd

        new_proto_bin = program.SerializeToString()
        data["compiledYarnProgram"]["Array"] = list(new_proto_bin)

        file = Path("sharedassets0") / case_name
        ret[file] = data

    return ret
