/*Importing the required Header*/
#include<SoftwareSerial.h>

/*Declaring the variables*/
int sensorValue = 0;  //Setting the initial value of LDR 
int data; //Data from OUTSIDE placed Sensor
int data1;  //Data from INSIDE Placed Sensor
boolean lock = true;  //random variable
int counter = 0;  //Counts the number of persons
int Light_status=10;  //It indicates the Bulb is OFF for Online Analytics
SoftwareSerial esp8266(3,4);  //Wifi module is connected to 3,4 ports/pins 
#define SSID "vishal" //your Wifi username
#define PASS "rex" //your Wifi password 

/*Making the Wifi Module connect to your Actual Wifi-Hotspot/Access point */ 
void connectwifi()
{
  sendAT("AT\r\n",1000);
  sendAT("AT+CWMODE=1\r\n",1000);
  sendAT("AT+CWJAP=\""SSID"\",\""PASS"\"\r\n",2000);  //passing username,password of wifi
  while(!esp8266.find("OK"))
    {
    }
  sendAT("AT+CIFSR\r\n",1000);
  sendAT("AT+CIPMUX=0\r\n",1000);
}

/*Standard Function for sending AT commands to the Wifi Module*/
String sendAT(String command,const int timeout)
{
  String response="";
  esp8266.print(command);
  long int time= millis();
  while((time + timeout) > millis())
  {
    while(esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  Serial.print(response);
  return response;
}

/*Function to send person count,light status to Wifi Module*/
void passTS(int c, int l) //c=counter,l=Light status
{
  String counter1 = String(c);
  Serial.print("Numbers of persons:");
  Serial.println(c);
  
  String Light_status1 = String(l);
  Serial.print("Light status:");
  Serial.println(l);
  updateTS(counter1,Light_status1); //Update the data to server
  delay(3000);  //wait for 3 sec
}

/*Function to send person count,light status to Thingspeak*/
void updateTS(String C, String L) //C=Counter,L=Light Status 
{
  Serial.print("");
  sendAT("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n",1000);  //Start a TCP connection with Thingspeak IP Address and Port 80
  delay(2000);  //wait for 2 sec
  String cmdlen;
  String cmd = "GET /update?key=3FIIQ6HFXO2R9SBR&field1="+C+"&field2="+L+"\r\n";  //key is the Channel ID,field 1 and field2 are values of Counter and Light status
  cmdlen = cmd.length(); 
  sendAT("AT+CIPSEND="+cmdlen+"\r\n",2000);
  esp8266.print(cmd); //publish to channel
  Serial.println("");
  sendAT("AT+CIPCLOSE\r\n",2000); //Close the connection
  Serial.println("");
  delay(1000);  //wait for 1 sec
}

/*Initialising the ports and the Modules*/ 
void setup() 
{ 
esp8266.begin(9600);  //Making the Wifi module to have baud rate of 115200
Serial.begin(9600); //Setting the Serial Communication of the Arduino Uno with baud rate of 9600
sendAT("AT+RST\r\n",2000);  //Resetting the Wifi Module
connectwifi();  //Connecting to Wifi
pinMode(5,INPUT);  //Outside placed Sensor as Input pin connected on 10th port/pin of Arduino
pinMode(6,INPUT);  //Inside placed Sensor as Input pin conected on 11th port/pin of Arduino
pinMode(7,OUTPUT);  //LED Bulb as Output pin connected on 8th port/pin of Arduino
pinMode(8,OUTPUT); //Buzzer as Output pin connected on 12th port/pin of Arduino
pinMode(13, OUTPUT);


Serial.print("Calibrating Sensor\n"); //Printing the message on the screen

 
/*function for printing 10 dots(".") on the screen to mention that the sensor is calibrating*/
for(int i=0;i<10;i++)
{
  Serial.print(".");
  delay(1000); //print the dots for every 1000ms for 10 times
}

Serial.print("Done"); //Calibrations is Completed
Serial.println("Sensor Active");  //Now sensors are Active 
digitalWrite(13,HIGH);
delay(5000);
digitalWrite(13,LOW); 
digitalWrite(7 ,HIGH);  //Buzzer ON
delay(2000);  //wait 2 sec or 2000 milli sec
digitalWrite(7,LOW); //Buzzer LOW
delay(50);  //wait 50 milli sec
} 

/*Anything placed in the loop will execute for infinite times*/
void loop() 
 
/*sensorValue = analogRead(A0); //LDR sensor connected to Analog Pin A0
delay(100); //wait for 100 milli sec
Serial.println(sensorValue);  //Print the LDR sensor reading

if(sensorValue>100) //If there is sufficient Lighting..in our case we consider 100 as minimum value to consider that there is some light*/

/*digitalWrite(8,LOW);  //Bulb is OFF*/
{
data=digitalRead(5); //data from Outside placed sensor
data1=digitalRead(6);  //data from Inside placed sensor

 if(data==HIGH&&lock==true) //If Outside PIR sensor is triggered.i.e, a person is entering the room from outside
 {
  lock=false;
  digitalWrite(7,HIGH);  //Buzzer ON
  delay(500); //wait for 500 milli sec
  digitalWrite(7,LOW); //Buzzer OFF
  Serial.print("Person IN");  //Person entered the room
  counter++;  //Add the Count of the person entered
    if(counter>0) //If there is a person in the room
    {
      digitalWrite(8,HIGH);
      Light_status=100;   
      passTS(counter,Light_status); //Send the person count,light status to Thingspeak 
    }
  delay(4000); //wait for 4 sec
  lock=true;
 }

 if(data1==HIGH&&lock==true)  //If Inside placed PIR sensor is triggered.i.e,the person is leaving the room
 {
  lock=false;
  digitalWrite(7,HIGH);  //Buzzer ON
  delay(500); //wait for 500 milli sec
  digitalWrite(7,LOW); //Buzzer OFF
  Serial.print("Person OUT"); //Person Left the room
  counter--;  //Decrease the Count of the person left
    if(counter==0) //If there is NO person in the room
    {digitalWrite(8,LOW);
       Light_status=10;
    }

 passTS(counter,Light_status); //Send the person count,light status to Thingspeak
 delay(4000);
 lock=true;
 }


  else{
  if(sensorValue<100)  //If there is NO sufficient Lighting..in our case we consider 100 as minimum value to consider that there is some light
{
  if(counter>=1)  //If there is a person in the room
  {
    digitalWrite(8,HIGH); //Bulb is ON
    Light_status=100; //Bulb is ON
  }
  
data=digitalRead(5); //data from Outside placed sensor
data1=digitalRead(6);  //data from Inside placed sensor
Serial.println(sensorValue);  //Print the LDR sensor reading
}
}
 if(data==HIGH&&lock==true) //If Outside placed PIR sensor is triggered.i.e, a person is entering the room from outside
 {
  lock=false;
  digitalWrite(7,HIGH);  //Buzzer ON
  delay(500); //wait for 500 milli sec
  digitalWrite(7,LOW); //Buzzer OFF
  Serial.print("Person IN");  //Person entered the room
  counter++;  //Add the Count of the person entered
    if(counter>=1)  //If there is a person in the room
    {
      digitalWrite(8,HIGH); //Bulb is ON
      Light_status=100; //indicating Bulb is ON
      passTS(counter,Light_status); //Send the person count,light status to Thingspeak
    }
 delay(4000); //wait for 4 sec
 lock=true;
 }

 if(data1==HIGH&&lock==true)  //If Inside placed PIR sensor is triggered.i.e,the person is leaving the room
 {
  lock=false;
  digitalWrite(7,HIGH);  //Buzzer ON
  delay(500); //wait for 500 milli sec
  digitalWrite(7,LOW); //Buzzer OFF
  Serial.print("Sensor OUT"); //Person left the room
  counter--;  //Decrease the Count of the person left
    if(counter<0) //If there is NO person in the room
    {
      counter=0;  //Set the counter to 0
    }

    if(counter==0)  //If there is NO person in the room
    {
      digitalWrite(8,LOW);  //Bulb is OFF
      Light_status=10;  //indicating Bulb is OFF
    }
  passTS(counter,Light_status); //Send the person count,light status to Thingspeak
  delay(4000);  //wait for 4 sec
  lock=true;
 }

}
