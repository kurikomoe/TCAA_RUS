# mypy: disable-error-code="name-defined, attr-defined"
import json
import re
import shlex
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Tuple

from . import yarn_spinner_pb2 as pb


def ParseProtoFromCase(proto_json: Path) -> pb.Program:
    print(f"reading: {proto_json}")
    with open(proto_json, "r", encoding="utf8") as f:
        data = json.load(f)
        proto_bin_json_data = data["compiledYarnProgram"]["Array"]
        proto_bin = bytearray(proto_bin_json_data)

    program = pb.Program()
    program.ParseFromString(proto_bin)

    return program


commands = {
    # "Heh… I usually only sell this stuff for 5sp a flask."
    "InterpretPrompt": [1, ],

    # "Commander White is stubborn and stuck in his ways. He seems to have a grudge against Ruby Tymora and will react negatively if you try to defend her. However, he will respect you if you defend yourself against personal attacks."
    "LoadArgueProfile": [1, ],

    # [
    #   "Assets/Visuals/Resources/Tutorials/Argument Tutorial.png",
    #   "Arguments",
    #   "You’ve started an ARGUMENT with another character. During an ARGUMENT, your opponent will make CLAIMS that you need to respond to. Responding correctly will lower your opponent’s CONFIDENCE. Responding incorrectly will lower [i]your[/i] CONFIDENCE. And once you run out of CONFIDENCE, you will lose the ARGUMENT."
    # ],
    "Tutorial": [2, 3],

    # "How did the bottle shatter?"
    "Deduction": [1, ],

    # "You lost the argument..."
    "Confirmation": [1, ],

    # "Episode End"
    "TitleCard": [1, ],

    # "It was selfish of me to learn magic."
    "Typewriter": [1, ],

    # "How did the bottle shatter?"
    "DeduceTypewriter": [1, ],

    # [0, "Celeste,The killer,Flinhart"]
    "SetDeductionField": [2, ],

    # StartArgument "Merchant" 2
    "StartArgument": [1, ],
}

def extractor(op: List[str], pos: List[int]):
    ret = []
    for idx in pos:
        ret.append(op[idx])
    return ret

def importer(op: List[str], pos: List[int], texts: List[str]):
    for idx_text, idx_op in enumerate(pos):
        op[idx_op] = texts[idx_text]
    return

# =============================================================


def GetOpXRaw(inst: pb.Instruction, idx: int) -> str:
    assert len(inst.operands) > idx
    opX =inst.operands[idx]
    return opX

def GetOpXAsString(inst: pb.Instruction, idx: int) -> str:
    opX = GetOpXRaw(inst, idx)
    assert opX.HasField("string_value")
    return opX.string_value

def GetCmdOpXString(inst: pb.Instruction, idx: int) -> str:
    op0str = GetOpXAsString(inst, 0)
    cmd0 = shlex.split(op0str)
    return cmd0[idx]


def CheckCmd(inst: pb.Instruction, cmd: str) -> bool:
    if inst.opcode != pb.Instruction.RUN_COMMAND:
        return False

    if len(inst.operands) == 0:
        return False

    op0 =inst.operands[0]
    if not op0.HasField("string_value"):
        return False

    cmd0 = shlex.split(op0.string_value)
    return cmd0[0] == cmd

def IsSetDeductionField(inst: pb.Instruction) -> bool:
    return CheckCmd(inst, "SetDeductionField")

def IsAnyPrompt(inst: pb.Instruction) -> bool:
    return CheckCmd(inst, "PresentPrompt") \
        or CheckCmd(inst, "InterpretPrompt")


def IsShowOptions(inst: pb.Instruction) -> bool:
    return inst.opcode == pb.Instruction.SHOW_OPTIONS

def IsAddOption(inst: pb.Instruction) -> bool:
    return inst.opcode == pb.Instruction.ADD_OPTION

# line:Assets/Case Scripts/Case 1/Garrick Cornered.yarn-Garrick_Present-1287
pat_extract_text_idx = re.compile(r"line:(.+/)*[^/]+-(\d+)")

def ExtractAddOption(inst: pb.Instruction) -> str|None:
    if not IsAddOption(inst):
        return None
    op0_str = GetOpXAsString(inst, 0)
    return op0_str

def ExtractPushString(inst: pb.Instruction) -> str|None:
    if inst.opcode != pb.Instruction.PUSH_STRING:
        return None
    op0_str = GetOpXAsString(inst, 0)
    return op0_str

# ===============================================================

@dataclass
class NodeAttrs:
    pass

@dataclass
class DeductionGroup:
    node_name: str = ""
    words: List[Tuple[str, str]] = field(default_factory=list)
    finals: List[str] = field(default_factory=list)

@dataclass
class SpecialCase:
    options: List[str] = field(default_factory=list)
    any_prompt: List[str] = field(default_factory=list)  # These translations should not be included

    deduction: Dict[str, DeductionGroup] = field(default_factory=dict)

    node_attrs: Dict[str, NodeAttrs] = field(default_factory=dict)


# =============================================================
# def IterInstructions(
#     program: pb.Program,
#     fn_enter_new_node: function,
#     fn_new_inst: function,
# ):
#     for node_name, node in program.nodes.items():
#         # Process All Options
#         for inst_idx, inst in enumerate(node.instructions):
#             if (text_id := ExtractAddOption(inst)):
#                 ret.Options.append(text_id)

def Key(*args) -> str:
    case_name = args[0]
    node_name = args[1]
    inst_idx = args[2]
    idx = args[3]
    return f"{case_name}-{node_name}-{inst_idx}-{idx}"

def GetSpecialCase(case_name: str, program: pb.Program) -> SpecialCase:
    sp_case: SpecialCase = SpecialCase()

    # Process All Options
    for node_name, node in program.nodes.items():
        assert node_name not in sp_case.node_attrs
        sp_case.node_attrs[node_name] = NodeAttrs()

        for inst_idx, inst in enumerate(node.instructions):
            if (text_id := ExtractAddOption(inst)):
                sp_case.options.append(text_id)

    # Process present_prompt
    for node_name, node in program.nodes.items():
        FLAG_AnyPrompt = False
        for inst_idx, inst in enumerate(node.instructions):
            if IsAnyPrompt(inst):
                FLAG_AnyPrompt = True
            elif (text_id := ExtractAddOption(inst)):
                if FLAG_AnyPrompt:
                    sp_case.any_prompt.append(text_id)
            elif IsShowOptions(inst):
                FLAG_AnyPrompt = False

    # process deduction
    ## Phase 1: Check Deduction Target Nodes
    for node_name, node in program.nodes.items():
        FLAG_Deduction = False
        last_string = ""

        deduction_words = []

        for inst_idx, inst in enumerate(node.instructions):
            if CheckCmd(inst, "ClearDeductionFields"):  # Enter a new Deduction
                FLAG_Deduction = True
            elif CheckCmd(inst, "SetDeductionField"):
                word = GetCmdOpXString(inst, 2)
                deduction_words.append((Key(case_name, node_name, inst_idx, 0), word))
            elif (tgt_node := ExtractPushString(inst)):
                last_string = tgt_node
            elif inst.opcode == pb.Instruction.RUN_NODE:
                tgt_node = last_string
                if FLAG_Deduction:
                    assert tgt_node not in sp_case.deduction
                    sp_case.deduction[tgt_node] = DeductionGroup(
                        node_name = tgt_node,
                        words = deduction_words,
                    )
                    FLAG_Deduction = False

    ## Phase 2: collect deduction options
    for node_name, node in program.nodes.items():
        FLAG_IsDeductionTargetNode = node_name in sp_case.deduction

        if not FLAG_IsDeductionTargetNode:
            continue

        for inst_idx, inst in enumerate(node.instructions):
            if (text_id := ExtractAddOption(inst)):
                if FLAG_IsDeductionTargetNode:
                    sp_case.deduction[node_name].finals.append(text_id)

            elif IsShowOptions(inst):
                FLAG_AnyPrompt = False

    return sp_case
