#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

SemaphoreHandle_t xmutex;
SemaphoreHandle_t ymutex;

void vHIGH(void *pvParameters);
void vLOW(void *pvParameters);

void Init(){
  SYSCTL_RCGCGPIO_R |= 0x00000020;   //Initialize clock to PORTF
  while((SYSCTL_PRGPIO_R&0x00000020) == 0){}  //safety for clock initialization
  GPIO_PORTF_LOCK_R = 0x4C4F434B;
  GPIO_PORTF_CR_R = 0x1F;       //Enable change to PORTF
  GPIO_PORTF_DIR_R = 0x0E;      //Make led ports as output
  GPIO_PORTF_DEN_R = 0x1F;      // digital enable to pins
  GPIO_PORTF_PUR_R = 0x11;
}

int main(void)
{
	Init();
	xmutex = xSemaphoreCreateMutex();
	ymutex=xSemaphoreCreateMutex();
	xTaskCreate(vLOW,"Task 1",100, NULL,1,NULL);
	vTaskStartScheduler();
}
void vLOW(void *pvParameters)
{
	while(1)
	{
		xSemaphoreTake(xmutex, portMAX_DELAY);
		xTaskCreate(vHIGH,"Task 2",100, NULL,2,NULL);
		xSemaphoreTake(ymutex,portMAX_DELAY);
		GPIO_PORTF_DATA_R=0x02;
		xSemaphoreGive(xmutex);	
	}
}

void vHIGH(void *pvParameters)
{
	while(1)
	{
		xSemaphoreTake(ymutex, portMAX_DELAY);
		xSemaphoreTake(xmutex, portMAX_DELAY);
		GPIO_PORTF_DATA_R=0x04;
		xSemaphoreGive(ymutex);			
	}
}
void vApplicationIdleHook(void){}
