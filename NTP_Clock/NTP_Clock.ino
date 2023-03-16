#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

#define LED_BUILTIN 25

#define MAX7219_DIN 10
#define MAX7219_CS  11
#define MAX7219_CLK 12

#define MAX7219_REGISTER_NOP          0x0  // 未使用:
#define MAX7219_REGISTER_DECODE_MODE  0x9
#define MAX7219_REGISTER_INTENSITY    0xA
#define MAX7219_REGISTER_SCAN_LIMIT   0xB
#define MAX7219_REGISTER_SHUTDOWN     0xC
#define MAX7219_REGISTER_TEST         0xF

// 0 ~ F のインデックスでセグメントの情報を出す配列:
const uint8_t SEGMENT_HEX_DECODER[16] = {0b01111110, 0b00110000, 0b01101101, 0b01111001,   // 0, 1, 2, 3:
                                         0b00110011, 0b01011011, 0b01011111, 0b01110000,   // 4, 5, 6, 7:
                                         0b01111111, 0b01111011, 0b01110111, 0b00011111,   // 8, 9, A, b:
                                         0b00001101, 0b10111101, 0b01001111, 0b01000111};  // c, d, E, F:

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  // MAC アドレス:

unsigned int LISTENPORT = 8888;  // ローカル通信用ポート:

const char timeServer[] = "ntp.nict.jp"; // NTP サーバーアドレス:

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

int8_t GMT = 9;  // JST に変換するため:

byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

EthernetUDP Udp;

void setup() {
  LED_init();
  Ethernet.init(17);  // CS pin

  Serial.begin(115200);
  while (!Serial);

  // start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  Udp.begin(LISTENPORT);
}

void loop() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(25);
  if (Udp.parsePacket()) {
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);
    
    epoch = epoch + GMT * 60 * 60;  // JST に変更:

        // print the hour, minute and second:
    Serial.print("The GMT ");
    if(GMT > 0) {
      Serial.print("+");
    }else if(GMT == 0) {
      Serial.print("±");
    }
    Serial.print(GMT);
    Serial.print(" time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    Display_time(epoch % 86400);  // Display to LED:
  }
  Serial.println();
  // // wait ten seconds before asking for the time again
  delay(5000);
  Ethernet.maintain();
}

// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void LED_init(void) {
  pinMode(MAX7219_CS , OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
  pinMode(MAX7219_DIN, OUTPUT);
  
  digitalWrite(MAX7219_CS , HIGH);
  digitalWrite(MAX7219_CLK,  LOW);
  digitalWrite(MAX7219_DIN,  LOW);

  LED_OUT(MAX7219_REGISTER_DECODE_MODE, 0b00000000);    //7セグでデコードするビットに1を立てる:
  LED_OUT(MAX7219_REGISTER_INTENSITY  , 0b00011100);    //輝度を設定, 0-15:
  LED_OUT(MAX7219_REGISTER_SCAN_LIMIT , 0b00000111);    //使用する桁数を指定, 桁数-1:
  LED_OUT(MAX7219_REGISTER_SHUTDOWN   , 0b00000001);    //特にいじる必要なし:
  LED_OUT(MAX7219_REGISTER_TEST       , 0b00000000);    //同上:
}

void LED_OUT(uint8_t addr, uint8_t dat){
  digitalWrite(MAX7219_CS,  LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, addr);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, dat);
  digitalWrite(MAX7219_CS, HIGH);
}

void Display_time(uint32_t times) {
  if(times > 86400) {
    times = times % 86400;
  }
  Serial.print("times = ");
  Serial.println(times);
  uint8_t S8 = times / (60 * 60 * 10);
  times = times % (60 * 60 * 10);
  uint8_t S7 = times / (60 * 60);
  times = times % (60 * 60);
  uint8_t S5 = times / (60 * 10);
  times = times % (60 * 10);
  uint8_t S4 = times /  60;
  times = times % (60);
  uint8_t S2 = times /  10;
  times = times % (10);
  uint8_t S1 = times;

  Display_null();
  LED_OUT(8, SEGMENT_HEX_DECODER[S8]);
  LED_OUT(7, SEGMENT_HEX_DECODER[S7]);
  LED_OUT(6, 0b00000001);  // Hyphen:
  LED_OUT(5, SEGMENT_HEX_DECODER[S5]);
  LED_OUT(4, SEGMENT_HEX_DECODER[S4]);
  LED_OUT(3, 0b00000001);  // Hyphen:
  LED_OUT(2, SEGMENT_HEX_DECODER[S2]);
  LED_OUT(1, SEGMENT_HEX_DECODER[S1]);

  Serial.print(S8);
  Serial.print(S7);
  Serial.print(":");
  Serial.print(S5);
  Serial.print(S4);
  Serial.print(":");
  Serial.print(S2);
  Serial.print(S1);
  Serial.print("\n");
}

void Display_null(void){
  LED_OUT(0x9, 0x00);
  LED_OUT(0x1, 0b00000000);
  LED_OUT(0x2, 0b00000000);
  LED_OUT(0x3, 0b00000000);
  LED_OUT(0x4, 0b00000000);
  LED_OUT(0x5, 0b00000000);
  LED_OUT(0x6, 0b00000000);
  LED_OUT(0x7, 0b00000000);
  LED_OUT(0x8, 0b00000000);
}
