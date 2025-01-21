import json
import logging
import re
from pathlib import Path
from typing import Dict, List

import pangu
from pydantic import BaseModel, TypeAdapter

from . import flags
from .case_utils import cmds

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
speaker_ignored_texts = [
    "For certain CLAIMS, you will need to look at your opponent’s THOUGHTS and EMOTIONS. Choose the correct response, depending on the information seen./n/nLOGIC: Choose this option if your opponent’s thought contradicts something in your NOTES./nINTUITION: Choose this option if your opponent’s THOUGHT contradicts their CLAIM./nEMPATHY: Choose this option if your opponent’s EMOTIONS contradict their CLAIM.",
    "Your bodyguard: Celeste can now cast the Detect Magic spell. While EXAMINING a location, press the “Detect Magic” button to make magical traces visible. If there are any magical traces at your location, you’ll see a colored overlay.",
    '[character name=""][/character]You arrive at the harbour,[p/] in search of Morrison: the harbourmaster.',
]
def check_speaker(a: str, b: str) -> bool:
    if a.startswith(":") and b.startswith(":"):
        return True


    for ignored_text in speaker_ignored_texts:
        if a == ignored_text:
            return True

    flag = True
    gp1 = pat_speaker.findall(a)
    gp2 = pat_speaker.findall(b)

    #  if len(gp1):
    #      print(gp1)
    #      print(gp2)
    #      input("")
    flag = flag and (gp1 == gp2)

    return flag

def check_pair(tgt: str) -> bool:
    if tgt == "<u>": return True;
    if tgt == "</u>": return True;
    if tgt == "[b]": return True;
    if tgt == "[/b]": return True;

    flag = True

    pat1 = re.compile(r"<u>")
    pat2 = re.compile(r"</u>")

    gp1 = pat1.findall(tgt)
    gp2 = pat2.findall(tgt)

    flag = flag and len(gp1) == len(gp2)

    pat1 = re.compile(r"\[b\]")
    pat2 = re.compile(r"\[/b\]")

    gp1 = pat1.findall(tgt)
    gp2 = pat2.findall(tgt)

    flag = flag and len(gp1) == len(gp2)

    return flag

def check_invalid_tag_format(tgt: str) -> bool:
    pats = [
        # [/p]
        re.compile(r"\[/p\]"),
        # ?/p]  ?p/]
        re.compile(r"[^[](p/|/p)\]"),
        # [/p?  [p/?
        re.compile(r"\[(p/|/p)[^]]"),
        # [p\] [p1
        re.compile(r"\[p[^\/\]]"),

        # [/s]
        re.compile(r"\[/s\]"),
        # ?/s]  ?s/]
        re.compile(r"[^[](s/|/s)\]"),
        # [/s?  [s/?
        re.compile(r"\[(s/|/s)[^]]"),
        # [s\] [s?
        re.compile(r"\[s[^\/\]]"),

        re.compile(r"<u/>"),
        re.compile(r"[^<\/]u>"),
        # [/p?  [p/?
        re.compile(r"<u[^>]"),

        re.compile(r"[^\[\/]b\]"),
        re.compile(r"\[b/\]"),
        re.compile(r"\[b[^\]]"),

        re.compile(r"\[mind\s*=\s*[^\"]"),
    ]
    for pat in pats:
        if (matches := pat.findall(tgt)):
            print(matches)
            return False

    return True

def check_punctuations(tgt: str) -> bool:
    pats = [
        re.compile(r"\.\."),
        re.compile(r"(\?|!)"),
        re.compile(r"[^\d]\."),
        re.compile(r"[^\d\w]-[^\d\w]"),
    ]
    for pat in pats:
        if (matches := pat.findall(tgt)):
            print(matches)
            return False

    pat = re.compile(r"\"")
    if matches := pat.findall(tgt):
        if "mind" in tgt \
            or "Doubt" in tgt \
            or "Agree" in tgt \
            or "Interpret" in tgt \
            or "Deflect" in tgt \
            or "Defend" in tgt \
            or "Rationalize" in tgt \
            or "Appeal" in tgt \
            or "Threaten" in tgt \
            or "character" in tgt \
        :
            if len(matches) > 2:
                return False
        else:
            if len(matches) > 0:
                return False

    return True

def check_pangu(tgt: str) -> bool:
    if "mind=" in tgt: return True

    if tgt != pangu.spacing_text(tgt):
        return False

    return True


def GenParazAcc(data: List, paraz_file: Path, checks: Dict[str, bool] = {}) -> Dict[str, Paratranz]:
    if not checks:
        checks = {
            "marks": False,
            "speaker": False,
            "pair": True,
            "tags": False,
            "punctuations": False,
            "pangu": False,
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
            logger.error("Invalid: Mismatch marks")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            #  input("cnt?")

        if checks["speaker"] \
                and not check_speaker(item.original, item.translation):
            logger.error("Invalid: Mismatch Speaker")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            #  input("cnt?")

        if checks["pair"] \
            and not check_pair(item.translation):
            logger.error("Invalid: Mismatch underline")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            # input("cnt?")

        if checks["tags"] \
            and not check_invalid_tag_format(item.translation):
            logger.error("Invalid: tag format")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True
            # input("cnt?")

        if checks["punctuations"] \
            and item.translation != item.original \
            and not check_punctuations(item.translation):
            logger.error("Invalid: punctuations")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True

        if False and checks["pangu"] \
            and item.translation != item.original \
            and not check_pangu(item.translation):
            logger.error("Invalid: pangu")
            logger.error(item.original)
            logger.error(item.translation)
            logger.error("")
            Flag_error = True


        if item.context:
            item.context = fix_slash_n(item.context)

        if item.translation != item.original:
            item.translation = italic_to_em(
                item.translation,
                is_case = ("Case " in paraz_file.name),
                is_serifu = ("Default" in paraz_file.name),
                is_item = bool(re.compile(r"\d+").match(paraz_file.name)),
                is_spell = bool("spell" in paraz_file.parent.name),
                is_chara = bool("charalist" in paraz_file.parent.name),
                context = item.context or "",
            )

        assert key not in mm
        mm[key] = item

    if Flag_error:
        exit(-1)


    return mm


def GetParazAcc(paraz_file: Path, checks: Dict[str, bool] = {}) -> Dict[str, Paratranz]:
    assert paraz_file.exists(), str(paraz_file)
    paraz_data = json.load(open(paraz_file, "r", encoding="utf-8"))

    checks = {
        "marks": False,
        "speaker": False,
        "pair": True,
        "tags": False,
        "punctuations": False,
        "pangu": False,
    }

    if "Case " in paraz_file.name \
        or "Default" in paraz_file.name \
    :
        checks["speaker"] = True
        checks["pair"] = True
        checks["punctuations"] = True
        checks["pangu"] = False

    # print(paraz_file.name)
    # if "-143" in paraz_file.name:
    #     checks["punctuations"] = True
    # else:
    #     checks["punctuations"] = False

    paraz_acc = GenParazAcc(paraz_data, paraz_file, checks=checks)
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

def italic_to_em(
    s: str,
    is_case: bool,
    is_serifu: bool,
    is_item: bool,
    is_spell: bool,
    is_chara: bool,
    context: str="",
) -> str:
    # pat1 = re.compile(r"(\[i\](.*?)\[/i\])")
    pat2 = re.compile(r"(<u>(.*?)</u>)")

    for pat in [pat2]:
        for found in pat.findall(s):
            text = found[1]

            # for ch in text:
            #     if ch.isalpha() or "[" in ch or "]" in ch:
            #         logger.warning(f"non chinese char in {ch}")
            #         break

            text_len = len(text)
            new_text = f"<nobr>{text}</nobr><space=-{text_len}em><voffset=-0.8em>{"・"*text_len}</voffset>"
            s = s.replace(found[0], new_text)

    allowed_cmds = [
        "Tutorial",
        "Testimony",
        "TitleCard",
        "LoadTalk",

        # For case
        "InterpretPrompt",
        "LoadArgueProfile",
        "Tutorial",
    ]

    while is_case or is_serifu or is_item or is_spell or is_chara:
        Flag_ignore = True

        if is_item:
            Flag_ignore = False

        if is_spell:
            Flag_ignore = False

        if is_chara:
            Flag_ignore = False

        if is_serifu:
            context_data = json.loads(context)
            if not context_data["keywords"]:
                Flag_ignore = False

        if is_case or is_serifu:
            context_data = context
            for cmd in allowed_cmds:
                if cmd in context_data:
                    Flag_ignore = False
                    break

        if Flag_ignore:
            logger.warning(f"Ignore line-height: {s}, {context}")
            # input("cnt?")
            break

        if s not in speaker_ignored_texts and (matches := pat_speaker.match(s)):
            # [abc: ]
            speaker = matches[0]
            s = f"{speaker}<line-height=1.6em>{s[len(speaker):]}"
        elif s.startswith(":"):
            s = f":<line-height=1.6em>{s[1:]}"
        else:
            s = "<line-height=1.6em>"+s
        break
    return s
