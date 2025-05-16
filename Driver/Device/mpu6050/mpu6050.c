#include "mpu6050.h"

I2C_HandleTypeDef *g_mpu6050_i2c_handle_ptr;

static void MPU6050_WriteOneByte(uint8_t address, uint8_t data);
static uint8_t MPU6050_ReadOneByte(uint8_t address);
static void MPU6050_ReadBytes(uint8_t address, uint8_t *data, uint8_t len);
static void MPU6050_SetGyroRate(uint16_t rate);
static void MPU6050_SetDLPF(uint16_t rate);

/**
 * @brief MPU6050初始化函数
 * 
 * @param hi2c I2C句柄
 */
void MPU6050_Init(I2C_HandleTypeDef *hi2c)
{
    uint8_t device_address = 0;

    g_mpu6050_i2c_handle_ptr = hi2c;

    MPU6050_WriteOneByte(MPU6050_PWR_MGMT_1, 0x80);                             // 复位MPU6050
    HAL_Delay(300);
    MPU6050_WriteOneByte(MPU6050_PWR_MGMT_1, 0x00);                             // 电源管理寄存器1，取消睡眠模式

    MPU6050_WriteOneByte(MPU6050_GYRO_CONFIG, 0x18);                            // 陀螺仪配置寄存器，选择满量程为±2000°/s
    MPU6050_WriteOneByte(MPU6050_ACCEL_CONFIG, 0x18);                           // 加速度计配置寄存器，选择满量程为±16g

    device_address = MPU6050_ReadOneByte(MPU6050_WHO_AM_I);
    if (device_address == MPU6050_ADDRESS)
    {
        MPU6050_WriteOneByte(MPU6050_PWR_MGMT_1, 0x01);                         // 电源管理寄存器1，选择时钟源为X轴陀螺仪
        MPU6050_SetGyroRate(100);                                               // 设置陀螺仪采样率和低通滤波
        MPU6050_WriteOneByte(MPU6050_PWR_MGMT_2, 0x00);                         // 电源管理寄存器2，使能角速度和加速度传感器，退出待机模式
    }
}

/**

 * @brief MPU6050写一个字节函数
 * 
 * @param hi2c I2C句柄
 * @param address 寄存器地址
 * @param data 待写入的数据
 */
static void MPU6050_WriteOneByte(uint8_t address, uint8_t data)
{
    HAL_I2C_Mem_Write(g_mpu6050_i2c_handle_ptr, MPU6050_ADDRESS << 1, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

/**
 * @brief MPU6050读一个字节函数
 * 
 * @param address 寄存器地址
 * @return uint8_t 读取到的数据
 */
static uint8_t MPU6050_ReadOneByte(uint8_t address)
{
    uint8_t data = 0;

    HAL_I2C_Mem_Read(g_mpu6050_i2c_handle_ptr, MPU6050_ADDRESS << 1, address, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);

    return data;
}

/**
 * @brief MPU6050读多个字节函数
 * 
 * @param address 寄存器地址
 * @return uint8_t 读取到的数据
 */
static void MPU6050_ReadBytes(uint8_t address, uint8_t *data, uint8_t len)
{
    HAL_I2C_Mem_Read(g_mpu6050_i2c_handle_ptr, MPU6050_ADDRESS << 1, address, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
}

/**
 * @brief MPU6050设置陀螺仪采样率函数
 * 
 * @param rate 陀螺仪采样率，期望范围4~1000
 */
static void MPU6050_SetGyroRate(uint16_t rate)
{
    // 采样率频率输出最小值是4K，最大值是8K，期望1K
    rate = rate < 4 ? 4 : rate;
    rate = rate > 1000 ? 1000 : rate;

    // 根据采样率去设置低通滤波
    MPU6050_SetDLPF(rate);

    // 采样率=输出频率/(1+分频值) --> 分频值 = (输出频率/采样率) -1
    uint8_t sample_div = 1000 / rate - 1;                       // 获取分频值
    MPU6050_WriteOneByte(MPU6050_SMPLRT_DIV, sample_div);       // 写入分频值
}

/**
 * @brief MPU6050根据采样率去设置低通滤波函数
 * 
 * @param rate 陀螺仪采样率，期望范围4~1000
 */
static void MPU6050_SetDLPF(uint16_t rate)
{
    uint8_t bandwidths[] = {188, 98, 42, 20, 10, 5};
    // 采样定理，采样率 >= 2 * 带宽，不会失真 --> 带宽 <= 采样率 / 2
    uint8_t bandwidth = rate / 2;

    for (uint8_t i = 0; i < sizeof(bandwidths) / sizeof(bandwidths[0]); i++)
    {
        if (bandwidth >= bandwidths[i])
        {
            bandwidth = bandwidths[i];
            break;
        }
    }

    MPU6050_WriteOneByte(MPU6050_CONFIG, bandwidth);           //  设置低通滤波
}

/**
 * @brief MPU6050获取原始数据函数
 * 
 * @param data 保存读取的数据
 */
void MPU6050_GetData(MPU6050_Data_t *data)
{
    uint8_t buff[6] = {0};

    MPU6050_ReadBytes(MPU6050_ACCEL_XOUT_H, buff, 6);
    data->accel_x = ((int16_t)buff[0] << 8) | buff[1];
    data->accel_y = ((int16_t)buff[2] << 8) | buff[3];
    data->accel_z = ((int16_t)buff[4] << 8) | buff[5];

    MPU6050_ReadBytes(MPU6050_GYRO_XOUT_H, buff, 6);
    data->gyro_x = ((int16_t)buff[0] << 8) | buff[1];
    data->gyro_y = ((int16_t)buff[2] << 8) | buff[3];
    data->gyro_z = ((int16_t)buff[4] << 8) | buff[5];
}