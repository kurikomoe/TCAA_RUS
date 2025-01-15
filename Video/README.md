视频文件存储在 resource.resource 中，无加密：

```json
  "m_ExternalResources": {
    "m_Source": "resources.resource",
    "m_Offset": 37590761,
    "m_Size": 438724
  },
```

<del>替换视频直接在 `resource.resource` 最后 append 数据，之后修改 `offset & size` 即可</del>

有些参数存在问题，需要将视频导入到 unity 工程中，添加 `videohandler`，重新编码视频后导出，用导出的 json 重新导入到游戏素材。
（可以将 json 中的 sharedassets0.resS 之类的改名，Unity 会自动在 Data 文件夹下面寻找对应文件）
