# overall
本文旨在厘清spi驱动框架

# code

## master
代码详见于spi目录，master代码框架如下：

```c
virtual_spi_master_init
    -->|platform_device_register(&virtual_spi_master_device)
    -->|platform_driver_register(&virtual_spi_master_driver)

virtual_spi_master_probe
    -->|master = spi_alloc_master(&(platform_dev->dev),sizeof(virtual_spi_master_info_t)); //分配master以及私有数据virtual_spi_master_info_t
    -->|nspi = spi_master_get_devdata(master); //获取私有数据virtual_spi_master_info_t
    -->|master->transfer_one_message = virtual_spi_transfer; //核心函数
    -->|master->setup = virtual_spi_master_setup; //在slave设备probe时首次调用，初始化通信参数，在传输时，若设备参数发生改变，也会调用setup函数
    -->|spi_register_master(master)
    -->|platform_set_drvdata(platform_dev, master) // platform_dev->dev->pri_data = master
    -->|spi_dev = spi_new_device(master, &virtual_spi_board_0) // 根据virtual_spi_board_0创建slave设备
```
在init函数中创建device与driver，match后会调用probe函数

### 函数api讲解

1. spi_alloc_master
probe函数中spi调用spi_alloc_master为 SPI 控制器分配内存空间，并将驱动的私有数据结构嵌入其中，为后续注册和使用 SPI 主机做准备, 成功时返回指向 struct spi_master 的指针
```c
struct spi_master *spi_alloc_master(struct device *dev, size_t size);

```
- dev：指向父设备的指针（通常是平台设备 struct platform_device 的 dev 成员）
- size：驱动私有数据结构的大小（以字节为单位）

2. list_for_each_entry

```c
#define list_for_each_entry(pos, head, member)                  \
    for (pos = list_entry((head)->next, typeof(*pos), member);  \
         &pos->member != (head);                               \
         pos = list_entry(pos->member.next, typeof(*pos), member))
```
- pos：遍历过程中当前结构体的指针（用户定义的变量）
- head：链表头（struct list_head 类型，通常是结构体中的链表头成员）
- member：结构体中 struct list_head 成员的名称（即链表节点在结构体中的字段名）

## slave
