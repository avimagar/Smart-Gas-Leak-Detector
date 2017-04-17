#include <SoftwareSerial.h>
SoftwareSerial SIM900(7,8);

int sensor=12;
int speaker=13;
int gas_value,Gas_alert_val, Gas_shut_val;
int Gas_Leak_Status;
int sms_count=0;
int sensorValue;
int flg=0;
int GasSensorPin = A0;
String s2;
void setup()
{

    pinMode(sensor,INPUT);
    SIM900.begin(9600);   
    Serial.begin(9600);
    s2=show(); 
    delay(500);

}

void loop()
{
    CheckGas();
    CheckShutDown();
}

void CheckGas()
{

    sensorValue = ScanGasLevel();       // read analog input pin 0
    Serial.println(sensorValue, DEC);  // prints the value read
    delay(100); // wait 100ms for next 
    if(sensorValue>800)
    {
      
      SetAlert();
      flg=1;
    }
}

int ScanGasLevel()
{
     sensorValue = analogRead(GasSensorPin); // read analog input pin 0
     Serial.println(sensorValue, DEC);  // prints the value read
     delay(100); // wait 100ms for next reading
     return sensorValue; // returns temperature value in degree celsius
}

void SetAlert()
{
    tone(13,1000,10000);
    while(sms_count<1&&flg==0) //Number of SMS Alerts to be sent
    {  
        SendTextMessage(); // Function to send AT Commands to GSM module
    }
    Gas_Leak_Status=1; 
}

void CheckShutDown()
{
    if(Gas_Leak_Status==1)
    {
    
      Gas_shut_val=ScanGasLevel();                                                   //scan the gas level
      if(Gas_shut_val<800)
      {
        noTone(13);                                                                  //stop the buzzer
        sms_count=0;
        Gas_Leak_Status=0;
      }
    }
}


void SendTextMessage()
{
  SIM900.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(1000);
  String str=show();
  //SIM900.println("ATD+ +917350617969;");
  SIM900.println("AT+CMGS=\"+917350617969\"\r"); // change to the phone number you using 
  delay(1000);
  
  SIM900.println("Leak Detected!!!\n location:"+s2);//the content of the message
  delay(200);
  SIM900.println((char)26);//the stopping character
  delay(1000);
  delay(1000);
  sms_count++;
}



int i=0;
String pass;
char *token;
const char s[2]=",";
char a[800],fin[30];
String loc[3];



void ShowSerialData()
{
  
  while(SIM900.available()!=0)
  {
    //Serial.write(char (SIM900.read()));
     a[i]=char (SIM900.read());
     Serial.print(a[i++]);
     
  }

  
}


String show()
{


  SIM900.println("AT+CSQ"); // Signal quality check

  delay(100);

 
  ShowSerialData();// this code is to show the data from gprs shield, in order to easily see the process of how the gprs shield submit a http request, and the following is for this purpose too.
  
  SIM900.println("AT+CGATT?"); //Attach or Detach from GPRS Support
  delay(100);
 
 
  ShowSerialData();
  SIM900.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(1000);
 
  ShowSerialData();
 
  SIM900.println("AT+SAPBR=3,1,\"APN\",\"CMNET\"");//setting the APN, Access point name string
  delay(4000);
  ShowSerialData();
 
  SIM900.println("AT+SAPBR=1,1");//setting the SAPBR

  delay(2000);

 
  ShowSerialData();

  SIM900.println("AT+SAPBR=2,1");//setting the SAPBR
  delay(2000);
 
  ShowSerialData();

  SIM900.println("AT+CIPGSMLOC=1,1");//setting the SAPBR

  delay(2000);
  
  ShowSerialData();
  
  SIM900.println("AT+CIPGSMLOC=2,1");//setting the SAPBR
  
  delay(2000);
  
  ShowSerialData();


  SIM900.println("AT+SAPBR=0,1");//Deactivating the SAPBR
  delay(2000);
 
  ShowSerialData();

 


  int j=0;
   for(i=200;i<222;++i)
   {
        fin[j]=a[i];j++;
   }
   String str(fin);
   token=strtok(fin, s);
   int x=0;
   while(token!=NULL)
   {
      if(x<3)
        loc[x]=token;
      x++;
      token=strtok(NULL,s);
    }
   
   pass= "http://google.co.in/maps/@";
   pass.concat(loc[2]);
   pass.concat(",");
   pass.concat(loc[1]);
   pass.concat(",15z");
   //Serial.print(pass);
   return pass;
}