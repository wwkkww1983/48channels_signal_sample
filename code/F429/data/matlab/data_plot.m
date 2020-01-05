filename = '../csv/data002.csv';
data = csvread(filename);
plot(data(2000:5000,:) / 4096 * 3.3)