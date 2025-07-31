#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/kdev_t.h>
#include <linux/interrupt.h>
#include <linux/syscalls.h>
#include <linux/mount.h>
#include <linux/device.h>
#include <linux/genhd.h>
#include <linux/namei.h>
#include <linux/shmem_fs.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/vfs.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/dcache.h>
#include <linux/spi/spi.h>

#define VIRTUAL_SPI_DEV_REGS_NUM    16

#define VRIUTAL_SPI_DEV_NUM     4


typedef struct virtual_spi_dev_info_s
{
    int chip_select;
    struct list_head node;
    u16 regs[VIRTUAL_SPI_DEV_REGS_NUM];
} virtual_spi_dev_info_t;

typedef struct virtual_spi_master_info_s
{
    struct list_head list;
} virtual_spi_master_info_t;

static struct spi_board_info virtual_spi_board_0 = {
    .modalias = "virtual_spi_dev0",
    .max_speed_hz = 16000000,
    .chip_select = 3,
    .mode = SPI_MODE_0,
};

static void	virtual_spi_master_dev_release(struct device *dev)
{
}

static struct platform_device virtual_spi_master_device = {
    .name =   "virtual_spi_master",
    .id = 1,
    .dev =
    {
        .release = virtual_spi_master_dev_release,
    }
};

static int virtual_spi_master_setup(struct spi_device *spi)
{
    int ret = 0;

    if(spi->chip_select >= spi->master->num_chipselect)
    {
        printk("%s:%d invalid chip_select\n", __FUNCTION__, __LINE__);
    }
    return ret;
}

static int virtual_spi_master_probe(struct platform_device *platform_dev)
{
    struct spi_master *master;
    virtual_spi_master_info_t *nspi;
    struct spi_device *spi_dev;

    master = spi_alloc_master(&(platform_dev->dev),sizeof(virtual_spi_master_info_t));
    if (!master) 
    {
        printk("%s: unable to alloc SPI master\n", __func__);
        return -EINVAL;
    }

    nspi = spi_master_get_devdata(master);
    master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST;
    master->bus_num = -1;

    master->num_chipselect = VRIUTAL_SPI_DEV_NUM;
    master->transfer_one_message = virtual_spi_transfer;
    master->setup = virtual_spi_master_setup;
    INIT_LIST_HEAD(&nspi->list);
    if (spi_register_master(master)) 
    {
        printk("%s: unable to register SPI master\n", __func__);
        return -EINVAL;
    }

    platform_set_drvdata(platform_dev, master);

    if ((spi_dev = spi_new_device(master, &virtual_spi_board_0)) == NULL)
    {
        printk("%s:%d add spi device failed\n", __FUNCTION__, __LINE__);

        return -EINVAL;
    }
    else
    {
        virtual_spi_dev_info_t *virtual_devp = kzalloc(sizeof(virtual_spi_dev_info_t), GFP_KERNEL);
        if(NULL == virtual_devp)
        {
            spi_unregister_device(spi_dev);
            return -EINVAL;
        }

        virtual_devp->chip_select = spi_dev->chip_select;
        virtual_devp->regs[2] = 10;
        list_add_tail(&(virtual_devp->node), &(nspi->list));
    }

    return 0;
}

static int virtual_spi_master_remove(struct platform_device *platform_dev)
{
    struct spi_master *master = platform_get_drvdata(platform_dev);

    spi_unregister_master(master);
    printk("%s:%d\n", __FUNCTION__, __LINE__);
    return 0;
}

static struct platform_driver virtual_spi_master_driver = {
    .driver = {
        .name = "virtual_spi_master",
        .owner = THIS_MODULE,
    },
    .probe = virtual_spi_master_probe,
    .remove = virtual_spi_master_remove,
};


static int __init virtual_spi_master_init(void)
{
    int ret = 0;

    ret = platform_device_register(&virtual_spi_master_device);

    if(ret == 0)
    {
        ret = platform_driver_register(&virtual_spi_master_driver);
    }

    return ret;
}

static void __exit virtual_spi_master_exit(void)
{
    printk("%s:%d, unregister virtual_spi_master_device\n", __FUNCTION__, __LINE__);
    platform_device_unregister(&virtual_spi_master_device);
    printk("%s:%d, unregister virtual_spi_master_driver\n", __FUNCTION__, __LINE__);

    platform_driver_unregister(&virtual_spi_master_driver);
}

module_init(virtual_spi_master_init);
module_exit(virtual_spi_master_exit);
MODULE_DESCRIPTION("Virtual Spi Controller Device Drivers");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jiamuyu");

