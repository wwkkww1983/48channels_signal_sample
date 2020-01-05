clear;
clc;
filename = '../csv/data001.csv';
data = csvread(filename);
plot(data(330000:340000,3) / 4096 * 3.3)