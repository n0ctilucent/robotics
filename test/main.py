from gpiozero import Motor
from time import sleep

# Define motor pins (forward, backward)
# If using a speed pin (ENA), use Motor(17, 27, enable=22)
motor = Motor(forward=23, backward=24)

print("Moving forward")
motor.forward(speed=0.5) # Speed 0.0 to 1.0
sleep(5)

print("Moving backward")
motor.backward(speed=0.5)
sleep(5)

print("Stopping")
motor.stop()
