视频文件存储在 resource.resource 中，无加密：

```json
  "m_ExternalResources": {
    "m_Source": "resources.resource",
    "m_Offset": 37590761,
    "m_Size": 438724
  },
```

替换视频直接在 `resource.resource` 最后 append 数据，之后修改 `offset & size` 即可