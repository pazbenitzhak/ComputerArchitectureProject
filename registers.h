#define registersSize 16


static float registers[registersSize];


void initRegisters();
float readRegister(int address);
float writeRegister(int address, float value);
void exitRegisters();