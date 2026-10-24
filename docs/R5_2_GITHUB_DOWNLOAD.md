# R5.2 complete repair bundle

Download [Ti60_R5_2_BOARD_FIX.tar.xz](../dist/Ti60_R5_2_BOARD_FIX.tar.xz) and extract it with `tar -xJf` on Linux or an XZ-capable archive tool on Windows. It contains the matched bitstream, debug ELF, release ELF, manifest, timing reports and updated source control panel, including volume +/− buttons. Archive contents were verified byte-for-byte against the local ZIP.

The debug ELF and bitstream are also stored directly under `BOARD_TEST/r5_2/`. GitHub's transport repeatedly failed the separate release-ELF and larger ZIP uploads, so the release ELF is supplied inside the complete archive. The local project still includes both raw ELFs and the ZIP. No release file was substituted with an older version.

GitHub 完整修复包采用上面的 TAR.XZ；包含新 bit、调试/发行两种 ELF、验证清单和带音量加减的上位机。单独发行 ELF 与较大 ZIP 上传多次返回传输错误，发行 ELF 已完整放入归档；本地交付仍同时保留原始两种 ELF 和 ZIP。原 R5 根目录下的生成文件属于历史基线，新源码需用构建脚本重建，当前验收以 `BOARD_TEST/r5_2` 和归档内的新清单为准。
