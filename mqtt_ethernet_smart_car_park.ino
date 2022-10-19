/***************************************************
  Adafruit MQTT Library Ethernet Example

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Alec Moore
  Derived from the code written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Dns.h>
#include <Dhcp.h>

/************************* Ethernet Client Setup *****************************/
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

//Uncomment the following, and set to a valid ip if you don't have dhcp available.
//IPAddress iotIP (192, 168, 0, 42);
//Uncomment the following, and set to your preference if you don't have automatic dns.
//IPAddress dnsIP (8, 8, 8, 8);
//If you uncommented either of the above lines, make sure to change "Ethernet.begin(mac)" to "Ethernet.begin(mac, iotIP)" or "Ethernet.begin(mac, iotIP, dnsIP)"


/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "raslanriz"
#define AIO_KEY         "aio_Lwse47SYlBTP1I50R2oZInp9YHbN"


/************ Global State (you don't need to change this!) ******************/

//Set up the ethernet client
EthernetClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }


/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish IOT_car_park = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/IOT_car_park");
Adafruit_MQTT_Publish Slot2 = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/Slot2");
Adafruit_MQTT_Publish Entrance_Gate = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/Entrance_Gate");
Adafruit_MQTT_Publish Exit_Gate = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/Exit_Gate");
Adafruit_MQTT_Publish slot_full = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/slot_full");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/
Servo servoEntrance; // Define our Entrance Servo
Servo servoExit; // Define our Exit Servo


//IR Slot 1
int slot1 = 3;
//Led Slot 1
int slot1led = 8;

//IR Slot 2
int slot2 = 7;
//Led Slot 1
//int slot2led = A0;

//Trigpin
int trigpin = 4;

//UltraSonic Entrance
int echopinentrance = 2;

//Exit Gate
int echopinexit = 9;

int distance;
float duration;
float cm;



void setup() {
  

    servoEntrance.attach(5); // servo exit on digital pin 5
    servoExit.attach(6); // servo exit on digital pin 6

    //Trigpin
    pinMode(trigpin, OUTPUT);

    //Entrance Gate
    pinMode(echopinentrance, INPUT); 

    //Exit Gate
    pinMode(echopinexit, INPUT);

    //IR Slot 1
    pinMode(slot1led, OUTPUT);
    pinMode(slot1, INPUT);

    //pinMode(slot2, INPUT);

    //IR Slot 2
    pinMode(A0, OUTPUT);
    pinMode(slot2, INPUT);
    
    Serial.begin(9600);

    
    Serial.println(F("Adafruit MQTT demo"));

    // Initialise the Client
    Serial.print(F("\nInit the Client..."));
    Ethernet.begin(mac);
    delay(1000); //give the ethernet a second to initialize
  

    mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;
uint32_t ir_sen_val = 0;
uint32_t ir_sen_val2 = 0;

uint32_t slotfull = 0;


uint32_t Entrance_Gate_Check = 0;
uint32_t Exit_Gate_Check = 0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }

  // Now we can publish stuff!
  Serial.print(F("\nSending IR  val "));
  Serial.print(x);
  Serial.print("...");

   ///==

      //Gate Entrance Code Start
      digitalWrite(trigpin, LOW);
      delay(2);  
      digitalWrite(trigpin, HIGH);
      delayMicroseconds(5);  
      digitalWrite(trigpin, LOW);  
      duration = pulseIn(echopinentrance, HIGH);  
      cm = (duration/5.82);
      distance = cm;
      
      if(digitalRead(3)== LOW && digitalRead(7)== LOW)
      {
        servoEntrance.write(90);
        digitalWrite(2,LOW);
        Entrance_Gate_Check = 1;
        slotfull = 1;
      }
       else if (distance<60)
      {
        servoEntrance.write(30);  // Turn Servo back to center position (90 degrees)
        delay(50);
        Entrance_Gate_Check = 0;
        slotfull = 0;
        
      }
       else{
         servoEntrance.write(90);
         delay(10);
         Entrance_Gate_Check = 1;
       }
   
   //Gate Entrance Code End

   //Gate Exit Code Start
    digitalWrite(trigpin, LOW);
    delay(2);  
    digitalWrite(trigpin, HIGH);
    delayMicroseconds(5);  
    digitalWrite(trigpin, LOW);  
    duration = pulseIn(echopinexit, HIGH);  
    cm = (duration/5.82);
    distance = cm;
    
    if(distance<50)
    {
      servoExit.write(30);  // Turn Servo back to center position (90 degrees)
      delay(50);
      Exit_Gate_Check = 0; 
    }
     else{
       servoExit.write(90);
       delay(50);
       Exit_Gate_Check = 1;
     }
   //Gate Exit Code End


      
      //IR Sensor Slot 1 Code Starts Here
      
      if (digitalRead(slot1)== LOW)
      {
        
        digitalWrite(slot1led,HIGH);
        ir_sen_val = 0;
        delay(100);     
         
      }
      else 
      { 
              
        digitalWrite(slot1led,LOW);
        ir_sen_val = 1;
        delay(100);
               
      }


      //IR Sensor Slot 2 Code Starts Here
      if (digitalRead(slot2)== LOW)
      {
        
        digitalWrite(A0,HIGH);
        ir_sen_val2 = 0;
        delay(100);     
         
      }
      else 
      { 
              
        digitalWrite(A0,LOW);
        ir_sen_val2 = 1;
        delay(100);
               
      }


    
    
    //Entrance Gate Publish
    if (! Entrance_Gate.publish(Entrance_Gate_Check)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F(" Entrance Gate OK!"));    
  }

  //Exit Gate Publish
    if (! Exit_Gate.publish(Exit_Gate_Check)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Exit Gate OK!"));    
  }
        

  //IR Slot 1 Publish
  if (! IOT_car_park.publish(ir_sen_val)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Slot 1 OK!"));    
  }

  //IR Slot 2 Publish
  if (! Slot2.publish(ir_sen_val2)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Slot 2 OK!"));    
  }

  //Slot Full Publish
  if (! slot_full.publish(slotfull)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("Slot Full OK!"));    
  }


  

  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

  delay(10000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
