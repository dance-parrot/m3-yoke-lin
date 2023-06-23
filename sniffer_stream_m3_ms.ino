// Designed for Arduino Due with mikro Arduino Mega Click Shield, and 2 mikro LIN clicks.
// LIN click on slot 1 is LIN slave and connected to SCCM of Model S 2021+
// LIN click on slot 3 is LIN master and connected to Model 3 steering wheel
//
// Scroll wheels from Model 3 steering wheel are mapped to the equivalent
// functions from the Model S steering wheel, with the exception of the 
// right scroll wheel left/right, which are mapped to right scroll wheel
// up/down to more easily control the cruise-control speed.
//
// Stalk pull flashes high beams. Stalk push simulates holding the high beams
// touch button for 700ms to activate high beams. If pulling the stalk during this
// time, the high beams activation is cancelled.
//
// Pulling the stalk for 2s activates "hard-flash" mode. In this mode, pulling
// the stalk quickly flashes the high beams, for use at track events where
// slower cars might not see you approaching. This mode deactivates itself after 1h.


#define EN1 53 //PB0
#define EN2 46 //PL3
#define EN3 45 //PL4


// Model 3 states
#define BUTTON_NONE     0
#define LW_UP           1
#define LW_UP_MULTI1    2
#define LW_UP_MULTI2    3
#define LW_UP_MULTI3    4
#define LW_DOWN         5
#define LW_DOWN_MULTI1 6
#define LW_DOWN_MULTI2 7
#define LW_DOWN_MULTI3 8
#define LW_LEFT 9
#define LW_RIGHT 10
#define LW_PRESS 11
#define RW_UP 12
#define RW_UP_MULTI1 13
#define RW_UP_MULTI2 14
#define RW_UP_MULTI3 15
#define RW_DOWN 16
#define RW_DOWN_MULTI1 17
#define RW_DOWN_MULTI2 18
#define RW_DOWN_MULTI3 19
#define RW_LEFT 20
#define RW_RIGHT 21
#define RW_PRESS 22
#define T_HORN 23
#define LW_RW_PRESS 24


#define ST_BLINK_LEFT1 25
#define ST_BLINK_LEFT2 26
#define ST_BLINK_RIGHT1 27
#define ST_BLINK_RIGHT2 28
#define ST_WASH1 29
#define ST_WASH2 30
#define ST_LIGHT1 31
#define ST_LIGHT2 32


// Model 3 steering wheel relevant LIN packet prefix
int model3DefSize = 25;
byte model3Def[][4] = { 
  {0x00,0x00,0x10,0x08},  //BUTTON_NONE
  
  {0x00,0x40,0x10,0x08},  //LW_UP  
  {0x00,0x80,0x10,0x08},  //LW_UP_MULTI
  {0x00,0xC0,0x10,0x08},  //LW_UP_MULTI
  {0x00,0x00,0x11,0x08},  //LW_UP_MULTI

  {0x00,0xC0,0x0F,0x08},  //LW_DOWN
  {0x00,0x40,0x0D,0x08},  //LW_DOWN_MULTI  
  {0x00,0x40,0x0E,0x08},  //LW_DOWN_MULTI  
  {0x00,0x40,0x0F,0x08},  //LW_DOWN_MULTI  
    
  {0x00,0x01,0x10,0x08},  //LW_LEFT
  {0x00,0x02,0x10,0x08},  //LW_RIGHT
  {0x00,0x03,0x10,0x08},  //LW_PRESS

  {0x00,0x00,0x30,0x08},  //RW_UP
  {0x00,0x00,0x30,0x08},  //RW_UP_MULTI  
  {0x00,0x00,0xB0,0x08},  //RW_UP_MULTI  
  {0x00,0x00,0x50,0x08},  //RW_UP_MULTI  
  
  {0x00,0x00,0xF0,0x07},  //RW_DOWN
  {0x00,0x00,0x70,0x07},  //RW_DOWN_MULTI  
  {0x00,0x00,0xB0,0x07},  //RW_DOWN_MULTI  
  {0x00,0x00,0xD0,0x07},  //RW_DOWN_MULTI  
   
  {0x00,0x08,0x10,0x08},  //RW_LEFT
  {0x00,0x10,0x10,0x08},  //RW_RIGHT
  {0x00,0x18,0x10,0x08},  //RW_PRESS
  
  {0x00,0x00,0x10,0x18},  //T_HORN
  {0x00,0x1B,0x10,0x08}   //LW_RW_PRESS
};

// Model S yoke left side packets (followed by counter)
byte modelSLeftIdent=0x4C;
byte modelSLeftDef[][7]=
{
  {0x10,0x00,0x00,0x00,0x00,0x00,0xC0}, //BUTTON_NONE
  {0x11,0x00,0x00,0x10,0x00,0x00,0xC0}, //LW_UP  
  {0x0F,0x00,0x00,0x10,0x00,0x00,0xC0}, //LW_DOWN
  {0x90,0x00,0x00,0x00,0x00,0x00,0xC0}, //LW_LEFT
  {0x10,0x09,0x00,0x00,0x00,0x00,0xC0}, //LW_RIGHT
  {0x50,0x00,0x00,0x00,0x00,0x00,0xC0}, //LW_PRESS
  {0x10,0xFC,0xFB,0x73,0x07,0x00,0xC0}, //LW_BLINK_LEFT
  {0x10,0xFC,0xF9,0xF7,0x07,0x00,0xC0}, //LW_BLINK_RIGHT
  {0x10,0x34,0xF0,0xF3,0x0F,0x00,0xC0}  //LW_HEADLIGHTS
};

// Model S yoke right side packets (followed by counter)
byte modelSRightIdent=0x0D;
byte modelSRightDef[][7]=
{
  {0x10,0x00,0x00,0x00,0x00,0x00,0xC0}, //BUTTON_NONE
  {0x11,0x00,0x00,0x00,0x00,0x00,0xC0}, //RW_UP
  {0x0F,0x00,0x00,0x00,0x00,0x00,0xC0}, //RW_DOWN
  {0x90,0x2C,0x00,0x00,0x00,0x00,0xC0}, //RW_LEFT
  {0x10,0x01,0x00,0x00,0x00,0x00,0xC0}, //RW_RIGHT
  {0x50,0x00,0x00,0x00,0x00,0x00,0xC0}, //RW_PRESS
  {0x10,0x00,0xF8,0xF3,0x0F,0x00,0xC0}, //RW_HORN
  {0x10,0x60,0xF8,0xF3,0x07,0x00,0xC0}, //RW_CRUISE
  {0x10,0xFC,0xF9,0xF7,0x07,0x00,0xC0}, //RW_WIPER
  {0x10,0xFC,0xFB,0xF3,0x07,0x00,0xC0}  //RW_MIC
};

// Mapping of Model 3 to Model S functions
int model3ToModelS[][2] = { 
  {0,0},  //BUTTON_NONE
  
  {1,0},  //LW_UP  
  {1,0},  //LW_UP_MULTI
  {1,0},  //LW_UP_MULTI
  {1,0},  //LW_UP_MULTI

  {2,0},  //LW_DOWN
  {2,0},  //LW_DOWN_MULTI  
  {2,0},  //LW_DOWN_MULTI  
  {2,0},  //LW_DOWN_MULTI  
    
  {3,0},  //LW_LEFT
  {4,0},  //LW_RIGHT
  {5,0},  //LW_PRESS

  {0,1},  //RW_UP
  {0,1},  //RW_UP_MULTI  
  {0,1},  //RW_UP_MULTI  
  {0,1},  //RW_UP_MULTI  
  
  {0,2},  //RW_DOWN
  {0,2},  //RW_DOWN_MULTI  
  {0,2},  //RW_DOWN_MULTI  
  {0,2},  //RW_DOWN_MULTI  
   
  {0,2},  //RW_LEFT -> RW_DOWN
  {0,1},  //RW_RIGHT -> RW_UP
  {0,5},  //RW_PRESS
  
  {0,6},  //T_HORN
  {5,5},  //LW_RW_PRESS

  {6,0},  //ST_BLINK_LEFT1
  {6,0},  //ST_BLINK_LEFT2
  
  {7,0},  //ST_BLINK_RIGHT1
  {7,0},  //ST_BLINK_RIGHT2
  
  {0,8},  //ST_WASH1
  {0,8},  //ST_WASH2
  
  {8,0},  //ST_LIGHT1
  {8,0},  //ST_LIGHT2
};

int linBaud = 19200;

int model3BufferPosition=-1;
byte model3Buffer[11];

int modelSBufferPosition=-1;
byte modelSBuffer[11];
int modelSResponseCounter=0;

int model3State=0;
int model3StateL=0;
int model3StateR=0;
int model3QueryIntervalMs=10;
long int model3LastQueryMs;

long int highBeamsMs=0;
int highBeamsDurationMs=700;

long int wiperMs=0;
int wiperDurationMs=2500;

bool hardSignalEnabled=false;

long int hardSignalEnablingMs=0;
bool hardSignalEnablingInProgress=false;
long int hardSignalEnablingDurationMs=2000;

long int hardSignalEnabledMs=0;
long int hardSignalAutoDisableIntervalMs=3600000; //1h

long int hardSignalActivatedMs=0;
int hardSignalDurationMs=1500;
int hardSignalFlashIntervalMs=75;

long int disableStalksMs=0;
int disableStalksDurationMs=200;
int lastStalkStatus=0;

long int bootTimeMs=0;
int bootWarmupMs=5000;
bool warmedUp=false;

bool sentSilence=false;
bool debug=false;

byte computeChecksum(byte id, byte data[], int dataLength){
  int result = id;
       
  for (int i=0;i<dataLength;i++){
    result = result + data[i];
    if (result>=255){
      result = result-255;
    }
  }
  result = 255-result;
  return result;
}

void setup() {
  bootTimeMs=millis();
  
  // Set LIN1 Write Enable
  pinMode(EN1, OUTPUT);  
  digitalWrite(EN1, HIGH);  

  // Set LIN2 Write Enable
  pinMode(EN3, OUTPUT);  
  digitalWrite(EN3, HIGH);  
  
  SerialUSB.begin(115200);
  
  // Init
  Serial1.begin(linBaud);
  Serial3.begin(linBaud);
}

void linBreak(){
  Serial3.flush();
  Serial3.begin(linBaud/2);
  byte zero=0x00;
  Serial3.write(zero);
  Serial3.flush();
  Serial3.begin(linBaud);
}

void queryModel3(){     
  linBreak();   
  Serial3.write(0x55);
  Serial3.write(0xA6);
  Serial3.flush();
}

String arrayToHex(byte data[], int length){
  String result="";
  for (int i=0;i<length;i++){
    if (i>0){
      result += ' ';
    }
    if(data[i] < 0x10) {
      result += '0';
    }

    result += String(data[i], HEX);
  }
  return result;
}

void handleModel3Lin(){
    if (Serial3.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial3.read();
    if (incomingByte == 0x55){
      model3BufferPosition=0;
    } 
    //SerialUSB.print(incomingByte, HEX);
    //SerialUSB.print("\t");    
    
    if (model3BufferPosition>=0 && model3BufferPosition<11){
      model3Buffer[model3BufferPosition]=incomingByte;      
    }

    if (model3BufferPosition>=0){
      model3BufferPosition++;
    }

    // After 6 bytes, we can determine the state of the Model 3 wheel
    // [0x55  ID  Data0  Data1  Data2  Data3]
    if (model3BufferPosition==6){           
      for (int i=0;i<model3DefSize;i++){
        bool match=true;
        for (int j=0;j<4;j++){
          if (model3Buffer[j+2]!=model3Def[i][j]){
            match=false;
            break;
          }          
        }
        if (match){
          model3State = i;
          // Don't clear states when receiving no-button state from the Model 3 wheel. The states are cleared after responding with Model S messages.
          if (i!=0){


            if ((i>=RW_UP && i<=RW_DOWN_MULTI2) && !sentSilence){
              // In between each RW_UP/RW_DOWN events, send a pause, to have the car perform cruise control speed increase/decrease by 1

              // Don't set the state
            } else {            
              model3StateL = i;
              model3StateR = i;
              sentSilence=false;
            }
          }
          break;
        }
      }

      // Hard-code Data2 check to detect horn in combination with other buttons
      if (model3Buffer[3+2]>0x16){
        model3StateL = T_HORN;
        model3StateR = T_HORN;
        sentSilence=false;
      }

      if (model3State>0){
        SerialUSB.print("Model 3 State: ");
        SerialUSB.print(model3State);
        SerialUSB.print(" from packet ");      
        SerialUSB.println(arrayToHex(model3Buffer,6));
      }
    }  
   }
}

void handleModelSLin(){
    if (Serial1.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial1.read();
    if (incomingByte == 0x55){
      modelSBufferPosition=0;
    } 
   
    if (modelSBufferPosition>=0){
      modelSBufferPosition++;
    }

    // After 2 bytes, we need to respond to the query
    // [0x55  ID ]
    if (modelSBufferPosition==2){

      bool printPacket=false;
      if (incomingByte==modelSLeftIdent || incomingByte==modelSRightIdent){
        // Prepare response packet
        byte data[9];

        // Is query for ModelS steering wheel left-side controls?
        if (incomingByte==modelSLeftIdent){
            for (int i=0;i<7;i++){
              int modelSLeftState = model3ToModelS[model3StateL][0];
              data[i] = modelSLeftDef[modelSLeftState][i];
            }
            if (model3StateL>0){
              printPacket=true;
            }
            if (model3StateL==0){
              sentSilence=true;
            }
            model3StateL=0;
        } 

        // Is query for ModelS steering wheel right-side controls?
        if (incomingByte==modelSRightIdent){
            for (int i=0;i<7;i++){
              int modelSRightState = model3ToModelS[model3StateR][1];
              data[i] = modelSRightDef[modelSRightState][i];
            }
            if (model3StateR>0){
              printPacket=true;
            }
            if (model3StateR==0){
              sentSilence=true;
            }            
            model3StateR=0;
        } 

        data[7]=16*modelSResponseCounter+15;
        data[8]= computeChecksum(incomingByte, data, 8);
        respondModelS(data,9);
        

        if (printPacket){
          SerialUSB.print("Model S Response to ID "); 
          SerialUSB.print(incomingByte,HEX); 
          SerialUSB.print(" : "); 
          SerialUSB.println(arrayToHex(data,9));
        }

        
        modelSResponseCounter++;
        if (modelSResponseCounter>15){
          modelSResponseCounter=0;
        }            
      }        
    }
   }
}

void respondModelS(byte data[],int length){
  for (int i=0;i<length;i++){
    Serial1.write(data[i]);
    Serial1.flush();
  }
}

void readModel3Stalk(){
  long int currentTime = millis();
  int sensorYellow= analogRead(A0);
  int sensorRed= analogRead(A1);
  int sensorWhite= analogRead(A2);

  bool highBeamAssist=false;
  bool wiperAssist=false;
      
  bool printState = false;
  if (model3StateL==0 && model3StateR==0){
    printState = true;
  }

  if (highBeamsMs>0 && currentTime-highBeamsMs<highBeamsDurationMs){
    highBeamAssist=true;
    model3StateL = ST_LIGHT1;
    model3StateR = ST_LIGHT1;
    if (printState) SerialUSB.println("Lights Assist");
  }

  if (wiperMs>0 && currentTime-wiperMs<wiperDurationMs){
    wiperAssist=true;
    model3StateL = ST_WASH1;
    model3StateR = ST_WASH1;
    if (printState) SerialUSB.println("Wiper Assist");
  }
  
  if (hardSignalEnabled){
    if (currentTime-hardSignalEnabledMs>hardSignalAutoDisableIntervalMs){
      SerialUSB.println("Auto-disabling hard signal");
      hardSignalEnabled = false;
    }   
    if (currentTime-hardSignalActivatedMs<hardSignalDurationMs){
      int hardSignalLengthMs=currentTime-hardSignalActivatedMs;
      if ((hardSignalLengthMs/hardSignalFlashIntervalMs)%2==0){
        model3StateL = ST_LIGHT1;
        model3StateR = ST_LIGHT1;
      } else {
        model3StateL = 0;
        model3StateR = 0;     
      }
    }
  }

  if (debug){
    SerialUSB.print("Read");
    SerialUSB.print("\t");
    SerialUSB.print(sensorYellow);
    SerialUSB.print("\t");
    SerialUSB.print(sensorRed);
    SerialUSB.print("\t");
    SerialUSB.print(sensorWhite);
    SerialUSB.print("\n");
  }

  if (sensorYellow>50 && sensorYellow<175){
    if (printState) SerialUSB.println("Wash1");
    model3StateL = ST_WASH1;
    model3StateR = ST_WASH1;
  } else if (sensorYellow>175 && sensorYellow<340){
    if (printState) SerialUSB.println("Wash2");
    wiperMs = currentTime;
    model3StateL = ST_WASH2;
    model3StateR = ST_WASH2;
  } else if (sensorRed>=705 && sensorRed<725){
    bool detectedStalkConflict=false;
    if (disableStalksMs>0 && currentTime-disableStalksMs<disableStalksDurationMs){
      // Disable conflicting stalk status for a short period after a turn signal message, to avoid false-readouts
      int remainingDisabledTimeMs = disableStalksDurationMs - (currentTime-disableStalksMs);
      if (lastStalkStatus==ST_BLINK_RIGHT1){
        detectedStalkConflict=true;
      }
    }
      
    if (printState) SerialUSB.println("Down1");
    if (!detectedStalkConflict){
      model3StateL = ST_BLINK_LEFT1;
      model3StateR = ST_BLINK_LEFT1;     
      disableStalksMs = currentTime;
      lastStalkStatus = model3StateL;
    } else {
      if (printState) SerialUSB.println("Disabled, state chaged too fast...");
    }
  } else if (sensorRed>1020){
    if (printState) SerialUSB.println("Down2");
    model3StateL = ST_BLINK_LEFT1;
    model3StateR = ST_BLINK_LEFT1;  
    disableStalksMs = currentTime;
    lastStalkStatus = model3StateL;
  } else if (sensorRed<10){    

    bool detectedStalkConflict=false;
    if (disableStalksMs>0 && currentTime-disableStalksMs<disableStalksDurationMs){
      // Disable conflicting stalk status for a short period after a turn signal message, to avoid false-readouts
      int remainingDisabledTimeMs = disableStalksDurationMs - (currentTime-disableStalksMs);
      if (lastStalkStatus==ST_BLINK_LEFT1){
        detectedStalkConflict=true;
      }
    }
      
    if (printState) SerialUSB.println("Up1");
    if (!detectedStalkConflict){
      model3StateL = ST_BLINK_RIGHT1;
      model3StateR = ST_BLINK_RIGHT1;   
      disableStalksMs = currentTime;
      lastStalkStatus = model3StateL;
    } else {
      if (printState) SerialUSB.println("Disabled, state chaged too fast...");    
    }   
  } else if (sensorRed>590 && sensorRed<610){
    if (printState) SerialUSB.println("Up2");
    model3StateL = ST_BLINK_RIGHT1;
    model3StateR = ST_BLINK_RIGHT1; 
    disableStalksMs = currentTime;
    lastStalkStatus = model3StateL;
  } else if (sensorWhite>75 && sensorWhite<200){
    if (printState) SerialUSB.println("Lights1");
    model3StateL = ST_LIGHT1;
    model3StateR = ST_LIGHT1;
    highBeamsMs = currentTime;
  } else if (sensorWhite>200 && sensorWhite<400){
    if (hardSignalEnablingInProgress){
      // check if enough time passed
      if (currentTime-hardSignalEnablingMs>hardSignalEnablingDurationMs){
        hardSignalEnablingInProgress=false;

        // Honk to confirm state change 
        model3StateL = T_HORN;
        model3StateR = T_HORN;
 
        hardSignalEnabled = !hardSignalEnabled;
        SerialUSB.print("Hard Signal Enabled: ");
        SerialUSB.println(hardSignalEnabled);
        
        if (hardSignalEnabled){  
          hardSignalEnabledMs = currentTime;
          hardSignalActivatedMs = currentTime;
        }               
      }
    } else {
      SerialUSB.println("Starting Hard Signal Toggle. ");
      SerialUSB.print("Hard Signal Enabled: ");
      SerialUSB.println(hardSignalEnabled);
      hardSignalEnablingInProgress = true;
      hardSignalEnablingMs = currentTime;
    }   

    if (hardSignalEnabled){
      hardSignalActivatedMs = currentTime;
    }
    
    if (printState) SerialUSB.println("Lights2");
    if (highBeamAssist){
      highBeamsMs = 0;
      model3StateL = 0;
      model3StateR = 0;
    } else {
      model3StateL = ST_LIGHT2;
      model3StateR = ST_LIGHT2;
    }
  } else {
    if (hardSignalEnablingInProgress){
      SerialUSB.print("Aborting Hard Signal Toggle. ");
      SerialUSB.print("Hard Signal Enabled: ");
      SerialUSB.println(hardSignalEnabled);
      hardSignalEnablingInProgress = false;
    }
  }
}

void loop() {
  int count=0;
  int wheelDataPosition=-1;
    
  long int currentTime = millis();
    
  if (currentTime-model3LastQueryMs>model3QueryIntervalMs){
    model3LastQueryMs=currentTime;
    queryModel3();
  }
  
  handleModel3Lin();
  readModel3Stalk();

  if (currentTime-bootTimeMs>bootWarmupMs){    
    warmedUp=true;    
  }
  
  if (warmedUp){
    // Don't send messages right away after boot, to avoid high beams or horn activation right after boot
    handleModelSLin();
  } else {
    SerialUSB.println("Warming up ...");
  }
}
