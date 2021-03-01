//Used Library
#include <NTPClient.h> // Date/Time manager 
#include <WiFi.h>
#include <WiFiUdp.h>                          // Library to send and receive UDP messages
#include <WiFiClient.h>                       // driver for WiFi client
#include <ArduinoJson.h>                      // Arduino Json to parse reauest into JSON object. Installed version 5.13, last version is not compatible.
#include <Adafruit_GFX.h>                     // LCD graphical driver
#include <TFT_eSPI.h>                         // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <SPIFFS.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel rgb(1, 16); 

#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F

#define summer          rgb.setPixelColor(0, 255, 255, 0);    // yellow
#define storm           rgb.setPixelColor(0, 0, 0, 0);        // black
#define lightrain       rgb.setPixelColor(0, 153, 153, 255);  // light blue
#define rain            rgb.setPixelColor(0, 0, 0, 255);      // blue
#define snow            rgb.setPixelColor(0, 255, 255, 255);  // white
#define fog             rgb.setPixelColor(0, 192, 192, 192);  // grey
// ----------------------------------------------------------------------------------------

// wifi ssid and password (hardcoded for the moment)
// @TODO: MAKE ssid and password NOT hardcoded?
#ifndef APSSID
#define APSSID "INSERT YOUR NETWORK NAME HERE" //Network name
#define APPSK  "INSERT YOUR NETWORK PASSWORD HERE" //password
#endif
/* Set these to your desired credentials. */
char *ssid = APSSID;
char *password = APPSK;

// ----------------------------------------------------------------------------------------

// APIKEY is a passtoken used to identify the request from data to "pool.ntp.org
String APIKEY = "INSERT YOUR OPENWEATHER API KEY HERE"; // API Key from OpenWeather
// @TODO: Make CityID configurable
String CityID = "INSERT YOUR CITY ID HERE"; // e.g Semenyih 1734798
// @TODO: Make Timezone configurable
int TimeZone = 1;// GMT +1
// @TODO: Make utcOffsetInSeconds configurable
const long utcOffsetInSeconds = INSERT YOUR TIME OFFSET HERE; // e.g 28800 Malaysia GMT +8
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Define NTP Client to get time and date
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;    // WIFI Client
char servername[]="api.openweathermap.org";    // remote server we will connect to
String result;
const char MyCity[] = "INSERT YOUR CITY NAME HERE"; // city name that will display on screen
// ----------------------------------------------------------------------------------------
// set of variables used into this sketch for different pourpose
boolean   night = false;
String    timeS = "";
String    day = "";
int       weatherID = 0;
String    location = "";
String    temperature = "";
String    weather = "";
String    description = "";
String    idString = "";
String    umidityPer = "";
float     Fltemperature = 0;
int       counter = 30;
String    windS = "";
String    pressure = "";
String    Visibility ="";
String    Wind_angle = "";
String    Clouds = "";


TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


#define ILI9341_DRIVER

#define ESP32_PARALLEL

  //Please edit User_setup.h file like this and  comment out other options or othervise it will not work!!

  // The ESP32 and TFT the pins used for testing are:
  #define TFT_CS   33  // Chip select control pin (library pulls permanently low)
  #define TFT_DC   15  // Data Command control pin - must use a pin in the range 0-31
  #define TFT_RST  32  // Reset pin, toggles on startup

  #define TFT_WR    4  // Write strobe control pin - must use a pin in the range 0-31
  #define TFT_RD    2  // Read strobe control pin

  #define TFT_D0   12  // Must use pins in the range 0-31 for the data bus
  #define TFT_D1   13  // so a single register write sets/clears all bits.
  #define TFT_D2   26  // Pins can be randomly assigned, this does not affect
  #define TFT_D3   25  // TFT screen update performance.
  #define TFT_D4   17
  #define TFT_D5   16
  #define TFT_D6   27
  #define TFT_D7   14

// =======================================================================================
// S E T U P
// =======================================================================================
void setup() {

  rgb.begin();
  Serial.begin(115200);
  SPIFFS.begin();
  listFiles();

  tft.init();
  tft.setRotation(0);  // portrait 
  tft.setTextSize(1);
   tft.fillScreen(TFT_WHITE); 

   #ifdef FORMAT_SPIFFS
    tft.drawString("Formatting SPIFFS, so wait!", 120, 195); SPIFFS.format();
  #endif
  
  // wifi connection
  //drawWifi();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED ){
    delay(500);
    Serial.print(".");
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(20,20);
    tft.print ( "Connecting to:" );
    tft.setCursor(20,45);
    tft.print ( ssid );
  }
  IPAddress myIP = WiFi.localIP();
  tft.setCursor(20,75);
  tft.print ( "Connected!" );
  tft.setCursor(20,30);
  tft.print ( myIP );
  // client to get date and time
  // @TODO: implement better this step with https://github.com/scanlime/esp8266-Arduino/blob/master/tests/Time/Time.ino
  timeClient.begin();
  delay (2000);
}
/*====================================================================================
  This sketch contains support functions for the ESP6266 SPIFFS filing system

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

// -------------------------------------------------------------------------
// List SPIFFS files in a neat format for ESP8266 or ESP32
// -------------------------------------------------------------------------
void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

#ifdef ESP32
  listDir(SPIFFS, "/", true);
#else
  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    // File path can be 31 characters maximum in SPIFFS
    int spaces = 33 - fileName.length(); // Tabulate nicely
    if (spaces < 1) spaces = 1;
    while (spaces--) Serial.print(" ");
    fs::File f = dir.openFile("r");
    Serial.print(f.size()); Serial.println(" bytes");
    yield();
  }

  Serial.println(line);
#endif
  Serial.println();
  delay(1000);
}

#ifdef ESP32
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 8 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }
}
#endif
//====================================================================================
// =======================================================================================
// L O O P
// =======================================================================================
void loop() {
  
  if(counter == 30) {//Get new data every 30 cycles (1 cycles = 60 sec)
    counter = 10;
    getWeatherData();
  }else{
    displayData();
    counter++;
    Serial.println(counter); 
  } 
  
  //get current time
  timeS = getTime();
  //get current day
  day = getDay();
  //to define if we are in night or day (to display moon or sun)
  nightOrDay (timeS);
 
}
// =======================================================================================

// get current date
String getDate(){
  timeClient.update(); 
}

// =======================================================================================

// get current time
String getTime(){
  timeClient.update();
  String timeS = timeClient.getFormattedTime();
  int length = timeS.length();
  return timeS.substring(length-8,length-3);
}

// =======================================================================================

// get current day
String getDay(){
  timeClient.update();
  return daysOfTheWeek[timeClient.getDay()];
}

// =======================================================================================

// get Weather data from openweathermap.org
// sent request for data
void getWeatherData(){ //client function to send/receive GET request data. 
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  // reading sent data
  while(client.connected() && !client.available()) delay(1); //waits for data
  Serial.println("Waiting for data");
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result+c;
  }

  // replacing character '['
  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  
  Serial.println(result);

  // format received data into a jsonArray.
  // to make this code working it has been becessary to install version 
  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);
  if (!root.success()){
    Serial.println("parseObject() failed");
  }
  
  //TODO : try to understand why this double assignement is necessary
  String temperatureLOC = root["main"]["temp"];
  String weatherLOC = root["weather"]["main"];
  String descriptionLOC = root["weather"]["description"];
  String idStringLOC = root["weather"]["id"];
  String umidityPerLOC = root["main"]["humidity"];
  String windLOC = root["wind"]["speed"];
  String pressureLOC = root["main"]["pressure"];
  String visibilityLOC= root["visibility"];
  String wind_angleLOC = root["wind"]["deg"];
  String cloudsLOC = root ["clouds"]["all"] ;//["main"] 

  temperature = temperatureLOC;
  weather = weatherLOC;
  description = descriptionLOC;
  idString = idStringLOC;
  umidityPer = umidityPerLOC;
  windS = windLOC;
  pressure= pressureLOC;
  Visibility = visibilityLOC;
  Wind_angle = wind_angleLOC;
  Clouds = cloudsLOC;
  
  int length = temperature.length();
  if(length==5){
    temperature.remove(length-3);
  }

  Fltemperature = temperature.toFloat();
  Fltemperature = Fltemperature - 273,15;


  weatherID = idString.toInt();
}

// =======================================================================================

//Display data on the LCD screen
//Une loop every 60 seconds

void displayData(){
  printGeneral(MyCity, timeS, day, weatherID, description, Fltemperature, umidityPer);
  delay (60000);
  //printWeather(MyCity, timeS, day, weatherID, description);
  //delay (pause);
  printTemperature(MyCity, timeS, day, Fltemperature);
  delay (1000);
  printUmidity(MyCity, timeS, day, umidityPer);
  delay (1000);
  printWind(MyCity, timeS, day, windS);
  delay (1000);
  printVISIBILITY(MyCity, timeS, day,  Visibility);
  delay (1000);
  printairpressure(MyCity, timeS, day,  pressure);
  delay (1000);
  printwindangle(MyCity, timeS, day,  Wind_angle);
  delay (1000);
  printclouds(MyCity, timeS, day,  Clouds);
  delay (1000);

  
}

// =======================================================================================
// Print Home page with all details
void printGeneral(String city, String timeS, String day, int weatherID, String description, float temperature, String umidity){
  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  printWeatherIcon(weatherID);
  printrgb(weatherID);
  rgb.show();

  tft.setCursor(10,130);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(description);
  
}

// =======================================================================================
// Print Weather with icon
void printWeather(String city, String timeS, String day, int weatherID, String description) {
  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20); 
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  printWeatherIcon(weatherID);
 // drawopenweather();
  //drawday();

  tft.setCursor(35,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(description);
}

// =======================================================================================
// Print temperature display
void printTemperature(String city, String timeS, String day, float temperature){

  tft.fillScreen(TFT_WHITE);
 
  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawThermometer();
  //drawopenweather();
  //drawday();

  tft.setCursor(40,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(temperature);
  tft.print("°C");
}

// =======================================================================================
// Print umidity display
void printUmidity(String city, String timeS, String day, String umidity){

  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawUmidity();
 // drawopenweather();
  //drawday();

  tft.setCursor(40,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(umidity);
  tft.print(" RH%");
}

// =======================================================================================
// Print wind display
void printWind(String city, String timeS, String day, String wind){
 
  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawWind();
 // drawopenweather();
 // drawday();

  tft.setCursor(40,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(wind);
  tft.print("m/s");
}



void printVISIBILITY(String city, String timeS, String day, String visibilityLOC){
 
  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawvisibility();
 // drawopenweather();
 // drawday();

  tft.setCursor(40,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(visibilityLOC);
  tft.print("M");
}

void printairpressure(String city, String timeS, String day, String pressure){

  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawairpressure();
 // drawopenweather();
 // drawday();

  tft.setCursor(35,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(pressure);
  tft.print(" Hpa");
}

void printwindangle(String city, String timeS, String day, String Wind_angle){
 
  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawwindangle();
//  drawopenweather();
 // drawday();

  tft.setCursor(60,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(Wind_angle);
  tft.print("°");

}

void printclouds(String city, String timeS, String day, String Clouds){
 
  tft.fillScreen(TFT_WHITE);

  tft.setCursor(40,5);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(city);

  tft.setCursor(10,20);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.print(timeS + ' ' + day);

  drawclouds();
//  drawopenweather();
  //drawday();

  tft.setCursor(50,140);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(1);
  tft.print(Clouds);
  tft.print(" %");
}


// Bodmers BMP image rendering function

void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
     // Serial.print("Loaded in "); Serial.print(millis() - startTime);
     // Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}





// =======================================================================================

// Print WeatherIcon based on id
void printrgb(int id) {
 switch(id) {  //Summer /  Coud
  case 800: summer ; break;
  case 801: summer ; break;
  case 802: summer ; break;
  case 803: summer ; break;;
  case 804: summer ; break;
              //storm 
  case 200: storm ; break;
  case 201: storm ; break;
  case 202: storm ; break;
  case 210: storm ; break;
  case 211: storm ; break;
  case 212: storm ; break;
  case 221: storm ; break;
  case 230: storm ; break;
  case 231: storm ; break;
  case 232: storm ; break;
            //light rain
  case 300: lightrain ; break;
  case 301: lightrain ; break;
  case 302: lightrain ; break;
  case 310: lightrain ; break;
  case 311: lightrain ; break;
  case 312: lightrain ; break;
  case 313: lightrain ; break;
  case 314: lightrain ; break;
  case 321: lightrain ; break;
          // rain
  case 500: rain ; break;
  case 501: rain ; break;
  case 502: rain ; break;
  case 503: rain ; break;
  case 504: rain ; break;
  case 511: rain ; break;
  case 520: rain ; break;
  case 521: rain ; break;
  case 522: rain ; break;
  case 531: rain ; break;
          // snow
  case 600: snow ; break;
  case 601: snow ; break;
  case 602: snow ; break;
  case 611: snow ; break;
  case 612: snow ; break;
  case 615: snow ; break;
  case 616: snow ; break;
  case 620: snow ; break;
  case 621: snow ; break;
  case 622: snow ; break;
          // fog
  case 701: fog ; break;
  case 711: fog ; break;
  case 721: fog ; break;
  case 731: fog ; break;
  case 741: fog ; break;
  case 751: fog ; break;
  case 761: fog ; break;
  case 762: fog ; break;
  case 771: fog ; break;
  case 781: fog ; break;
  default:break; 
  }
}

  void printWeatherIcon(int id) {
 switch(id) {
  case 800: drawBmp("/summer.bmp",15, 35); break;
  case 801: drawBmp("/cloud.bmp",15, 35); break;
  case 802: drawBmp("/cloud.bmp",15, 35); break;
  case 803: drawBmp("/cloudy.bmp",15, 35); break;
  case 804: drawBmp("/cloudy.bmp",15, 35); break;
  
  case 200: drawBmp("/storm.bmp",15, 35); break;
  case 201: drawBmp("/storm.bmp",15, 35); break;
  case 202: drawBmp("/storm.bmp",15, 35); break;
  case 210: drawBmp("/storm.bmp",15, 35); break;
  case 211: drawBmp("/storm.bmp",15, 35); break;
  case 212: drawBmp("/storm.bmp",15, 35); break;
  case 221: drawBmp("/storm.bmp",15, 35); break;
  case 230: drawBmp("/storm.bmp",15, 35); break;
  case 231: drawBmp("/storm.bmp",15, 35); break;
  case 232: drawBmp("/storm.bmp",15, 35); break;

  case 300: drawBmp("/rain2.bmp",15, 35); break;
  case 301: drawBmp("/rain2.bmp",15, 35); break;
  case 302: drawBmp("/rain2.bmp",15, 35); break;
  case 310: drawBmp("/rain2.bmp",15, 35); break;
  case 311: drawBmp("/rain2.bmp",15, 35); break;
  case 312: drawBmp("/rain2.bmp",15, 35); break;
  case 313: drawBmp("/rain2.bmp",15, 35); break;
  case 314: drawBmp("/rain2.bmp",15, 35); break;
  case 321: drawBmp("/rain2.bmp",15, 35); break;

  case 500: drawBmp("/rain.bmp",15, 35); break;
  case 501: drawBmp("/rain.bmp",15, 35); break;
  case 502: drawBmp("/rain.bmp",15, 35); break;
  case 503: drawBmp("/storm.bmp",15, 35); break;
  case 504: drawBmp("/storm.bmp",15, 35); break;
  case 511: drawBmp("/rain.bmp",15, 35); break;
  case 520: drawBmp("/rain.bmp",15, 35); break;
  case 521: drawBmp("/rain.bmp",15, 35); break;
  case 522: drawBmp("/rain.bmp",15, 35); break;
  case 531: drawBmp("/rain.bmp",15, 35); break;

  case 600: drawBmp("/snow.bmp",15, 35); break;
  case 601: drawBmp("/snow.bmp",15, 35); break;
  case 602: drawBmp("/snow.bmp",15, 35); break;
  case 611: drawBmp("/sleet.bmp",15, 35); break;
  case 612: drawBmp("/sleet.bmp",15, 35); break;
  case 615: drawBmp("/sleet.bmp",15, 35); break;
  case 616: drawBmp("/snow.bmp",15, 35); break;
  case 620: drawBmp("/snow.bmp",15, 35); break;
  case 621: drawBmp("/snow.bmp",15, 35); break;
  case 622: drawBmp("/snow.bmp",15, 35); break;

  case 701: drawBmp("/fog.bmp",15, 35); break;
  case 711: drawBmp("/fog.bmp",15, 35); break;
  case 721: drawBmp("/fog.bmp",15, 35); break;
  case 731: drawBmp("/fog.bmp",15, 35); break;
  case 741: drawBmp("/fog.bmp",15, 35); break;
  case 751: drawBmp("/fog.bmp",15, 35); break;
  case 761: drawBmp("/fog.bmp",15, 35); break;
  case 762: drawBmp("/fog.bmp",15, 35); break;
  case 771: drawBmp("/fog.bmp",15, 35); break;
  case 781: drawBmp("/fog.bmp",15, 35); break;
  default:break; 
  }
}

// =======================================================================================
// To switch between day/night icon
void nightOrDay(String timeS) {
  timeS = timeS.substring(0,2);
  int time = timeS.toInt();
  Serial.print ( "====" );
  Serial.print ( time );
  if(time > 20 ||  time<7) {
 night = true;
 tft.invertDisplay(true);}
 
 else {
 night = false;
 tft.invertDisplay(false);
  }
}

// =======================================================================================
// Clear the screen

void clearScreen() {
    tft.fillScreen(TFT_WHITE);
}

void drawClearWeather(){
  if(night){
    drawBmp("/moon.bmp",15, 35);
  }else{
    drawBmp("/summer.bmp",15, 35);
  }
}

void drawFewClouds(){
  if(night){
    drawBmp("/night.bmp",15, 35);
  }else{
    drawBmp("/cloud.bmp",15, 35);
  }
}

void drawTheSun(){
  drawBmp("/summer.bmp",15, 35);
}

void drawTheFullMoon(){
  drawBmp("/moon.bmp",15, 35);
}

void drawTheMoon(){
  drawBmp("/moon.bmp",15, 35);
  
}

void drawCloud(){
  drawBmp("/cloud_sky.bmp",15, 35);
}

void drawThermometer(){
  drawBmp("/heat.bmp",15, 35);
}

void drawUmidity(){
  drawBmp("/humidity.bmp",15, 35);
}

void drawWifi(){
  drawBmp("/wifi_logo.bmp",0, 0);
}

void drawCloudWithSun(){
  
  drawBmp("/cloud.bmp",15, 35);
}

void drawLightRainWithSunOrMoon(){
  if(night){  
    drawBmp("/moon_rain.bmp",15, 35);
  }else{
    drawBmp("/rain.bmp",15, 35);
  }
}

void drawLightRain(){
  drawBmp("/rain.bmp",15, 35);
}

void drawModerateRain(){
  drawBmp("/rain.bmp",15, 35);
}

void drawHeavyRain(){
   drawBmp("/rain.bmp",15, 35);
}

void drawThunderstorm(){
   drawBmp("/storm.bmp",15, 35);
}

void drawLightSnowfall(){
   drawBmp("/snow.bmp",15, 35);
}

void drawModerateSnowfall(){
   drawBmp("/snow.bmp",15, 35);
}

void drawHeavySnowfall(){
   drawBmp("/snow.bmp",15, 35);
}

void drawCloudSunAndRain(){
   drawBmp("/rain.bmp",15, 35);
}

void drawCloudAndTheMoon(){
   drawBmp("/night.bmp",15, 35);
}

void drawCloudTheMoonAndRain(){
   drawBmp("/moon_rain.bmp",15, 35);
}

void drawWind(){  
  drawBmp("/wind.bmp",15, 35); 
}

void drawFog()  {
   drawBmp("/fog.bmp",15, 35);
}

void clearIcon(){
   drawBmp("/summer.bmp", 15, 35);
}
void drawtemp_min(){
  drawBmp("/heat.bmp", 15, 35);
}
void drawtemp_max(){
  drawBmp("/heat.bmp", 15, 35);
}
void drawvisibility(){
  drawBmp("/mountain.bmp", 15, 35);
}
void drawairpressure(){
  drawBmp("/air_pressure.bmp", 15, 35);
}
void drawwindangle(){
  drawBmp("/weathercock.bmp", 15, 35);
}
void drawclouds(){
  drawBmp("/cloud_sky.bmp", 15, 35);
}
void drawsleet(){
  drawBmp("/sleet.bmp", 15, 35);
  }

  /*
void drawopenweather(){
  drawBmp("/openweather_logo.bmp", 7,230);    
}

void drawday(){
  if(night){
    drawBmp("/night_moon.bmp",0,0);
  }else{
    drawBmp("/day.bmp",0, 0);
  }
  
}
*/
