游戏中**未使用**的字体（大概）

> 只放了一个 ttf，没有对应的 TMPro

OpenSans-MediumItalic.ttf

PerfectDOSVGA437.ttf

OpenSans-Italic.ttf

Merriweather-Italic.ttf

Merriweather-Light.ttf



**resource:**

NotoSerifHK-Regular.ttf

Merriweather-Regular.ttf  （NO SDF file）

LiberationSans.ttf



**level0:**

Raleway-Italic.ttf

Raleway-Regular.ttf

Raleway-SemiBold.ttf



OpenSans-Regular.ttf

OpenSans-Medium.ttf



目前看起来所有的资源都能在 resource 里面找到，因此直接在 resource 里面替换就行。

游戏中所有用到的字库图定义见 `gen_fonts.py` 中的 `FontDef`，目前全部替换成了更纱黑体 SC。



码表放在`Texts/chinese.txt` 中，由 `just charset`生成。

TMPro 生成所需 Unity 工程未包括（等汉化结束后打包丢 repo 里面，大概 1G 左右）。



**TODO：**

- [ ] 目前是所有字体都直接用更纱字体替换了，为了更好地观感可能需要每种字体找一个中文的对应字体