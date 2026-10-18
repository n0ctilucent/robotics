from gpiozero import Robot, Motor
import time

front_left__motor = Motor(forward=23, backward=24, enable=12)
front_right_motor = Motor(forward=9, backward=10, enable=13)
#back__left__motor = Motor(forward=23, backward=24, enable=12)
#back__right_motor = Motor(forward=9, backward=10, enable=13)


robot = Robot(left=front_left__motor, right=front_right_motor)

def test_sequence():
    print("Moving Forward...")
    robot.forward(speed=0.5)
    time.sleep(8)

    print("Moving Backward...")
    robot.backward(speed=0.5)
    time.sleep(8)

    print("Stopping.")
    robot.stop()

if __name__ == "__main__":
    try:
        test_sequence()
    except KeyboardInterrupt:
        robot.stop()
