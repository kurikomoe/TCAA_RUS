文件组织：

```shell
"Texts/"
├── "charalist"
├── "@dist" # 最终生成的文件，用于 UABEA[Next] 读取导入，按 data.unity3d 内部格式组织
│   ├── "level0"
│   └── "resources"
├── "@old"  # @raw, 原始文件，read-only
│   ├── "case"
│   ├── "m_text"
│   ├── "serifu"
│   ├── "tooltips"
│   ├── "CharacterLibrary-level0-599.json"
│   ├── "ItemLibrary-level0-603.json"
│   └── "SpellLibrary-level0-602.json"
├── "@paraz" # `just export` 命令导出文件，用于 paratranz
│   ├── "case"       # Case 字节码文本
│   ├── "charalist"  # 人物定义文本
│   ├── "item"       # 物品文本
│   ├── "m_text"     # 控件，按钮文本
│   ├── "serifu"     # 台本，Case 字节码中的多语言定义文本
│   ├── "spell"      # 咒语文本
│   └── "tooltips"   # 几个按钮上的注释文本
├── "@paraz-out" # Paratranz 导出文件，和 @paraz 相同但是包含了汉化，手动或者GitHub Action导出
│   ├── "case"
│   ├── "charalist"
│   ├── "item"
│   ├── "m_text"
│   ├── "serifu"
│   ├── "spell"
│   └── "tooltips"
├── "utils"          # Texts 相关的导入导出函数
│   ├── "case.py"
│   ├── "charalist.py"
│   ├── "flags.py"
│   ├── "__init__.py"
│   ├── "item.py"
│   ├── "m_text.py"
│   ├── "serifu.py"
│   ├── "spell.py"
│   ├── "tooltips.py"
│   ├── "yarn_spinner_pb2.py"  # 根据 yarnspinner 2.2.1 版本的 yarn_spinner.proto 定义文件，通过 protoc --python_out=.  ./yarn_spinner.proto 生成
│   └── "yarn_spinner.proto"   # 从 yarnspinner 2.2.1 中偷出来的
├── "chinese.txt"     # `just charset` 生成的码表
├── "get_chars.py"    # `just charset` 用到的脚本
├── "readme.md"
└── "text_io.py"      # 核心的文本导入导出脚本，用于`just import|export`

```

## Case 特别说明

在 @Work 里面执行如下命令生成 instruction dump
```shell
# fishshell
for i in *.json ; python3 extractCaseYarnBinary.py $i; end
for i in *.json.bin; python3 dump_insts.py $i > $i.txt; end
```

### Item 选择判断

对于如下命令，通过 `PresentPrompt` 唤出的证据窗口，会选择一个物品/Spell/人物，此时会拿到对应物品的 Name（非 displayName）。
```shell
1, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "PresentPrompt"', '}', 'operands {', '  float_value: 0', '}'] }
2, instruction { ['opcode: ADD_OPTION', 'operands {', '  string_value: "line:Assets/Case Scripts/Case 1/Merchant Argument.yarn-Merchant_Present-1719"', '}', 'operands {', '  string_value: "L6shortcutoption_Merchant_Present_1"', '}', 'operands {', '  float_value: 0', '}', 'operands {', '  bool_value: false', '}'] }
3, instruction { ['opcode: ADD_OPTION', 'operands {', '  string_value: "line:Assets/Case Scripts/Case 1/Merchant Argument.yarn-Merchant_Present-1723"', '}', 'operands {', '  string_value: "L7shortcutoption_Merchant_Present_2"', '}', 'operands {', '  float_value: 0', '}', 'operands {', '  bool_value: false', '}'] }
4, instruction { ['opcode: SHOW_OPTIONS'] }
```

接下来判断根据如下两个文本 Key 进行，因此这种文本不能翻译，需要保持和 Item name 一致。
```json
{
"key": "Default (en-US)-sharedassets0.assets-144-1719",
"original": "Store-Bought Orange Juice",
"translation": "Store-Bought Orange Juice",
},
{
"key": "Default (en-US)-sharedassets0.assets-144-1723",
"original": "Default",
"translation": "Default",
},
```

### Deduction 相关文本


观察发现 `SetDeductionField` 会有对应的文本。
```shell
16, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "ClearDeductionFields"', '}', 'operands {', '  float_value: 0', '}'] }
17, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "SetDeductionField 0 \\"Celeste,The killer,Flinhart\\""', '}', 'operands {', '  float_value: 0', '}'] }
18, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "SetDeductionField 1 \\"dropped,threw,placed\\""', '}', 'operands {', '  float_value: 0', '}'] }
19, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "SetDeductionField 2 \\"the bottle,the sword,the killer\\""', '}', 'operands {', '  float_value: 0', '}'] }
20, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "SetDeductionField 3 \\"during the attack,after the attack,before the attack\\""', '}', 'operands {', '  float_value: 0', '}'] }
21, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "Deduction \\"How did the bottle shatter?\\""', '}', 'operands {', '  float_value: 0', '}'] }
22, instruction { ['opcode: PUSH_STRING', 'operands {', '  string_value: "Orym_Deduce3"', '}'] }
23, instruction { ['opcode: RUN_NODE'] }
```

之后跳转 `Orym_Deduce3` 节点
```shell
Orym_Deduce3
0, instruction { ['opcode: RUN_COMMAND', 'operands {', '  string_value: "SetDialogueActive false"', '}', 'operands {', '  float_value: 0', '}'] }
1, instruction { ['opcode: ADD_OPTION', 'operands {', '  string_value: "line:Assets/Case Scripts/Case 1/Orym Deduction.yarn-Orym_Deduce3-1834"', '}', 'operands {', '  string_value: "L7shortcutoption_Orym_Deduce3_1"', '}', 'operands {', '  float_value: 0', '}', 'operands {', '  bool_value: false', '}'] }
2, instruction { ['opcode: ADD_OPTION', 'operands {', '  string_value: "line:Assets/Case Scripts/Case 1/Orym Deduction.yarn-Orym_Deduce3-1835"', '}', 'operands {', '  string_value: "L8shortcutoption_Orym_Deduce3_2"', '}', 'operands {', '  float_value: 0', '}', 'operands {', '  bool_value: false', '}'] }
3, instruction { ['opcode: SHOW_OPTIONS'] }
4, instruction { ['opcode: JUMP'] }
```

观察可知，两个判断对应的文本为：
```json
{
"key": "Default (en-US)-sharedassets0.assets-144-1834",
"original": "Player: Flinhart dropped the bottle during the attack",
"translation": "",
},
{
"key": "Default (en-US)-sharedassets0.assets-144-1835",
"original": "Player: Fail",
"translation": "",
},
```

对应游戏中的表现为：
![deduction](../docs/deduction.png)

因此可知，拼句子完成后会根据对应的文本做判断来决定跳转那个分支，（aka label L7shortcutoption_Orym_Deduce3_1 指向的 pc）。

所以，在汉化的时候对于这些文本需要做特别判断，告诉翻译如何修改对应的语句。
