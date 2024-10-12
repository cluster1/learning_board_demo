/*Library Yang digunakan : */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "lb_config.h";  // Libaray ini  memuat configurasi yang digunakan pada learning board model WEMOS & NodeMCU 



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*Setup Button */
  pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP); // up button
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); // select button
  
  /*Setup Display OLED*/
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  /*Setup LED */  
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  digitalWrite(led_1, LOW); // set led 1 off
  digitalWrite(led_2, LOW); // set led 2 off  
  
  /*Setup Buzzer */
  pinMode(pin_buzzer, OUTPUT);
  digitalWrite(pin_buzzer, LOW);
    
  /* Setup Temperature Sensor */
  dht.begin();

  /* Setup WIFI */
  // WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apGateway, apSubnet) ;    
  WiFi.softAP(ssid, password);
  delay(100);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot );
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  
  Serial.println(F("Learning Board Demo "));
  Serial.println(F("(c) 2024, n00bwork.com "));
  displayTitle();  
  delay(2000);
}

void loop() {
 
  
  hum = dht.readHumidity();
  temp = dht.readTemperature();
   nValue =  analogRead(PotensioPin);   
   
  Serial.println(current_screen);
 server.handleClient();
  
  if (current_screen == 0) { // MENU UTAMA / ketika sedang diposisi menu pertama 
    if ((digitalRead(BUTTON_NEXT_PIN) == LOW) && (button_next_clicked == 0)) { // Jika button NEXT / Blue ditekan, pindah ke menu selanjutnya 
        item_selected = item_selected + 1; // Pilih Menu selanutnya          
        button_next_clicked = 1; // tandai tombol NEXT sudah ditekan 
        if (item_selected >= NUM_ITEMS) { // Jika Menu selanjutnya adalah menu terakhir, pilih menu pertama 
          item_selected = 0;
        }         
     }
     if ((digitalRead(BUTTON_NEXT_PIN) == HIGH) && (button_next_clicked == 1)) { // Jika tombil NEXT diangkat  
        button_next_clicked = 0;
      }
  }else  if (current_screen == 1) { // Jika di Sub menu 

          if ((digitalRead(BUTTON_NEXT_PIN) == LOW) && (button_next_clicked == 0)) { // up button clicked - jump to previous menu item
               button_next_clicked = 1; // set button to clicked to only perform the action once
          }      
          if ((digitalRead(BUTTON_NEXT_PIN) == HIGH) && (button_next_clicked == 1)) { // unclick 
              button_next_clicked = 0;
          }    
  }

  if ((digitalRead(BUTTON_SELECT_PIN) == LOW) && (button_select_clicked == 0)) { // Jika tombol select ditekan, pindah ke sub menu 
     button_select_clicked = 1; // Tandai jika tombol select sedang dipilih 
     if (current_screen == 0) {current_screen = 1;} //Jika sedang di Main Menu, pindahkan screen ke sub menu 
     else {current_screen = 0;} // 
  }
  if ((digitalRead(BUTTON_SELECT_PIN) == HIGH) && (button_select_clicked == 1)) { // Jika tombol select diangkat, pindah ke sub menu  
    button_select_clicked = 0;
  }
 
  item_sel_previous = item_selected - 1;
  if (item_sel_previous < 0) {item_sel_previous = NUM_ITEMS - 1;} // Jika index menu yang dipilih < 0 , menu yang dipilih menu terakhir 
  
  item_sel_next = item_selected + 1;  
  if (item_sel_next >= NUM_ITEMS) {item_sel_next = 0;} // Jika menu yang dipilih menu terakhir, menu selanjutnya menu ke-1 

  current_menu = item_sel_previous ;

  if (current_screen == 0) { // MENU SCREEN
      Serial.println("Display Menu Utama");
      display.clearDisplay();  
      display.setTextColor(SSD1306_WHITE);    // set text color putih         
      display.setTextSize(2);                 // ukuran text 2 
      display.setCursor(30,8);                
      display.print(menu_items[item_sel_previous]);
      display.drawBitmap(0,5, bitmap_icons[item_sel_previous], 16, 16, 1) ;
       display.drawLine((120/NUM_ITEMS) * (item_selected),28, (120/NUM_ITEMS) * (item_selected+1), 28, 1);        
      display.display();    // Tampilkan display 
           
   } else if (current_screen == 1) { // SUB MENU SCREEN 
               Serial.print("Display Sub Menu : ");
               Serial.print(menu_items[current_menu]);
               Serial.println(current_menu);
                
               if (current_menu == 0 ){ // Menu Sensor 
                    displayTemperature();       
                } else if (current_menu == 1 ){
                       display_wifi();                 
                }else if (current_menu == 2 ){
                       display_LED();                 
                  } else if (current_menu == 3 ){
                       display_LDR();                 
                  }else if (current_menu == 4 ){
                      display_BEEP();
                  }else if (current_menu == 5 ){
                       display_OLED();                 
                  }else { 
                    displayTitle(); 
                  } 
    }
}

void displayTitle(void) {
  display.clearDisplay();  
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("LEARNING BOARD DEMO"));
  display.setCursor(0,10);             // Start at top-left corner
  display.println(F("Ver. 1.1"));
  display.display();      // Show initial text  
}

void displayTemperature(void){
  display.clearDisplay();  
  display.setTextSize(1);             // Normal 1:1 pixel scale  
  display.setCursor(0,0);
  display.print(menu_items[current_menu]);  
  display.setCursor(0,10);
  display.print("Temperature : ");
  display.print(temp);
  display.print(" ");
  display.write(0xF7);
  display.print("C");
  display.setCursor(0,20);
  display.print("Humidity    : ");
  display.print(hum);
  display.print(" ");
  display.print("%"); 
  display.display();    // for the changes to make effect 
  
}
 
void display_LED(){ 
  static int m = LOW;
  static int n = HIGH ; 
  int i = 0 ; 

  display.clearDisplay();    
  display.setTextSize(2);             // Normal 1:1 pixel scale  
  display.setCursor(0,0);
  display.print(menu_items[current_menu]);
  display.display();    

  while (i<=2){        
      m = !m;
      n = !n;         
    display.setTextSize(1);
    
    display.setCursor(0,25);
    display.print("GREEN");
    display.drawCircle(42,27, 4, WHITE);
  
    display.setCursor(62,25);
    display.print("Blue"); 
    display.drawCircle(90,27, 4, WHITE);
    
    digitalWrite(led_1, m);  
    digitalWrite(led_2, n);
    
    display.fillCircle(42,27, 3, m);  
    display.fillCircle(90,27, 3, n);
    display.display();    
    i++ ; 
    delay(250);
  }
  
  if (button_next_clicked == 1 ) {  /* exit */
      Serial.print("BTN Blue Click :");
      current_screen =  0 ; 
  }     

  
};

void display_LDR(void){
   
   display.clearDisplay();    
   display.setTextSize(1);             
   display.setCursor(0,0);
   display.print(menu_items[current_menu]);
   display.setCursor(0,14);
   display.print("LIGHT");
   display.drawRect(38, 15, 87, 4, WHITE);   
   display.display(); 
    
   if (nValue != trimpot_value ) {
    
      int percentage1 = map(trimpot_value, 400, 1024, 0, 85);
      int percentage2 = map(nValue, 400, 1024, 0, 85);              
      
      display.fillRect(39, 16, percentage1 , 2, INVERSE);
      display.fillRect(39, 16, percentage2 , 2, WHITE);     
      display.display();
    
      trimpot_value = nValue ;
      Serial.print("Potensio Read : ")  ;
      Serial.println(trimpot_value)  ;
   }
  if (button_next_clicked == 1 ) {  /* exit */
      Serial.print("BTN Blue Click :");
      current_screen =  0 ; 
  }     
   
} 

void display_OLED(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(1000);  
  if (button_next_clicked == 1 ) {  /* exit */
      Serial.print("BTN Blue Click :");
      current_screen =  0 ; 
  }     
  
}

void beep(void){
  digitalWrite(pin_buzzer, HIGH);
  delay(50);
  digitalWrite(pin_buzzer, LOW);  
}

void beep_long(void){
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

void display_BEEP(void){
  display.clearDisplay();  
  display.setTextSize(1);             // Normal 1:1 pixel scale  
  display.setCursor(0,0);
  display.print(menu_items[current_menu]);  
  display.setCursor(0,10);
 
  display.display();    // for the changes to make effect 
  beep(); 
  delay(1000) ;
  beep_long();                 
  delay(2000);
}

void display_wifi(void){
  display.clearDisplay();  
  display.setTextSize(1);             // Normal 1:1 pixel scale  
  display.setCursor(0,0);
  display.print(menu_items[current_menu]);  
  display.setCursor(0,10);
  display.print("IP : ");
  display.print(apIP);
 
  display.setCursor(0,20);
  display.print("SSID : ");
  display.print(ssid);
 
  display.display();    // for the changes to make effect 
  
}
void handleRoot() {
 
 String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Learning Board Web Server Demo </title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
 
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  ptr +="<p>Temperature : "+String(temp)+"&deg;C</p>";
  ptr +="<p>Humidity : "+String(hum)+" %</p>";
  
  ptr +="</body>\n";
  ptr +="</html>\n";
  server.send ( 200, "text/html", ptr );
 
}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
