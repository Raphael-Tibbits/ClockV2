Thermal_mass; % m*c
ambient_temp = 25; 
resistance_ambient = 33.8;


function y = f(u)
    % Constants
    a = 4.338600283818352;
    b = 1.773775267395158;
    c = 4.1875407235123205;

    % Compute function
    y = (a / c) * log(1 + exp(c * (u - b)));
end 