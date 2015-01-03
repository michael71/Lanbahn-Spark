// tinkerplus.ino
// by Michael Blank

int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int lanbahnFun(String state);
// NOTE There is a maximum of 4 functions !!!!!!

#define IDENT_KEY  D2
#define INDICATOR_RED   A2
#define INDICATOR_GREEN A3
#define ENABLE_TURNOUTS  D3

#define IDENT_KEY_PRESSED     1
#define IDENT_KEY_NOTPRESSED  0
#define IDENT_KEY_UNKNOWN     2
#define KEYPRESS_COUNT_LIMIT 50     // 50 = 5 seconds

#define NSIG  4
#define NT    4


//#define UDP_ENABLED

int last_key = IDENT_KEY_UNKNOWN;  // at start
int keypress_count = 0;
int key_state = 0;
int indicator = 0;  // indicator for remote selection of this module
int indicator_count = 0;

int dim[8]={255,200,160,120,80,40,20,10};
int count[8]={0,0,0,0,0,0,0,0};
int state[8]={0,0,0,0,0,0,0,0};

int sig1[NSIG]={A4,A5,A6,A7};
int sig1act[NSIG];
int sig1tobe[NSIG]={0,255,255,255};   // red

int sig2[NSIG]={A0,A1,D0,D1};
int sig2act[NSIG];
int sig2tobe[NSIG]={0,255,255,255};   // red

int turn[NT]={D4,D5,D6,D7};   // Turnouts


// lanbahn
// 
#ifdef UDP_ENABLED
IPAddress ipAddress(239,200,201,250);
UDP Udp;
#define LANBAHN_PORT  27027
#endif

// A = 2 Signals (UK) + 4 Turnouts - red LED=A2 - green LED=A3

char *version = "A121 tinkerplus 1.21 - 21 Dec 2014";
int rssi = -255;

unsigned long currentTime=0;
unsigned long lastTime=0;
int mincount = 0;

void setup()
{
    Spark.function("digitalread", tinkerDigitalRead);
    Spark.function("digitalwrite", tinkerDigitalWrite);

    Spark.function("lanbahnfun", lanbahnFun);
    
    Spark.variable("rssi", &rssi, INT);
    Spark.variable("version", version, STRING);
    
    // setup keys and indicator leds
    pinMode(IDENT_KEY, INPUT);
    pinMode(INDICATOR_RED, OUTPUT);
    pinMode(INDICATOR_GREEN, OUTPUT);
    digitalWrite(INDICATOR_RED, LOW);
    digitalWrite(INDICATOR_GREEN, LOW);
    
    // digitalWrite(KEY,HIGH); does not work!! needs actual pull-up of 10k

    
    // setup signal 0
    for (int i=0; i <NSIG; i++) {
       
        pinMode(sig1[i], OUTPUT);   // 0=red, 1=green
                              // 2=yellow, 3=feather
        analogWrite(sig1[i],sig1tobe[i]); 
        sig1act[i]=sig1tobe[i];
    
        pinMode(sig2[i], OUTPUT);   // 0=red, 1=green
                              // 2=yellow, 3=feather
        analogWrite(sig2[i],sig2tobe[i]);  
        sig2act[i]=sig2tobe[i];
    }
    
    
    
    // setup turnouts 0..3
    for (int i=0; i < NT; i++) {
        pinMode(turn[i], OUTPUT);
        digitalWrite(turn[i],LOW);  // all turnouts closed.
    }
    
    pinMode(ENABLE_TURNOUTS,OUTPUT);
    digitalWrite(ENABLE_TURNOUTS,HIGH);  // enable all turnouts
    
    lanbahnFun("S 0 R");   // init signal 0 to RED
    lanbahnFun("S 1 R");   // init signal 1 to RED
    lanbahnFun("T 0 C");   // init turnout to closed
    lanbahnFun("T 1 C");   // init turnout to closed
    lanbahnFun("T 2 C");   // init turnout to closed
    lanbahnFun("T 3 C");   // init turnout to closed

#ifdef UDP_ENABLED    
    Udp.begin(LANBAHN_PORT);
#endif
}

void loop()
{   currentTime = millis();
    if ( (currentTime - lastTime) >= 100) {
        lastTime = currentTime;
        every100ms();
        mincount++;
        if (mincount >= 60) {
            everyMinute();
            mincount = 0;
        }
    }
   
#ifdef UDP_ENABLED
    Udp.flush();
#endif 
 
}

void everyMinute() {
    rssi = WiFi.RSSI();
}

void every100ms() {
     //check if key was pressed (==LOW)

    key_state = (digitalRead(IDENT_KEY) == 0);
    // end without debounce
    
    if (last_key != key_state) {
        if (key_state == IDENT_KEY_PRESSED) {
            Spark.publish("keyon");
#ifdef UDP_ENABLED
            Udp.beginPacket(ipAddress,LANBAHN_PORT);
            Udp.write("Key ON");
            Udp.endPacket(); 
#endif
        } else {
            Spark.publish("keyoff");
        } 
        last_key = key_state;

    }
    
    // check connected state
    if (Spark.connected) {
        if (indicator_count <5 ) {
            // inverted !!! 
            digitalWrite(INDICATOR_GREEN,HIGH);  
            digitalWrite(INDICATOR_RED,LOW);
        } else {
            digitalWrite(INDICATOR_GREEN,LOW);
            digitalWrite(INDICATOR_RED,HIGH);
        }
        if (indicator) {
            indicator_count++;
            if (indicator_count >10) indicator_count = 0;
        } else {
            indicator_count = 0;
        }
    } else {
        digitalWrite(INDICATOR_GREEN,LOW);
        digitalWrite(INDICATOR_RED,LOW);
    }
    
    for (int i=0; i <NSIG; i++) {
        if (sig1act[i] > sig1tobe[i]) {
            sig1act[i] -= 50;
            if (sig1act[i] < 0) sig1act[i]=0;
            analogWrite(sig1[i],sig1act[i]);
        }
        if (sig1act[i] < sig1tobe[i]) {
            sig1act[i] += 50;
            if (sig1act[i] > 255) sig1act[i]=255;
            analogWrite(sig1[i],sig1act[i]);
        }
        if (sig2act[i] > sig2tobe[i]) {
            sig2act[i] -= 50;
            if (sig2act[i] < 0) sig2act[i]=0;
            analogWrite(sig2[i],sig2act[i]);
        }
        if (sig2act[i] < sig2tobe[i]) {
            sig2act[i] += 50;
            if (sig2act[i] > 255) sig2act[i]=255;
            analogWrite(sig2[i],sig2act[i]);
        }
        
    }
}

int lanbahnFun(String state) {
    // 2 "sub" functions for signals and turnout
    // S 0 R   = signal 0 (pins A4..A7) to R (red) 
    //   signal: 0 or 1, R=Red, G=Green, Y=Yellow, F=Y+Feather
    //   turnout: 0,1,2,3 - T=thrown, C=closed
    // T 1 T   = turnout 1 thrown

    if (state.startsWith("S")) {
        if (state.charAt(2) == '0') {
            // signal 0  (pins A4..A7)
            switch (state.charAt(4))
            {
                case 'R':
                    sig1tobe[0]=255;
                    sig1tobe[1]=0;
                    sig1tobe[2]=0;
                    sig1tobe[3]=0;
                    break;
                case 'G':
                    sig1tobe[0]=0;
                    sig1tobe[1]=255;
                    sig1tobe[2]=0;
                    sig1tobe[3]=0;
                    break;
                case 'Y':
                    sig1tobe[0]=0;
                    sig1tobe[1]=0;
                    sig1tobe[2]=255;
                    sig1tobe[3]=0;
                    break;
                case 'F':
                    sig1tobe[0]=0;
                    sig1tobe[1]=0;
                    sig1tobe[2]=255;
                    sig1tobe[3]=255;
                    break;
                default:
                    return -2;
            }
        }  else if (state.charAt(2) == '1') {
            // signal 1  (pins A0,A1,D0,D1)
            switch (state.charAt(4))
            {
                case 'R':
                    sig2tobe[0]=255;
                    sig2tobe[1]=0;
                    sig2tobe[2]=0;
                    sig2tobe[3]=0;
                    break;
                case 'G':
                    sig2tobe[0]=0;
                    sig2tobe[1]=255;
                    sig2tobe[2]=0;
                    sig2tobe[3]=0;
                    break;
                case 'Y':
                    sig2tobe[0]=0;
                    sig2tobe[1]=0;
                    sig2tobe[2]=255;
                    sig2tobe[3]=0;
                    break;
                case 'F':
                    sig2tobe[0]=0;
                    sig2tobe[1]=0;
                    sig2tobe[2]=255;
                    sig2tobe[3]=255;
                    break;  
                default:
                    return -2;
            }
        }
       return 1;
    } else if (state.startsWith("T")) {
       int nturnout = state.charAt(2) - '0';
       if ((nturnout < 0) || (nturnout >3)) return -4;
       if (state.charAt(4) == 'T') {
           digitalWrite(turn[nturnout], HIGH);
           return 2;
       } else if (state.charAt(4) == 'C') {
           digitalWrite(turn[nturnout], LOW);
           return 2;
       } else {
           return -5;  // error in setting turnout
       }
    } else if (state.startsWith("I")) {
       if (state.charAt(2) == '0') {
           indicator = 0;
           return 3;
       } else if  (state.charAt(2) == '1') {
           indicator = 1;
           return 3;
       } else {
           return -6;  // error in setting turnout
       }
    } else {
        return -3;  // command not understood
    }
}

int tinkerDigitalRead(String pin) {
    int pinNumber = pin.charAt(1) - '0';
    if (pinNumber< 0 || pinNumber >7) return -1;
    if(pin.startsWith("D")) {
        pinMode(pinNumber, INPUT_PULLDOWN);
        return digitalRead(pinNumber);}
    else if (pin.startsWith("A")){
        pinMode(pinNumber+10, INPUT_PULLDOWN);
        return digitalRead(pinNumber+10);}
    return -2;}

int tinkerDigitalWrite(String command){
    bool value = 0;
    int pinNumber = command.charAt(1) - '0';
    if (pinNumber< 0 || pinNumber >7) return -1;
    if(command.substring(3,7) == "HIGH") value = 1;
    else if(command.substring(3,6) == "LOW") value = 0;
    else return -2;
    if(command.startsWith("D")){
        pinMode(pinNumber, OUTPUT);
        digitalWrite(pinNumber, value);
        return 1;}
    else if(command.startsWith("A")){
        pinMode(pinNumber+10, OUTPUT);
        digitalWrite(pinNumber+10, value);
        return 1;}
    else return -3;}



