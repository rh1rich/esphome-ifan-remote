# esphome-ifan-remote

ESPHome component for the IFAN04/IFAN03 remote control receiver

## Implementation details

The RF remote control receiver sends the remote control commands via the serial interface UART0 to the ESP8266.

Each command consists of 8 bytes.
The first two bytes are always `0xAA55` and mark the start of the command.
The next 5 bytes represent the received command and the last byte is a checksum over these 5 bytes.

If a command is received and the checksum is ok, the `on_command` action is triggered and the command is passed via the following 2 variables:
- `command.high` ... bytes 3 and 4
- `command.low` ... bytes 5, 6 and 7

## Commands of the IFAN04/IFAN03 remote control

| RC button | command.high | command.low | label |
| --------- | ------------ | ----------- | ----- |
| 1 (left-top) | 0104 | 000104 | Light on/off |
| 2 | 0106 | 000101 | Mute |
| 3 | 0104 | 000103 | High speed |
| 4 | 0104 | 000102 | Medium speed |
| 5 | 0104 | 000100 | Fan off |
| 6 | 0104 | 000101 | Low speed |
| 7 | 0101 | 000102 | RF clearing |
| 7 | 0107 | 000101 | RF clearing (long press of 5s) |
| 8 (right-bottom) | 0101 | 000102 | Wi-Fi pairing |
| 8 (right-bottom) | 0101 | 000101 | Wi-Fi pairing (long press of 5s) |

## Configuration example

The external component is implemented in the ESPHome configuration file as follows:

    external_components:
      - source:
          type: git
          url: https://github.com/rh1rich/esphome-ifan-remote

    logger:
      baud_rate: 0

    uart:
      rx_pin: GPIO03
      baud_rate: 9600
      id: uart_bus

    ifan_remote:
      on_command:
        then:
          - logger.log:
              format: "RC command '%04X:%06X' received."
              args: [ 'command.high','command.low' ]
          - if:
              condition:
                lambda: return (command.high == 0x0104 && command.low == 0x000100);
              then:
                - fan.turn_off: fan_comp
          - if:
              condition:
                lambda: return (command.high == 0x0104 && command.low == 0x000101);
              then:
                - fan.turn_on:
                    id: fan_comp
                    speed: 1
                    ...

To use the RF remote control the uart debugging MUST be disabled by setting `baud_rate: 0` in the logger-configuration!
