import json
import logging
import re
from pathlib import Path
from typing import Dict, List

from pydantic import BaseModel, TypeAdapter

from . import flags

logger = logging.getLogger(__name__)

class Paratranz(BaseModel):
    key: str
    original: str
    translation: str|None = None
    context: str|None = None


def fix_slash_n(s: str) -> str:
    s = s.replace("\\u200B", "\u200B")
    s = s.replace("\\u200b", "\u200b")
    s = s.replace("\\n", "\n")
    s = s.replace("$r", "\r")
    s = s.replace("$n", "\n")
    return s

def check_marks(a: str, b: str) -> bool:
    if not flags.ENABLE_MARK_CHECK: return True

    pat1 = re.compile(r"\[(\w+)/\]")
    pat2 = re.compile(r"\[(\w+)\].*?\[/\1\]")

    flag = True

    # stage 1
    #  ignored_controls = [ 'p', ]
    ignored_controls: List[str] = [ ]
    gp1 = pat1.findall(a)
    gp2 = pat1.findall(b)
    for ch in ignored_controls:
        while ch in gp1:
            gp1.remove(ch)
        while ch in gp2:
            gp2.remove(ch)
    flag = flag and gp1 == gp2

    # stage 2
    #! TODO(kuriko): modify this
    ignored_controls = [ 'i', ]
    #  ignored_controls = [ ]

    gp1 = pat2.findall(a)
    gp2 = pat2.findall(b)
    for ch in ignored_controls:
        while ch in gp1:
            gp1.remove(ch)
        while ch in gp2:
            gp2.remove(ch)

    flag = flag and gp1 == gp2

    return flag

pat_speaker = re.compile(r"^[^:]+: ")
def check_speaker(a: str, b: str) -> bool:
    flag = True

    gp1 = pat_speaker.findall(a)
    gp2 = pat_speaker.findall(b)

    #  if len(gp1):
    #      print(gp1)
    #      print(gp2)
    #      input("")
    flag = flag and (gp1 == gp2)

    return flag

def check_underline(tgt: str) -> bool:
    if tgt == "<u>": return True;
    if tgt == "</u>": return True;
    pat1 = re.compile(r"<u>")
    pat2 = re.compile(r"</u>")
    gp1 = pat1.findall(tgt)
    gp2 = pat2.findall(tgt)

    return len(gp1) == len(gp2)

def check_invalid_tag_format(tgt: str) -> bool:
    pats = [
        re.compile(r"\[/p\]"),
        re.compile(r"[^[](p/|/p)\]"),
        re.compile(r"\[(p/|/p)[^]]"),
    ]
    for pat in pats:
        if (matches := pat.findall(tgt)):
            print(matches)
            return False

    return True


def GenParazAcc(data: List, checks: Dict[str, bool] = {}) -> Dict[str, Paratranz]:
    if not checks:
        checks = {
            "marks": False,
            "speaker": False,
        }

    paraz_data = TypeAdapter(List[Paratranz]).validate_python(data)
    mm = {}

    Flag_error = False;
    for item in paraz_data:
        key = item.key

        item.original = fix_slash_n(item.original)

        if item.translation:
            item.translation = fix_slash_n(item.translation)
        else:
            item.translation = item.original

        if checks["marks"] \
                and not check_marks(item.original, item.translation):
            logger.error("Mismatch marks")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            #  input("cnt?")

        if checks["speaker"] \
                and not check_speaker(item.original, item.translation):
            logger.error("Mismatch Speaker")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            #  input("cnt?")

        if not check_underline(item.translation):
            logger.error("Mismatch underline")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            # input("cnt?")

        if not check_invalid_tag_format(item.translation):
            logger.error("Invalid tag format")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            # input("cnt?")


        if item.context:
            item.context = fix_slash_n(item.context)

        assert key not in mm
        mm[key] = item

    if Flag_error:
        exit(-1)

    return mm


def GetParazAcc(paraz_file: Path) -> Dict[str, Paratranz]:
    assert paraz_file.exists(), str(paraz_file)
    paraz_data = json.load(open(paraz_file, "r", encoding="utf-8"))
    paraz_acc = GenParazAcc(paraz_data)
    return paraz_acc


def kquote(s: str, idx) -> str:
    if idx == 0:
        return s
    elif s.isdigit():
        return s
    elif s.lower() in ["false", "true", "none"]:
        return s
    elif s == "":
        return '""'
    else:
        return f'"{s}"'
