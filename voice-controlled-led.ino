#define SSID "ENTER_YOU_OWN"        //type your own SSID name
#define PASSWORD "ENTER_YOUR_OWN"        //type your own WIFI password
#define SERIAL_BUFFER_SIZE 256

#include "uartWIFI.h"
#include <SoftwareSerial.h>
WIFI wifi;
extern int chlID;        //client id(0-4)
int Send_Debug_EN = 0,Send_Con_EN=0; 

// Pin 13 will be used for the long lead of the LED.
int led = 13;

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(4,OUTPUT);
  Serial.begin (9600);
  wifi.begin();
  bool b = wifi.Initialize(AP_STA, SSID, PASSWORD);
  if(!b)
  {
    Serial.println("Init error");
  }
  delay(1000); //make sure the module can have enough time to get an IP address 
  String ipstring = wifi.showIP();
  int ip_dress_1=ipstring.indexOf('"');
  int ip_dress_2=ipstring.indexOf('"',ip_dress_1 + 1);
  int ip_dress_3=ipstring.indexOf('"',ip_dress_2 + 40);
  int ip_dress_4=ipstring.indexOf('"',ip_dress_3 + 1);
  Serial.print("APIP:");                                             //show the ip address of module
  Serial.println(ipstring.substring(ip_dress_1,ip_dress_2+1));        
  Serial.print("STAIP:");             
  Serial.println(ipstring.substring(ip_dress_3,ip_dress_4+1));       
  
  delay(1000);
  wifi.confMux(1);
  delay(100);
  if(wifi.confServer(1,8089))
  Serial.println("Server is set up");
  Serial.println("Say `Turn on` to turn ON-LED and `Turn off` to Turn OFF-LED");
}

void loop()
{
  
  char buf[1000];
  int iLen = wifi.ReceiveMessage(buf);                       // Read the data received by wifi


  if(iLen > 0)
  {
    //delay (10);
    Serial.println (buf);
    if (buf[5]=='o'&&buf[6]=='n')                           // CH0 ON
    {      
        digitalWrite (4,LOW);
        Serial.println ("CH0 ON");
        Send_Debug_EN = 1;
        digitalWrite(led, HIGH);
        Serial.println("LED turned ON"); 
        Response ();
        delay (100);
    }
    if (buf[5]=='o'&&buf[6]=='f'&&buf[7]=='f')              // CH0 OFF
    {
        digitalWrite (4,HIGH);
        Serial.println ("CH0 OFF");
        Send_Debug_EN = 1;
        digitalWrite(led, LOW);
        Serial.println("LED turned OFF"); 
        Response ();
        delay (100); 
    }
    if (buf[5]=='c'&&buf[6]=='o'&&buf[7]=='n')              // CH0 OFF
    {
        Serial.println ("Connected");
        Send_Con_EN = 1;
        digitalWrite(led, LOW);
        Serial.println("LED turned OFF"); 
        Response ();
        delay (100); 
    }
  }
  delay (50);
} 

/***********************************************************
    Function of respond to phone's commands
***********************************************************/

void Response ()
{
    _cell.print("AT+CIPSEND=0,126\r\n");
    unsigned long start;
    bool found;
    start = millis();
    while (millis()-start<250) 
    {                            
        if(_cell.find(">")==true )
        {
          found = true;
          break;
        }
    }
     if(found)
     {        
        _cell.print("HTTP/1.1 200 OK\r\nDate: Sat, 31 Dec 2005 23:59:59 GMT\r\nContent-Type: text/html;charset=ISO-8859-1\r\nContent-Length: 7\r\n\r\n");
        if (Send_Debug_EN)
        {
          _cell.print("debugen");  
          Send_Debug_EN=0;
        }
        if (Send_Con_EN)
        {
          _cell.print("connect");  
          Send_Con_EN=0;
        }
        
        _cell.print("\r\n");
     }
     else
     {
        wifi.closeMux();
     }
        String data;
        start = millis();
        while (millis()-start<250) 
     {
         if(_cell.available()>0)
         {
           char a =_cell.read();
           data=data+a;
         }
         if (data.indexOf("SEND OK")!=-1)
         {
           Serial.println("Send OK");
           break;
         }
     }
     _cell.print("AT+CIPCLOSE=0\r\n"); 
}

