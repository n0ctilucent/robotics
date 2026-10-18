#include <gpiod.h>
#include <iostream>
#include <unistd.h>

int main() {
    const char* chipname = "gpiochip0"; // Use 'gpiodetect' to confirm chip
    struct gpiod_chip *chip;
    struct gpiod_line *in1, *in2;

    chip = gpiod_chip_open_by_name(chipname);
    if (!chip) {
        perror("Open chip failed");
        return 1;
    }

    // Get lines (replace with your actual BCM pin numbers)
    in1 = gpiod_chip_get_line(chip, 16); 
    in2 = gpiod_chip_get_line(chip, 18);

    // Request lines as output
    gpiod_line_request_output(in1, "motor_control", 0);
    gpiod_line_request_output(in2, "motor_control", 0);

    // Spin Forward
    std::cout << "Moving Forward..." << std::endl;
    gpiod_line_set_value(in1, 1);
    gpiod_line_set_value(in2, 0);
    sleep(2);

    // Stop
    gpiod_line_set_value(in1, 0);
    gpiod_line_set_value(in2, 0);

    gpiod_line_release(in1);
    gpiod_line_release(in2);
    gpiod_chip_close(chip);
    return 0;
}
