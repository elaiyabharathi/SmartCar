#include <NewPing.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <XBee.h>
#include <toneAC.h>

#define MAX_DIS 300

// create the XBee object
XBee xbee = XBee();

//uint8_t payload[] = { 0, 0 };
char* payload = "hello";
char Buffer[200];
// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x408bacd8);
ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *)payload, strlen(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

// constants won't change. Used here to set a pin number :
const int ledPin =  53;      // the number of the LED pin
const int ledPin2 = 51;
int ledState = LOW;             // ledState used to set the LED

class ultrasonic{
    long onTime;
    unsigned long previousMillis=0;    // will store last time LED was updated
    NewPing *sonar;
public:
  ultrasonic(int trigg_pin,int echo_pin,int pingSpeed){
    sonar = new NewPing(trigg_pin,echo_pin,MAX_DIS);
    
    onTime = pingSpeed;
  }
  void update(int us){
     // check to see if it's time to change the state of the LED
     NewPing s = (*sonar);
    unsigned long currentMillis = millis();
    if ((currentMillis-previousMillis)>=onTime) {   // pingSpeed milliseconds since last ping, do another ping.
    previousMillis = currentMillis;
    Serial.print("Ultrasonic sensor ");
  //  Serial3.print("Ultrasonic sensor ");
    Serial.print(us);
  //  Serial3.print(us);
    Serial.print("\n");
  //  Serial3.print("\n");
    int cm = s.ping_cm();
      if(cm<=30)
      {
            ledState = LOW;
            digitalWrite(ledPin, ledState);
             Serial.print("hey the man behind you is so near!!");
             for (unsigned long freq = 150; freq <= 15000; freq += 10) {  
             toneAC(freq); // Play the frequency (150 Hz to 15 kHz in 10 Hz steps).
             delay(1);     // Wait 1 ms so you can hear it.
             }
             toneAC(0); // Turn off toneAC, can also use noToneAC().
      }
      else
      {
          ledState = HIGH;
          digitalWrite(ledPin,ledState);
      }
    Serial.print("dis");
   // Serial3.print("dis");
    Serial.print(cm);
   // Serial3.print(cm);
    Serial.print("\n");
    Serial.print("\n");
    }
  }
  
};

static const int RXPin = 48, TXPin = 49;
static const uint32_t GPSBaud = 9600;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);
 TinyGPSPlus gps;
class GPS
{
 public:
 GPS(){
    Serial2.begin(GPSBaud);
  }

  void getGpsdata(float lati,float longi){
    Serial.write("printing lat lon");
    //Serial3.write("printing lat lon");
    printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
    printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
    Serial.write("\n"); 
    //Serial3.write("\n");
    double courseToLondon =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      lati, 
      longi);
    Serial.print("CourseTo\t");
    printFloat(courseToLondon, gps.location.isValid(), 7, 2);
    const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

    printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

    smartDelay(1000);
    
  }
  static void printFloat(float val, bool valid, int len, int prec)
  {
    if (!valid)
    {
      while (len-- > 1){
        Serial.print('*');
        //Serial3.print('*');
        strcpy(Buffer,"*");
      zbTx = ZBTxRequest(addr64, (uint8_t *)Buffer, strlen(Buffer));
      //xbee.send(zbTx);
      }
      Serial.print(' ');
      //Serial3.print(' ');
    }
    else
    {
      Serial.print(val, prec);
      //Serial3.print(val, prec);
      Serial.print(val);
      Serial.print("printing buffer\n");
      String bu = String(val,prec);
      Serial.print(bu);
      bu.toCharArray(Buffer,20);
      
      //Serial.print(Buffer);
      zbTx = ZBTxRequest(addr64, (uint8_t *)Buffer, strlen(Buffer));
    //  xbee.send(zbTx);
      int vi = abs((int)val);
      int flen = prec + (val < 0.0 ? 2 : 1); // . and -
      flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
      for (int i=flen; i<len; ++i)
        Serial.print(' ');
        //Serial3.print(' ');
    }
    smartDelay(0);
  }
  static void printStr(const char *str, int len)
  {
    int slen = strlen(str);
    for (int i=0; i<len; ++i)
      Serial.print(i<slen ? str[i] : ' ');
    smartDelay(0);
  }
  static void smartDelay(unsigned long ms)
  {
    unsigned long start = millis();
    do 
    {
      while (Serial2.available())
        gps.encode(Serial2.read());
        
    } while (millis() - start < ms);
  }
};

//response object
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();


ultrasonic NW(6,7,2000);
ultrasonic N(8,9,6000);
ultrasonic NE(4,5,7000);
ultrasonic S(10,13,8000);
GPS g;
LiquidCrystal lcd(27,26,25,24,23,22);
byte rx_byte = 0;        // stores received byte
String rcd;
void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  Serial3.begin(9600);
  Serial2.begin(9600);
  xbee.setSerial(Serial3);
  lcd.begin(16,2);
  lcd.print("Hello");
}
//Function to read a packet from xbee
void readPacketXBee(){
  delay(1000);
    //reading a packet.......
  xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      Serial.print("aleast got something");
      xbee.getResponse().getZBRxResponse(rx);
      for (int i= 0; i < rx.getDataLength(); i++){
                  Serial.write(' ');
                  if (iscntrl(rx.getData()[i]))
                    Serial.write(' ');
                  else
                    Serial.write(rx.getData()[i]);
                  Serial.write(' ');
                }
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        // got a zb rx packet
        //Serial.print("ZB_RX_RESPONSE");        
        // now fill our zb rx class
        xbee.getResponse().getZBRxResponse(rx);
            
        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
            // the sender got an ACK
            Serial.print("sender got ack\n");
        } else {
            // we got it (obviously) but sender didn't get an ACK
            Serial.print("Sender didn't get ack");
        }
        Serial.print("printing inside getresponse apiid");
         Serial.println();
                for (int i= 0; i < rx.getDataLength(); i++){
                  Serial.write(' ');
                  if (iscntrl(rx.getData()[i]))
                    Serial.write(' ');
                  else{
                    Serial.write(rx.getData()[i]);
                    lcd.print((char)rx.getData()[i]);
                  }
                  Serial.write(' ');
                }
        
      } else if (xbee.getResponse().getApiId() == MODEM_STATUS_RESPONSE) {
        xbee.getResponse().getModemStatusResponse(msr);
        // the local XBee sends this response on certain events, like association/dissociation
        
        if (msr.getStatus() == ASSOCIATED) {
          // yay this is great.  flash led
          Serial.print("associated");
        } else if (msr.getStatus() == DISASSOCIATED) {
          // this is awful.. flash led to show our discontent
          Serial.print("disassociated");
        } else {
          // another status
          Serial.print("other");
        }
      } else {
        // not something we were expecting
        Serial.print("something's wrong");    
      }
    } else if (xbee.getResponse().isError()) {
      Serial.print("something is wrong");
    }  
}

void loop() {
  // put your main code here, to run repeatedly:
  NW.update(1);
  N.update(2);
  NE.update(3);
  S.update(4);
  //g.getGpsdata();
  g.getGpsdata(13.013176, 80.235224);
  
  // check for data byte on serial port 3
  if (Serial3.available()) {
    // get a byte from serial port 3
    rx_byte = Serial3.read();
    //rcd = Serial3.read();
    // send the byte to the USB serial port
    //lcd.print(rcd);
    
      if(rx_byte = 'H'){
        
      }
    }
    readPacketXBee();
  }

