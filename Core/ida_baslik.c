/*
 * ida_baslik.c
 *
 *  Created on: 21 May 2026
 *      Author: ea.stajyer3
 */

#include "ida_baslik.h"

extern TIM_HandleTypeDef htim1;

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

