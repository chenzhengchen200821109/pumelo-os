# pumelo-os
a simple implementation of unix-like operating system

***主要参考：linux0.2 xv6 ucore***  

## goals
1. multi-cpu
2. signals catch
3. built-in debugger
4. fancy file system

## 安装Bochs
1. 下载bochs
2. 解压压缩包, tar zxvf bochs-2.6.9.tar.gz
3. 编译。先进入到目录, cd bochs-2.6.9，开始configure、make、sudo make install三步曲
```
    ./configure \
    --prefix=/usr/local \
    --enable-debugger \
    --enable-disasm \
    --enable-iodebug \
    --enable-x86-debugger \
    --with-x \
    --with-x11 \
    LDFLAGS='-pthread' \
    LIBS='-lX11'
```
