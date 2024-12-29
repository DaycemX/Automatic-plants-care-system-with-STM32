#include "stm32f1xx_hal.h"

#define TEMP_SENSOR_PIN GPIO_PIN_0
#define LAMP_POT_PIN GPIO_PIN_1
#define AC_PIN GPIO_PIN_1
#define HEATER_PIN GPIO_PIN_0
#define BUTTON_ON_PIN GPIO_PIN_12
#define BUTTON_OFF_PIN GPIO_PIN_13

ADC_HandleTypeDef hadc;
GPIO_InitTypeDef GPIO_InitStruct = {0};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void ControlSystem(void);
static uint32_t ReadTemperature(void);
static uint32_t ReadLampBrightness(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC_Init();
  
  uint8_t systemOn = 0;

  while (1)
  {
    if (HAL_GPIO_ReadPin(GPIOB, BUTTON_ON_PIN) == GPIO_PIN_SET)
    {
      systemOn = 1;
    }
    
    if (HAL_GPIO_ReadPin(GPIOB, BUTTON_OFF_PIN) == GPIO_PIN_SET)
    {
      systemOn = 0;
      HAL_GPIO_WritePin(GPIOB, AC_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, HEATER_PIN, GPIO_PIN_RESET);
    }

    if (systemOn)
    {
      ControlSystem();
    }
  }
}

static void ControlSystem(void)
{
  uint32_t temperature = ReadTemperature();
  uint32_t lampBrightness = ReadLampBrightness();
  
  if (temperature > 30)
  {
    HAL_GPIO_WritePin(GPIOB, HEATER_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, AC_PIN, GPIO_PIN_SET);
  }
  else if (temperature < 20)
  {
    HAL_GPIO_WritePin(GPIOB, HEATER_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, AC_PIN, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOB, HEATER_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, AC_PIN, GPIO_PIN_RESET);
  }

  // Lamp brightness control (using PWM or DAC)
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, lampBrightness);
}

static uint32_t ReadTemperature(void)
{
  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
  uint32_t value = HAL_ADC_GetValue(&hadc);
  return value;
}

static uint32_t ReadLampBrightness(void)
{
  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
  uint32_t value = HAL_ADC_GetValue(&hadc);
  return value;
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin = TEMP_SENSOR_PIN | LAMP_POT_PIN | BUTTON_ON_PIN | BUTTON_OFF_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = AC_PIN | HEATER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BUTTON_ON_PIN | BUTTON_OFF_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void MX_ADC_Init(void)
{
  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc.Instance = ADC1;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfDiscConversion = 0;
  HAL_ADC_Init(&hadc);
}

void SystemClock_Config(void)
{
  // Clock configuration code here
}
