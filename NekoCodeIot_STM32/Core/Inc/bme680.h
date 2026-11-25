

#ifndef INC_BME680_H
#define INC_BME680_H

#include "spi.h"


// modes: unfortunatly, only SLEEP_MODE and FORCED_MODE are documented
#define BME680_SLEEP_MODE           0x00    // low power sleeping
#define BME680_FORCED_MODE          0x01    // perform one TPHG cycle (field data 0 filled)
#define BME680_PARALLEL_MODE        0x02    // no information what it does :-(
#define BME680_SQUENTUAL_MODE       0x02    // no information what it does (field data 0+1+2 filled)

// register addresses
#define BME680_REG_RES_HEAT_VAL     0x00
#define BME680_REG_RES_HEAT_RANGE   0x02
#define BME680_REG_RANGE_SW_ERROR   0x06

#define BME680_REG_IDAC_HEAT_BASE   0x50    // 10 regsrs idac_heat_0 ... idac_heat_9
#define BME680_REG_RES_HEAT_BASE    0x5a    // 10 registers res_heat_0 ... res_heat_9
#define BME680_REG_GAS_WAIT_BASE    0x64    // 10 registers gas_wait_0 ... gas_wait_9
#define BME680_REG_CTRL_GAS_0       0x70
#define BME680_REG_CTRL_GAS_1       0x71
#define BME680_REG_CTRL_HUM         0x72
#define BME680_REG_STATUS           0x73
#define BME680_REG_CTRL_MEAS        0x74
#define BME680_REG_CONFIG           0x75
#define BME680_REG_ID               0x50


#define BME680_CDM_T1               0xE9
#define BME680_CDM_T2               0x8A
#define BME680_CDM_T3               0x8C


#define BME680_CDM_P1               0x8E
#define BME680_CDM_P2               0x90
#define BME680_CDM_P3               0x92
#define BME680_CDM_P4               0x94
#define BME680_CDM_P5               0x96
#define BME680_CDM_P6               0x99
#define BME680_CDM_P7               0x98
#define BME680_CDM_P8               0x9C
#define BME680_CDM_P9               0x9E
#define BME680_CDM_P10              0xA0

#define BME680_CDM_H2_MSB           0xE1
#define BME680_CDM_H1_H2_LSB        0xE2
#define BME680_CDM_H1_MSB           0xE3
#define BME680_CDM_H3               0xE4
#define BME680_CDM_H4               0xE5
#define BME680_CDM_H5               0xE6
#define BME680_CDM_H6               0xE7
#define BME680_CDM_H7               0xE8



#define BME680_T_OVERSAMPLING_NONE        0x00
#define BME680_T_OVERSAMPLING_1X          0x01

#define BME680_REG_TEMP_MSB         0x22
#define BME680_REG_HUM_MSB          0x25
#define BME680_REG_PRESS_MSB        0x1F
#define BME680_GAS_R_MSB            0x2A

typedef enum
{
    BME680_OK = 0,
    BME680_ERROR = 1
} BME680_Status;

typedef struct BME680_HandleTypeDef
    {
        SPI_HandleTypeDef *hspi;
        uint8_t address;
        int32_t t_fine;
        

        //caliration data
        uint16_t par_t1;
        int16_t par_t2;
        int8_t par_t3;
        uint16_t par_h1;
        uint16_t par_h2;
        int8_t par_h3;
        int8_t par_h4;
        int8_t par_h5;
        uint8_t par_h6;
        int8_t par_h7;
        int8_t par_wwwwh1;
        int16_t par_gh2;
        int8_t par_gh3;
        uint16_t par_p1;
        int16_t par_p2;
        int8_t par_p3;
        int16_t par_p4;
        int16_t par_p5;
        int8_t par_p6;
        int8_t par_p7;
        int16_t par_p8;
        int16_t par_p9;
        uint8_t par_p10;
        BME680_Status (*read_cb_8)(struct BME680_HandleTypeDef *bme680, uint8_t tx, uint8_t *retValue);
        BME680_Status (*write_cb_8)(struct BME680_HandleTypeDef *bme680, uint8_t regAddr, uint8_t value);
        void(*delay_cb)(uint32_t delay_ms);
    } BME680_HandleTypeDef;



typedef struct
{
    uint16_t gas_raw;   /* 10-bit gas ADC: 0..1023 */
    uint8_t gas_range;  /* 0..15 */
    uint8_t gas_valid;  /* 0 lub 1 */
    uint8_t heat_stab;  /* 0 lub 1 */
} BME680_GasSample;

BME680_Status BME680_Init(BME680_HandleTypeDef *bme680, SPI_HandleTypeDef *hspi);
BME680_Status BME680_SetMode(BME680_HandleTypeDef *bme680, uint8_t mode);
BME680_Status BME680_ReadTemperature(BME680_HandleTypeDef *bme680, int32_t *temperature);
BME680_Status BME680_ReadHumidity(BME680_HandleTypeDef *bme680, int32_t *humidity);
BME680_Status BME680_ReadPressure(BME680_HandleTypeDef *bme680, int32_t *pressure);
BME680_Status BME680_SetTemperatureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling);
BME680_Status BME680_SetHumidityOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling);
BME680_Status BME680_SetPressureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling);
BME680_Status BME680_ReadGasRaw(BME680_HandleTypeDef *bme680,uint16_t *gas_raw);

#endif /* INC_BME680_H */




