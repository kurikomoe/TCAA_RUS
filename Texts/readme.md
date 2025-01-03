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

