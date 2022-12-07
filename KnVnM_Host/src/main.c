
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "KnVnM.h"

#include "bsp/board.h"
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"

void main1(void);
void init_uarts(void);
void hid_app_task(void);
bool key_report_pop_code(volatile hid_keyboard_report_t *report, uint8_t keycode);
static void key_report_send(uart_inst_t *uart_id, volatile hid_keyboard_report_t* report);
static void key_report_reset_send(uart_inst_t *uart_id);

extern volatile bool enable_main;

extern volatile int report_buffer_last_position;
extern volatile int report_buffer_send_position;
extern volatile hid_keyboard_report_t report_buffer[HID_KEYBOARD_REPORT_BUF_SIZE];

extern volatile uart_inst_t *uart_send_id;

extern volatile uint8_t hid_device_address;
extern volatile uint8_t hid_device_instance;

volatile uint8_t hid_led_last_report = 0;
volatile uint8_t hid_led_send_report = 0;

/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  multicore_launch_core1(main1);

  tusb_init();

  while(true)
  {
    //tiny_usb host task
    tuh_task();
    //tiny_usb send data
    hid_app_task();
  }

  return 0;
}

void hid_app_task(void)
{
  if(hid_device_address > 0) {
    if(hid_led_send_report != hid_led_last_report) {
      hid_led_send_report = hid_led_last_report;
      board_led_write(
        tuh_hid_set_report(hid_device_address, hid_device_instance, 0x06, HID_REPORT_TYPE_OUTPUT, (void*) &hid_led_send_report, 1)
      );
    }
  }
}

void main1(void)
{
  uint8_t uart_led = 0;
  uint8_t uart_code = 1;

  init_uarts();

  while (true)
  {
    while(uart_is_readable(UART_ID))
    {
      uart_read_blocking(UART_ID, &uart_led, 1);
      uart_led = uart_led & 0b00011111;
    }

    while(report_buffer_last_position != report_buffer_send_position)
    {
      report_buffer_send_position = next_buffer_position(report_buffer_send_position, HID_KEYBOARD_REPORT_BUF_SIZE);

      if(key_report_pop_code(&report_buffer[report_buffer_send_position], KEY_CODE_APPLICATION)) {
        if(key_report_pop_code(&report_buffer[report_buffer_send_position], KEY_CODE_ARROW_LEFT)
          || key_report_pop_code(&report_buffer[report_buffer_send_position], KEY_CODE_ARROW_UP)) {
          uart_code = 1;
        } else if(key_report_pop_code(&report_buffer[report_buffer_send_position], KEY_CODE_ARROW_RIGHT)
          || key_report_pop_code(&report_buffer[report_buffer_send_position], KEY_CODE_ARROW_DOWN)) {
          uart_code = 2;
        }
      }

        hid_led_last_report = uart_led;
        report_buffer[report_buffer_send_position].reserved = hid_led_last_report | ((uart_code << 5) & 0b01100000);
        key_report_send(UART_ID, &report_buffer[report_buffer_send_position]);
    }
  }
}

static void key_report_send(uart_inst_t *uart_id, volatile hid_keyboard_report_t *report)
{
  static uint8_t ex_report[9] = { 0xFF };
  ex_report[1] = report->modifier;
  ex_report[2] = report->reserved;
  for(uint8_t i=0; i<6; i++)
  {
      ex_report[i + 3] = report->keycode[i];
  }
  
  uart_write_blocking(uart_id, (const uint8_t *) &ex_report, 9);
}

static void key_report_reset_send(uart_inst_t *uart_id)
{
  static uint8_t ex_report[9] = { 0x00 };
  ex_report[0] = 0xFF;
  
  uart_write_blocking(uart_id, (const uint8_t *) &ex_report, 9);
}

bool key_report_pop_code(volatile hid_keyboard_report_t *report, uint8_t keycode)
{
  bool contain = false;
  for(int p=0; p<6; p++)
  {
    if(report->keycode[p] == keycode) {
      contain = true;
    }
    if(contain) {
      if(p < 5) {
        report->keycode[p] = report->keycode[p + 1];
      } else {
        report->keycode[p] = 0;
      }
    }
  }
  return contain;
}

int next_buffer_position(int position, int buf_size)
{
  position++;
  if(position >= buf_size) {
    position = 0;
  }
  return position;
}

void init_uarts(void)
{
  uart_init(UART_ID, UART_BAUD_RATE);
  gpio_set_function(UART_TX0_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX0_PIN, GPIO_FUNC_UART);
}
