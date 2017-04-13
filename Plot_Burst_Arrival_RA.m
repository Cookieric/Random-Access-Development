clear;
clc;
%=============模擬結果===========%
%A(1,:) 第一Row ; A（：，1）第一Col
%Anal_Fig1 = csvread('rst.csv');
%%
% TrafficModel1_Simulation = csvread('SimArrivalMode1_M_5000_RA_Slot_1_12000.csv');
% TrafficModel1_Analysis = csvread('AnalyArrivalMode1_M_5000_RA_Slot_1_12000.csv');
% TrafficModel1_Analysis_CDF = csvread('CDF_AnalyArrivalMode1_M_5000_RA_Slot_1_12000.csv');
% TrafficModel1_Simulation_CDF = csvread('CDF_SimArrivalMode1_M_5000_RA_Slot_1_12000.csv');
%%
% TrafficModel2_Analysis = csvread('Analysis_TrafficMode2_M_5000_RA_Slot_0_2000.csv');
% TrafficModel2_Analysis_CDF = csvread('CDF_TrafficMode2_M_5000_RA_Slot_0_2000.csv');
% ArrivalModel2_Simulation = csvread('SimArrivalMode2_M_RA_Slot_1_2000.csv');
% TrafficModel2_Simulation_CDF = csvread('Sim_CDF_TrafficMode2_M_5000_RA_Slot_0_2000.csv');
%%
% Ta_Analysis = csvread('Analysis_Ta_N_M_100_N_5_45.csv');
% Pc_Analysis = csvread('Analysis_Pc_N_M_100_N_5_45.csv');
% %%
% x = Ps_Simulation(:,1);
% y = Ps_Simulation(:,2);
% xd = Ta_Simulation(:,1);
% yd = Ta_Simulation(:,2);
% xc = Pc_Simulaiton(:,1);
% yc = Pc_Simulaiton(:,2);
% %%
% xa0 = TrafficModel1_Analysis(:,1);
% ya0 = TrafficModel1_Analysis(:,2);
% xa1 = TrafficModel1_Analysis_CDF(:,1);
% ya1 = TrafficModel1_Analysis_CDF(:,2);
% xs1 = TrafficModel1_Simulation_CDF(:,1);
% ys1 = TrafficModel1_Simulation_CDF(:,2);
% xaa0 = TrafficModel2_Analysis(:,1);
% yaa0 = TrafficModel2_Analysis(:,2);
% xaa1 = TrafficModel2_Analysis_CDF(:,1);
% yaa1 = TrafficModel2_Analysis_CDF(:,2);
% xss0 = TrafficModel2_Simulation(:,1);
% yss0 = TrafficModel2_Simulation(:,2);
% xss1 = TrafficModel2_Simulation_CDF(:,1);
% yss1 = TrafficModel2_Simulation_CDF(:,2);
%%
% xd1 = Ta_Analysis(:,1);
% yd1 = Ta_Analysis(:,2);
% xc1 = Pc_Analysis(:,1);
% yc1 = Pc_Analysis(:,2);
%============公式驗算===========%
%% Collision Probability Model 1 &2
figure(1);
Pc_Simulation = csvread('SimPcMode1_M_5000_RA_Slot_1_12000.csv');
xPcMode1 = Pc_Simulation(:,1);
yPcMode1 = Pc_Simulation(:,2);
semilogy(xPcMode1, yPcMode1,'go'); 
axis([5000, 30000, 0,1]);
set(gca,'FontSize',12)
% ax = gca;
% ax.XTick = [0 10^-4 10^-3 10^-2 10^-1 10^-0];
xlabel('M'),ylabel('Pc');
title('Collision Probability');
legend('Model 1 Simulation');
%% Access Success Probability Model 1 &2
figure(2);
Ps_Simulation = csvread('SimPsMode1_M_5000_RA_Slot_1_12000.csv');
xPsMode1 = Ps_Simulation(:,1);
yPsMode1 = Ps_Simulation(:,2);
plot(xPsMode1, yPsMode1,'go'); 
% set(gca,'xtick',[5000:5000:30000],'ytick',[0:0.1:1]);
axis([5000, 30000, 0,1]);
% ylim([0, 1]);%只?定y?的?制范?
set(gca,'FontSize',12);
xlabel('M'),ylabel('Ps');
title('Access success probability');
legend('Model 1 Simulation');
%% Average Access Delay Model 1 &2
figure(3);
Da_Simulation = csvread('SimDaMode1_M_5000_RA_Slot_1_12000.csv');
xDaMode1 = Da_Simulation(:,1);
yDaMode1 = Da_Simulation(:,2);
plot(xDaMode1, yDaMode1,'go'); 
% set(gca,'xtick',[5000:5000:30000],'ytick',[0:10:100]);
% ylim([0, 100]);%只?定y?的?制范?
axis([5000, 30000, 0,100]);
% set(gca,'YTick',[0:10:100]);
% set(gca,'ytick', [0:10:100]);
set(gca,'FontSize',12);
xlabel('M'),ylabel('Da');
title('Average access delay');
legend('Model 1 Simulation');
%% CDF nthPreamble Model 1
figure(4);
nthPreamble_Simulation = csvread('SimAccumNthPreambleMode1_M_5000_RA_Slot_1_12000.csv');
xNthRAOMode1 = nthPreamble_Simulation(1:10,1);
% yNthRAOMode1 = nthPreamble_Simulation(:,2);
y1NthRAOMode1 = nthPreamble_Simulation(1:10,2);
y2NthRAOMode1 = nthPreamble_Simulation(11:20,2);
y3NthRAOMode1 = nthPreamble_Simulation(21:30,2);
plot(xNthRAOMode1, y1NthRAOMode1,'bsquare', xNthRAOMode1, y2NthRAOMode1, 'rdiamond', xNthRAOMode1, y3NthRAOMode1, 'mo'); 
axis([1, 10, 0,1]);
set(gca,'FontSize',12);
xlabel('Number of preamble transmission'),ylabel('F(m)');
title('Model 1');
legend('M=5000 Simulation','M=10000 Simulation','M=30000 Simulation');
%% CDF Access Delay Model 1
figure(5);
accumAccessDelay_Simulation = csvread('SimAccumAccessDelayMode1_M_5000_RA_Slot_1_12000.csv');
xAAD1 = accumAccessDelay_Simulation(1:276,1);
y1AADMode1 = accumAccessDelay_Simulation(1:276,2);
y2AADMode1 = accumAccessDelay_Simulation(277:552,2);
y3AADMode1 = accumAccessDelay_Simulation(553:828,2);
% y3NthRAOMode1 = accumAccessDelay_Simulation(21:30,2);
plot(xAAD1, y1AADMode1,'bsquare', xAAD1, y2AADMode1, 'gdiamond', xAAD1, y3AADMode1, 'ro'); 
axis([0, 275, 0,1]);
set(gca,'FontSize',12);
xlabel('Access Delay(ms)'),ylabel('G(d)');
title('Model 1');
legend('M=5000 Simulation','M=10000 Simulation','M=30000 Simulation');
%% Check Arrival Model 1
% figure(6);
% xs0 = TrafficModel1_Simulation(1:12000,1);
% ys0 = TrafficModel1_Simulation(1:12000,2);
% ys1= TrafficModel1_Simulation(12001:24000,2);
% ys2= TrafficModel1_Simulation(24001:36000,2);
% % ha = plot(xs0, ys0,'bsquare',xs0, ys0, 'gdiamond', xs0, ys2, 'ro'); 
% ha = plot(xs0, ys0,'b-');
% %  ha=plot(xa0, ya0,'b-'); 
% set(ha, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[0:2000:18000],'ytick',[0:500:5000]);
% set(gca,'ytick',[0:1:10]);
% axis([0, 12000, 0,15]);
% xlabel('ith random-access slot'),ylabel('Average number of arrived MSs in ith random access slot');
% title('Arrival in Traffic Model 1');
% %legend('Nc/N Simulation','Nc/N Numerical','(Nc+Ns)/N Simulation','(Nc+Ns)/N Numerical');
% legend('Sim Mi[1], M=5000','Sim Mi[1], M=10000','Sim Mi[1], M=30000');
% % legend('Analysis');
% grid on
% %% Check Arrival Model 2
% figure(6);
% xs1 = ArrivalModel2_Simulation(1:2000,1);
% ys1 = ArrivalModel2_Simulation(1:2000,2);
% % ys1= ArrivalModel2_Simulation(12001:24000,2);
% % ys2= ArrivalModel2_Simulation(24001:36000,2);
% % ha = plot(xs0, ys0,'bsquare',xs0, ys0, 'gdiamond', xs0, ys2, 'ro'); 
% ha1 = plot(xs1, ys1,'b--');
% %  ha=plot(xa0, ya0,'b-'); 
% set(ha1, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[0:2000:18000],'ytick',[0:500:5000]);
% % set(gca,'ytick',[0:1:10]);
% axis([0,2000, 0,15]);
% xlabel('ith random-access slot'),ylabel('Average number of arrived MSs in ith random access slot');
% title('Simulation Arrival Model 2');
% % legend('Sim Mi[1], M=5000','Sim Mi[1], M=10000','Sim Mi[1], M=30000');
% legend('Sim Mi[1], M=5000');
% grid on
%% Arrival Mode
% figure(1);
% ha=plot(xa0, ya0,'b-',xs0, ys0, 'r--'); 
% %  ha=plot(xa0, ya0,'b-'); 
% set(ha, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[0:2000:18000],'ytick',[0:500:5000]);
% axis([0, 18000, 0,2.5]);
% xlabel('ith random-access slot'),ylabel('Average number of arrived MSs in ith random access slot');
% title('Arrival in Traffic Model 1, M=5000');
% %legend('Nc/N Simulation','Nc/N Numerical','(Nc+Ns)/N Simulation','(Nc+Ns)/N Numerical');
% legend('Analysis','Simulation');
% % legend('Analysis');
% grid on
%%
% figure(2);
% ha1=plot(xa1, ya1, 'b-', xs1, ys1, 'r--');
% % ha1=plot(xa1, ya1, 'b-'); 
% set(ha1, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[5:5:45],'ytick',[0:2:10]);
% axis([0,18000, 0, 5000]);
% xlabel('ith random-access slot'),ylabel('Accumulated MSs');
% title('CDF arrival in Traffic Model 1, M=5000');
% %legend('Nc/N Simulation','Nc/N Numerical','(Nc+Ns)/N Simulation','(Nc+Ns)/N Numerical');
% % legend('Analysis, Eq. (9)','Simulation Results');
% legend('Analysis','Simulation');
% grid on
%%
% figure(3);
% haa0=plot(xaa0, yaa0, 'b-', xss0, yss0, 'bo');
% % haa0=plot(xaa0, yaa0,'b-');
% set(haa0, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[0:50:300],'ytick',[0:5:50]);
% axis([0, 2200, 0, 8]);
% xlabel('ith random-access slot'),ylabel('Average number of arrived MSs in ith random access slot');
% title('Arrival in Traffic Model 2, M=5000');
% legend('Analysis','Simulation');
% % legend('Analysis');
% grid on
%hold on;
%%
% figure(4);
% haa1=plot(xaa1, yaa1, 'b-', xss1, yss1, 'r--');
% % haa1=plot(xaa1, yaa1,'b-');
% set(haa1, 'linewidth', 1.5);    % 曲線寬度改為1.5
% %hold on;
% % set(gca,'xtick',[0:50:300],'ytick',[0:5:50]);
% axis([0, 2200, 0, 5000]);
% xlabel('ith random-access slot'),ylabel('Average number of arrived MSs in ith random access slot');
% title('CDF arrival in Traffic Model 2, M=5000');
% legend('Analysis','Simulation');
% % legend('Analysis');
% grid on
%hold on;
%%
