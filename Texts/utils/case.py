# mypy: disable-error-code="name-defined, attr-defined"
import json
import re
import shlex
from pathlib import Path
from typing import Dict, List

from . import GetParazAcc, Paratranz, kquote
from . import yarn_spinner_pb2 as pb
from .case_utils import (CheckCmd, GetOpXAsString, Key, ParseProtoFromCase,
                         commands, extractor, importer)


def Key_psychComplete(*args) -> str:
    case_name = args[0]
    node_name = args[1]
    tag = args[2]
    return f"{case_name}-{node_name}-psychComplete-{tag}"

def File(*args) -> Path:
    case_name = args[0]
    return Path(case_name).with_suffix(".json")


def ToParaTranz(in_root: Path) -> Dict[Path, List[Paratranz]]:
    proto_bin_path = in_root / "case"

    psychComplete_name_set = set()

    ret = {}
    for proto_json in proto_bin_path.glob("Case *.json"):
        case_name = proto_json.name

        program, data = ParseProtoFromCase(proto_json)

        tmp: List[Paratranz] = []
        for node_name, node in program.nodes.items():
            last_insts = []
            for inst_idx, inst in enumerate(node.instructions):
                # Process the psychComplete
                last_insts.append(inst)
                if len(last_insts) > 5: last_insts.pop(0)

                if CheckCmd(inst, "psychComplete"):
                    name_inst = last_insts[-3]
                    assert name_inst.opcode == pb.Instruction.PUSH_STRING, name_inst
                    name = GetOpXAsString(name_inst, 0)

                    if name not in psychComplete_name_set:
                        psychComplete_name_set.add(name)
                        tmp.append(Paratranz(
                            key=Key_psychComplete(case_name, node_name, name),
                            original=name,
                        ))


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
        assert file not in ret, file
        ret[file] = tmp

        tmp = []
        # Extra Steps deal with alias:
        pat_alias = re.compile(r"alias:([\w_]+)")
        line_ids = data["lineMetadata"]["_lineMetadata"]["keys"]["Array"]
        lines = data["lineMetadata"]["_lineMetadata"]["values"]["Array"]
        for idx, (line_id, line) in enumerate(zip(line_ids, lines)):
            matches = pat_alias.findall(line)
            if not matches: continue

            tmp.append(Paratranz(
                key=Key(case_name, node_name, line_id, 0),
                original=line,
                context=json.dumps({
                    "node_name": node_name,
                }, ensure_ascii=False, indent=2),
            ))

        #! FIXME(kuriko)
        file = Path(f"{proto_json.stem}-alias.json")
        assert file not in ret, file
        ret[file] = tmp

    return ret


def ToRaw(raw_root: Path, paraz_root: Path) -> Dict[Path, Dict]:
    proto_bin_path = raw_root / "case"

    ret = {}

    for proto_json in proto_bin_path.glob("Case *.json"):
        print(f"="*80)
        print(f"Case: {proto_json}")

        case_name = proto_json.name

        program, data = ParseProtoFromCase(proto_json)

        tmp: List[Paratranz] = []

        # Read corresponding Paratranz file
        paraz_file = paraz_root / File(case_name)
        paraz_acc = GetParazAcc(paraz_file)

        for node_name, node in program.nodes.items():
            pass

            for inst_idx, inst in enumerate(node.instructions):
                # Process the psychComplete
                # last_insts.append(inst)
                # if len(last_insts) > 5: last_insts.pop(0)

                # if CheckCmd(inst, "psychComplete"):
                #     name_inst = last_insts[-3]
                #     assert name_inst.opcode == pb.Instruction.PUSH_STRING, name_inst
                #     name = GetOpXAsString(name_inst, 0)

                #     key_name = Key_psychComplete(case_name, node_name, name)
                #     assert key_name in paraz_acc, key_name
                #     assert name_inst.operands[0].string_value == paraz_acc[key_name].original
                #     name_inst.operands[0].string_value = paraz_acc[key_name].translation

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

        # Extra Steps deal with alias:
        #! FIXME(kuriko)
        paraz_file = paraz_root / Path(f"{proto_json.stem}-alias.json")
        paraz_acc = GetParazAcc(paraz_file)

        pat_alias = re.compile(r"alias:([\w_]+)")
        line_ids = data["lineMetadata"]["_lineMetadata"]["keys"]["Array"]
        lines = data["lineMetadata"]["_lineMetadata"]["values"]["Array"]
        for idx, (line_id, line) in enumerate(zip(line_ids, lines)):
            matches = pat_alias.findall(line)
            if not matches: continue

            def getter(tag, key=None, data=lines, name=case_name, idx=0):
                key = key if key else Key(case_name, node_name, line_id, 0)
                assert key in paraz_acc, key
                paraz_data = paraz_acc[key]
                assert data[tag] == paraz_data.original, \
                    f"Mismatch:\n{data[tag]}\n{paraz_data.original}\nFile: {case_name}\nTranz: {paraz_file}"
                # TODO(kuriko): add checker here
                return paraz_data.translation

            lines[idx] = getter(idx)

        file = Path("sharedassets0") / case_name
        ret[file] = data

    return ret
