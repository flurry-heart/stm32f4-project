#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stm32f4xx_hal.h"

#include "mpu6050_register.h"
#include "bsp_i2c.h"

#define MPU6050_ADDRESS     0x68

typedef struct MPU6050_Data_t
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} MPU6050_Data_t;

void MPU6050_Init(I2C_HandleTypeDef *hi2c);
void MPU6050_GetData(MPU6050_Data_t *data);

#endif // !__MPU6050_H__