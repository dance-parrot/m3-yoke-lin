# m3-yoke-lin
LIN man-in-the-middle for using a Model 3 steering wheel with turn signal stalk on a Model S 2021+

Designed for Arduino Due with mikro Arduino Mega Click Shield, and 2 mikro LIN clicks.
LIN click on slot 1 is LIN slave and connected to SCCM of Model S 2021+
LIN click on slot 3 is LIN master and connected to Model 3 steering wheel

Scroll wheels from Model 3 steering wheel are mapped to the equivalent
functions from the Model S steering wheel, with the exception of the 
right scroll wheel left/right, which are mapped to right scroll wheel
up/down to more easily control the cruise-control speed.

Stalk pull flashes high beams. Stalk push simulates holding the high beams
touch button for 700ms to activate high beams. If pulling the stalk during this
time, the high beams activation is cancelled.

Pulling the stalk for 2s activates "hard-flash" mode. In this mode, pulling
the stalk quickly flashes the high beams, for use at track events where
slower cars might not see you approaching. This mode deactivates itself after 1h.

See TMC thread 
https://teslamotorsclub.com/tmc/threads/work-in-progress-stalks-on-2021-model-s-and-optional-round-y-wheel.242853
