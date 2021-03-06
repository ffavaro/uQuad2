close all
clear all
clc

data = importdata('log_debug_8');

figure
plot(data(:,13),data(:,14),'*r');
%axis equal
grid on

t = data(:,11)*1000 + data(:,12)/1000;
figure
plot(t,data(:,5),'*r');

figure
hold on
plot(t,data(:,16),'r');
plot(t,data(:,17),'b');
legend('yaw deseado','yaw simulado')
grid on