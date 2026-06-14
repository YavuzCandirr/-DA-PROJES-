/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SIFIR 0
#define FULLISIK 999
#define STRING_NUMARALARI 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

const char ilk_msg[]              = "\r\nBir kesif alani bulundu rota olusturulsun mu? Evet=a/Hayir=b";
const char basarisiz_gorev[]      = "\r\nGorev basarisiz oldu!!!";
const char kaza[]                 = "\r\nKesif yerine giderken sag matoru carptik! \r\nyakit kacriyor!\r\nRisk alip yola devam etmek istiyor musunuz? Evet=a/Hayir=b";
const char ilk_sorgu[]            = "\r\nyakit kaciriyor! \r\n mazottan mi devam edelim? \r\nbataryaya mi gecelim? Mazot=a/Batarya=b";
const char secim_yakit[]          = "\r\nyakit kacagi var! \r\nyakit durumu = %30 Tamam=a";
const char secim_batarya[]        = "\r\n yakıt kanalları kapatıldı. Batarya devrede. \r\nBatarya durum = %90 Tamam=a";
const char kesif[]                = "\r\nKesif alanina gidebilmek icin lufen guc tusuna basin! Power=a";
const char y_kamera_kesif[]       = "\r\nKesif Yapildi! \nKamerayı acarsaniz gorev tamamlanacak ancak eve donecek kadar yakit kalmayacak batarya kamera icin kullanilacak \n Evet=a/Hayir=b";
const char y_kamera_kesif_on[]    = "\r\nTebrikler! Gorev tamamlandi. \r\nYakit ve batarya bitti \r\nEve donus imkansiz...";
const char y_kamera_kesif_off[]   = "\r\nGorev %50 tamamlandi \r\nEve donuluyor...";
const char kritik_batarya_uyari[] = "\r\nKesif Yapildi! \r\nBatarya durumu kritik secim yapin \r\nGPS iptal et(Riskli=a)\r\nyuzeye cikin(yuksek risk)=b\r\nDevam (Yuksek Risk)=c";
const char secim_A[]              = "\r\nGPS iptal et(Riskli)\r\nRotasiz Suruklenme";
const char evedon []              = "\r\nTum yakitlar eve donmek icin kullanilacak \r\nEve donus baslatildi!";
const char b_risk[]               = "\r\nKamerayı acarsaniz gorev tamamlanacak ancak eve donecek batarya kalmayacak yakit kamera icin kullanilacak";
const char b_kamera_kesif_on[]    = "\r\nTebrikler! Gorev tamamlandi. \r\nyakit ve batarya bitti \r\nEve donus imkansiz";
const char a[]                    = "\r\nevet";
const char b[]                    = "\r\nhayır";
const char guc_mod[]              = "\r\nBatarya guc modu";
const char guc_mod2[]             = "\r\nyakit guc modu";
const char onay[]                 = "\r\n Onaylamak istiyor musunuz ? Onay=a";

volatile int button1 = 0;
volatile char tus = 0;
volatile uint32_t suan = 0;
volatile uint32_t son_zaman = 0;
volatile int baslangic = 0;
volatile uint8_t cakar = 0;
volatile int led_kilit = 0;
volatile int artis = 10;
volatile int zaman=0;
volatile int artis_yonu = 1;
volatile int pwm = 0; 

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
enum ida {
	ilk_soru,
	grv_basarisiz,
	kaza_durumu,
	ilk_sorgu_durumu,
	batarya_az,
	yakit_az,
	onay_durumu,
	onay2,
	kesif_durumu,
	kesif_durumu_2,
	guc_modu,
	guc_modu_2,
	y_kamera_secim,
	y_kamera_on,
	y_kamera_off,
	b_kamera_kesif,
	secim_a_durumu,
	b_kamerasi_kesif_on,
	eve_don,
	_b_risk
};
volatile enum ida ilk = ilk_soru;

typedef enum{
	yakit_secimi,
	batarya_secimi,
	secilmedi
}secimler;

secimler SecimDurumu = secilmedi;


volatile int mesaj_yazdirildi = SIFIR;
uint8_t rx_buffer[15];


void Mesaj_Gonder(const char* mesaj){
	HAL_UART_Transmit(&huart1, (uint8_t*)mesaj, strlen(mesaj),  STRING_NUMARALARI);

	mesaj_yazdirildi = 1;
}

const char* mesajKatalogu[] = {
		[ilk_soru]            = ilk_msg,
		[grv_basarisiz]       = basarisiz_gorev,
		[kaza_durumu]         = kaza,
		[ilk_sorgu_durumu]    = ilk_sorgu,
		[batarya_az]          = secim_batarya,
		[yakit_az]            = secim_yakit,
		[onay_durumu]         = onay,
		[onay2]               = onay,
		[kesif_durumu]	      = kesif,
		[kesif_durumu_2]	  = kesif,
		[guc_modu]		      = guc_mod,
		[guc_modu_2]	      = guc_mod2,
		[y_kamera_secim]      = y_kamera_kesif,
		[y_kamera_on]	      = y_kamera_kesif_on,
		[y_kamera_off]        = y_kamera_kesif_off,
		[b_kamera_kesif]      = kritik_batarya_uyari,
		[secim_a_durumu]	  = secim_A,
		[b_kamerasi_kesif_on] = b_kamera_kesif_on,
		[eve_don]		      = evedon
};


//PWM led fonksiyonu
void Nefes_Alan_Led (){
	uint32_t suan = HAL_GetTick();

	if(suan - zaman >=10){
		zaman = suan;

		if(artis_yonu == 1){
			pwm += artis;
		}else{
			pwm -= artis;
		}
		if(pwm >= FULLISIK){
			pwm = FULLISIK;
			artis_yonu = SIFIR;
		}
		else if(pwm <= SIFIR){
			pwm = SIFIR;
			artis_yonu = 1;
		}
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, pwm);

	}
}


//Çakar led fonksiyonu;
void Cakar_Led(){

	uint32_t suan = HAL_GetTick();

	if(led_kilit == SIFIR){

		if (suan - baslangic >= 3020) {
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, FULLISIK);
			led_kilit = 1;
			son_zaman = suan;
		}


		else if(suan - son_zaman >= 215){
			son_zaman = suan;

			if(cakar == SIFIR){
				cakar = 1;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, FULLISIK);
			}else{
				cakar = 0;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			}
		}
	}
}


void Batarya_Led_Mod(){
	HAL_GPIO_WritePin(GPIOE, blue_led_Pin, SET);
} 
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
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, 10);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	__HAL_TIM_MOE_ENABLE(&htim1);

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);




	for(int i = 0; i<5; i++){
		HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15); HAL_Delay(100);
	}

	MX_IWDG_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */



	while (1)
	{
		/* USER CODE END 2 */


		if (ilk == kaza_durumu) {
			Cakar_Led();
		}
		else if(ilk == batarya_az){
			Batarya_Led_Mod();
		}
		else if(ilk == yakit_az){
			Nefes_Alan_Led();
		}
		if(mesaj_yazdirildi == SIFIR){
			Mesaj_Gonder(mesajKatalogu[ilk]);
		}




		if(tus != SIFIR){
			char aktif_komut = (char)tus;
			tus = SIFIR;

			switch(ilk){




			case ilk_soru:
				if(aktif_komut == 'a'){
					ilk = kaza_durumu;
					mesaj_yazdirildi = SIFIR;
					son_zaman = HAL_GetTick();
					baslangic = HAL_GetTick();

				}
				else if(aktif_komut == 'b'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case kaza_durumu:
				if(aktif_komut == 'a'){
					ilk = ilk_sorgu_durumu;
					mesaj_yazdirildi = SIFIR;
				}
				else if(aktif_komut == 'b'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case ilk_sorgu_durumu:
				if(aktif_komut == 'b'){
					ilk = batarya_az;
					mesaj_yazdirildi = SIFIR;
				}
				else if(aktif_komut == 'a'){
					ilk = yakit_az;
					mesaj_yazdirildi = SIFIR;
				}
				break;


			case yakit_az:
				if(aktif_komut == 'a'){
					ilk = onay_durumu;
					mesaj_yazdirildi = SIFIR;
					SecimDurumu = yakit_secimi;

				}
				else if(aktif_komut == 'b'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = SIFIR;
				}
				break;
			case batarya_az:
				if(aktif_komut == 'a'){
					ilk = onay2;
					mesaj_yazdirildi = SIFIR;
					SecimDurumu = batarya_secimi;
				}
				else if(aktif_komut ==  'b'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = 0;
				}
				break;

			case onay_durumu:
				if(aktif_komut == 'a'){
					ilk = kesif_durumu;
					mesaj_yazdirildi =SIFIR;
				}
				else if(aktif_komut == 'b'){
					ilk = ilk_sorgu_durumu;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case onay2:
				if(aktif_komut == 'a'){
					ilk = kesif_durumu_2;
					mesaj_yazdirildi =SIFIR;
				}
				else if(aktif_komut == 'b'){
					ilk = ilk_sorgu_durumu;
					mesaj_yazdirildi = SIFIR;
				}
				break;


			case kesif_durumu:
				if(aktif_komut == 'a'){
					ilk = guc_modu;
					mesaj_yazdirildi = SIFIR;

				}
				else if(aktif_komut == 'b'){
					ilk = onay_durumu;
					mesaj_yazdirildi = SIFIR;
				}
				break;


			case kesif_durumu_2:
				if(aktif_komut == 'a'){
					ilk = guc_modu_2;
					mesaj_yazdirildi = SIFIR;

				}
				else if(aktif_komut == 'b'){
					ilk = onay2;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case guc_modu:
				if(aktif_komut == 'a'){
					ilk = y_kamera_secim;
					mesaj_yazdirildi = SIFIR;
				}
				else if(aktif_komut == 'b'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case guc_modu_2:
				if(aktif_komut == 'a'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = SIFIR;
				}
				else if(aktif_komut == 'b'){
					ilk = b_kamera_kesif;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case y_kamera_secim:
				if(aktif_komut == 'a'){
					ilk = y_kamera_on;
					mesaj_yazdirildi = 0;
				}
				else if(aktif_komut == 'b'){
					ilk = y_kamera_off;
					mesaj_yazdirildi = SIFIR;
				}
				break;

			case b_kamera_kesif:
				if(aktif_komut == 'a'){
					ilk = secim_a_durumu;
					mesaj_yazdirildi = SIFIR;
				}
				else if(aktif_komut == 'b'){
					ilk = grv_basarisiz;
					mesaj_yazdirildi = SIFIR;

				}
				else if(aktif_komut == 'c'){
					ilk = b_kamerasi_kesif_on;
					mesaj_yazdirildi = SIFIR;
				}
				break;


				aktif_komut = SIFIR;
			default:
				break;
			}
		}
		HAL_IWDG_Refresh(&hiwdg);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4090;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 47;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, blue_led_Pin|buton_led_Pin|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : emg_Pin */
  GPIO_InitStruct.Pin = emg_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(emg_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : blue_led_Pin buton_led_Pin PE15 */
  GPIO_InitStruct.Pin = blue_led_Pin|buton_led_Pin|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	tus = rx_buffer[0];
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, 10);
}

volatile uint32_t son_buton_basma;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == emg_Pin){

		uint32_t suan = HAL_GetTick();

		if(suan - son_buton_basma > 200){
			son_buton_basma = suan;
			HAL_GPIO_TogglePin(GPIOE, buton_led_Pin);
			ilk = eve_don;
			mesaj_yazdirildi = 0;
		}
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
	while (1)
	{
	}
}
  /* USER CODE END Error_Handler_Debug */

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
