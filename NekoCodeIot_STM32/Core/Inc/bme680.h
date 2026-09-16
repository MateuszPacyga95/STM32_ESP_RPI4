

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

#define BME680_T_OVERSAMPLING_NONE        0x00
#define BME680_T_OVERSAMPLING_1X          0x01

#define BME680_REG_TEMP_MSB         0x22

typedef struct 
{
    SPI_HandleTypeDef *hspi;
    uint8_t address;
    

    //caliration data
    uint16_t par_t1;
    int16_t par_t2;
    int8_t par_t3;
} BME680_HandleTypeDef;

typedef enum
{
    BME680_OK = 0,
    BME680_ERROR = 1
} BME680_Status;


BME680_Status BME680_Init(BME680_HandleTypeDef *bme680, SPI_HandleTypeDef *hspi);
BME680_Status BME680_SetForcedMode(BME680_HandleTypeDef *bme680);
BME680_Status BME680_CalculateTemperature(BME680_HandleTypeDef *bme680, int32_t adc_temp, int32_t *temp);
BME680_Status BME680_ReadTemperature    (BME680_HandleTypeDef *bme680, int32_t *temperature);
BME680_Status BME680_SetTemperatureOversampling(BME680_HandleTypeDef *bme680, uint8_t oversampling);

#endif /* INC_BME680_H */