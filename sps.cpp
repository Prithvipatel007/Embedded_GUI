// sps.cpp
// Lothar Berger - Jun 2021
//
// SPS Simulation

// #include <Arduino.h> // Added by me

#define SIMULATION
// #undef  SIMULATION

#define LED_BUSY  13

static unsigned short WAIT = 50;

static bool PENDING = false;

static void PLC2PC(unsigned char *outmsg, unsigned short len)
{
  unsigned short idx;
  
  // send output
  if (PENDING)
  {
    for (idx=0; idx<len; idx++)
    {
      Serial.write(outmsg[idx]);
    }
    Serial.flush();
    PENDING = false;
  }
}

static void PC2PLC(unsigned char *inmsg, unsigned short len)
{
  unsigned short idx;
  
  // receive input
  idx = 0;
  while (Serial.available() > 0)
  {
    inmsg[idx++] = Serial.read();

    if (idx >= len-1)
    {
      PENDING = true;
      break;
    }
  }
  
  // clear buffer
  while (Serial.available() > 0)
    (void) Serial.read();
}

static unsigned int TOGGLE = 1;

static void loop_toggle(void)
{
  if (TOGGLE)
    digitalWrite(LED_BUSY, HIGH);
  else
    digitalWrite(LED_BUSY, LOW);
  
  TOGGLE = 1 - TOGGLE;
}

/* SPS STATES */

#define STATE_IDLE    0
#define STATE_BUSY    1
static unsigned short STATE = STATE_IDLE;

#define INMSG_MAX_LEN   15
static unsigned char INMSG[]  = "(------,------)";
static unsigned char OUTMSG[] = "(------,------)";
#define OUTMSG_MAX_LEN  15

/* STATE = BUSY */

static unsigned short CYCLES_MAX = 0;
static unsigned short CYCLES = 0;

// PLC cycle 300 ms
static void loop_busy(void)
{
  // ? PLC cycles remaining
  if (CYCLES > 0)
  {
    CYCLES--;
  }
  else
  {
    // done
    STATE = STATE_IDLE;
  }
  
  // ! clear
  INMSG[ 1] = '-';
  INMSG[ 2] = '-';
  INMSG[ 3] = '-';
  INMSG[ 4] = '-';
  INMSG[ 5] = '-';
  INMSG[ 6] = '-';

  INMSG[ 8] = '-';
  INMSG[ 9] = '-';
  INMSG[10] = '-';
  INMSG[11] = '-';
  INMSG[12] = '-';
  INMSG[13] = '-';
  
  signed short plc_val_1st, plc_val_2nd;

  /*** PLC half cycle 150 ms ***/
  {
    // do PLC work for 150 ms
    delay(WAIT);
    delay(WAIT);
    delay(WAIT);
  }
  
  PC2PLC(INMSG, INMSG_MAX_LEN);

  /*** PLC half cycle 150 ms ***/
  {
    // do PLC work for 150 ms
    delay(WAIT);
    delay(WAIT);
    delay(WAIT);
  }

#ifdef SIMULATION
  plc_val_1st = (100 - rand() % 20) * sin((PI * CYCLES) / CYCLES_MAX);
  plc_val_2nd = ( 50 - rand() % 10) * sin((PI * CYCLES) / CYCLES_MAX);
#endif

  // ! default
  OUTMSG[ 1] = '-';
  OUTMSG[ 2] = '-';
  OUTMSG[ 3] = '-';
  OUTMSG[ 4] = '-';
  OUTMSG[ 5] = '-';
  OUTMSG[ 6] = '-';

  OUTMSG[ 8] = '-';
  OUTMSG[ 9] = '-';
  OUTMSG[10] = '-';
  OUTMSG[11] = '-';
  OUTMSG[12] = '-';
  OUTMSG[13] = '-';

  // ? change STATE to IDLE
  if (INMSG[1] == 'I' && INMSG[2] == 'D' && INMSG[3] == 'L' && INMSG[4] == 'E' && INMSG[5] == '-' && INMSG[6] == '-')
  {
    STATE = STATE_IDLE;
    
    OUTMSG[ 1] = 'I';
    OUTMSG[ 2] = 'D';
    OUTMSG[ 3] = 'L';
    OUTMSG[ 4] = 'E';
    OUTMSG[ 5] = '-';
    OUTMSG[ 6] = '-';

    OUTMSG[ 8] = 'D';
    OUTMSG[ 9] = 'O';
    OUTMSG[10] = 'N';
    OUTMSG[11] = 'E';
    OUTMSG[12] = '-';
    OUTMSG[13] = '-';
  }
  
  // ? READ values
  if (INMSG[1] == 'R' && INMSG[2] == 'E' && INMSG[3] == 'A' && INMSG[4] == 'D' && INMSG[5] == '-' && INMSG[6] == '-')
  {
    unsigned short val = 0;
    
    if (plc_val_1st < 0)
    {
      val = abs(plc_val_1st);

      OUTMSG[ 1] = '-';
      OUTMSG[ 2] = 0x30 + val/10000;
      OUTMSG[ 3] = 0x30 + (val%10000)/1000;
      OUTMSG[ 4] = 0x30 + (val%1000)/100;
      OUTMSG[ 5] = 0x30 + (val%100)/10;
      OUTMSG[ 6] = 0x30 + val%10;
    }
    else
    {
      val = plc_val_1st;

      OUTMSG[ 1] = '+';
      OUTMSG[ 2] = 0x30 + val/10000;
      OUTMSG[ 3] = 0x30 + (val%10000)/1000;
      OUTMSG[ 4] = 0x30 + (val%1000)/100;
      OUTMSG[ 5] = 0x30 + (val%100)/10;
      OUTMSG[ 6] = 0x30 + val%10;
    }
    
    if (plc_val_2nd < 0)
    {
      val = abs(plc_val_2nd);

      OUTMSG[ 8] = '-';
      OUTMSG[ 9] = 0x30 + val/10000;
      OUTMSG[10] = 0x30 + (val%10000)/1000;
      OUTMSG[11] = 0x30 + (val%1000)/100;
      OUTMSG[12] = 0x30 + (val%100)/10;
      OUTMSG[13] = 0x30 + val%10;
    }
    else
    {
      val = plc_val_2nd;

      OUTMSG[ 8] = '+';
      OUTMSG[ 9] = 0x30 + val/10000;
      OUTMSG[10] = 0x30 + (val%10000)/1000;
      OUTMSG[11] = 0x30 + (val%1000)/100;
      OUTMSG[12] = 0x30 + (val%100)/10;
      OUTMSG[13] = 0x30 + val%10;
    }
  }

  PLC2PC(OUTMSG, OUTMSG_MAX_LEN);

  loop_toggle();
}

/* STATE = IDLE */

// PLC cycle 300 ms
static void loop_idle(void)
{
  // ! clear
  INMSG[ 1] = '-';
  INMSG[ 2] = '-';
  INMSG[ 3] = '-';
  INMSG[ 4] = '-';
  INMSG[ 5] = '-';
  INMSG[ 6] = '-';

  INMSG[ 8] = '-';
  INMSG[ 9] = '-';
  INMSG[10] = '-';
  INMSG[11] = '-';
  INMSG[12] = '-';
  INMSG[13] = '-';
  
  /*** PLC half cycle 150 ms ***/
  {
    // do PLC work for 150 ms
    delay(WAIT);
    delay(WAIT);
    delay(WAIT);
  }
  
  PC2PLC(INMSG, INMSG_MAX_LEN);

  /*** PLC half cycle 150 ms ***/
  {
    // do PLC work for 150 ms
    delay(WAIT);
    delay(WAIT);
    delay(WAIT);
  }
  
  // ! default
  OUTMSG[ 1] = '-';
  OUTMSG[ 2] = '-';
  OUTMSG[ 3] = '-';
  OUTMSG[ 4] = '-';
  OUTMSG[ 5] = '-';
  OUTMSG[ 6] = '-';

  OUTMSG[ 8] = '-';
  OUTMSG[ 9] = '-';
  OUTMSG[10] = '-';
  OUTMSG[11] = '-';
  OUTMSG[12] = '-';
  OUTMSG[13] = '-';

  // ? change STATE to BUSY
  if (INMSG[1] == 'B' && INMSG[2] == 'U' && INMSG[3] == 'S' && INMSG[4] == 'Y' && INMSG[5] == '-' && INMSG[6] == '-')
  {
    STATE = STATE_BUSY;
    
    OUTMSG[ 1] = 'B';
    OUTMSG[ 2] = 'U';
    OUTMSG[ 3] = 'S';
    OUTMSG[ 4] = 'Y';
    OUTMSG[ 5] = '-';
    OUTMSG[ 6] = '-';

    OUTMSG[ 8] = 'D';
    OUTMSG[ 9] = 'O';
    OUTMSG[10] = 'N';
    OUTMSG[11] = 'E';
    OUTMSG[12] = '-';
    OUTMSG[13] = '-';
  }
  
  // ? SET parameter: PLC cycles
  if (INMSG[1] == 'S' && INMSG[2] == 'E' && INMSG[3] == 'T' && INMSG[4] == '-' && INMSG[5] == '-' && INMSG[6] == '-')
  { 
    unsigned short val = 0;

    // example: 5000
    // INMSG[ 8] = '+';
    // INMSG[ 9] = '0';
    // INMSG[10] = '5';
    // INMSG[11] = '0';
    // INMSG[12] = '0';
    // INMSG[13] = '0';

    val  = (INMSG[ 9]-0x30)*10000;
    val += (INMSG[10]-0x30)*1000;
    val += (INMSG[11]-0x30)*100;
    val += (INMSG[12]-0x30)*10;
    val += (INMSG[13]-0x30);

    // limit 9999
    if (val > 9999)
    {
      val = 9999;
    }
    
    CYCLES_MAX = val;
    CYCLES = CYCLES_MAX;
    
    OUTMSG[ 1] = 'S';
    OUTMSG[ 2] = 'E';
    OUTMSG[ 3] = 'T';
    OUTMSG[ 4] = '-';
    OUTMSG[ 5] = '-';
    OUTMSG[ 6] = '-';

    OUTMSG[ 8] = '+';
    OUTMSG[ 9] = 0x30 + val/10000;
    OUTMSG[10] = 0x30 + (val%10000)/1000;
    OUTMSG[11] = 0x30 + (val%1000)/100;
    OUTMSG[12] = 0x30 + (val%100)/10;
    OUTMSG[13] = 0x30 + val%10;
  }

  PLC2PC(OUTMSG, OUTMSG_MAX_LEN);

  loop_toggle();
}

void loop(void)
{
  if (STATE == STATE_IDLE)
    loop_idle();

  if (STATE == STATE_BUSY)
    loop_busy();
}

#define SER_BAUD      9600

void setup(void)
{
  Serial.begin(SER_BAUD);

  pinMode(LED_BUSY, OUTPUT);
}
