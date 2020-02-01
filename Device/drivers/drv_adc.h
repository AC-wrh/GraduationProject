/*
 * @Description: 
 * @version: 
 * @Author: Adol
 * @Date: 2020-01-28 14:21:25
 * @LastEditTime : 2020-01-31 20:46:23
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_ADC_H__
#define __DRV_ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
rt_uint32_t get_adc_value(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /*__DRV_ADC_H__ */





