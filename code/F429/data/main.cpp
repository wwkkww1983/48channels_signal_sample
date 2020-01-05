#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>

using namespace std;

int file_index = 1;
char csv_file_name[30];
char bin_file_name[30];
ifstream TXT_file("./U_DIR.txt");
string U_DIR;

int main()
{
	ifstream Ifile;
	ofstream oFile;

	/* 先从U_DIR中读出U盘中的文件 */
	if (!TXT_file.is_open())
	{
		cout << "can not open this file" << endl;
		return 0;
	}
	int temp1 = 0, temp2 = 0, temp3 = 0;
	TXT_file >> U_DIR >> temp1 >> temp2 >> temp3; //分别是DMA每个buff的长度 ADC1的通道数 ADC2的通道数

	const int ADC_BUFFSIZE = temp1;
	const int ADC1_channel = temp2;
	const int ADC2_channel = temp3;
	const int ADC_channel = ADC1_channel + ADC2_channel;
	const int ADC1_buffer_size = ADC_BUFFSIZE * ADC1_channel * 2;
	const int ADC2_buffer_size = ADC_BUFFSIZE * ADC2_channel * 2;
	const int ADC_EACH_BUFFSIZE = ADC1_buffer_size + ADC2_buffer_size;

	/* 以下是移动文件的操作 */
	while (1)
	{
		sprintf(bin_file_name, "\\data%03d.bin", file_index);
		string src = U_DIR + bin_file_name;				 //U盘中的bin文件
		string dst = string(".\\bin\\") + bin_file_name; //要转移到的的那个目标文件夹
		string name = "move " + src + " " + dst;		 //调用指令
		const char *des_name = name.c_str();
		cout << des_name << endl;
		int test = system(des_name); //调用系统命令
		if (test != 0)
		{
			cout << "file not found!" << endl;
			break;
		}
		file_index++;
	}

	/* 以下是将bin转成csv的操作 */
	file_index = 1;
	while (1)
	{
		sprintf(csv_file_name, "./csv/data%03d.csv", file_index);
		sprintf(bin_file_name, "./bin/data%03d.bin", file_index);
		Ifile.open(bin_file_name, ios::binary);

		/* 如果文件打开成功 */
		if (Ifile.is_open())
		{
			oFile.open(csv_file_name, ios::out | ios::trunc);

			char table[ADC_EACH_BUFFSIZE];					//读出一次DMA中断所传输的所有通道的所有数据
			uint16_t ADC1_temp[ADC_BUFFSIZE][ADC1_channel]; //ADC1的部分
			uint16_t ADC2_temp[ADC_BUFFSIZE][ADC2_channel]; //ADC2的部分

			while (Ifile.read(table, ADC_EACH_BUFFSIZE)) //读出一次DMA中断所传输的所有通道的所有数据
			{
				memcpy(ADC1_temp, table, ADC1_buffer_size);
				memcpy(ADC2_temp, table + ADC1_buffer_size, ADC2_buffer_size);

				for (int i = 0; i < ADC_BUFFSIZE; i++)
				{
					char str[30];
					int j;
					j = sprintf(str, "%d", ADC1_temp[i][0]);   //第一个放ADC1第一个通道的数据
					for (int n = 0; n < ADC1_channel - 1; n++) //放ADC1剩下的通道数据
					{
						j += sprintf(str + j, ",%d", ADC1_temp[i][n + 1]);
					}
					for (int k = 0; k < ADC2_channel; k++) //放ADC2所有通道数据
					{
						j += sprintf(str + j, ",%d", ADC2_temp[i][k]);
					}
					oFile << str << endl;
				}

				// char str[30];
				// int j;
				// j = sprintf(str, "%d", temp[0]);
				// for (int i = 0; i < Channel - 1; i++)
				// {
				// 	j += sprintf(str + j, ",%d", temp[i + 1]);
				// }
				// oFile << str << endl;
			}
			oFile.close();
			Ifile.close();
			cout << csv_file_name << " created!\n";
			file_index++;
		}
		else
		{
			cout << "open file error" << endl;
			break;
		}
	}

	// system("pause");
	return 0;
}
