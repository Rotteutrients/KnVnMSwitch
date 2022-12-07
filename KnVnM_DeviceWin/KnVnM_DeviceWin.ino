#include <Adafruit_NeoPixel.h>
#include <Adafruit_TinyUSB.h>

#define UART_BAUDRATE 57600

#define PRESSED_KEY_REORT_SIZE 9

#define LED_RED   17 //GPIO17
#define LED_GREEN 16 //GPIO16
#define LED_BLUE  25 //GPIO25

#define NUMPIXELS 1
#define NEO_PWR 11 //GPIO11
#define NEOPIX 12 //GPIO12

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIX, NEO_GRB + NEO_KHZ800);

uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD()
};

Adafruit_USBD_HID usb_hid;

volatile uint8_t kbd_leds_last = 0;
volatile uint8_t kbd_leds_send = 0;
int data_length = 0;

uint8_t report[PRESSED_KEY_REORT_SIZE] = {0x00};
volatile int report_point = 0;

void set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    if (report_id == 0)
    {
      if ( bufsize < 1 ) return;
      kbd_leds_last = buffer[0];
    }
  }
}

void setup()
{
  Serial.begin(UART_BAUDRATE);
  Serial1.begin(UART_BAUDRATE);
  
  usb_hid.setPollInterval(1);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setReportCallback(nullptr, set_report_cb);
  usb_hid.begin();
  
  while( !USBDevice.mounted() ) delay(1);

}

int data;
void loop()
{
  if(Serial1.available()) {
    data = Serial1.read();
    Serial.println(data);
    if(data == 0xFF) {
      report_point = 0;
    } else {
      report_point++;
    }
    if(report_point < PRESSED_KEY_REORT_SIZE) {
      report[report_point] = data;
    }

    if(report_point >= (PRESSED_KEY_REORT_SIZE - 1)) {
      report_point = 0;
      
      if (USBDevice.suspended()) {
        USBDevice.remoteWakeup();
      }

      if (usb_hid.ready()) {
        if(is_myreport(report)) {
          convert_modifykey(report);
          usb_hid.keyboardReport(0, report[1], &report[3]);
        }
      }
    }
  }
  if(kbd_leds_send != kbd_leds_last) {
    kbd_leds_send = kbd_leds_last;
    Serial1.write(kbd_leds_send);
  }
}

void setup1()
{
  pixels.begin();
  pinMode(NEO_PWR,OUTPUT);
  digitalWrite(NEO_PWR, HIGH);
  delay(20);
}

void loop1()
{
  int blue8 = 0;
  int green = 0;
  while(true){
    if(blue8 != report_point || green != kbd_leds_last) {
      blue8 = report_point;
      green = kbd_leds_last;
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, green, blue8 * 30));
      pixels.show();
    }
  }
}
