/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sim800l.h"
#include "nmea.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define URL_LEN 150
#define SMS_LEN 75

#define MAX_BATTERY_VOLTAGE 4200
#define MIN_BATTERY_VOLTAGE 3400
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */
//USART 2 - GPS
//USART 3 - SIM800L
uint8_t decoy[NMEA0183_MAX_LENGTH + 1] = "$GPRMC,203522.00,A,5109.0262308,N,11401.8407342,W,0.004,133.4,130522,0.0,E,D*2B\r\n";

extern uint8_t usart2_buffer[2];
extern uint8_t usart3_buffer[2];
extern uint8_t sentence[NMEA0183_MAX_LENGTH + 1];

extern bool gpsSentenceReady;
extern bool simResponseReady;

bool connectToInternet = false;
bool sendBatteryPercentage = true;

uint8_t GPSLat[15];
uint8_t GPSLong[15];

uint8_t elDiegoDesbordado = 0;
uint8_t perc = 0;

uint8_t state = 0;

uint32_t UDID = /*100*/80085;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
//uint16_t VoltageToPercentage(uint16_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

	GPRMC_t GPS;

	uint8_t url[URL_LEN];
	uint8_t sms[SMS_LEN];
	uint8_t check = 0;
	uint8_t check2 = 0;
	uint16_t volt = 0;

	bool bypass = false;

	SIM800L_Wakeup();
	SIM800L_Reset();

	HAL_Delay(2000);

	HAL_UART_Receive_DMA(&huart2, usart2_buffer, 1);
	HAL_UART_Receive_DMA(&huart3, usart3_buffer, 1);

	state = GPS_UPDATE; //Outside while statement, but inside of main so it's executed upon wakeup
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while(1)
	{
		switch(state)
		{
			case GPS_UPDATE:
				if(gpsSentenceReady)
				{
					if(!ParseGPRMC(sentence, &GPS))
					{
						if(CalculateChecksum(sentence) == GPS.checksum)
						{
              BuildGPSCoord(GPS.latdeg, GPS.Hlatmin, GPS.Llatmin, GPS.latdir, GPSLat);
              BuildGPSCoord(GPS.longdeg, GPS.Hlongmin, GPS.Llongmin, GPS.longdir, GPSLong);
							sprintf(url,"AT+HTTPPARA=\"URL\",\"http://186.13.143.39:80/gpsdata.php\?lat=%s&lng=%s&did=%ld\"\n\r", GPSLat, GPSLong, UDID);

							state = WEB_UPDATE;
						}
					}
				}
				break;

			case WEB_UPDATE:
				check = SIM800L_WEBUpdate(url);
				switch(check)
				{
					case SUCCESS:
						state = BAT_UPDATE;
						//HAL_Delay(5000);

						break;

					case FATAL_ERROR:
						state = GPS_UPDATE; //Continuing because I have no idea what it's actually supposed to do
						break;
				}
				break;

			case BAT_UPDATE:
				if(/*elDiegoDesbordado >= 2*/1) //Bypassing because everything gets reset and the variable is always 0
				{
					if(!bypass) check = SIM800L_BatteryCheck(&volt);

					switch(check)
					{
						case SUCCESS:
							if(!bypass)
							{
								perc = VoltageToPercentage(volt);
								sprintf(url, "AT+HTTPPARA=\"URL\",\"http://186.13.143.39:80/bl.php\?bl=%d&did=%ld\"\n\r", perc, UDID);
								//HAL_Delay(1000);
								bypass = true;
							}

							check2 = SIM800L_WEBUpdate(url);
							switch(check2) //They are identical for some reason, leaving it in in case it's a mistake (ask Lucas)
							{
								case SUCCESS:
                  if(perc <= 10 && sendBatteryPercentage)
                  {
                    sprintf(sms, "Advertencia: Batería baja. Porcentaje: %d%%\n\r\x1A", perc);
                    sendBatteryPercentage = false;
                    state = AYUDAME_LOCOOOOOOOOOO;
                  }
                  else
                  {
									  state = A_MIMIR;
                  }

								case FATAL_ERROR:
									bypass = false;
									break;
							}

							break;

						case FATAL_ERROR:
							//Do something, I guess?
							break;
					}
				}
				else
				{
					state = A_MIMIR;
				}
				break;


			case A_MIMIR:
				RTC_Sleep(1);
				break;

      case AYUDAME_LOCOOOOOOOOOO:
        check = SIM800L_SendSMS(sms);
        switch(check)
        {
          case SUCCESS:
            state = A_MIMIR;
            break;

          case FATAL_ERROR:
            state = A_MIMIR;
            break;
        }
        break;
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

uint16_t VoltageToPercentage(uint16_t Volt) {
	uint16_t aux = ((MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE)
			- (MAX_BATTERY_VOLTAGE - Volt)) * 100
			/ (MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE);
	return aux;
	}

void RTC_Sleep(uint8_t min)
{
  RTC_AlarmTypeDef aux;
  RTC_TimeTypeDef currTime;

	//Perform power-down tasks
	SIM800L_Sleep(); //Put SIM800L to sleep

	//Timer mode calculation
  aux.Alarm = RTC_ALARM_A;
  
   HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
  
  aux.AlarmTime = currTime;
  
  aux.AlarmTime.Minutes += min; //This is buggy, in theory min can't be a value higher than 196 or else it will overflow
  if(aux.AlarmTime.Minutes > 59) 
  {
    aux.AlarmTime.Minutes = 0;
    aux.AlarmTime.Hours++;
  }

  aux.AlarmTime.Hours += (min / 60); //Should be an integer division, ffs
  if(aux.AlarmTime.Hours > 23) aux.AlarmTime.Hours = 0;

	HAL_RTC_SetAlarm_IT(&hrtc, &aux, RTC_FORMAT_BIN);

	//Final powerdown
	HAL_SuspendTick();

  //Clear leftover flags
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

//Goodnight my sweet prince
	HAL_PWR_EnterSTANDBYMode(); 

}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) //Executed upon wakeup
{
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
	  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
	  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		HAL_ResumeTick(); //Wakey wakey
		/*
		MX_USART3_UART_Init();
		SIM800L_Wakeup(); //Wake up the SIM800L module

	  //These lines below might not be necessary at all
		HAL_UART_Receive_DMA(&huart2, usart2_buffer, 1);
		state = GPS_UPDATE; //Reset machine state
		//
		 * */

//Gets reset
		/*
	  if(elDiegoDesbordado < 10)
	  {
		elDiegoDesbordado++;
	  }
	  else
	  {
		elDiegoDesbordado = 0;
	  }

	  if(UDID < 500)
	  {
		UDID += 100;
	  }
	  else
	  {
		UDID = 0;
	  }
*/
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
		/* User can add his own implementation to report the HAL error return state */
		__disable_irq();
		while (1) {
		}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
