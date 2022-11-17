
#include <Adafruit_Fingerprint.h>
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#define LED 11  
#define LED2 12

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(8, 9);
#else
#define mySerial Serial1
#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

BridgeServer server;
int motion=0;
int buzzer = 2;
String isMotion="0";
String isGas="0";
int smokeA5 = A5;
int flameA0=A0;
String isFlame="0";
int result=0;
int ledstate = 0;
String isLocked="1";
//Gas threshold
int sensorThres = 400;
int flameThres = 200;
void setup() {
  
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Bridge.begin();
  pinMode(smokeA5, INPUT);

  
  server.listenOnLocalhost();
  server.begin();
  finger.begin(57600);

}

void loop() {
  // Get clients coming from server
  BridgeClient client = server.accept();
  // There is a new client?
  if (client) {
    // Process request
    process(client);

    // Close connection and free resources.
    client.stop();
  }
getFingerprintID();
  delay(50); // Poll every 50ms
}


//http process start
void process(BridgeClient client) {  
  String command = client.readStringUntil('/'); 
  if(command == "motion"){
    motionAlarm(client);
    }
   if(command == "readsensors"){
      readSensors(client);
    }  

   if(command == "lights"){
    lightsCommand(client);
    }

    if(command=="lock"){
      lock(client);
      }
}
//http process end

//Client sensor communication begin
void readSensors(BridgeClient client){     
  gasSensor();
  flameSensor();  
  if(isGas=="1")
  client.print("1");
  else if(isFlame=="2")
  client.print("2");
  else
  client.print("0");
     
}
//Client sensor communication end



//Gas sensor start
void gasSensor(){
  int analogSensor = analogRead(smokeA5);   
  
  if (analogSensor > sensorThres)
  {   
    isGas="1";      
  }
  else {
    isGas="0";
    } 
   
  }

//Gas sensor end

void flameSensor(){
    int flameReading= analogRead(A0);
    if(flameReading<flameThres){
      isFlame="2";
      }
    else{
      isFlame="0";
      }

     
  }

void lightsCommand(BridgeClient client){
  int pin;
  pin=client.parseInt();
  lightsSrv(pin); 

  
  
  }

void lightsSrv(int ledNo){
   ledstate = digitalRead(ledNo);
  if (ledstate == LOW)
  {
    digitalWrite(ledNo, HIGH);
  } 
  else{
    digitalWrite(ledNo, LOW);
  }
  
  }

 void motionAlarm(BridgeClient client){
 int code =1327;
 int clientCode=client.parseInt();
int  val=digitalRead(7);
  if(val==HIGH){
    isMotion="1";
   if(motion="0"){
    motion=1;    
    buzzerP();
    }
    }
   else{    
    if(clientCode==code){
      motion=0;
      isMotion="0";
      client.print("ok");
      noTone(buzzer);
      }
      
    }

    

  client.print(isMotion);
 
  
 }

 void buzzerT(){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(100);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
}

 void buzzerP(){
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(100);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  tone(buzzer,800);  
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    buzzerT();
    if(isLocked=="1")
    isLocked="0";
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void lock(BridgeClient client){
  if(isLocked=="0"){
    isLocked="1";
     
  }
    else{
    isLocked="0";    
    }

   client.print(isLocked);
  }




 

  
