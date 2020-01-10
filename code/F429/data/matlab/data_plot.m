close all;
clear;
clc;
begin_point_num = 1300;
end_point_num = 1500;
filename = '../csv/data001.csv';
data = csvread(filename);
plot(data(begin_point_num:end_point_num,:) / 4096 * 3.3)
figure
subplot(3,1,1);
plot(data(begin_point_num:end_point_num,1) / 4096 * 3.3)
subplot(3,1,2);
plot(data(begin_point_num:end_point_num,2) / 4096 * 3.3)
subplot(3,1,3);
plot(data(begin_point_num:end_point_num,3) / 4096 * 3.3)