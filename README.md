# addr2bt
&nbsp;&nbsp;addr2bt工具通过查找进程maps文件，将虚拟地址转换为模块名加偏移地址，方便使用addr2line工具查找对应的源码位置。

***
### 编译
在linux环境下编译，直接make即可。


***
### 内存泄漏调试方法：
以调试mediaserver为例：
1. push libc_malloc_debug_leak.so到/system/lib目录下；
2. 开启检测内存泄漏功能，setprop libc.debug.malloc 1
3. 重启mediaserver进程，保存maps文件：cat /proc/$(pidof mediaserver)/maps > /data/mediaserver.maps
4. 抓取测试前的内存情况，dumpsys media.player -m > /data/mediaserver.m1
5. 开始测试
6. 结束测试，抓取测试后的内存情况，dumpsys media.player -m > /data/mediaserver.m2

将mediaserver.maps、mediaserver.m1和mediaserver.m2 pull到主机；  
用对比工具查看mediaserver.m1和mediaserver.m2的差异，mediaserver.m2多出来的内存地址为泄漏的内存地址；  
比如：
```
size       32, dup    1, 0xf1fcd1bc, 0xf76f1d40, 0xf767b2c8, 0xf767d83a, 0xf767d8c6, 0xf769d3ac, 0xf7622f10, 0xf769d28c, 0xf76a1800, 0xf76a1b7e, 0xf76a1bf4, 0xf76a5a20, 0xf767ca5c, 0xf767c58e, 0xf76f11a0, 0xf76f1338
```

0xf1fcd1bc开头的这些地址是堆栈地址，用addr2bt工具可以方便地还原出堆栈信息，用法如下：
```
addr2bt mediaserver.maps 0xf1fcd1bc, 0xf76f1d40, 0xf767b2c8, 0xf767d83a, 0xf767d8c6, 0xf769d3ac, 0xf7622f10, 0xf769d28c, 0xf76a1800, 0xf76a1b7e, 0xf76a1bf4, 0xf76a5a20, 0xf767ca5c, 0xf767c58e, 0xf76f11a0, 0xf76f1338
```
示例输出：
```
============================= backtrace ================================
             /system/lib/libc_malloc_debug_leak.so              0000d1bc
                               /system/lib/libc.so              0000dd40
                           /system/lib/libutils.so              0000d2c8
                           /system/lib/libutils.so              0000f83a
                           /system/lib/libutils.so              0000f8c6
                          /system/lib/libbinder.so              000193ac
                           /system/lib/libmedia.so              00058f10
                          /system/lib/libbinder.so              0001928c
                          /system/lib/libbinder.so              0001d800
                          /system/lib/libbinder.so              0001db7e
                          /system/lib/libbinder.so              0001dbf4
                          /system/lib/libbinder.so              00021a20
                           /system/lib/libutils.so              0000ea5c
                           /system/lib/libutils.so              0000e58e
                               /system/lib/libc.so              0000d1a0
                               /system/lib/libc.so              0000d338

```
最后再用addr2line工具定位到源码行，比如：
```
arm-linux-androideabi-addr2line -e out/target/product/p201_iptv/symbols/system/lib/libmedia.so 00058f10
/frameworks/av/media/libmedia/IMediaPlayerClient.cpp:30
```

