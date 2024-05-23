#ifndef GD32_SPI_H
#define GD32_SPI_H

#include "stdio.h"
#include "main.h"
#include "hal_gpio.h"
#include "gd32f4xx_spi.h"
#include "spi_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


#define DUMMY_BYTE       0xA5
#define GD32F4XX_SPI_MAX 6


typedef struct {
    uint32_t transMode;
    uint32_t masterSlave;
    uint32_t framesize;
    uint32_t clkpolarity;
    uint32_t nssmode;
    uint32_t prescale;
    uint32_t endian;
    uint32_t csgpionum;
} SpiSource;

typedef struct {
    uint8_t spiCSPort;
    uint16_t spiCSPin;
    uint8_t spiSckPort;
    uint16_t spiSckPin;
    uint8_t spiMosiPort;
    uint16_t spiMosiPin;
    uint8_t spiMisoPort;
    uint16_t spiMisoPin;
} SpiUartSet;

typedef struct {
    uint32_t spiId;
    SpiSource spiSource;
    SpiUartSet spiUartSet;
    uint32_t csnum;
} SpiDevice;

static const uint16_t g_spi_rcuPeriphMaps[GD32F4XX_SPI_MAX] = {
    RCU_SPI0,
    RCU_SPI1,
    RCU_SPI2,
    RCU_SPI3,
    RCU_SPI4,
    RCU_SPI5,
};

static const uint32_t g_spi_BaseMaps[GD32F4XX_SPI_MAX] = {
    SPI0,
    SPI1,
    SPI2,
    SPI3,
    SPI4,
    SPI5,
};

static const uint32_t g_transmode_BaseMaps[4] = {
    SPI_TRANSMODE_FULLDUPLEX,
    SPI_TRANSMODE_RECEIVEONLY,
    SPI_TRANSMODE_BDRECEIVE,
    SPI_TRANSMODE_BDTRANSMIT,
};

static const uint32_t g_spimode_BaseMaps[2] = {
    SPI_MASTER,
    SPI_SLAVE,
};

static const uint32_t g_framesize_BaseMaps[2] = {
    SPI_FRAMESIZE_16BIT,
    SPI_FRAMESIZE_8BIT,
};

static const uint32_t g_clkpolarity_BaseMaps[4] = {
    SPI_CK_PL_LOW_PH_1EDGE,
    SPI_CK_PL_HIGH_PH_1EDGE,
    SPI_CK_PL_LOW_PH_2EDGE,
    SPI_CK_PL_HIGH_PH_2EDGE,
};

static const uint32_t g_nssmode_BaseMaps[2] = {
    SPI_NSS_SOFT,
    SPI_NSS_HARD,
};

static const uint32_t g_clkprescale_BaseMaps[8] = {
    SPI_PSC_2,
    SPI_PSC_4,
    SPI_PSC_8,
    SPI_PSC_16,
    SPI_PSC_32,
    SPI_PSC_64,
    SPI_PSC_128,
    SPI_PSC_256,
};

static const uint32_t g_endian_BaseMaps[2] = {
    SPI_ENDIAN_MSB,
    SPI_ENDIAN_LSB,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* GD32_SPI_H */



