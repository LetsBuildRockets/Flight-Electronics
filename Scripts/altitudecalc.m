clear;

h= (0:100:20000);
newP = (calc(20000):1000:101325);


for ix = 1 : length(h)
    p(ix) = calc(h(ix));
end


for ix = 1 : length(newP)
    newH(ix) = reversecalc(newP(ix));
end

%f = fit(p',h','exp2')
%pNEW = (0:100:200000);
%CC = coeffvalues(f);
%hNEW = CC(1)*exp(CC(2)*pNEW) + CC(3)*exp(CC(4)*pNEW);
plot(p,h,newP,newH);
axis([calc(20000),101325,0,20000]);
calc(11000)
calc(20000)
function P = calc(h)
    M = 0.0289644; % kg/mol
    g_ = 9.80665; % m/s2
    R = 8.3144598; % J/mol/K
    if h<11000
        H=0;
        L=-0.0065;
        Tb=288.15;
        Ps=101325.00;
        P=Ps*(Tb/(Tb+L*(h-H)))^((g_*M)/(R*L));
    elseif h<20000
        H=11000;
        L=0.0;
        Tb=216.65;
        Ps=22632.10;
        P=Ps*exp((-g_*M*(h-H))/(R*Tb));
    else
        H=20000;
        L=0.001;
        Tb=216.65;
        Ps=5474.89;
        P=Ps*(Tb/(Tb+L*(h-H)))^((g_*M)/(R*L));
    end
end

function h = reversecalc(p)
    M = 0.0289644; % kg/mol
    g_ = 9.80665; % m/s2
    R = 8.3144598; % J/mol/K
    if p>2.2632e+04
        H=0;
        L=-0.0065;
        Tb=288.15;
        Ps=101325.00;
        h=Tb/(L*exp( log(p/Ps) / ((g_*M)/(R*L))))-Tb/L+H;
    elseif p>5.4749e+03
        H=11000;
        L=0.0;
        Tb=216.65;
        Ps=22632.10;
        h=-Tb*R*log(p/Ps)/(g_*M)+H;
    else
        H=20000;
        L=0.001;
        Tb=216.65;
        Ps=5474.89;
        h=Tb/(L*exp( log(p/Ps) / ((g_*M)/(R*L))))-Tb/L+H;
    end
end

function H = baseHeight(h)
    if h<=11000
        H=0;
    elseif h<=20000
        H=11000;
    elseif h<=32000
        H=20000;
    elseif h<=47000
        H=32000;
    elseif h<=51000
        H=47000;
    elseif h<=71000
        H=51000;
    else
        H=71000;
    end
end

function L = lapseRate(h)
    if h<=11000
        L=-0.0065;
    elseif h<=20000
        L=0.0;
    elseif h<=32000
        L=0.001;
    elseif h<=47000
        L=0.0028;
    elseif h<=51000
        L=0.0;
    elseif h<=71000
        L=-0.0028;
    else
        L=-0.002;
    end
end

function T = staticTemp(h)
    if h<=11000
        T=288.15;
    elseif h<=20000
        T=216.65;
    elseif h<=32000
        T=216.65;
    elseif h<=47000
        T=228.65;
    elseif h<=51000
        T=270.65;
    elseif h<=71000
        T=270.65;
    else
        T=214.65;
    end
end

function P = staticPressure(h)
    if h<=11000
        P=101325.00;
    elseif h<=20000
        P=22632.10;
    elseif h<=32000
        P=5474.89;
    elseif h<=47000
        P=868.02;
    elseif h<=51000
        P=110.91;
    elseif h<=71000
        P=66.94;
    else
        P=3.96;
    end
end
