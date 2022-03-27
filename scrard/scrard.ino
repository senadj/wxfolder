#define SERIAL_BAUDRATE 115200
#define NUM_ALL_PINS NUM_DIGITAL_PINS
#define MAX_SERVO_PINS 12   // max 12 for Uno and 48 for arduino Mega


#include <Servo.h>
Servo servos[MAX_SERVO_PINS];

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// https://www.arduino.cc/en/Tutorial/SerialEvent
void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    }
    else
      inputString += inChar;
  }
}

void setup()
{
  inputString.reserve(64);
  Serial.begin(SERIAL_BAUDRATE);
}


int cmpfunc (const void * a, const void * b) {
   return ( *(char*)a - *(char*)b );
}


class wxSerialDataManager
{
  public:
    unsigned long previousMillis = 0;
    unsigned long currentMillis;
    uint8_t aa[NUM_ANALOG_INPUTS];
    uint8_t dd[NUM_DIGITAL_PINS];
    uint8_t acnt, dcnt, pinNo, pinVal, msglen, delayMillis;
    const char zerosbuff[2] = {'0','0'};
    char cbuff[3]  = {'?','?','\0'}; // used for conversion of hex value to int using strtol
    char pwm_pins[NUM_ALL_PINS] = {0};
    char servo_pins[NUM_ALL_PINS] = {0};
    char pin_servo_map[NUM_ALL_PINS];
    char temp_array[NUM_ALL_PINS];


  wxSerialDataManager() : acnt(0), dcnt(0), delayMillis(100)
  {
    memset(pin_servo_map,-1,sizeof(pin_servo_map));
  }

  void ServoAttach(uint8_t pin)
  {
    if ( !servo_pins[pin] )
    {
      // find first unoccupied servo
      memcpy(temp_array, pin_servo_map, sizeof(pin_servo_map));
      qsort(temp_array, NUM_ALL_PINS, sizeof(char), cmpfunc);
      char min_servo_idx = 0;

      for ( uint8_t i=0; i < NUM_ALL_PINS; i++ )
      {
        if ( temp_array[i] < 0 )
          continue;

        if ( temp_array[i] == min_servo_idx )
          min_servo_idx++;
        else
          break;
      }

      if ( min_servo_idx < MAX_SERVO_PINS )
      {
        servos[min_servo_idx].attach(pin);
        servo_pins[pin] = 1;
        pin_servo_map[pin] = min_servo_idx;
      }
    }
  }

  void ServoDetach(uint8_t pin)
  {
    if ( servo_pins[pin] )
    {
      if ( servos[pin_servo_map[pin]].attached() )
        servos[pin_servo_map[pin]].detach();

      servo_pins[pin] = 0;
      pin_servo_map[pin] = -1;
    }
  }

  void DigitalReadSubscribe(uint8_t pin, bool doRead=true)
  {
    bool pinFound = false;

    if (!doRead && dcnt==1 && dd[0] == pin) { dcnt=0; } // no swap in this case

    for (uint8_t i=0; i<dcnt; i++)
    {
      if ( pinFound && !doRead ) { dd[i-1] = dd[i]; } // swap values
      if ( dd[i] == pin ) {  pinFound = true; }
    }

    if (pinFound && !doRead) { dcnt--; }

    if (!pinFound && doRead)
    {
      dd[dcnt] = pin;
      dcnt++;
    }

    if (doRead) AnalogReadSubscribe(pin,false); // unsubscribe from analogRead
  }

  void AnalogReadSubscribe(uint8_t pin, bool doRead=true)
  {
    bool pinFound = false;

    if (!doRead && acnt==1 && aa[0] == pin) { acnt=0; } // no swap in this case

    for (uint8_t i=0; i<acnt; i++)
    {
      if ( pinFound && !doRead ) { aa[i-1] = aa[i]; } // swap values
      if ( aa[i] == pin ) {  pinFound = true; }
    }

    if (pinFound && !doRead) { acnt--; }

    if (!pinFound && doRead)
    {
      aa[acnt] = pin;
      acnt++;
    }

    if (doRead) DigitalReadSubscribe(pin,false); // unsubscribe from digitalRead
  }

  void PrintHex3(int num) // int to lpad(hex,'0',3)
  {
    if (num<16)
      Serial.write(zerosbuff,2);
    else if (num<256)
      Serial.write(zerosbuff,1);

    Serial.print(num,HEX);
  }

  void PrintHex2(int num) // int to lpad(hex,'0',2)
  {
    if (num<16)
      Serial.write(zerosbuff,1);

    Serial.print(num,HEX);
  }

  void ProcessOutgoing()
  {
    msglen = 0;
    currentMillis = millis();

    if (currentMillis - previousMillis < delayMillis)
      return;

    previousMillis = currentMillis;

    for (uint8_t i=0; i<acnt; i++)
    {
      PrintHex2(aa[i]); // send out pinNo
      PrintHex3(analogRead(aa[i])); // send out analogRead value
      msglen+=5;
    }

    for (uint8_t i=0; i<dcnt; i++)
    {
      PrintHex2(dd[i]); // send out pinNo
      PrintHex3(digitalRead(dd[i])); // send out digitalRead value
      msglen+=5;
    }

    if ( msglen > 0 )
    {
      PrintHex2(msglen);  // send out message length
      //Serial.print("\r\n");
      Serial.write('\n');
      //Serial.flush();
    }
  }

  void SpecialCommand( uint8_t pinNo, char pinCmd )
  {
        switch( pinCmd )
        {
          case 'D' : ServoDetach(pinNo); DigitalReadSubscribe(pinNo,true); break;
          case 'A' : ServoDetach(pinNo); AnalogReadSubscribe(pinNo, true); break;
          case 'I' : ServoDetach(pinNo); pinMode(pinNo, INPUT); break;
          case 'P' : ServoDetach(pinNo); pinMode(pinNo, INPUT_PULLUP); break;
          case 'O' : ServoDetach(pinNo); pinMode(pinNo, OUTPUT); DigitalReadSubscribe(pinNo,false); AnalogReadSubscribe(pinNo, false); break;
          case 'S' : ServoAttach(pinNo); DigitalReadSubscribe(pinNo,false); AnalogReadSubscribe(pinNo, false); break;
          case 'U' : ServoDetach(pinNo); if (digitalPinHasPWM(pinNo))  pwm_pins[pinNo] = 1; break; // enable analogWrite PWM
          case 'W' : pwm_pins[pinNo] = 0; break; // digitalWrite (only disables analogWrite flag)

          // unsubscribe
          case 'd' : DigitalReadSubscribe(pinNo,false); break;
          case 'a' : AnalogReadSubscribe(pinNo, false); break;
          case 'i' : AnalogReadSubscribe(pinNo, false); DigitalReadSubscribe(pinNo,false); break;
          case 's' : ServoDetach(pinNo); break;

          case 127 : Serial.print("FFFFF05\n"); break; // ping
        }

        if (pinCmd < 0) { delayMillis = -1*pinCmd; } // delay
  }

  void ProcessIncoming(String& s)
  {
      uint8_t slen = s.length(); // could be longer than msg if serial buffer not empty
      const char* p = s.c_str();
      char* pend; // after strtol used for validation from hex to number

      if (slen < 5)
        return;
      else
        slen-=2;

      msglen = strtol(p+slen,&pend,16); // this should be msg length hex encoded
      if (*pend) { Serial.println("Err1"); return; } // hex to num conversion error

      if (slen < msglen) { Serial.println("Err2 " + s); return; } // missing beginning of msg?

      if (slen > msglen)  // serial buffer garbage - maybe something old prefixed?
      {
        //Serial.println("ignored:" + s.substring(0,slen-msglen));
        Serial.println("Err3");
        p += slen-msglen;
      }

      bool even = true;
      for ( uint8_t i = 1; i < msglen; i += 2, p += 2, even = !even )
      {
          memcpy(&cbuff[0],p,2);

          if ( even )
            pinNo = strtol(cbuff,&pend,16);
          else
            pinVal = strtol(cbuff,&pend,16);

          if (*pend) { Serial.println("Err4"); return; } // hex to num conversion error

          if ( !even )
          {
            if ( pinNo >= 63 )  // pin numbers from 63(==pin0) denote special commands
                SpecialCommand( pinNo-63, pinVal );
            else
            {
                if ( servo_pins[pinNo] )
                    servos[pin_servo_map[pinNo]].write( pinVal );
                else if ( pwm_pins[pinNo] )
                    analogWrite( pinNo, pinVal );
                else
                    digitalWrite( pinNo, pinVal ? HIGH : LOW );
            }
          }
      }

      if ( msglen == 3 ) // CMD
      {
        //Serial.println("CMD");
        //Serial.println(pinNo);
        char cmd = *p;
        SpecialCommand(pinNo, cmd);
      }
  }
} serialMgr;



// the loop routine runs over and over again forever:
void loop()
{
  if (stringComplete)
  {
    //Serial.println(inputString);
    serialMgr.ProcessIncoming(inputString);
    inputString = "";
    stringComplete = false;
  }

  serialMgr.ProcessOutgoing();
}
