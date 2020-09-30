#include <ArduinoJson.h>
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h> // Touchscreen library

//   D0 connects to digital pin 8  
//   D1 connects to digital pin 9   
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// Touchscreen pins 
#define YP A1  // must be an analog pin
#define XM A2  // must be an analog pin
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// Touchscreen calibration values
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // sensitivity = 300

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define MINPRESSURE 10
#define MAXPRESSURE 1000  // touchscreen presure

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Colors stock 
//********************
typedef enum 
{
  BLACK =     0x0000,
  BLUE  =     0x001F,
  RED   =     0xF800,
  GREEN =     0x07E0,
  CYAN  =     0x07FF,
  MAGENTA =   0xF81F,
  YELLOW =    0xFFE0,
  WHITE =     0xFFFF,
  ORANGE =    0xFD40,
  LIGHTGRAY = 0xAD55, 
} Color;
//********************

// Manipulation keybord input data variables
//*******************************************
uint16_t width = tft.width();
uint16_t height = tft.height();

String Keys[4][4] = 
{
  { "1", "2", "3", "4" },
  { "5", "6", "7", "8" },
  { "9", "0", "-", "#" },
  { "U", "C", "OK", "_" }
};

String CurrentKey, AcumKeys, ShowKey;
bool updata = false;
byte KeyInc = 0;
bool InterfaceUpdate = false;

char *lowerAlphabet = "abcdefghijklmnopqrstuvwxyz";
char *upperAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char  AlphabetIterator;
bool  upperKey = true;
unsigned long startTime, currentTime;

#define DELAY_BETWEEN_KEYS 310 // ms delay between keys

String buff;
char a_to_ptr;
const char *str;
//*******************************************


// Data for sending to esp
//*******************************************
bool done_ssid_input = false, done_ssid_pass = false;

bool ReadMsg = false;
String Msg;
char* showip;

DynamicJsonDocument doc(256);
//*******************************************

// Main menu handlings variables
//*******************************************
byte currentPage = 0; // null page, no page was selected
char *ssidName;
bool relay1 = false, relay2 = false, relay3 = false, relay4 = false;
TSPoint p;
//bool update_MsgRead = false;

void setup() 
{
  Serial.begin(115200); // begin serial

  tft.reset(); // reset the lcd
  tft.begin(0x9325); // tft begin with driver ID
  tft.setRotation(0); // rotation default

  LoadingProgressBar(WHITE, "Loading OS..", RED);

  //done_ssid_pass = true;
  
}

void loop() 
{
  //SETUP_INTERFACE:
  if(done_ssid_pass == false)
  {
      if(InterfaceUpdate == false)
      {
        tft.fillScreen(BLACK);
        tft.setCursor(1, 10);
        tft.setTextSize(2);
        tft.setTextColor(ORANGE);
        tft.println("WIFI SSID Name:");
    
        tft.fillRect(0, 120, 240, 240, WHITE);

        for(int i = 120; i <= 320; i+= 50) // for optimization 60 bytes saved
        {
          tft.drawFastHLine(0, i, 240, BLACK);
        }
  
        for(byte i = 0; i < 5; i++) // for optimization 60 bytes saved
        {
          tft.drawFastVLine(i * 60, 80, 240, BLACK);
        }

        for (byte x = 0; x < 4; x++) 
        {
          for(byte y = 0; y < 4; y++) 
          { 
            switch(x)
            {
              case 0:
              {
                tft.setCursor(25 + (60 * y), 140 + (65 * x));
                break;
              }
              case 1:
              {
                tft.setCursor(25 + (60 * y), 135 + (55 * x));
                break;
              }
              case 2:
              {
                tft.setCursor(25 + (60 * y), 130 + (55 * x));
                break;
              }
              case 3:
              {
                tft.setCursor(25 + (60 * y), 125 + (55 * x));
                break;
              }
            }

            tft.setTextSize(2);
            tft.setTextColor(BLACK);
            tft.println(Keys[x][y]);
          }
        }
        InterfaceUpdate = true;
      }
      
    p = waitTouch(false);
  
    updata = false;
  
    for(byte x = 0; x < 4; x++) 
    {
      for(byte y = 0; y < 4; y++) 
      {
        if((p.y >= 120 + (x * 50) && p.y <= 120 + (x+1) * 50) && (p.x >= (y*50)+1 && p.x <= ((y+1)*50)-1)) 
        {
            if(Keys[x][y] == CurrentKey && !((Keys[x][y] == "-") || (Keys[x][y] == "#") || (Keys[x][y] == "U") || (Keys[x][y] == "C") || (Keys[x][y] == "OK") || (Keys[x][y] == "_")))
            { 
              KeyInc++;
          
              buff = Keys[x][y];
              str = buff.c_str();
              a_to_ptr = *str;
            
              switch(a_to_ptr)
              {
                case '2':
                {        
                  if(KeyInc < 4)
                  {   
                      if(upperKey)
                        AlphabetIterator = *upperAlphabet + KeyInc - 1;
                      else
                        AlphabetIterator = *lowerAlphabet + KeyInc - 1; 
                      
                      ShowKey = AlphabetIterator;
                  } 
                  else { KeyInc = 0; ShowKey = Keys[x][y];}
                  break;
                }
                case '3':
                {  
                  if(KeyInc < 4)
                  {
                      if(upperKey)
                        AlphabetIterator = *upperAlphabet + KeyInc - 1 + 3;
                      else
                        AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 3;
                       
                      ShowKey = AlphabetIterator;
                  } 
                  else { KeyInc = 0; ShowKey = Keys[x][y];}
                  break;
                }
                case '4':
                {                
                  if(KeyInc < 4)
                  {
                      if(upperKey)
                        AlphabetIterator = *upperAlphabet + KeyInc - 1 + 6;
                      else
                        AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 6;
                      
                    ShowKey = AlphabetIterator;
                  }
                  else { KeyInc = 0; ShowKey = Keys[x][y]; }
                  break;
               }
               case '5':
               {
                   
                 if(KeyInc < 4)
                 {
                     if(upperKey)
                       AlphabetIterator = *upperAlphabet + KeyInc - 1 + 9;
                     else
                       AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 9;

                   ShowKey = AlphabetIterator;
                 }
                 else { KeyInc = 0; ShowKey = Keys[x][y]; }
                 break;
               }
               case '6':
               {
                 if(KeyInc < 4)
                 {
                     if(upperKey)
                       AlphabetIterator = *upperAlphabet + KeyInc - 1 + 12;
                     else
                       AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 12;
                      
                   ShowKey = AlphabetIterator;
                 }
                 else { KeyInc = 0; ShowKey = Keys[x][y]; }
                 break;
               }
               case '7':
               {
                 if(KeyInc < 5)
                 {
                   if(upperKey)
                       AlphabetIterator = *upperAlphabet + KeyInc - 1 + 15;
                     else
                       AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 15;
                      
                   ShowKey = AlphabetIterator;
                 }
                 else { KeyInc = 0; ShowKey = Keys[x][y]; }
                 break;
               }
               case '8':
               {
                 if(KeyInc < 4)
                 {
                     if(upperKey)
                       AlphabetIterator = *upperAlphabet + KeyInc - 1 + 19;
                     else
                       AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 19;
                      
                   ShowKey = AlphabetIterator;
                 }
                 else { KeyInc = 0; ShowKey = Keys[x][y]; }
                 break;
               }
               case '9':
               {
                 if(KeyInc < 5)
                 {
                    if(upperKey)
                       AlphabetIterator = *upperAlphabet + KeyInc - 1 + 22;
                     else
                       AlphabetIterator = *lowerAlphabet + KeyInc - 1 + 22;

                   ShowKey = AlphabetIterator;
                 }
                 else { KeyInc = 0; ShowKey = Keys[x][y]; }
                 break;
               }
             }
             if(currentTime - startTime > DELAY_BETWEEN_KEYS)
             {
               KeyInc = 0;
               AcumKeys += ShowKey;
             }
             else if(AcumKeys.length() == 0)
                 AcumKeys = ShowKey;
             else
             {
                AcumKeys.remove(AcumKeys.length() - 1);
                AcumKeys += ShowKey;
             }
           }
           else
           {
             if(!((Keys[x][y] == "-") || (Keys[x][y] == "#") || (Keys[x][y] == "U") || (Keys[x][y] == "C") || (Keys[x][y] == "OK") || (Keys[x][y] == "_")))
             {
               KeyInc = 0;
               CurrentKey = Keys[x][y];
               AcumKeys += CurrentKey;
               ShowKey = AcumKeys;
             }
             else
             {
               buff = Keys[x][y];
               str = buff.c_str();
               a_to_ptr = *str;

               if(Keys[x][y] == "OK" && done_ssid_input == false)
               {
                 done_ssid_input = true;
                 String strid = ShowKey;
                 doc["ssid"] = strid;
                 ssidName = (char *)malloc(strid.length() + 1);
                 memcpy(ssidName, strid.c_str(), strid.length() + 1);
                 KeyInc = 0;
                 CurrentKey = "";
                 AcumKeys = "";
                 ShowKey = "";
               }
               else if(Keys[x][y] == "OK" && done_ssid_input == true && done_ssid_pass == false)
               {
                 done_ssid_pass = true;
                 doc["pass"] = ShowKey;
                 KeyInc = 0;
                 CurrentKey = "";
                 AcumKeys = "";
                 ShowKey = "";

                 doc["type"] = "GiveWIFIdata";
                 serializeJson(doc, Serial);

                 doc.clear();
                 
                 while(ReadMsg == false)
                 {
                    if(Serial.available())
                    {
                      Msg = Serial.readString();
                      ReadMsg = true;
                    }
                 }
                 DeserializationError error = deserializeJson(doc, Msg);

                 if(error)
                 {
                    //Serial.print(F("deserializeJson Failed "));
                    //Serial.println(error.c_str());
                    ReadMsg = false;
                    return;
                 }

                 if(doc["type"] == "ReturnIP")
                 {
                    showip = doc["IP"];
                    doc.clear();
                 }
                 ReadMsg = false;
               }
            
               switch(a_to_ptr)
               {
                 case 'C':
                {
                    KeyInc = 0;
                    CurrentKey = "";
                    AcumKeys = "";
                    ShowKey = "";
                    break;
                }
                case '_':
                {
                    KeyInc = 0;
                    AcumKeys += "_"; // add in the list _ character
                    break;
                   
                }
                case '-':
                {
                  if(AcumKeys.length() != 0)
                  {
                    KeyInc = 0;
                  
                    AcumKeys.remove(AcumKeys.length() - 1); // delete last char
                    ShowKey = AcumKeys;
                    break;
                  }
                }
                case 'U':
                {
                  KeyInc = 0;
                  upperKey = !upperKey;
                  break;
                }
              }
            } 
          }
          ShowKey = AcumKeys;
          updata = true;
       }
     }
  }
  
   if (updata) 
   {
      tft.fillRect(0, 0, 240, 80, BLACK);
    
      tft.setCursor(1, 10);
      tft.setTextSize(2);
      tft.setTextColor(ORANGE);
    
      if(done_ssid_input == false)
      {
        tft.println("WIFI SSID Name:");
      }
      else
      {
        tft.println("WIFI SSID Password:");
      }
    
      tft.setCursor(1, 30);
      tft.setTextSize(2);
      tft.setTextColor(WHITE);
      tft.println(ShowKey);
   }
   
   delay(300);
  }
  else
  {
    if(currentPage == 0)
    {
      DrawHomeTitle(BLACK, ORANGE, BLACK ,"Main Menu", BLACK, "", false, width / 3.9, 10);

      tft.fillRoundRect(20, 60, 200, 60, 8, WHITE); // Show IP
      tft.drawRoundRect(20, 60, 200, 60, 8, ORANGE);
      tft.setCursor(30, 80);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
      tft.println("Show IP Address");

      tft.fillRoundRect(20, 150, 200, 60, 8, WHITE); // Relay interface
      tft.drawRoundRect(20, 150, 200, 60, 8, ORANGE);
      tft.setCursor(30, 170);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
      tft.println("Reley Control UI");

      tft.fillRoundRect(20, 240, 200, 60, 8, WHITE); // wi-fi reconnection
      tft.drawRoundRect(20, 240, 200, 60, 8, ORANGE);
      tft.setCursor(30, 260);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
      tft.println("WIFI Reconection");
    }                 

      p = waitTouch(currentPage == 2 ? true : false);
   
    
    if(((p.x >= 20 && p.x <= 220) && (p.y >= 60 && p.y <= 120)) && currentPage == 0)
    {
      currentPage = 1;
    }
    if(((p.x >= 20 && p.x <= 220) && (p.y >= 150 && p.y <= 210)) && currentPage == 0)
    {
      currentPage = 2;
    }
    if(((p.x >= 20 && p.x <= 220) && (p.y >= 240 && p.y <= 300)) && currentPage == 0)
    {
        if(showip == NULL || strcmp(showip, "Failed") == 0)
        {
          currentPage = 0; // after retrying to reconect the main page will be 0
          done_ssid_input = false; // allow to start again the setup interface
          done_ssid_pass = false; // allow return to setup interface
          InterfaceUpdate = false; // allow for interface to update
          free(ssidName); // delete from memory the allocation cells
        }
    }

    if(currentPage == 1)
    {
        tft.fillScreen(BLACK);

        tft.fillRoundRect(1, 1, 60, 30, 8, ORANGE);
        tft.setCursor(20, 11);
        tft.setTextSize(1);
        tft.setTextColor(BLACK);
        tft.println("Home");
      
        if(showip == NULL)
        {
          tft.setCursor(1, 120);
          tft.setTextSize(2);
          tft.setTextColor(RED);
          tft.println("Communication error, the system will restart..");
          delay(700);
          done_ssid_input = false;
          done_ssid_pass = false;
          InterfaceUpdate = false;
          free(ssidName); // delete from memory the allocation cells
        }
        else if(strcmp(showip, "Failed") == 0)
        {
          tft.setCursor(1, 120);
          tft.setTextSize(2);
          tft.setTextColor(RED);
          tft.println("The system could't connect to ssid :");
          tft.setTextColor(WHITE);
          tft.println(ssidName);
        }

        if(strcmp(showip, "Failed") != 0 && showip != NULL)
        {
          tft.setCursor(1, 110);
          tft.setTextSize(2);
          tft.setTextColor(WHITE);
          tft.println("Your device IP is :");
          tft.setTextColor(ORANGE);
          tft.println(showip);
        }

        if(p.x >= 0 && p.x <= 60 && p.y >= 0 && p.y <= 30)
        {
          currentPage = 0;
        }
      }
      else if(currentPage == 2)
      {
        tft.fillScreen(BLACK);

        tft.fillRoundRect(1, 1, 60, 30, 8, ORANGE);
        tft.setCursor(20, 11);
        tft.setTextSize(1);
        tft.setTextColor(BLACK);
        tft.println("Home");
        
        tft.fillRoundRect(1, 80, 100, 60, 8, relay1 ? ORANGE : WHITE); 
        tft.drawRoundRect(1, 80, 100, 60, 8, ORANGE);
        tft.setCursor(10, 100);
        tft.setTextSize(2);
        tft.setTextColor(BLACK);
        tft.println("Relay 1");

        tft.fillRoundRect(120, 80, 110, 60, 8, relay2 ? ORANGE : WHITE);
        tft.drawRoundRect(120, 80, 110, 60, 8, ORANGE);
        tft.setCursor(140, 100);
        tft.setTextSize(2);
        tft.setTextColor(BLACK);
        tft.println("Relay 2");

        tft.fillRoundRect(1, 180, 100, 60, 8, relay3 ? ORANGE : WHITE);
        tft.drawRoundRect(1, 180, 100, 60, 8, ORANGE);
        tft.setCursor(10, 200);
        tft.setTextSize(2);
        tft.setTextColor(BLACK);
        tft.println("Relay 3");

        tft.fillRoundRect(120, 180, 110, 60, 8, relay4 ? ORANGE : WHITE);
        tft.drawRoundRect(120, 180, 110, 60, 8, ORANGE);
        tft.setCursor(140, 200);
        tft.setTextSize(2);
        tft.setTextColor(BLACK);
        tft.println("Relay 4");

        while(Serial.available() > 0)
        {
            Msg = Serial.readString();
            ReadMsg = true; // received data true
        }

        if(ReadMsg)
        {
          DeserializationError error = deserializeJson(doc, Msg);

          if(error)
          {
            //Serial.print(F("deserialization Failed: "));
            //Serial.println(error.c_str());
            ReadMsg = false;
            return;
          }
          
          if(doc["Relay1"] == "ON")
          {
              relay1 = false;
              goto RELAY1;
          }
          else if(doc["Relay1"] == "OFF")
          {
              relay1 = true;
              goto RELAY1;
          }

          if(doc["Relay2"] == "ON")
          {
              relay2 = false;
              goto RELAY2;
          }
          else if(doc["Relay2"] == "OFF")
          {
              relay2 = true;
              goto RELAY2;
          }

          if(doc["Relay3"] == "ON")
          {
              relay3 = false;
              goto RELAY3;
          }
          else if(doc["Relay3"] == "OFF")
          {
              relay3 = true;
              goto RELAY3;
          }

          if(doc["Relay4"] == "ON")
          {
              relay4 = false;
              goto RELAY4;
          }
          else if(doc["Relay4"] == "OFF")
          {
              relay3 = true;
              goto RELAY4;
          }
            
        }
        
        if(p.x >= 1 && p.x <= 100 && p.y >= 80 && p.y <= 140) // relay 1
        {
          RELAY1:
          if(relay1 == false)
          {
             relay1 = true;
             tft.fillRoundRect(1, 80, 100, 60, 8, ORANGE);
             doc["Relay1"] = "ON";
          }
          else
          {
             relay1 = false;
             tft.fillRoundRect(1, 80, 100, 60, 8, WHITE);
             doc["Relay1"] = "OFF";
          }
          
          tft.setCursor(10, 100);
          tft.setTextSize(2);
          tft.setTextColor(BLACK);
          tft.println("Relay 1");

          if(!ReadMsg)
          serializeJson(doc, Serial);
        }
        if(p.x >= 120 && p.x <= 220 && p.y >= 80 && p.y <= 140) // relay 2
        {
          RELAY2:
          if(relay2 == false)
          {
            relay2 = true;
            tft.fillRoundRect(120, 80, 110, 60, 8, ORANGE);
            doc["Relay2"] = "ON";
          }
          else
          {
            relay2 = false;
            tft.fillRoundRect(120, 80, 110, 60, 8, WHITE);
            doc["Relay2"] = "OFF";
          }

          tft.setCursor(140, 100);
          tft.setTextSize(2);
          tft.setTextColor(BLACK);
          tft.println("Relay 2");

          if(!ReadMsg)
          serializeJson(doc, Serial);
        }
        if(p.x >= 1 && p.x <= 100 && p.y >= 180 && p.y <= 240) // relay 3
        {
          RELAY3:
          if(relay3 == false)
          {
            relay3 = true;
            tft.fillRoundRect(1, 180, 100, 60, 8, ORANGE);
            doc["Relay3"] = "ON";
          }
          else
          {
            relay3 = false;
            tft.fillRoundRect(1, 180, 100, 60, 8, WHITE);
            doc["Relay3"] = "OFF";
          }

          tft.setCursor(10, 200);
          tft.setTextSize(2);
          tft.setTextColor(BLACK);
          tft.println("Relay 3");
          
          if(!ReadMsg)
          serializeJson(doc, Serial);
          //else ReadMsg = false;
        }
        if(p.x >= 120 && p.x <= 220 && p.y >= 180 && p.y <= 240) // relay 4
        {
          RELAY4:
          if(relay4 == false)
          {
            relay4 = true;
            tft.fillRoundRect(120, 180, 110, 60, 8, ORANGE);
            doc["Relay4"] = "ON";
          }
          else
          {
            relay4 = false;
            tft.fillRoundRect(120, 180, 110, 60, 8, WHITE);
            doc["Relay4"] = "OFF";
          }

          tft.setCursor(140, 200);
          tft.setTextSize(2);
          tft.setTextColor(BLACK);
          tft.println("Relay 4");

          if(!ReadMsg)
          serializeJson(doc, Serial);
          //else ReadMsg = false;
       }

       if(p.x >= 0 && p.x <= 60 && p.y >= 0 && p.y <= 30)
       {
         currentPage = 0;
       }

       if(ReadMsg)
       ReadMsg = false;

       doc.clear();
       
     }
    
  }
}

void DrawHomeTitle(Color screenColor, Color rectColor, Color HeaderTextColor,String HeaderText, Color DescriptionColor, String DescriptionText, bool WantDescText, byte x_pos, byte y_pos)
{
  tft.fillScreen(screenColor);
  tft.fillRect(1, 1, width, height - 290, rectColor);

  tft.setCursor (x_pos, y_pos); // center text
  tft.setTextSize(2);
  tft.setTextColor(HeaderTextColor);
  tft.println(HeaderText);

  if(WantDescText)
  {
    tft.setCursor(width - (width - 1), 40);
    tft.setTextSize(1);
    tft.setTextColor(DescriptionColor);
    tft.println(DescriptionText);
  }
}

void LoadingProgressBar(Color TextColor, String Text, Color ProgressBarColor)
{
   tft.fillScreen(BLACK);
   tft.setCursor(10, 110);
   tft.setTextSize(3);
   tft.setTextColor(WHITE);
   tft.println(Text);

   for(byte i = 1; i < 239; i++)
   {
     tft.fillRect(1, 160, i, 10, ProgressBarColor);
     delayMicroseconds(10);
   }
}

TSPoint waitTouch(bool CheckSerial) 
{
  TSPoint p;

  startTime = millis();
  do 
  {
    if(CheckSerial)
    {
      if(Serial.available() > 0)
      break;
    }
    
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));

  currentTime = millis();
  
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
  
  return p;
}



