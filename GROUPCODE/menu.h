void move_up();
void move_down();
void setup_menu();
void filesToMenu();
void fileSelect(char file[17]);
void viewInfo(char* fname);
int keypad();
void sniff();
void keypadwrite(uint8_t* input);
void resetInterrupt();
// Status sendBytes (char address, uint8_t * data, uint8_t data_len);
void record_light(int value);
void playback_light(int value);
void viewInfo();
int fileSelected;
char file[17];
char viewFile[17];
char words[17];
int viewingInfo;
int screen;

char list[100][17];
uint8_t listLen;
char menuMode;

#define MENU_MAIN  'M'
#define MENU_FILES 'F'
#define MENU_INFO  'I'
