
#define CMD_PLAY    'P' // P\x00[file]\x00
#define CMD_COPY    'C' // C\x00[file]\x00[file]\x00
#define CMD_DELETE  'D' // D\x00[file]\x00
#define CMD_VOLUME  'V' // V\x00%3d\x00
#define CMD_REVERSE 'R' // R\x00[file]\x00
#define CMD_LSTFILS 'F' // F\x00
#define CMD_NONE  0x00  // Lack of a command detected

// We delimit with null bytes to allow use of strlen, 
// and to avoid having to escape filenames with spaces

typedef struct {
	char cmd;
	char infile[17];
	char outfile[17];
} COMMAND;


void readCmdBuff(COMMAND* cmd);
void parseCommand(char* str, COMMAND* cmd);
