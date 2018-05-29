#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <GSM.h>

#define PINNUMBER ""
#define owner_num ""
String owner_sms = "";
int parked_latitude = 0;
int parked_longitude = 0;
int new_latitude = 0;
int new_longitude = 0;
String vehicle_status = "";
String response_sms = "";
// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 4800;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Function to check the gsm status 
void gsm_status(){
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  boolean notConnected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (notConnected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
 }
void send_location_by_sms(){
  String txtMsg = response_sms; //msg return by gps
  Serial.println("SENDING");
  Serial.println();
  Serial.println("Message:");
  Serial.println(txtMsg);

  // send the message
  sms.beginSMS(owner_num);
  sms.print(txtMsg);
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
}  
void get_owner_sms(){
  char c;

  // If there are any SMSs available()
  if (sms.available()) {
    Serial.println("Message received from:");

    // Get remote number
    sms.remoteNumber(owner_num, 20);
    Serial.println(owner_num);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#') {
      Serial.println("Discarded SMS");
      sms.flush();
    }

    // Read message bytes and print them
    while (c = sms.read()) {
      Serial.print(c);
      owner_sms = owner_sms.concat(c);
    }
    Serial.println("\nEND OF MESSAGE");

    // Delete message from modem memory
    sms.flush();
    Serial.println("MESSAGE DELETED");
  }

  delay(1000);
}//End of get_owner_sms method  
  
boolean set_vehicle_moving_status(){
  
   if(parked_latitude != new_latitude || parked_longitude != new_longitude){
      boolean vehicle_mov_status = true;
      return vehicle_mov_status; 
   }
} 

void check_vehicle_status(){
      vehicle_status = owner_sms; 
}



void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Serial.println(F("Tracking Device Project"));
  Serial.println(F("by Abhishek Kumar"));
  
  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  
  Serial.println();
}

void loop()
{  
  if(set_vehicle_moving_status()){
          if(vehicle_status.equalsIgnoreCase("parked")){
              send_location_by_sms();
              //code for buzzer on
          }
    }         
  if(owner_sms.equalsIgnoreCase("track")){
          send_location_by_sms();
              
    }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
   }
}

void display_location(){
  if(ss.available() > 0){
    
    if (gps.encode(ss.read())){
       Serial.print(F("Location: ")); 
       if (gps.location.isValid()){
            Serial.print(gps.location.lat(), 6);
            Serial.print(F(","));
            Serial.print(gps.location.lng(), 6);
            new_latitude = (gps.location.lat(), 6);
            new_longitude =(gps.location.lng(), 6);
            response_sms = "Latitude: " + new_latitude;
            response_sms = response_sms + " Longitude: " + new_longitude; 
       }
       else{
            Serial.print(F("INVALID"));
       }

       Serial.print(F("  Date/Time: "));
       if (gps.date.isValid()){
            Serial.print(gps.date.month());
            Serial.print(F("/"));
            Serial.print(gps.date.day());
            Serial.print(F("/"));
            Serial.print(gps.date.year());
            response_sms = response_sms +" Date: " +gps.date.month() +"/"+ gps.date.day()+"/"+gps.date.year();
       }
       else{
            Serial.print(F("INVALID"));
       }

      Serial.print(F(" "));
      if (gps.time.isValid()){
            if (gps.time.hour() < 10) Serial.print(F("0"));
            Serial.print(gps.time.hour());
            Serial.print(F(":"));
            if (gps.time.minute() < 10) Serial.print(F("0"));
            Serial.print(gps.time.minute());
            Serial.print(F(":"));
            if (gps.time.second() < 10) Serial.print(F("0"));
            Serial.print(gps.time.second());
            Serial.print(F("."));
            if (gps.time.centisecond() < 10) Serial.print(F("0"));
            Serial.print(gps.time.centisecond());
            response_sms = response_sms + "Time: "+gps.time.hour()+":"+gps.time.minute()+":"+gps.time.second();
       }
      else{
            Serial.print(F("INVALID"));
      }

      Serial.println();
    }
  }
 }//End of display location method