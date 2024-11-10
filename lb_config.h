/* Library ini memuat tentang configurasi setiap pin/GPIO/ Sensor yang digunakan pada learning board */

/* OLED CONFIG */
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* Timmer Potensio Config */ 
#define PotensioPin A0 
int trimpot_value = 0 ; 
int nValue = 0;   

/* LED Blink Config */ 
#define led_1 0 // GPIO 0 - D3 / GREEN
#define led_2 2 // GPIO 2 - D4 / RED >>  LED_BUILTIN

/* Button PIN */
#define BUTTON_NEXT_PIN 12      // GPIO 12 /D6 / Blue button  
#define BUTTON_SELECT_PIN 13    // GPIO 13 /D7 / Green Button 

/* Temperature Sensor Config */
#include "DHT.h"
#define DHTPIN D5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Tipe Sensor Suhu : DHT 11 
DHT dht(DHTPIN, DHTTYPE);
float hum, temp = 0; 

/*BUZZER PIN */
#define pin_buzzer 15  // GPIO 15 / D8 

/*WIFI */
IPAddress apIP(192, 168, 4, 1);  // Defining a static IP address: local & gateway
IPAddress apGateway(192,168,4,1);
IPAddress apSubnet(255, 255, 255, 0);  // SUBNET MASK 

 

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "n00bwork";
const char *password = "1234567890";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);
 
/* Menu variable */

const int NUM_ITEMS = 6; // number of items in the list and also the number of screenshots and screenshots with QR codes (other screens)
const int MAX_ITEM_LENGTH = 20; // maximum characters for the item name

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  // array with item names
  { "SENSOR" }, 
  { "WIFI" }, 
  { "LED" }, 
  { "LDR" }, 
  { "BEEP" }, 
  { "OLED" }
 };

/* Button Variable */
int button_next_clicked = 0; // only perform action when button is clicked, and wait until another press
int button_select_clicked = 0; // same as above
 
int item_selected = 0; // which item in the menu is selected

int item_sel_previous; // previous item - used in the menu screen to draw the item before the selected one
int item_sel_next; // next item - used in the menu screen to draw next item after the selected one

/* Display Variable */
int current_screen = 0;   // 0 = menu, 1 = Modul  
int current_menu = 0 ;    // Menu 
int page_status = 1 ;


/* ICON */

const unsigned char wifi2_icon1[] PROGMEM = {
              
  0b00000000, 0b00000000, //                 
  0b00000111, 0b11100000, //      ######     
  0b00001111, 0b11110000, //     ########    
  0b00011000, 0b00011000, //    ##      ##   
  0b00000011, 0b11000000, //       ####      
  0b00000111, 0b11100000, //      ######     
  0b00000100, 0b00100000, //      #    #     
  0b00000001, 0b10000000, //        ##       
  0b00000001, 0b10000000, //        ##       
  0b00000000, 0b00000000, //                 
} ;

const unsigned char temp_icon[] =
{
  0b00000001, 0b11000000, //        ###      
  0b00000011, 0b11100000, //       #####     
  0b00000111, 0b00100000, //      ###  #     
  0b00000111, 0b11100000, //      ######     
  0b00000111, 0b00100000, //      ###  #     
  0b00000111, 0b11100000, //      ######     
  0b00000111, 0b00100000, //      ###  #     
  0b00000111, 0b11100000, //      ######     
  0b00000111, 0b00100000, //      ###  #     
  0b00001111, 0b11110000, //     ########    
  0b00011111, 0b11111000, //    ##########   
  0b00011111, 0b11111000, //    ##########   
  0b00011111, 0b11111000, //    ##########   
  0b00011111, 0b11111000, //    ##########   
  0b00001111, 0b11110000, //     ########    
  0b00000111, 0b11100000, //      ######     
};

const unsigned char bulb_on_icon16x16[] =
{
  0b00000000, 0b00000000, //                 
  0b00000011, 0b11100000, //       #####     
  0b00000111, 0b11110000, //      #######    
  0b00001111, 0b11111000, //     #########   
  0b00011111, 0b11111100, //    ###########  
  0b00011111, 0b11111100, //    ###########  
  0b00011111, 0b11111100, //    ###########  
  0b00011111, 0b11111100, //    ###########  
  0b00011111, 0b11111100, //    ###########  
  0b00001111, 0b11111000, //     #########   
  0b00000100, 0b00010000, //      #     #    
  0b00000011, 0b11100000, //       #####     
  0b00000010, 0b00100000, //       #   #     
  0b00000011, 0b11100000, //       #####     
  0b00000010, 0b00100000, //       #   #     
  0b00000011, 0b11100000, //       #####     
};

// 'icon_3dcube', 16x16px
const unsigned char bitmap_icon_3dcube [] PROGMEM = {
  0x00, 0x00, 0x01, 0x80, 0x07, 0x60, 0x19, 0x18, 0x61, 0x06, 0x51, 0x0a, 0x45, 0xa2, 0x41, 0x02, 
  0x45, 0x22, 0x41, 0x02, 0x45, 0xa2, 0x51, 0x0a, 0x61, 0x06, 0x19, 0x18, 0x07, 0x60, 0x01, 0x80
};
// 'icon_battery', 16x16px
const unsigned char bitmap_icon_battery [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x40, 0x04, 0x5b, 0x66, 0x5b, 0x66, 
  0x5b, 0x66, 0x40, 0x04, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'icon_dashboard', 16x16px
const unsigned char bitmap_icon_dashboard [] PROGMEM = {
  0x07, 0xe0, 0x18, 0x18, 0x21, 0x24, 0x50, 0x02, 0x48, 0x0a, 0x84, 0x01, 0x83, 0x81, 0xa2, 0x45, 
  0x82, 0x41, 0x81, 0x81, 0xa0, 0x05, 0x40, 0x02, 0x4b, 0xd2, 0x23, 0xc4, 0x18, 0x18, 0x07, 0xe0
};
 
// 'icon_gps_speed', 16x16px
const unsigned char bitmap_icon_gps_speed [] PROGMEM = {
  0x00, 0x00, 0x03, 0xf0, 0x00, 0x08, 0x01, 0xe4, 0x00, 0x12, 0x00, 0xca, 0x06, 0x2a, 0x07, 0x2a, 
  0x07, 0x8a, 0x07, 0xc2, 0x07, 0xc0, 0x0a, 0x00, 0x1f, 0x00, 0x20, 0x80, 0x7f, 0xc0, 0x00, 0x00
};
// 'icon_knob_over_oled', 16x16px
const unsigned char bitmap_icon_knob_over_oled [] PROGMEM = {
  0x00, 0x00, 0x1f, 0xf0, 0x13, 0x50, 0x1b, 0xb0, 0x11, 0x50, 0x1f, 0xf0, 0x03, 0x80, 0x01, 0x00, 
  0x00, 0x00, 0x09, 0x20, 0x49, 0x24, 0x20, 0x08, 0x00, 0x01, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00
};
// 'icon_parksensor', 16x16px
const unsigned char bitmap_icon_parksensor [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x22, 0x00, 0x25, 0x00, 0xf9, 0x00, 0x00, 0x81, 
  0x0c, 0x85, 0x12, 0x95, 0xd2, 0x95, 0x0c, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 
// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 384)
const unsigned char* bitmap_icons[6] = {
  temp_icon,
  wifi2_icon1,
  bulb_on_icon16x16,
  bitmap_icon_parksensor,
  bitmap_icon_3dcube,
  bitmap_icon_dashboard
};

void buzz_pasive(void){
  tone(pin_buzzer, 440); // A4
  delay(200);
  tone(pin_buzzer, 494); // B4
  delay(200);
  tone(pin_buzzer, 523); // C4
  delay(200);
  tone(pin_buzzer, 587); // D4
  delay(100);
  noTone(pin_buzzer);
}
void beep_pasive(void){
  tone(pin_buzzer, 784); // G4
  delay(50);
  tone(pin_buzzer, 698); // F4
  delay(50);
  noTone(pin_buzzer);
}
void buzz_active(void){
  digitalWrite(pin_buzzer, HIGH);
  delay(50);
  digitalWrite(pin_buzzer, LOW);  
  delay(25);
  digitalWrite(pin_buzzer, HIGH);
  delay(50);
  digitalWrite(pin_buzzer, LOW);  
  delay(25);
  digitalWrite(pin_buzzer, HIGH);
  delay(50);
  digitalWrite(pin_buzzer, LOW);  
}
void beep_active(void){
  digitalWrite(pin_buzzer, HIGH);
  delay(50);
  digitalWrite(pin_buzzer, LOW);  
  noTone(pin_buzzer);
  
}
 
