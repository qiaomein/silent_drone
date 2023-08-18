%% prepare workspace

close all; clear; clc;

%% 
sr = 500;
a = .4;
T = 1/sr;
h = tf(1,[1, -a],T);
bode(h)

%%
s = tf([1 0],[1],T);

Td = .02;
tau = .123;
[n,d] = pade(Td,1);
td = tf(n,d);
thetaf = tf(2081.405,[tau-Td 1 0])*td;

kp = 1;
ki = .00;
kd = .5;

K = .0004;


gc = K*(pid(kp,ki,kd)); % pd controller

oltf = thetaf*gc

cltf = c2d(oltf/(1 + oltf),T,'zoh');


figure;
bode(oltf);
figure;
rlocus(oltf/(1 + oltf));
figure;
rlocus(cltf)

%%
figure;
step(cltf)



t = 0:T:100;
u = awgn(t*90,5);
figure;
y = lsim(cltf,u,t).';
lsimplot(cltf,u,t)
title("Ramp Response")

figure;
plot(t,y-u);


