#include "my_math.h"

/**
  * @brief     	int类型限幅
  * @param[in] 	amt：要限制的数的指针
	* @param[in] 	low：最小值
  * @param[in] 	high：最大值
  * @retval    	none
  */
void constrain_int(int *amt, int low, int high)
{
  if (*amt > high)
    *amt = high;
  if (*amt < low)
    *amt = low;
}

/**
  * @brief     	float类型限幅
  * @param[in] 	amt：要限制的数的指针
	* @param[in] 	low：最小值
  * @param[in]  high：最大值
  * @retval    	none
  */
void constrain_float(float *amt, float low, float high)
{
	if (*amt > high)
    *amt = high;
  if (*amt < low)
    *amt = low;
}

/**
  * @brief     	float类型的绝对值限幅
  * @param[in] 	amt：要限制的数的指针
	* @param[in] 	ABS_MAX：限制的绝对值最大值
  * @retval    	none
  */
void fabs_limit(float *amt, float ABS_MAX)
{
  if (*amt > ABS_MAX)
    *amt = ABS_MAX;
  if (*amt < -ABS_MAX)
    *amt = -ABS_MAX;
}



__asm void WFI_SET(void)
{
	WFI;		  
}

__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}

__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}

__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}



