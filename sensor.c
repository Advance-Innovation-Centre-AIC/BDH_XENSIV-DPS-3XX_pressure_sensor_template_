/******************************************************************************
* File Name:   sensor.c
*
* Description: This file implements the interface with the motion sensor, as 
*              well as the buffer management of the sensor data.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2021-2022, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#include "mtb_bmx160.h"

#include "cyhal.h"
#include "cybsp.h"
#include "main.h"
#include <math.h>

/** Number of axis used and sample data format */
#define INERTIAL_AXIS_SAMPLED   3

/*******************************************************************************
* Constants
*******************************************************************************/
#ifdef CY_BMX_160_IMU_SPI
    #define IMU_SPI_FREQUENCY 10000000

#endif

#ifdef CY_BMX_160_IMU_SPI
    /* BMX160 driver structures */
    static mtb_bmx160_t motion_sensor;
    static mtb_bmx160_data_t raw_data;
    static float imu_data[INERTIAL_AXIS_SAMPLED];

    /* SPI object for data transmission */
    static cyhal_spi_t mSPI;

    bool ret = false;

#endif

/*******************************************************************************
* Function Name: sensor_init
********************************************************************************
* Summary:
*   Initialize the sensor data.
*
* Parameters:
*     None
*
* Return:
*   The status of the initialization.
*******************************************************************************/
cy_rslt_t sensor_init(void)
{
    cy_rslt_t result;

#ifdef CY_IMU_SPI

    /* Make sure BMX160 Accelerometer gets SS pin high early after power-up, so it switches to SPI mode */
        result = cyhal_gpio_init(mSPI_SS, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
        CY_ASSERT(result == CY_RSLT_SUCCESS);

        /* SPI chip select is controlled by the BMX160 driver, not the SPI controller */
        result = cyhal_spi_init(&mSPI, mSPI_MOSI, mSPI_MISO, mSPI_SCLK, NC, NULL, 8, CYHAL_SPI_MODE_00_MSB, false);
        CY_ASSERT(result == CY_RSLT_SUCCESS);

        result = cyhal_spi_set_frequency(&mSPI, SPI_FREQ_HZ);
        CY_ASSERT(result == CY_RSLT_SUCCESS);

        result = mtb_bmx160_init_spi(&motion_sensor, &mSPI, mSPI_SS);
        if (result != CY_RSLT_SUCCESS)
        {
            printf("ERR: IMU init failed (0x%04x)!\n", ret);
        }
        else {
        	ret = true;
            printf("BMX160 IMU is online\n");
        }

#endif

    return result;
}

/*******************************************************************************
* Function Name: sensor_get_data
********************************************************************************
* Summary:
*   Return sensor data. This function is blocking. Only returns when data is
*   available.
*
* Parameters:
*     None
*
* Return:
*   Returns success if data returned.
*******************************************************************************/
cy_rslt_t sensor_get_data(void)
{
    /* Read data from IMU sensor */
    cy_rslt_t result;

    float temp_data[INERTIAL_AXIS_SAMPLED];

    #ifdef CY_BMX_160_IMU_SPI
    	result = mtb_bmx160_read(&motion_sensor, &raw_data);
        bool ret = false;

        if(result == CY_RSLT_SUCCESS) {
            ret = true;
            float heading;
            // get x, y, and z magnetic field readings from the sensor
            heading = atan2(raw_data.mag.y, raw_data.mag.x) * 180 / M_PI;

            printf("Accelerometer: X:%6d Y:%6d Z:%6d\r\n", raw_data.accel.x, raw_data.accel.y, raw_data.accel.z);
            printf("Gyroscope: X:%6d Y:%6d Z:%6d\r\n\r\n", raw_data.gyro.x, raw_data.gyro.y, raw_data.gyro.z);
            printf("Magnetometer: X:%6d Y:%6d Z:%6d\r\n\r\n", raw_data.mag.x, raw_data.mag.y, raw_data.mag.z);
            printf("Compass Heading: %6f\r\n\r\n", heading);

            // For Serial Studio
            //printf("/*%d,%d,%d,%d,%d,%d,%f*/\r\n",raw_data.accel.x, raw_data.accel.y, raw_data.accel.z,
            //  		raw_data.gyro.x, raw_data.gyro.y, raw_data.gyro.z,heading);

        }
        // Fusion Data of Inertial Sensor
//        if(result == CY_RSLT_SUCCESS) {
//            temp_data[0] = raw_data.accel.x / IMU_SCALING_CONST;
//            temp_data[1] = raw_data.accel.y / IMU_SCALING_CONST;
//            temp_data[2] = raw_data.accel.z / IMU_SCALING_CONST;
//
//            imu_data[0] = temp_data[0] * CONVERT_G_TO_MS2;
//            imu_data[1] = temp_data[1] * CONVERT_G_TO_MS2;
//            imu_data[2] = temp_data[2] * CONVERT_G_TO_MS2;
//            printf("Converted Accel: X:%6d Y:%6d Z:%6d\r\n", imu_data[0], imu_data[1], imu_data[2]);
//
//        }
//        else {
//            printf("ERR: no Accel data!\n");
//            imu_data[0] = 0.0f;
//            imu_data[1] = 0.0f;
//            imu_data[2] = 0.0f;
//        }
#endif
    return CY_RSLT_SUCCESS;
}


