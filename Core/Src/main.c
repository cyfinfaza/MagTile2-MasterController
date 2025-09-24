/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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

#include "tusb.h"
#include <stdio.h>
#include "i2c_master.h"
#include "mt2_types.h"
#include "can.h"
#include "adc.h"
#include "reporter.h"
#include "safety_config.h"
#include "uart.h"
#include "serial_terminal.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_NodeTypeDef Node_GPDMA1_Channel0;
DMA_QListTypeDef List_GPDMA1_Channel0;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

DTS_HandleTypeDef hdts;

FDCAN_HandleTypeDef hfdcan1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

PCD_HandleTypeDef hpcd_USB_DRD_FS;

/* USER CODE BEGIN PV */

// make global variables for all digital inputs
uint8_t OV_SENSE_HV;
uint8_t OC_SENSE_HV;
uint8_t FAULT_12;
uint8_t BUTTON;
uint8_t BOOT0_SENSE;

// button last state
uint8_t BUTTON_LAST = 0;

MT2_Master_PowerSwitchStatus power_switch_status;
MT2_Master_PowerSystemFaults power_system_faults;
MT2_Master_UsbInterfaceStatus usb_interface_status;
MT2_Global_State global_state;

uint8_t clear_faults_requested = 0;

// HV state machine
uint8_t hv_active = 0;
uint8_t hv_activated_by_cdc = 0; // tells us whether to send ok response to cdc, whether to disable when disconnected
uint8_t precharge_complete = 0;

// make global variables for all digital outputs
extern uint8_t HV_RELAY;
uint8_t HV_RELAY = 0;
uint8_t SHDN_12 = 0;
uint8_t IND_R = 1;
uint8_t IND_G = 1;
uint8_t IND_B = 0;
uint8_t PRECHG_SSR = 0;

float vddcore;
int16_t mcu_temp;

int buffer_available_cdc = -2;
int buffer_available_vendor_0 = -2;
int buffer_available_vendor_1 = -2;
int buffer_available_vendor_2 = -2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_ADC1_Init(void);
static void MX_FLASH_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ICACHE_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_DTS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	IND_R = r;
	IND_G = g;
	IND_B = b;
}

// Loop time
uint32_t loop_time_start = 0;
uint32_t loop_time = 0;

// Precharge time
uint32_t precharge_time_start = 0;

// other
uint8_t can_blink = 0;

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
  MX_GPDMA1_Init();
  MX_ADC1_Init();
  MX_FLASH_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_ICACHE_Init();
  MX_USB_PCD_Init();
  MX_FDCAN1_Init();
  MX_DTS_Init();
  /* USER CODE BEGIN 2 */

	// start USB
	tusb_init();

	uint32_t inactivity_interval_start = HAL_GetTick();

	ADC_Init(&hadc1);

//	I2C_Init(&hi2c1);

	CAN_Init(&hfdcan1);

	UART_Init(&huart1);

	HAL_DTS_Start(&hdts);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	loop_time_start = HAL_GetTick();
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		// read all digital inputs
		OV_SENSE_HV = HAL_GPIO_ReadPin(OV_SENSE_HV_GPIO_Port, OV_SENSE_HV_Pin);
		OC_SENSE_HV = HAL_GPIO_ReadPin(OC_SENSE_HV_GPIO_Port, OC_SENSE_HV_Pin);
		FAULT_12 = HAL_GPIO_ReadPin(FAULT_12_GPIO_Port, FAULT_12_Pin);
		BUTTON = HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
		BOOT0_SENSE = HAL_GPIO_ReadPin(BOOT0_SENSE_GPIO_Port, BOOT0_SENSE_Pin);

		// read temp from DTS
		int32_t dts_temp = 0;
		HAL_DTS_GetTemperature(&hdts, &dts_temp);
		mcu_temp = dts_temp;

		// clear faults if requested
		if (clear_faults_requested) {
			power_system_faults.byte = 0;
			power_switch_status.flags.hv_shutdown_from_fault = 0;
			global_state.flags.global_fault_clear = 1;
			clear_faults_requested = 0;
		}

		// reset all fault flags if normal shutdown
		if (!power_switch_status.flags.hv_shutdown_from_fault) {
			power_system_faults.byte = 0;
		}

		// Check all fault thresholds
		if (v_sense_hv > TH_OV_HV) power_system_faults.flags.ov_hv = 1;
		if (v_sense_hv_in > TH_OV_HV) power_system_faults.flags.ov_hv = 1;
		if (v_sense_hv < TH_UV_HV) power_system_faults.flags.uv_hv = 1;
		if (v_sense_hv_in < TH_UV_HV) power_system_faults.flags.uv_hv = 1;
//		if (i_sense_hv > TH_OC_HV) power_system_faults.flags.oc_hv = 1;

		if (v_sense_12 > TH_OV_12) power_system_faults.flags.ov_12v = 1;
		if (v_sense_12_in > TH_OV_12) power_system_faults.flags.ov_12v = 1;
		if (v_sense_12 < TH_UV_12) power_system_faults.flags.uv_12v = 1;
		if (v_sense_12_in < TH_UV_12) power_system_faults.flags.uv_12v = 1;
		if (i_sense_12 > TH_OC_12) power_system_faults.flags.oc_12v = 1;

		if (v_sense_5 > TH_OV_5) power_system_faults.flags.ov_5v = 1;
		if (v_sense_5 < TH_UV_5) power_system_faults.flags.uv_5v = 1;
		if (i_sense_5 > TH_OC_5) power_system_faults.flags.oc_5v = 1;

		if (mcu_temp > TH_OVERTEMP_MCU) power_system_faults.flags.master_overtemp = 1;

		// check other fault reasons
		if (!FAULT_12) power_system_faults.flags.efuse_12v_fault = 1;
		if (!OV_SENSE_HV) power_system_faults.flags.ov_hv = 1;
		if (!OC_SENSE_HV) power_system_faults.flags.oc_hv = 1;

		// button logic
		if ((BUTTON || BOOT0_SENSE) && !BUTTON_LAST) {
			if (power_system_faults.byte) { // if there are faults, button clears them
				clear_faults_requested = 1;
			} else {
				hv_active = !hv_active;
			}
		}
		BUTTON_LAST = BUTTON || BOOT0_SENSE;

		// hv state machine
		if (hv_active && hv_activated_by_cdc && !usb_interface_status.flags.cdc_active) { // CDC activated HV then abandoned (once)
			hv_active = 0;
		}
		if (hv_active && !PRECHG_SSR) { // HV just requested (once)
			precharge_time_start = HAL_GetTick();
		}
		PRECHG_SSR = hv_active; // precharge SSR always matches HV state
		if (hv_active) {
			if (v_sense_hv > v_sense_hv_in * (1 - TH_PRECHG_VDROP) && v_sense_hv < v_sense_hv_in * (1 + TH_PRECHG_VDROP)) {
				// output within precharge thresholds (repeatedly)
				precharge_complete = 1;
			}
			if (precharge_complete) { // last precharge was successful (repeatedly)
				if (!HV_RELAY) { // precharge just completed (once)
					if (hv_activated_by_cdc) { // tell CDC that its "arm" command was successful (once)
						SerialTerminal_ReplyOk("arm");
					}
				}
				HV_RELAY = 1;
			} else if (HAL_GetTick() - precharge_time_start > TH_PRECHG_TIMEOUT) { // precharge just failed (once)
				power_system_faults.flags.precharge_fault = 1;
				if (hv_activated_by_cdc) { // tell CDC that its "arm" command failed (once)
					SerialTerminal_ReplyError("arm : precharge failed");
				}
			} // otherwise, we are still precharging
		} else {
			HV_RELAY = 0;
			PRECHG_SSR = 0;
			precharge_complete = 0;
			hv_activated_by_cdc = 0; // reset the flag when HV is deactivated
		}

		// clear all setpoints if the HV relay is off
		if (!HV_RELAY) {
			memset(coil_setpoints, 0, sizeof(coil_setpoints));
		}


		// process tile data to check for faults
		for (uint8_t i = 0; i < MAX_TILES; i++) {
			if (tile_last_seen[i] + ALIVE_TIMEOUT < HAL_GetTick() && tile_data[i].slave_status.flags.alive) {
				tile_data[i].slave_status.flags.alive = 0;
				if (hv_active) power_system_faults.flags.communication_fault = 1;
			}
			if (tile_data[i].slave_status.flags.alive) {
				if (tile_data[i].slave_faults.byte) {
					power_system_faults.flags.slave_fault = 1;
				}
			}
		}


		if (power_system_faults.byte && hv_active) {
			hv_active = 0;
			power_switch_status.flags.hv_shutdown_from_fault = 1;
			HV_RELAY = 0;
			PRECHG_SSR = 0;
		}

		power_switch_status.flags.hv_relay_on = HV_RELAY;
		power_switch_status.flags.precharge_ssr_on = PRECHG_SSR;
		power_switch_status.flags.shdn_12_on = SHDN_12;
		power_switch_status.flags.fault_12 = !FAULT_12;
		power_switch_status.flags.hv_ready = !power_system_faults.byte;

		global_state.flags.global_arm = HV_RELAY;

		// set LED
		if (HV_RELAY) {
			set_rgb(0, 0, 1); // Blue: Relay on (armed)
		} else if (hv_active && !precharge_complete) {
			set_rgb(0, 1, 1); // Cyan: Precharging
		} else if (power_switch_status.flags.hv_ready) {
			set_rgb(0, 1, 0); // Green: Ready to arm
		} else if (power_switch_status.flags.hv_shutdown_from_fault) {
			set_rgb(1, 0, 0); // Red: Fault
		} else {
			MT2_Master_PowerSystemFaults red_faults = power_system_faults;
			// clear fault flags that are not critical
			red_faults.flags.uv_5v = 0;
			red_faults.flags.uv_12v = 0;
			red_faults.flags.efuse_12v_fault = 0;
			if (red_faults.byte) {
				set_rgb(1, 0, 0); // Red: Fault
			} else {
				if (power_system_faults.flags.uv_5v && power_system_faults.flags.uv_12v) {
					set_rgb(1, 1, 0); // Yellow: Both 5v and 12v missing, assume cable unplugged
				} else {
					set_rgb(1, 0, 0); // Red: Only one is missing, bigger problem likely
				}
			}
		}

		// Write digital outputs
		HAL_GPIO_WritePin(HV_RELAY_GPIO_Port, HV_RELAY_Pin, HV_RELAY);
		HAL_GPIO_WritePin(SHDN_12_GPIO_Port, SHDN_12_Pin, SHDN_12);
		HAL_GPIO_WritePin(IND_R_GPIO_Port, IND_R_Pin, !IND_R);
		HAL_GPIO_WritePin(IND_G_GPIO_Port, IND_G_Pin, !IND_G);
		HAL_GPIO_WritePin(IND_B_GPIO_Port, IND_B_Pin, !IND_B);
		HAL_GPIO_WritePin(PRECHG_SSR_GPIO_Port, PRECHG_SSR_Pin, PRECHG_SSR);

		// update additional variables

		usb_interface_status.flags.vendor_active = tud_vendor_mounted();
		usb_interface_status.flags.cdc_active = tud_cdc_connected();

		if (HAL_GetTick() - inactivity_interval_start > 100) {
			inactivity_interval_start = HAL_GetTick();
			TileData_MarkInactiveTiles();
			TileData_ComputeCoordinates();
		}

		// Handle CAN failure
		CAN_KeepAlive();

//		I2C_ReadAllTiles_StatusOnly();
//		I2C_IterativeReadAllTiles();
//		I2C_ReadTileReg(0x01, 0x0A, &mt2_slave_data[0x01].v_sense_hv, 4);

		TileData_IterativeSendSetpoints();

		Reporter_IterativeReportMaster();
		for (int i = 0; i < 32; i++) {
			Reporter_IterativeReportAllTiles();
		}
		tud_task();
	}
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 250;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 8;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = ENABLE;
  hadc1.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_256;
  hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_4;
  hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_19;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DTS Initialization Function
  * @param None
  * @retval None
  */
static void MX_DTS_Init(void)
{

  /* USER CODE BEGIN DTS_Init 0 */

  /* USER CODE END DTS_Init 0 */

  /* USER CODE BEGIN DTS_Init 1 */

  /* USER CODE END DTS_Init 1 */
  hdts.Instance = DTS;
  hdts.Init.QuickMeasure = DTS_QUICKMEAS_DISABLE;
  hdts.Init.RefClock = DTS_REFCLKSEL_PCLK;
  hdts.Init.TriggerInput = DTS_TRIGGER_HW_NONE;
  hdts.Init.SamplingTime = DTS_SMP_TIME_8_CYCLE;
  hdts.Init.Divider = 0;
  hdts.Init.HighThreshold = 0x0;
  hdts.Init.LowThreshold = 0x0;
  if (HAL_DTS_Init(&hdts) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DTS_Init 2 */

  /* USER CODE END DTS_Init 2 */

}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 25;
  hfdcan1.Init.NominalSyncJumpWidth = 4;
  hfdcan1.Init.NominalTimeSeg1 = 13;
  hfdcan1.Init.NominalTimeSeg2 = 6;
  hfdcan1.Init.DataPrescaler = 3;
  hfdcan1.Init.DataSyncJumpWidth = 4;
  hfdcan1.Init.DataTimeSeg1 = 16;
  hfdcan1.Init.DataTimeSeg2 = 8;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief FLASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_FLASH_Init(void)
{

  /* USER CODE BEGIN FLASH_Init 0 */

  /* USER CODE END FLASH_Init 0 */

  /* USER CODE BEGIN FLASH_Init 1 */

  /* USER CODE END FLASH_Init 1 */
  if (HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_FLASH_Lock() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FLASH_Init 2 */

  /* USER CODE END FLASH_Init 2 */

}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00C035A6;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /** I2C Fast mode Plus enable
  */
  if (HAL_I2CEx_ConfigFastModePlus(&hi2c1, I2C_FASTMODEPLUS_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_DRD_FS.Instance = USB_DRD_FS;
  hpcd_USB_DRD_FS.Init.dev_endpoints = 8;
  hpcd_USB_DRD_FS.Init.speed = USBD_FS_SPEED;
  hpcd_USB_DRD_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_DRD_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_DRD_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.battery_charging_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.bulk_doublebuffer_enable = DISABLE;
  hpcd_USB_DRD_FS.Init.iso_singlebuffer_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_DRD_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, HV_RELAY_Pin|SHDN_12_Pin|IND_G_Pin|IND_R_Pin
                          |IND_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PRECHG_SSR_GPIO_Port, PRECHG_SSR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OV_SENSE_HV_Pin OC_SENSE_HV_Pin FAULT_12_Pin */
  GPIO_InitStruct.Pin = OV_SENSE_HV_Pin|OC_SENSE_HV_Pin|FAULT_12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : HV_RELAY_Pin SHDN_12_Pin */
  GPIO_InitStruct.Pin = HV_RELAY_Pin|SHDN_12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IND_G_Pin IND_R_Pin IND_B_Pin */
  GPIO_InitStruct.Pin = IND_G_Pin|IND_R_Pin|IND_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_Pin BOOT0_SENSE_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin|BOOT0_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PRECHG_SSR_Pin */
  GPIO_InitStruct.Pin = PRECHG_SSR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PRECHG_SSR_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
