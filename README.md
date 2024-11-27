# NodeMCU to L293N Wiring for Car Control

This documentation shows the pin mapping between **NodeMCU** and **L293N Motor Driver** for steering and driving motors of your car.

## Pin Mapping

| **NodeMCU Pin**          | **L293N Pin**     | **Function**                    |
| ------------------------ | ----------------- | ------------------------------- |
| D4 (PIN_STEERING_RIGHT)  | IN1               | Controls steering motor right   |
| D2 (PIN_STEERING_LEFT)   | IN2               | Controls steering motor left    |
| D6 (PIN_STEERING_ENABLE) | EN1               | Enables/disables steering motor |
| D8 (PIN_DRIVE_BWD)       | IN3               | Controls driving motor backward |
| D7 (PIN_DRIVE_FWD)       | IN4               | Controls driving motor forward  |
| D5 (PIN_DRIVE_ENABLE)    | EN2               | Enables/disables driving motor  |
| 3.3V or 5V (NodeMCU)     | VCC (L293N logic) | Power for L293N logic           |
| GND                      | GND               | Common ground                   |

## Power Connections

- **VCC (12V external)** → External battery (for motors power)
- **GND** → Ground (common ground with NodeMCU and L293N)

## Diagram

Here's a visual representation of how to wire the NodeMCU to the L293N motor driver:

                         +----------------------+
                         |     NodeMCU          |
                         |                      |
            D4  -------->| IN1 (Steering Right) |
            D2  -------->| IN2 (Steering Left)  |
            D6  -------->| EN1 (Steering Enable)|
            D8  -------->| IN3 (Drive Backward) |
            D7  -------->| IN4 (Drive Forward)  |
            D5  -------->| EN2 (Drive Enable)   |
                         |                      |
          3.3V/5V  ----->| VCC (Logic Power)    |
           GND   --------| GND                  |
                         +----------------------+

Make sure all grounds are connected to avoid potential issues with motor control.

### Notes

- **IN1-IN4** on the L293N control the direction of the motors.
- **EN1 and EN2** control the enabling/disabling of motors.
- Power the motors through the external power connected to the **VCC** pin of the L293N.
