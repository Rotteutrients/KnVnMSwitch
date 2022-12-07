#ifndef _KnVnM_H_

  #define _KnVnM_H_

  #define UART_ID uart0
  #define UART_BAUD_RATE 57600
  #define UART_TX0_PIN 0
  #define UART_RX0_PIN 1

  #define HID_KEYBOARD_REPORT_BUF_SIZE 5

  #define UART_DATA_DELIMITER 0xFF

  #define KEY_CODE_APPLICATION 0x65
  #define KEY_CODE_ARROW_RIGHT 0x4F
  #define KEY_CODE_ARROW_LEFT  0x50
  #define KEY_CODE_ARROW_DOWN  0x51
  #define KEY_CODE_ARROW_UP    0x52

  int next_buffer_position(int position, int buf_size);
#endif