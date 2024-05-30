#include <stdlib.h>
#include <securec.h>
#include "hal_i2c.h"
#include "i2c_core.h"
#include "i2c_if.h"
#include "hdf_device_desc.h"
#include "osal_spinlock.h"
#include "hdf_log.h"

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#endif

#define I2C_SLAVE_ADDRESS7     0xDE

struct Gd32f4xxI2cCntlr {
    struct I2cCntlr cntlr;
    OsalSpinlock spin;
    uint32_t bus;
    uint16_t sclpin_group;
    uint16_t sdapin_group;
    uint32_t sclpin;
    uint32_t sdapin;
    uint32_t speed;
};

struct Gd32f4xxI2cCntlr *gd32f4xx = NULL;

void i2c_buf_write(uint32_t i2c_x, uint8_t* p_buffer, uint8_t write_address, uint16_t WriteAddr, uint8_t number_of_byte)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(g_i2c_BaseMaps[i2c_x]);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(g_i2c_BaseMaps[i2c_x], WriteAddr, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_ADDSEND));

    /* clear the ADDSEND bit */
    i2c_flag_clear(g_i2c_BaseMaps[i2c_x], I2C_FLAG_ADDSEND);

    /* wait until the transmit data buffer is empty */
    while( SET != i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_TBE));

    /* send the devices's internal address to write to : only one byte address */
    i2c_data_transmit(g_i2c_BaseMaps[i2c_x], write_address);

    /* wait until BTC bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_BTC));

    /* while there is data to be written */
    while(number_of_byte--){
        i2c_data_transmit(g_i2c_BaseMaps[i2c_x], *p_buffer);

        /* point to the next byte to be written */
        p_buffer++; 

        /* wait until BTC bit is set */
        while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_BTC));
    }
    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(g_i2c_BaseMaps[i2c_x]);

    /* wait until the stop condition is finished */
    while(I2C_CTL0(g_i2c_BaseMaps[i2c_x])&I2C_CTL0_STOP);
}

void i2c_buf_read(uint32_t i2c_x, uint8_t* p_buffer, uint8_t read_address, uint16_t WriteAddr, uint16_t number_of_byte)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_I2CBSY));

    if(2 == number_of_byte){
        i2c_ackpos_config(g_i2c_BaseMaps[i2c_x], I2C_ACKPOS_NEXT);
    }

    /* send a start condition to I2C bus */
    i2c_start_on_bus(g_i2c_BaseMaps[i2c_x]);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(g_i2c_BaseMaps[i2c_x], WriteAddr, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_ADDSEND));

    /* clear the ADDSEND bit */
    i2c_flag_clear(g_i2c_BaseMaps[i2c_x], I2C_FLAG_ADDSEND);
    
    /* wait until the transmit data buffer is empty */
    while(SET != i2c_flag_get( g_i2c_BaseMaps[i2c_x], I2C_FLAG_TBE));

    /* enable I2C*/
    i2c_enable(g_i2c_BaseMaps[i2c_x]);

    /* send the devices's internal address to write to */
    i2c_data_transmit(g_i2c_BaseMaps[i2c_x], read_address);  

    /* wait until BTC bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_BTC));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(g_i2c_BaseMaps[i2c_x]);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(g_i2c_BaseMaps[i2c_x], WriteAddr, I2C_RECEIVER);

    if(number_of_byte < 3){
        /* disable acknowledge */
        i2c_ack_config(g_i2c_BaseMaps[i2c_x], I2C_ACK_DISABLE);
    }

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_ADDSEND));

    /* clear the ADDSEND bit */
    i2c_flag_clear(g_i2c_BaseMaps[i2c_x], I2C_FLAG_ADDSEND);

    if(1 == number_of_byte){
        /* send a stop condition to I2C bus */
        i2c_stop_on_bus(g_i2c_BaseMaps[i2c_x]);
    }

    /* while there is data to be read */
    while(number_of_byte){
        if(3 == number_of_byte){
            /* wait until BTC bit is set */
            while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_BTC));

            /* disable acknowledge */
            i2c_ack_config(g_i2c_BaseMaps[i2c_x], I2C_ACK_DISABLE);
        }
        if(2 == number_of_byte){
            /* wait until BTC bit is set */
            while(!i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_BTC));

            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(g_i2c_BaseMaps[i2c_x]);
        }

        /* wait until the RBNE bit is set and clear it */
        if(i2c_flag_get(g_i2c_BaseMaps[i2c_x], I2C_FLAG_RBNE)){
            /* read a byte from the devices */
            *p_buffer = i2c_data_receive(g_i2c_BaseMaps[i2c_x]);

            /* point to the next location where the byte read will be saved */
            p_buffer++; 

            /* decrement the read bytes counter */
            number_of_byte--;
        } 
    }

    /* wait until the stop condition is finished */
    while(I2C_CTL0(g_i2c_BaseMaps[i2c_x])&I2C_CTL0_STOP);
    /* enable acknowledge */
    i2c_ack_config(g_i2c_BaseMaps[i2c_x], I2C_ACK_ENABLE);

    i2c_ackpos_config(g_i2c_BaseMaps[i2c_x], I2C_ACKPOS_CURRENT);
}

static int32_t GD32f4xxI2cTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count)
{
    uint8_t ret;
    struct Gd32f4xxI2cCntlr *gd32f4xx = NULL;
    uint16_t MemAddress;

    if (cntlr == NULL || msgs == NULL || cntlr->priv == NULL) {
        HDF_LOGE("[%s]: I2cDataTransfer param is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (count <= 0) {
        HDF_LOGE("[%s]: I2cDataTransfer count err\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    gd32f4xx = (struct Gd32f4xxI2cCntlr *)cntlr;

    HDF_LOGI("%s: [line = %d] [bus = %d]\n", __func__, __LINE__, gd32f4xx->bus);

    /* i2c read */
    if(msgs[0].flags == 1) {
        MemAddress = msgs[0].buf[0] << 8 | msgs[0].buf[1];
        i2c_buf_read(gd32f4xx->bus, msgs[1].buf, MemAddress, msgs->addr, msgs[1].len);
    }

    /* i2c write */
    else if(msgs[0].flags == 0) {
        MemAddress = msgs[0].buf[0] << 8 | msgs[0].buf[1];
        i2c_buf_write(gd32f4xx->bus, msgs[1].buf, MemAddress, msgs->addr, msgs[1].len);
    }

    return HDF_SUCCESS;
}

static const struct I2cMethod g_GD32f4I2Cmethod = {
    .transfer = GD32f4xxI2cTransfer,
};

static int32_t GD32f4xxI2cLock(struct I2cCntlr *cntlr)
{
    struct Gd32f4xxI2cCntlr *gd32f4xx = (struct Gd32f4xxI2cCntlr *)cntlr;
    if (gd32f4xx != NULL) {
        (void)OsalSpinLock(&gd32f4xx->spin);
    }

    return HDF_SUCCESS;
}

static void GD32f4xxI2cUnlock(struct I2cCntlr *cntlr)
{
    struct Gd32f4xxI2cCntlr *gd32f4xx = (struct Gd32f4xxI2cCntlr *)cntlr;
    if (gd32f4xx != NULL) {
        (void)OsalSpinUnlock(&gd32f4xx->spin);
    }
}

static const struct I2cLockMethod g_GD32f4I2clockOps = {
    .lock = GD32f4xxI2cLock,
    .unlock = GD32f4xxI2cUnlock,
};

static int32_t GD32_I2C_Init(struct Gd32f4xxI2cCntlr *gd32f4xx)
{
    if(gd32f4xx == NULL) {
        HDF_LOGE("[%s]: I2cDataTransfer param is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%s: [line = %d] [speed = %d] [sclpin = %d] [sdapin = %d]\n", __func__, __LINE__, gd32f4xx->speed, gd32f4xx->sclpin, gd32f4xx->sdapin);

    /* enable GPIOB clock */
    rcu_periph_clock_enable(g_rcuPeriphMaps[gd32f4xx->sdapin_group]);
    rcu_periph_clock_enable(g_rcuPeriphMaps[gd32f4xx->sclpin_group]);

    /* enable I2C clock */
    rcu_periph_clock_enable(g_i2c_rcuPeriphMaps[gd32f4xx->bus]);

    /* connect i2c SCL */
    gpio_af_set(g_gpioxMaps[gd32f4xx->sclpin_group], GPIO_AF_4, g_GdRealPinMaps[gd32f4xx->sclpin]);

    /* connect i2c SDA */
    gpio_af_set(g_gpioxMaps[gd32f4xx->sdapin_group], GPIO_AF_4, g_GdRealPinMaps[gd32f4xx->sdapin]);

    gpio_mode_set(g_gpioxMaps[gd32f4xx->sdapin_group], GPIO_MODE_AF, GPIO_PUPD_PULLUP,g_GdRealPinMaps[gd32f4xx->sclpin]);
    gpio_output_options_set(g_gpioxMaps[gd32f4xx->sdapin_group], GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,g_GdRealPinMaps[gd32f4xx->sclpin]);
    gpio_mode_set(g_gpioxMaps[gd32f4xx->sdapin_group], GPIO_MODE_AF, GPIO_PUPD_PULLUP,g_GdRealPinMaps[gd32f4xx->sdapin]);
    gpio_output_options_set(g_gpioxMaps[gd32f4xx->sdapin_group], GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,g_GdRealPinMaps[gd32f4xx->sdapin]);

    /* enable I2C clock */
    rcu_periph_clock_enable(g_i2c_rcuPeriphMaps[gd32f4xx->bus]);
    /* configure I2C clock */
    i2c_clock_config(g_i2c_BaseMaps[gd32f4xx->bus], gd32f4xx->speed, I2C_DTCY_2);

    /* configure I2C address */
    i2c_mode_addr_config(g_i2c_BaseMaps[gd32f4xx->bus], I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_SLAVE_ADDRESS7);
    /* enable I2C */
    i2c_enable(g_i2c_BaseMaps[gd32f4xx->bus]);
    /* enable acknowledge */
    i2c_ack_config(g_i2c_BaseMaps[gd32f4xx->bus], I2C_ACK_ENABLE);

    return HDF_SUCCESS;
}

static int32_t GD32f4xxI2cReadDrs(struct Gd32f4xxI2cCntlr *gd32f4xx, const struct DeviceResourceNode *node)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint16(node, "bus", (uint32_t *)&gd32f4xx->bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: gd32f4xx->bus %x", __func__, gd32f4xx->bus);

    ret = drsOps->GetUint16(node, "sclpin_group", (uint16_t *)&gd32f4xx->sclpin_group, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read sclpin_group fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: gd32f4xx->sclpin_group %d", __func__, gd32f4xx->sclpin_group);

    ret = drsOps->GetUint16(node, "sdapin_group", (uint16_t *)&gd32f4xx->sdapin_group, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read sdapin_group fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: gd32f4xx->sdapin_group %d", __func__, gd32f4xx->sdapin_group);

    ret = drsOps->GetUint32(node, "sclpin", (char *)&gd32f4xx->sclpin, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read sclpin fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: gd32f4xx->sclpin %x", __func__, gd32f4xx->sclpin);

    ret = drsOps->GetUint32(node, "sdapin", (char *)&gd32f4xx->sdapin, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read sdapin fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: gd32f4xx->sdapin %x", __func__, gd32f4xx->sdapin);

    ret = drsOps->GetUint32(node, "speed", (uint16_t *)&gd32f4xx->speed, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read speed fail!", __func__);
        return ret;
    }
    HDF_LOGI("%s: gd32f4xx->speed %d", __func__, gd32f4xx->speed);

    return HDF_SUCCESS;
}



struct RealI2cResource {
    uint8_t port;
    uint8_t devMode;
    uint32_t devAddr;
    uint32_t speed;
    struct OsalMutex mutex;
};

#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define I2C_FIND_CONFIG(node, name, resource) \
    do { \
        if (strcmp(HCS_PROP(node, match_attr), name) == 0) { \
            resource->bus = HCS_PROP(node, bus); \
            resource->sclpin_group = HCS_PROP(node, sclpin_group); \
            resource->sdapin_group = HCS_PROP(node, sdapin_group); \
            resource->sclpin = HCS_PROP(node, sclpin); \
            resource->sdapin = HCS_PROP(node, sdapin); \
            resource->speed = HCS_PROP(node, speed); \
            result = HDF_SUCCESS; \
        } \
    } while (0)
#define PLATFORM_CONFIG HCS_NODE(HCS_ROOT, platform)
#define PLATFORM_I2C_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), i2c_config)
static uint32_t GetI2cDeviceResource(struct Gd32f4xxI2cCntlr *i2cResource, const char *deviceMatchAttr)
{
    int32_t result = HDF_FAILURE;
    struct Gd32f4xxI2cCntlr *resource = NULL;
    if (i2cResource == NULL || deviceMatchAttr == NULL) {
        HDF_LOGE("device or deviceMatchAttr is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    resource = i2cResource;
#if HCS_NODE_HAS_PROP(PLATFORM_CONFIG, i2c_config)
    HCS_FOREACH_CHILD_VARGS(PLATFORM_I2C_CONFIG, I2C_FIND_CONFIG, deviceMatchAttr, resource);
#endif
    if (result != HDF_SUCCESS) {
        HDF_LOGE("resourceNode %s is NULL\r\n", deviceMatchAttr);
    } else {
        //HdfI2cInit(i2cResource->port, i2cResource->speed, i2cResource->devAddr);
		GD32_I2C_Init(i2cResource);

		//gd32f4xx->cntlr.priv = (void *)node;
		gd32f4xx->cntlr.priv = (void *)i2cResource;
		i2cResource->cntlr.busId = gd32f4xx->bus;
		i2cResource->cntlr.ops = &g_GD32f4I2Cmethod;
		i2cResource->cntlr.lockOps = &g_GD32f4I2clockOps;
		(void)OsalSpinInit(&i2cResource->spin);
		int ret = I2cCntlrAdd(&i2cResource->cntlr);
		if (ret != HDF_SUCCESS) {
			HDF_LOGE("%s: add i2c controller fail:%d!", __func__, ret);
			(void)OsalSpinDestroy(&i2cResource->spin);
		}
    }
    return result;
}


#else
static int32_t GD32f4xxI2cParseAndInit(struct HdfDeviceObject *device, const struct DeviceResourceNode *node)
{
    int32_t ret;

    gd32f4xx = (struct Gd32f4xxI2cCntlr *)OsalMemCalloc(sizeof(*gd32f4xx));
    if (gd32f4xx == NULL) {
        HDF_LOGE("%s: malloc gd32f4xx fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = GD32f4xxI2cReadDrs(gd32f4xx, node);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: %d, read drs fail! ret:%d, HDF_SUCCESS %x", __func__, __LINE__, ret, HDF_SUCCESS);
    }
	else {
		ret = GD32_I2C_Init(gd32f4xx);

		gd32f4xx->cntlr.priv = (void *)node;
		gd32f4xx->cntlr.busId = gd32f4xx->bus;
		gd32f4xx->cntlr.ops = &g_GD32f4I2Cmethod;
		gd32f4xx->cntlr.lockOps = &g_GD32f4I2clockOps;
		(void)OsalSpinInit(&gd32f4xx->spin);
		ret = I2cCntlrAdd(&gd32f4xx->cntlr);
		if (ret != HDF_SUCCESS) {
			HDF_LOGE("%s: add i2c controller fail:%d!", __func__, ret);
			(void)OsalSpinDestroy(&gd32f4xx->spin);
		}
	}

    return ret;
}
#endif


int32_t GD32f4xxI2cInit(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: [line = %d]\n", __func__, __LINE__);

    int32_t ret = HDF_SUCCESS;
    const struct DeviceResourceNode *childNode = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        ret = GD32f4xxI2cParseAndInit(device, childNode);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }

#else
    gd32f4xx = (struct Gd32f4xxI2cCntlr *)OsalMemCalloc(sizeof(*gd32f4xx));
    if (gd32f4xx == NULL) {
        HDF_LOGE("%s: malloc gd32f4xx fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    ret = GetI2cDeviceResource(gd32f4xx, device->deviceMatchAttr);
#endif

    HDF_LOGI("%s: i2c init sucess!", __func__);

    return ret;
}

static void GD32f4xxI2cRemoveByNode(const struct DeviceResourceNode *node)
{
    int32_t ret;
    int16_t bus;
    struct I2cCntlr *cntlr = NULL;
    struct Gd32f4xxI2cCntlr *gd32f4xx = NULL;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return;
    }

    ret = drsOps->GetUint16(node, "bus", (uint16_t *)&bus, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!", __func__);
        return;
    }
    HDF_LOGI("%s: (uint16_t *)&gd32f4xx->bus %x", __func__, bus);

    cntlr = I2cCntlrGet(bus);
    if (cntlr != NULL && cntlr->priv == node) {
        I2cCntlrPut(cntlr);
        I2cCntlrRemove(cntlr);
        gd32f4xx = (struct Gd32f4xxI2cCntlr *)cntlr;
        OsalMemFree(gd32f4xx);
    }
    return;
}

void GD32f4xxI2cRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("%s: %d", __func__, __LINE__);
    const struct DeviceResourceNode *childNode = NULL;

#ifndef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL", __func__);
        return;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(device->property, childNode) {
        GD32f4xxI2cRemoveByNode(childNode);
    }
#else
    struct I2cCntlr *i2cCntrl = device->priv;
    if (i2cCntrl == NULL || i2cCntrl->priv == NULL) {
        HDF_LOGE("%s: i2cCntrl is NULL\r\n", __func__);
        return;
    }
    i2cCntrl->ops = NULL;
    struct RealI2cResource *i2cDevice = (struct I2cDevice *)i2cCntrl->priv;
    OsalMemFree(i2cCntrl);
    if (i2cDevice != NULL) {
        OsalMutexDestroy(&i2cDevice->mutex);
        OsalMemFree(i2cDevice);
    }
#endif

}

struct HdfDriverEntry g_hdfI2CDevice = {
    .moduleVersion = 1,
    .moduleName = "gd32f4xx_i2c_driver",
    .Init = GD32f4xxI2cInit,
    .Release = GD32f4xxI2cRelease,
};

HDF_INIT(g_hdfI2CDevice);
