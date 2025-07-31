# overall
本文旨在厘清设备树overalay机制，并在以后的实验中使用此机制进行实验

# 步骤

## 编译内核

内核需要打开以下配置选项
CONFIG_OF_OVERLAY = y
CONFIG_OF_CONFIGFS = y

## 通过 /sys/firmware/devicetree 查看属性

设备树节点和属性以目录和文件的形式存在于 /sys/firmware/devicetree 中。例如，查看某个节点的 status 属性：

`cat /sys/firmware/devicetree/base/soc/i2c@12340000/status`



# ref

https://blog.csdn.net/qq_53221728/article/details/133908557
https://www.cnblogs.com/jiejunwang/p/18715868
