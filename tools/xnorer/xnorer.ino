#include "iClickerEmulator.h"
#include <string.h>

//Attempt to figure out 7 byte ack payload

/* UPDATE THESE FOR YOUR PARTICULAR BOARD */
#define IS_RFM69HW false //make true if using w version
#define IRQ_PIN 6 // This is 3 on adafruit feather
#define CSN 10 // This is 8 on adafruit feather
/* END THINGS YOU MUST UPDATE */


iClickerEmulator clicker(CSN, IRQ_PIN, digitalPinToInterrupt(IRQ_PIN), IS_RFM69HW);
volatile bool inited = false;
volatile int count = 0;
iClickerResponsePacket_t mask;

uint8_t id[ICLICKER_ID_LEN];

void setup()
{
    while(!Serial);
    Serial.begin(115200);
    clicker.begin(iClickerChannels::AA);
    // enter promiscouse mode on sending channel
    //clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
    delay(1000);

    iClickerEmulator::randomId(id);
    Serial.print("Using ID: ");
    printID(id);
    Serial.println();


    clicker.submitAnswer( id, ANSWER_A, false, 100);
    clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
    clicker.stopPromiscuous();

}


void loop()
{
  clicker.submitAnswer( id, ANSWER_A, false, 100);
  clicker.startPromiscuous(CHANNEL_SEND, recvPacketHandler);
  delay(500); //wait for response
  clicker.stopPromiscuous();

  // print mask
  Serial.print("Count: ");
  Serial.print(count);
  Serial.print(", ");
  if (inited) {
    for (int i=0; i < 7; i++)
      Serial.print(mask.unknown[i], HEX);
      Serial.print(" ");
  }
  Serial.println();

}


void recvPacketHandler(iClickerPacket_t *recvd)
{
    if (recvd->type == PACKET_RESPONSE) {
      count++;
      iClickerResponsePacket_t resp = recvd->packet.respPacket;

      if (inited) { //xnor it
        for (int i=0; i < 7; i++) {
          mask.unknown[i] = ~(resp.unknown[i]^mask.unknown[i]);
        }
      } else {
        //memcpy(mask.);
        mask = resp;
        inited = true;
      }
    }
}

void printID(uint8_t *id)
{
  char tmp[50];
  snprintf(tmp, sizeof(tmp), "(%02X, %02X, %02X, %02X)", id[0], id[1], id[2], id[3]);
  Serial.print(tmp);
}
