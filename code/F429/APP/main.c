#include "system.h"

/**
  * @brief   	main函数，先初始化后在loop中执行任务
  * @param 		none
  * @retval   none
  */
int main(void)
{
	systemInit();
	while(1)
	{
		Loop();
	}
}
