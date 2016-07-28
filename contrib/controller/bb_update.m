% S
sensor_left  = 1.0/3.0 * (S(1) + S(2) + S(3));
sensor_right = 1.0/3.0 * (S(4) + S(5) + S(6));

% sl_history = [sl_history, sensor_left];
% sr_history = [sr_history, sensor_right];

motor_left  = tanh(-sensor_right);
motor_right = tanh(-sensor_left);

A = [motor_left, motor_right];

% al_history = [al_history, motor_left];
% ar_history = [ar_history, motor_left];

% clf;
% plot([1:length(sl_history)], sl_history);
% plot([1:length(sr_history)], sr_history);
% plot([1:length(al_history)], al_history);
% plot([1:length(ar_history)], ar_history);

