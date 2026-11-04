# 证据索引与来源说明

此目录存放现有交付中**已经存在的 JSON 摘要**，不是原始 Efinity 数据库、完整波形或板级日志。请结合对应版本、适用范围和证据等级使用。

| 文件 | 内容 | 限制 |
|---|---|---|
| [timing_summary.json](timing_summary.json) | 各时钟域 setup/hold 摘要 | 不是原始 STA 报告全文 |
| [hdmi_sync_build.json](hdmi_sync_build.json) | 选定同步构建、TMDS 比较和时序 | 声音实板结论注明待确认；含先前主机绝对路径 |
| [software_native.json](software_native.json) | 已有软件运行记录 | 本分支未重跑 |
| [software_payload.json](software_payload.json) | ELF 载荷关联检查 | 不能证明实际 HDMI 品质 |
| [documentation_scope.json](documentation_scope.json) | 早期整理时的文档改动范围 | 历史文件，不能当作本次重构的验收报告 |

在新实验中应保存 `tool_version`、`device`、`git_sha`、`bit_sha256`、`elf_sha256`、`test_config`、`time_window`、`raw_log` 和 `pass_criteria`。已上传的完整比赛技术报告作为以上数字的主要来源，但其 PDF 尚未添加到此 Git 仓库中。
