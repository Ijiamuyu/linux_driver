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

## slave
