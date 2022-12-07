#define MY_REPORT_BIT 0b00100000

#define REPORT_BIT_WIN 0b00100000
#define REPORT_BIT_MAC 0b01000000


bool is_myreport(uint8_t key_report[9])
{
  return (key_report[2] & MY_REPORT_BIT) > 0;
}

void convert_modifykey(uint8_t key_report[9]){
  if(MY_REPORT_BIT == REPORT_BIT_MAC) {
    _convert_modifykey(key_report);
  }
}

void _convert_modifykey(uint8_t key_report[9]){
  uint8_t modify_key = key_report[1];
  key_report[1] = 0b00000000;
  key_report[1] |= (modify_key & 0b00010001) << 3; //Ctrl    => Command
  key_report[1] |= (modify_key & 0b00100010) << 0; //Shift   => Shift
  key_report[1] |= (modify_key & 0b01000100) >> 0; //Alt     => Option
  key_report[1] |= (modify_key & 0b10001000) >> 1; //Windows => Option

  for(int i=3; i<9; i++) {
    switch(key_report[i]) {
      case 57: // CapsLock => Ctrl
        key_report[1] |= 0b00000001;
        // don't break;
      case 101: // Menu disable
      case 136: // Kana disable
        for(int j=i; j<9; j++){
          key_report[j] = (j<8)? key_report[j+1]: 0x00;
        }
        break;
      case 138: // Henkan => Kana
      case 139: // Muhenkan => Eisu
        key_report[i] = key_report[i] + 6;
        break;
      default:
        // No change
        break;
    }
  }
}
