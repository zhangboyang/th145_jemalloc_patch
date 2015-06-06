th145 jemalloc patch

一个用来把 东方深秘录 中的内存分配器替换为 jemalloc 的程序

文档及已编译的程序：
    http://blog.zbyzbyzby.com/?p=993

代码说明:
    patch.c     : patch.dll 的源代码
    rmalloc.c   : rmalloc.dll 的源代码 
    shellcode.c : shellcode.exe 的源代码

脚本说明：
    所有脚本均为 simple_patcher 格式
        https://github.com/zhangboyang/simple_patcher
    th145_full.txt        : 完整的补丁脚本，其中包含的 shellcode 代码是从 shellcode.exe 中提取出来的
    th145_simple.txt      : 禁用反调试代码的脚本
    th145_disableaslr.txt : 禁用 ASLR 的脚本

补丁使用方法：
    运行 compile.bat 即可编译本程序，然后把 patch.dll, rmalloc.dll, th145_full.txt 拷贝到游戏目录下
    下载 jemalloc 并使用 --disable-munmap 编译，然后把 jemalloc.dll 拷贝到游戏目录下
    下载 simple_patcher 并编译，然后把 run_script.exe 拷贝到游戏目录下
    在游戏目录下，把 th145_full.txt 拖到 run_script.exe 上执行，会生成 th145_full_patched.exe
    运行 th145_full_patched.exe 即可开始游戏
