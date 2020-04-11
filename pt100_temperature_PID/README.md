This project uses a pt100 sensor for sensing the temperature and then uses a very simple PID control library to adjust the PWM output of arduino between 0-255 to controller the heating and cooling devices connected to it.

It also has a tolerancce function which lets the PID work better for resistive type heater because resistive type heaters take some time to heat up and cool which changes temperature.

