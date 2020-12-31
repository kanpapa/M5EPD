#include "M5EPD.h"
#include <WiFi.h>
#include "FS.h"

#define USE_SD
//#define MOO_TEST

char temStr[10];
char humStr[10];

float tem;
float hum;

// Clock
char yyStr[4];
char moStr[4];
char ddStr[4];
char hhStr[4];
char mmStr[4];
char ssStr[4];
struct tm timeInfo;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 9*3600;
const int   daylightOffset_sec = 3600;

int disp_flag = 0;

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.SHT30.Begin();
    //M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);

#ifdef USE_SD
    // Load font files from SD Card
    //canvas.loadFont("/ipaexg.ttf", SD);
    //canvas.loadFont("/Let_s_go_Digital_Regular.ttf", SD);
    //canvas.loadFont("/GenSenRounded-R.ttf", SD); // Load font files from SD Card
    canvas.loadFont("/CascadiaCode-Bold.ttf", SD); // Load font files from SD Card
#endif
    
    // Wi-Fi
    WiFi.begin("wifi_ssid", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      
    canvas.createCanvas(960, 540);

#ifdef MOO_TEST
    disp_moo();
#endif

}

void disp_moo()
{
    canvas.setTextSize(3);
    //canvas.drawJpgUrl("https://m5stack.oss-cn-shenzhen.aliyuncs.com/image/example_pic/flower.jpg");
    canvas.drawJpgUrl("http://192.168.100.251/IMG_0756.jpg");
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
}

void loop()
{
    //
    if(!getLocalTime(&timeInfo)){
      Serial.println("Failed to obtain time");
    }

    canvas.setTextSize(36);
    canvas.createRender(36, 256);
    sprintf(yyStr, "%04d", timeInfo.tm_year + 1900);
    sprintf(moStr, "%02d", timeInfo.tm_mon + 1);
    sprintf(ddStr, "%02d", timeInfo.tm_mday);
    canvas.drawString(String(yyStr) + "-" + String(moStr) + "-" + String(ddStr), 180, 320);

    canvas.setTextSize(54);
    canvas.createRender(54, 256);
    sprintf(hhStr, "%02d", timeInfo.tm_hour);
    sprintf(mmStr, "%02d", timeInfo.tm_min);
    sprintf(ssStr, "%02d", timeInfo.tm_sec);
    canvas.drawString(String(hhStr) + ":" + String(mmStr) + ":" + String(ssStr), 140, 370);

    canvas.setTextSize(32);
    canvas.createRender(32, 256);
    M5.SHT30.UpdateData();
    tem = M5.SHT30.GetTemperature();
    hum = M5.SHT30.GetRelHumidity();
    Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%\r\n", tem, hum);
    dtostrf(tem, 2, 2 , temStr);
    dtostrf(hum, 2, 2 , humStr);
    canvas.drawString("Temperatura: " + String(temStr) + "*C", 72, 452);
    canvas.drawString("Humedad: " + String(humStr) + "%" , 72, 490);

    canvas.pushCanvas(0,0,UPDATE_MODE_A2);
    //canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    
    if(timeInfo.tm_min == 00 && disp_flag == 0) {
      disp_moo();
      disp_flag = 1;
    }
    
    delay(400);
}
