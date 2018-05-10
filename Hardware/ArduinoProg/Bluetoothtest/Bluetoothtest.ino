#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(10, 11); // RX, TX
#define VCC 7
#define EN 6

/* ---------------------DHT11模块初始化-------------------- */
const int dataPin = 8;    // 引脚定义
int temperature = 99;    // 初始温度值，若设备或程序初始化故障DHT11未能正确工作会显示这个值
int humidity = 99;    // 初始湿度值，若设备或程序初始化故障DHT11未能正确工作会显示这个值
/* --------------------------------------------------- */

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
  Serial.println("Ready!");
 
  // set the data rate for the SoftwareSerial port
 
  // for HC-05 use 38400 when poerwing with KEY/STATE set to HIGH on power on
  mySerial.begin(9600);

  pinMode(VCC,OUTPUT);
  pinMode(EN,OUTPUT);
  digitalWrite(VCC,LOW);
  delay(100);
  digitalWrite(EN,HIGH);
  //delay(10000);
  digitalWrite(VCC,HIGH);
}

/*-------------DHT11模块读取温湿度信息程序。一般情况无需修改---------------------*/
int readDHT11() {
  uint8_t recvBuffer[5];  // <1>
  uint8_t cnt = 7;
  uint8_t idx = 0;
  for (int i = 0; i < 5; i++) recvBuffer[i] = 0; // <2>

  // Start communications with LOW pulse
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  delay(18);
  digitalWrite(dataPin, HIGH);

  delayMicroseconds(40);  // <3>
  pinMode(dataPin, INPUT);
  pulseIn(dataPin, HIGH);   // <4>
  // Read data packet
  unsigned int timeout = 10000; // loop iterations
  for (int i = 0; i < 40; i++) // <5>
  {
    timeout = 10000;
    while (digitalRead(dataPin) == LOW) {
      if (timeout == 0) return -1;
      timeout--;
    }

    unsigned long t = micros(); // <6>

    timeout = 10000;
    while (digitalRead(dataPin) == HIGH) { // <7>
      if (timeout == 0) return -1;
      timeout--;
    }

    if ((micros() - t) > 40) recvBuffer[idx] |= (1 << cnt); // <8>
    if (cnt == 0)   // next byte?
    {
      cnt = 7;    // restart at MSB
      idx++;      // next byte!
    }
    else cnt--;
  }

  humidity = recvBuffer[0]; // %  // <9>
  temperature = recvBuffer[2];  // C
  uint8_t sum = recvBuffer[0] + recvBuffer[2];
  if (recvBuffer[4] != sum) return -2; // <10>
  return 0;
}
/*--------------------------------------------------------------------------*/
 
void loop() // run over and over
{
//  delay(1000);
//  mySerial.write("12 ");
//  Serial.write("12 ");
  int msg_recv; 
  if(mySerial.available())
  {
    //Serial.println("yyyeees");
    msg_recv = mySerial.read();
    Serial.println((char)msg_recv);
  }
  if(msg_recv=='1'){
     Serial.println("Command received");
     mySerial.write(humidity);
    }
  //Serial.write(mySerial.read());
//  if (Serial.available())
//    mySerial.write(Serial.read());
}
