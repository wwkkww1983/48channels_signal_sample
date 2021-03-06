#include "sd.h"

u8 res;
FIL* f_bin=0;
u8 *buf;
u8 *pname=0;

bool SD_RC_FLAG = 0,SD_POINT_FLAG = 0;

void SD_mem_init(void)
{
	my_mem_init(SRAMIN);
	my_mem_init(SRAMCCM);

 	exfuns_init();
  f_mount(fs[0],"0:",1);
	
	f_mkdir("0:bin");
	
	f_bin=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	buf=(u8*)mymalloc(SRAMIN,32);
	pname=(u8*)mymalloc(SRAMIN,32);
	while(!f_bin||!buf)
	{
		yixiuge_printf("malloc fail\n");
		delay_ms(200);		
	}
	text_new_pathname(pname);
}

u32 time;
void Write_to_Card(void)
{
//	time++;
//	u8 str[4];
//	memcpy(str,&time,4);
	char str[1000];
	int j;
	j  = sprintf( str,     "%.3f  ",1.234);
	j += sprintf( str + j, "%d  ",time);
	j += sprintf( str + j, "%c",'\r');
	j += sprintf( str + j, "%c",'\n');
	
	res=f_open(f_bin,"test.txt",FA_OPEN_ALWAYS|FA_WRITE);
	if(res==0)
	{
		f_lseek(f_bin,f_bin->fsize);
		res=f_write(f_bin,str,j,(UINT*)&br);
		if(res==0)
		f_close(f_bin);
	}
}

void save_data(u16 data)
{
	char str[100];
	int j;
	j  = sprintf( str,     "%d\r\n",data);
	res=f_open(f_bin,"test.txt",FA_OPEN_ALWAYS|FA_WRITE);
	if(res==0)
	{
		f_lseek(f_bin,f_bin->fsize);
		res=f_write(f_bin,str,j,(UINT*)&br);
		if(res==0)
		f_close(f_bin);
	}
}

void SD_save_test(void)
{
	u16 data_temp[10][10];
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			data_temp[i][j] = j * 10;
		}
	}
	res=f_open(f_bin,"0:bin/test.bin",FA_OPEN_ALWAYS|FA_WRITE);
	if(res==0)
	{
		f_lseek(f_bin,f_bin->fsize);
		res=f_write(f_bin,data_temp,200,(UINT*)&br);
		res=f_write(f_bin,data_temp,200,(UINT*)&br);
		if(res==0)
		f_close(f_bin);
	}
}

void text_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=1;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:bin/data%03d.bin",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);
		if(res==FR_NO_FILE)break;
		index++;
	}
}


void save_adc_data(void)
{
	if(ADC1_data_update)
	{
		while(ADC2_data_update == 0);
		res=f_open(f_bin,(const TCHAR*)pname,FA_OPEN_ALWAYS|FA_WRITE);
		if(res==0)
		{
			f_lseek(f_bin,f_bin->fsize);
			ADC1_data_update = 0;
			ADC2_data_update = 0;
			res=f_write(f_bin,ADC_PTR.ADC1_CurrentBuffPtr,ADC_BUFFSIZE*ADC1_CHANNEL*2,(UINT*)&br);			//写入ADC1的数据 长度为ADC_BUFFSIZE*ADC1_CHANNEL*2
			res=f_write(f_bin,ADC_PTR.ADC2_CurrentBuffPtr,ADC_BUFFSIZE*ADC2_CHANNEL*2,(UINT*)&br);			//写入ADC2的数据 长度为ADC_BUFFSIZE*ADC2_CHANNEL*2
			if(res==0)
				f_close(f_bin);
		}
	}
	else if(ADC2_data_update)
	{
		while(ADC1_data_update == 0);
		res=f_open(f_bin,(const TCHAR*)pname,FA_OPEN_ALWAYS|FA_WRITE);
		if(res==0)
		{
			f_lseek(f_bin,f_bin->fsize);
			ADC1_data_update = 0;
			ADC2_data_update = 0;
			res=f_write(f_bin,ADC_PTR.ADC1_CurrentBuffPtr,ADC_BUFFSIZE*ADC1_CHANNEL*2,(UINT*)&br);			//写入ADC1的数据 长度为ADC_BUFFSIZE*ADC1_CHANNEL*2
			res=f_write(f_bin,ADC_PTR.ADC2_CurrentBuffPtr,ADC_BUFFSIZE*ADC2_CHANNEL*2,(UINT*)&br);			//写入ADC2的数据 长度为ADC_BUFFSIZE*ADC2_CHANNEL*2
			if(res==0)
				f_close(f_bin);
		}
	}
}

void save_big_data_test(void)
{
	res=f_open(f_bin,(const TCHAR*)pname,FA_OPEN_ALWAYS|FA_WRITE);
	if(res==0)
	{
		f_lseek(f_bin,f_bin->fsize);
		res=f_write(f_bin,ADC_PTR.ADC1_CurrentBuffPtr,ADC_BUFFSIZE*ADC1_CHANNEL*2,(UINT*)&br);			//写入ADC1的数据 长度为ADC_BUFFSIZE*ADC1_CHANNEL*2
		res=f_write(f_bin,ADC_PTR.ADC2_CurrentBuffPtr,ADC_BUFFSIZE*ADC2_CHANNEL*2,(UINT*)&br);			//写入ADC2的数据 长度为ADC_BUFFSIZE*ADC2_CHANNEL*2
		res=f_write(f_bin,ADC_PTR.ADC1_CurrentBuffPtr,ADC_BUFFSIZE*ADC1_CHANNEL*2,(UINT*)&br);			//写入ADC1的数据 长度为ADC_BUFFSIZE*ADC1_CHANNEL*2
		res=f_write(f_bin,ADC_PTR.ADC2_CurrentBuffPtr,ADC_BUFFSIZE*ADC2_CHANNEL*2,(UINT*)&br);			//写入ADC2的数据 长度为ADC_BUFFSIZE*ADC2_CHANNEL*2
		res=f_write(f_bin,ADC_PTR.ADC1_CurrentBuffPtr,ADC_BUFFSIZE*ADC1_CHANNEL*2,(UINT*)&br);			//写入ADC1的数据 长度为ADC_BUFFSIZE*ADC1_CHANNEL*2
		res=f_write(f_bin,ADC_PTR.ADC2_CurrentBuffPtr,ADC_BUFFSIZE*ADC2_CHANNEL*2,(UINT*)&br);			//写入ADC2的数据 长度为ADC_BUFFSIZE*ADC2_CHANNEL*2
		if(res==0)
			f_close(f_bin);
	}
}


