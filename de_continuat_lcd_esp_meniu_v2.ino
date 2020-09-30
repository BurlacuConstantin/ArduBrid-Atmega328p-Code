#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h> // Touchscreen library

//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

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
  BLACK =   0x0000,
  BLUE  =   0x001F,
  RED   =   0xF800,
  GREEN =   0x07E0,
  CYAN  =   0x07FF,
  MAGENTA = 0xF81F,
  YELLOW =  0xFFE0,
  WHITE =   0xFFFF,
  ORANGE =  0xFD40,
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
  { "9", "0", "DW", "UP" },
  { "U", "C", "OK", "_" }
};

String Key, ShowKey;
bool updata = false;
byte KeyInc = 0;

char *lowerAlphabet = "abcdefghijklmnopqrstuvwxyz";
char *upperAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char  AlphabetIterator;
String AcumKey;

String buff;
char a_to_ptr;
//*******************************************

void setup() 
{
  Serial.begin(9600); // begin serial

  tft.reset(); // reset the lcd
  tft.begin(0x9325); // tft begin with driver ID
  tft.setRotation(0); // rotation default

  
  DrawHomeTitle(BLACK, ORANGE, BLACK,"Welcome", RED, 
  "- This is an smart multi-relay control system, this project was designed by    Burlacu Constantin 2020\n - Press the touchscreen to continue\n - Software Version 1.0"
  , true);

  DrawStartSimbol(ORANGE, BLACK, "START", RED);

  WaitOneScreenTouch();
  

  tft.fillScreen(BLACK);

  
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

  tft.setCursor(1, 10);
  tft.setTextSize(2);
  tft.setTextColor(ORANGE);
  tft.println("WIFI SSID Name:");

}

void loop() 
{
  TSPoint p = waitTouch();
  
  updata = false;
  
  for(byte x = 0; x < 4; x++) 
  {
    for(byte y = 0; y < 4; y++) 
    {
      if((p.y >= 120 + (x * 50) && p.y <= 120 + (x+1) * 50) && (p.x >= (y*50)+1 && p.x <= ((y+1)*50)-1)) 
      {
          if(Keys[x][y] == Key && !((Keys[x][y] == "DW") || (Keys[x][y] == "UP") || (Keys[x][y] == "U") || (Keys[x][y] == "C") || (Keys[x][y] == "OK") || (Keys[x][y] == "_")))
          {
            KeyInc++;

          
            buff = Keys[x][y];
            const char *str = buff.c_str();
            a_to_ptr = *str;
            
            Serial.print(a_to_ptr);
            Serial.print("  ");
            Serial.println(KeyInc);
            
            switch(a_to_ptr)
            {
              case '2':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1;
                  AcumKey = AlphabetIterator;
                } 
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '3':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 3;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '4':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 6;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '5':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 9;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '6':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 12;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '7':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 15;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '8':
              {
                if(KeyInc < 4)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 18;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
              case '9':
              {
                if(KeyInc < 5)
                {
                  AlphabetIterator = *upperAlphabet + KeyInc - 1  + 21;
                  AcumKey = AlphabetIterator;
                }
                else { KeyInc = 0; AcumKey = Keys[x][y]; }
                break;
              }
            }
            ShowKey += AcumKey;
          }
          else
          {
            if(!((Keys[x][y] == "DW") || (Keys[x][y] == "UP") || (Keys[x][y] == "U") || (Keys[x][y] == "C") || (Keys[x][y] == "OK") || (Keys[x][y] == "_")))
            {
              KeyInc = 0;
              Key += Keys[x][y];
              ShowKey += Key;
            }
            else
            {
              buff = Keys[x][y];
              const char *str = buff.c_str();
              a_to_ptr = *str;
            
              switch(a_to_ptr)
              {
                case 'C':
                {
                    KeyInc = 0;
                    Key = "";
                    ShowKey = "";
                    break;
                }
              }
            }
          }
          
          
        /*
        if ((x<=2 && y<=2)||(x==3 && y==1)) 
        {
            N1 = N1 + Keys[x][y];
            ShowSC = N1;
        } 
        else 
        {
            N2 = N2 + Keys[x][y];
            ShowSC =  N2;
          }
        } 
        */
        //}
        /*
        else 
        {
          if(Keys[x][y]=="C") 
          {
            N1 = N2 = "";
            opt = "";
            answers = 0;
            ShowSC = N1;
          } 
          else if (y==3) 
          {
            if (N1=="") N1 = String(answers);
            opt = Keys[x][y];
            ShowSC = Keys[x][y];
          } 
          else if (Keys[x][y]=="=") 
          {
            if (opt=="+") answers = N1.toInt() + N2.toInt();
            else if (opt=="-") answers = N1.toInt() - N2.toInt();
            else if (opt=="*") answers = N1.toInt() * N2.toInt();
            else if (opt=="/") answers = N1.toInt() / N2.toInt();
            N1 = N2 = opt = "";
            ShowSC = answers;
          }
        }
        */
        Serial.println(ShowKey);
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
    tft.println("WIFI SSID Name:");
    tft.setCursor(1, 30);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.println(ShowKey);
  }
  delay(300);
}

void DrawHomeTitle(Color screenColor, Color rectColor, Color HeaderTextColor,String HeaderText, Color DescriptionColor, String DescriptionText, bool WantDescText)
{
  tft.fillScreen(screenColor);
  tft.fillRect(1, 1, width, height - 290, rectColor);

  tft.setCursor (width / 3, 10); // center text
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

void DrawStartSimbol(Color PrimaryCircleColor, Color SecondaryCircleColor, String CircleText, Color TextColor)
{
  tft.fillCircle(width / 2, height / 1.5, 80, PrimaryCircleColor);
  tft.fillCircle(width / 2, height / 1.5, 75, SecondaryCircleColor);

  tft.setCursor(width / 3, height / 1.6);
  tft.setTextSize(3);
  tft.setTextColor(TextColor);
  tft.println(CircleText);
}

TSPoint WaitOneScreenTouch() 
{
  // wait 1 touch to exit function
  TSPoint p;
  
  do 
  {
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT); //Pins configures again for TFT control
    pinMode(YP, OUTPUT);
  
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  
  return p;
}

TSPoint waitTouch() 
{
  TSPoint p;
  
  do 
  {
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);
  
  return p;
}



