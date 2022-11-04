/*          
 * LCD      SCL/22    SDA/21
 * RFID     SCK/18    MISO/19   MOSI/23   SDA(SS)/5   RST/3
 * SERVO    OUT/2
 * XHM209   OUT/34
 * SOLID    OUT/35
 * DHT11    OUT/15
 * BUTTON   MENU/17   LEN/16    XUONG/4
 * DEVICES  HOCA/14   LAMP1/27  LIGHT2/26   FAN/25    MOTOR/33
 */

#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <Wire.h>
#include <Eeprom24Cxx.h>
#include <SPI.h>
#include <MFRC522.h>
#include "rfid.h"
#include <Servo.h>

// Thông tin về wifi
//#define ssid "Full House"
//#define password "11111111"

//#define ssid "Kita"
//#define password "88881111"

//#define ssid "NhaOngTinh_2.4G"
////#define ssid "NhaOngTinh_5G"
//#define password "hoiongtinh"

#define ssid "Nang Thu"
#define password "15051505"
#define mqtt_server "broker.hivemq.com"

const uint16_t mqtt_port = 1883; //Port của MQT

#define topic_list "park/docs"

#define DHTPIN 15            //dhtpin
#define DHTTYPE DHT11       
#define light_sensor  34    //xh m209 pin
#define solid_sensor  35    // solid pin
  
#define RST_PIN         3  
#define SS_PIN          5
#define EEPROM_SIZE     64

#define hoca            14
#define lamp1           25
#define light2          33
#define fan             26
#define motor           27
int servo =2;     //door
DHT dht(DHTPIN, DHTTYPE);

unsigned long UID[4];
unsigned long i;
int diachi = 1; int diachi2 = 0;// int a;
int gtmas = 0; int tong = 0; int gttong = 0;
int bandau;
int menu = 17; int gtmenu = 0;
int len = 16; int gtlen = 0;
int xuong = 4; int gtxuong = 0;
int macdinh = 1;
int dem_menu = 0; int dem = 0; 
int vitri = 0;
//int id_moi[4]; int id_er[4];
//int diachi_zero; 
//int m = 5; 
//int moi = 0;
//int gt_zero;
//int gt_er = 0;
//int o=0;
//int n = 4;
//int demco = 0; int demchua = 0; int demmas = 0;

WiFiClient espClient;
PubSubClient client(espClient);

Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();    
  mfrc522.PCD_Init();
  EEPROM.begin(EEPROM_SIZE);
  lcd.init();
  lcd.backlight();
  myservo.attach(servo);
  setup_wifi();                             //thực hiện kết nối Wifi
  client.setServer(mqtt_server, mqtt_port); // cài đặt server và lắng nghe client ở port 1883
  client.setCallback(callback);

  client.subscribe("parkHoca");
  client.subscribe("parkLamp1");
  client.subscribe("parkDoor");
  client.subscribe("parkLight2");
  client.subscribe("parkFan");

  pinMode(hoca, OUTPUT); // [
  pinMode(lamp1, OUTPUT); // lamp1
  pinMode(light2, OUTPUT); // Light2
  pinMode(fan, OUTPUT); // fan
  pinMode(motor, OUTPUT);
  pinMode(solid_sensor, INPUT);
  pinMode(light_sensor, INPUT);

  pinMode(menu, INPUT_PULLUP);
  pinMode(len, INPUT_PULLUP);
  pinMode(xuong, INPUT_PULLUP);

  dht.begin();
}
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // in ra thông báo đã kết nối và địa chỉ IP của ESP8266
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Hàm call back để nhận dữ liệu
void callback(char *topic, byte *payload, unsigned int length)
{
  //-----------------------------------------------------------------
  //in ra tên của topic và nội dung nhận được
  Serial.print("Co tin nhan moi tu topic: ");
  Serial.println(topic);
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);

  if (String(topic) == "parkHoca")
    {
      if (message == "parkHocaOn")
      {
        digitalWrite(hoca, LOW);
      }
  if (message == "parkHocaOff")
      {
        digitalWrite(hoca, HIGH);
      }
    }
  
    if (String(topic) == "parkLamp1")
    {
      if (message == "parkLamp1On")
      {
        digitalWrite(lamp1, LOW);
      }
      if (message == "parkLamp1Off")
      {
        digitalWrite(lamp1, HIGH);
      }
    }
  
    if (String(topic) == "parkDoor")
    {
      if (message == "parkDoorOn")
      {
          for(vitri = 0; vitri < 91; vitri++)
          {
            myservo.write(vitri);
          } 
//          delay(5000);
//          for(vitri = 91; vitri >0 ; vitri--)
//          {
//            myservo.write(vitri);
//          }
      }
      if (message == "parkDoorOff")
      {
          for(vitri = 91; vitri > 0; vitri--)
          {
            myservo.write(vitri);
            //delay(10);
          }
      }
    }

        if (String(topic) == "parkLight2")
    {
      if (message == "parkLight2On")
      {
        digitalWrite(light2, LOW);
      }
      if (message == "parkLight2Off")
      {
        digitalWrite(light2, HIGH);
      }
    }

    if (String(topic) == "parkFan")
    {
      if (message == "parkFanOn")
      {
        digitalWrite(fan, LOW);
      }
      if (message == "parkFanOff")
      {
        digitalWrite(fan, HIGH);
      }
    }

  Serial.println(message);
  Serial.write(payload, length);
  Serial.println();
  //-------------------------------------------------------------------------
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
      String clientId = "Kita-ESP32";
    if (client.connect("Kita-ESP32")) //kết nối vào broker
    {
      Serial.println("Đã kết nối:");
      //đăng kí nhận dữ liệu từ topic
    client.subscribe("parkHoca");
    client.subscribe("parkLamp1");
    client.subscribe("parkDoor");
    client.subscribe("parkLight2");
    client.subscribe("parkFan");
    }
    else
    {
      // in ra trạng thái của client khi không kết nối được với broker
      Serial.print("Lỗi:, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}

unsigned long lastMsg = 0;
void loop()
{
    if (!client.connected()){// Kiểm tra kết nối
      reconnect();
    }
     client.loop();

  long now = millis();
  if (now - lastMsg > 2000)
  {
    lastMsg = now;
    int h = dht.readHumidity();
    int t = dht.readTemperature();
    int lux = analogRead(light_sensor);
    int solid_tb = analogRead(solid_sensor);//, solid_cb, solid_tb;
    int sol = 100 - map (solid_tb, 0, 4096, 0, 100);
//    for(int i =0; i<10; i++){
//      solid_cb = digitalRead(solid_sensor);
//      delay(100);
//      solid_cb +=1;
//    }
//    solid_tb = solid_cb/40;
//    sol      = 100 - map(solid_tb, 0, 1023, 0, 100);
  //  solid_cb = 0;
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    
    char tempString[10];
    sprintf(tempString, "%d", t);
    char humiString[10];
    sprintf(humiString, "%d", h);
    char solidString[10];
    sprintf(solidString, "%d", sol);
    char lightString[10];
    sprintf(lightString, "%f", lux);

    StaticJsonDocument<100> doc;
    doc["Temperature"] = t;
    doc["Humidity"] = h;
    doc["Solid"] = sol;
    doc["Light"] = lux;

    char buffer[256];
    serializeJson(doc, buffer);
    client.publish("topic", buffer);
    client.publish(topic_list, buffer);
    Serial.println(buffer);
  }
  
  /*===========================RFID========================================*/
  bandau = true;
  gtmenu = digitalRead(menu);
  gtlen = digitalRead(len);
  gtxuong = digitalRead(xuong);

  // Serial.print("Menu: "); Serial.print(gtmenu); Serial.print("  ");
  // Serial.print("Lên: "); Serial.print(gtlen); Serial.print("  ");
  // Serial.print("Xuống: "); Serial.println(gtxuong);

if(bandau == true) //Kiểm tra có thẻ MAS chưa
{
  for(int j = 1; j < 4; j++) // đọc từ 1 - 4 xem trong ô nhớ có gì ko, Nếu không là chưa có thẻ MAS
  {
      gtmas = EEPROM.read(j);
      gttong = gttong + gtmas;
      //Serial.print("Gtmas: "); Serial.print(gtmas); Serial.print("  ");
      //Serial.print("Tổng: "); Serial.print(tong); Serial.print("  ");
  } 
}

if (gttong == 0) //Chưa có thẻ MAS
{
  tong = 0;
  if (tong == 0 && dem == 0)//Nếu ô 1, 2, 3, 4 chưa có Mas thì nhập Mas
  { 
    nhap_mas();

    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++) 
    { 
          // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " " :" "); 
          // Serial.print(mfrc522.uid.uidByte[i], HEX);         
      UID[i] = mfrc522.uid.uidByte[i];
      EEPROM.write(diachi, UID[i]); //bắt đầu lưu từ ô 1-4
      diachi = diachi + 1;
//      in();
    }
    mfrc522.PICC_HaltA();  
    mfrc522.PCD_StopCrypto1();
    delay(50);
    lcd.clear();   
  }
} 
else // Đã có thẻ MAS
{
  tong = 1; bandau = false;
  if (tong == 1 && dem == 0) // Đã có MASTER
  { 
    manhinh();
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }
    for (byte i = 0; i < 4; i++) 
      {        
        UID[i] = mfrc522.uid.uidByte[i];
        id_moi[i] = UID[i];
      }
    mfrc522.PICC_HaltA();  
    mfrc522.PCD_StopCrypto1();

    if (id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2)
        && id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //Check xem phải thẻ MAS ko
    {
      dem = 1;
      if (tong == 1 && dem == 1) //Để thẻ MASTER để vào vào menu
      {
        menu_tong();
      }   
    }
    else //Nếu không phải thẻ MASTER
    {
      ss_epprom(); //So sánh thẻ mới với EEPROM
      if(demco == 1 && demchua == 0 && demmas == 0) //Nếu thẻ có trong EEPROM
        {
            // Serial.print("................THE DA CO TRONG EEPROM..................");
            // Serial.print(" Zero: "); Serial.print(diachi_zero);
            // Serial.print("   Demco: "); Serial.println(demco);     
          for(vitri = 0; vitri < 91; vitri++)
          {
            myservo.write(vitri);
            //delay(10);
          }    
          o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
          lcd.setCursor(0,1);
          lcd.print("   MO CUA....   "); 
          delay(5000); 
          for(vitri = 90; vitri > 0; vitri--)
          {
            myservo.write(vitri);
            //delay(10);
          } 
          lcd.clear();
        }  
      else if(demchua > 0 && demco == 0 && demmas == 0) //Nếu thẻ chưa có trong EEPROM
        {
          //   Serial.print("................THE CHUA CO..................");
          //   Serial.print(" Zero: "); Serial.print(diachi_zero);
          //   Serial.print("   Demchua: "); Serial.print(demchua); 
          // Serial.print("  ID mới: ");Serial.print(id_moi[0]);Serial.print(":");Serial.print(id_moi[1]);Serial.print(":");
          // Serial.print(id_moi[2]);Serial.print(":");Serial.println(id_moi[3]);
          for(vitri = 90; vitri > 0; vitri--)
          {
            myservo.write(vitri);
            //delay(10);
          }
          o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
          lcd.setCursor(0,1);
          lcd.print("    SAI THE!    "); 
          delay(5000); 
          lcd.clear();        
        } 
    }
  }  
}

if (dem_menu == 1 && dem_tong == 1 && dem == 1) //Nếu chọn THÊM THẺ MỚI
{   
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 

  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    int diachi_phu = 5; // 5,6,7,8
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1(); 
    if(id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2) && 
        id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //THẺ mới nhập là MASTER  
    {
      lcd.clear();
      menu_tong();
      dem_menu = 0;      
    }
    
    else //THẺ MỚI KHÔNG PHẢI LÀ THẺ MASTER thi LƯU vào ô 5,6,7,8
    { 
      for (byte i = 0; i < 4; i++)
      {
        EEPROM.write(diachi_phu, id_moi[i]);
        diachi_phu = diachi_phu + 1;
        a = diachi_phu;
      }
      EEPROM.write(0, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
      lcd.setCursor(0,1);
      lcd.print("   DANG LUU...  "); 
      delay(1000); 
      lcd.clear();
      chon_menu();       
    }  
  }

  else if( a != 0) // Đã có 1 or nhiều thẻ phụ
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();  

    ss_epprom(); //So sánh thẻ mới đưa vào với Eeprom
    
    if(demco == 1 && demchua == 0 && demmas == 0) //Nếu thẻ đã có
    {
        // Serial.print("................THE DA CO TRONG EEPROM..................");
        // Serial.print(" Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demco: "); Serial.println(demco);           
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.setCursor(0,1);
      lcd.print("   THE DA CO!   "); 
      delay(1000); 
      lcd.clear();
      chon_menu(); 
      //in();
    }
    else if(demchua > 0 && demco == 0 && demmas == 0) //Nếu thẻ chưa có...THÌ LƯU THẺ ĐÓ VÀO EEPROM
    {
      tim_zero(); //Tìm vị trí 0 đầu tiên
      //   Serial.print("................THE CHUA CO..................");
      //   Serial.print(" Zero: "); Serial.print(diachi_zero);
      //   Serial.print("   Demchua: "); Serial.print(demchua); 
      // Serial.print("  ID mới: ");Serial.print(id_moi[0]);Serial.print(":");Serial.print(id_moi[1]);Serial.print(":");
      // Serial.print(id_moi[2]);Serial.print(":");Serial.println(id_moi[3]);
      if(diachi_zero == a) //Nếu trong đoạn từ 4 đến số ô đã sử dụng không có ô nào có GT 0
                           // a là ô cuối cùng lưu thẻ       
      {
        for(int i=0; i<4; i++)
        {
          EEPROM.write(diachi_zero, id_moi[i]);
          diachi_zero = diachi_zero + 1;
          a = diachi_zero;
        }
        EEPROM.write(0, a); //Sau khi lưu 1 thẻ mới vào thì cập nhật số ô nhớ đã sử dụng vào ô 0
      }

      else if(diachi_zero == n) ////Nếu trong đoạn từ 4 đến số ô đã sử dụng CÓ ô = 0, thì gán ô đó vào n
      {
        for(int i=0; i<4; i++) //Lưu thẻ mới vào bắt đầu từ ô 0 đó
        {
          EEPROM.write(diachi_zero, id_moi[i]);
          diachi_zero = diachi_zero + 1;
          //a = diachi_zero;
        }        
        diachi_zero=a;
      }
      tim_zero();
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.setCursor(0,1);
      lcd.print("   DANG LUU...  "); 
      delay(1000); 
      lcd.clear();
      chon_menu();     
      //in();    
    }
    if(demmas == 1 && demco == 0 && demchua == 0) //THẺ MASTER thì quay về menu
    {
      //   Serial.print("................THE NAY LA THE MASTER..................");
      //   Serial.print(" Zero: "); Serial.print(diachi_zero);
      //   Serial.print("   Demchua: "); Serial.print(demchua); 
      // Serial.print("  ID mới: ");Serial.print(id_moi[0]);Serial.print(":");Serial.print(id_moi[1]);Serial.print(":");
      // Serial.print(id_moi[2]);Serial.print(":");Serial.println(id_moi[3]);
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.clear();
      menu_tong();
      dem_menu = 0; 
      //in();            
    }
  }
}

else if (dem_menu == 1 && dem_tong == 2 && dem == 1) //Nếu chọn XÓA THẺ BẤT KỲ
{
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 

  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    lcd.setCursor(0,1);
    lcd.print(" CHUA CO THE... ");
    delay(1000);
    lcd.clear();
    menu_tong();
    dem_menu = 0;     
  }
  else //Nếu đã có 1 hoặc nhiều thẻ PHỤ
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    { return; } 
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    { return; }  
    for (byte i = 0; i < 4; i++)  //Quét thẻ mới
    {          
      UID[i] = mfrc522.uid.uidByte[i];
      id_moi[i] = UID[i];
    }
    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1();  
    ss_epprom();
    if(demco == 1 && demchua == 0 && demmas == 0) //thẻ có trong EEPROM
    {
      tim_zero();
        //Serial.print("................THE DA CO TRONG EEPROM..................");
      m = m - 4;
        // Serial.print(" m: "); Serial.print(m);
        // Serial.print("   Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demco: "); Serial.println(demco); 
      for(int i=0; i<4; i++)
      {
        EEPROM.write(m, 0);
        //Serial.print(" m: "); Serial.print(m);
        m = m + 1; 
        //Serial.print(" m: "); Serial.print(m);
      }
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0; n=4;
      lcd.setCursor(0,1);
      lcd.print(" DA XOA THE.... "); 
      delay(1000); 
      lcd.clear();
      chon_menu();
      tim_zero();
      //in();      
    }
    else if (demchua > 0 && demco == 0 && demmas == 0) //Thẻ chưa có trong EEPROM
    {
        // Serial.print("................THẺ CHƯA CÓ TRONG EEPROM..................");
        // Serial.print(" Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demco: "); Serial.println(demco);           
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0; 
      lcd.setCursor(0,1);
      lcd.print(" THE CHUA CO... "); 
      delay(1000); 
      lcd.clear();
      chon_menu();  
      //in();      
    }
    if(demmas == 1 && demco == 0 && demchua == 0) //Là thẻ MASTER
    {
        // Serial.print("................THE NAY LA THE MASTER..................");
        // Serial.print(" Zero: "); Serial.print(diachi_zero);
        // Serial.print("   Demchua: "); Serial.print(demchua); 
      // Serial.print("  ID mới: ");Serial.print(id_moi[0]);Serial.print(":");Serial.print(id_moi[1]);Serial.print(":");
      // Serial.print(id_moi[2]);Serial.print(":");Serial.println(id_moi[3]);
      o = 0; m=5; moi=0; demco=0; demchua=0; demmas=0;
      lcd.clear();
      menu_tong();
      dem_menu = 0;
      //in();
    }
  }
}

else if (dem_menu == 1 && dem_tong == 3 && dem == 1) //Chọn XÓA TẤT cả thẻ
{
  a = EEPROM.read(0); //đọc ô nhớ 0 xem đã sử dụng bao nhiêu ô nhớ
  //Serial.print("a: "); Serial.println(a); 
  if (a == 0) //Nếu chưa có thẻ PHỤ nào
  {
    lcd.setCursor(0,1);
    lcd.print(" CHUA CO THE... ");
    dem_menu = 0;
    delay(2000);
    lcd.clear();
    menu_tong();
  }
  else if (a > 0) //Nếu đã có 1 hoặc nhiều thẻ PHỤ
  {  
    lcd.setCursor(0,1);
    lcd.print("  DANG XOA....  "); 
    for (int diachi = 5; diachi<a; diachi++) //Bắt đầu từ ô 5 đến ô đang sử dụng
    {
      EEPROM.write(diachi, 0);
    }  
    EEPROM.write(0, 0); //Sau đó cập nhật vào ô 0 là 0 vì đã xóa hết thẻ   
    delay(3000);
    lcd.clear();
    menu_tong();
    dem_menu = 0;
  } 
  diachi_zero = 0; n=4; a=0;
  tim_zero();
  //in(); 
}

if (gtlen != macdinh) //Nút lên
{
  if(gtlen == 0)
  {
    if (dem == 1) //Lên ở menu
    {      
      if (dem_tong <= 0)
        dem_tong = 3;
      else 
        dem_tong --;
      menu_tong();
    }
  delay(100);
  }
  macdinh = gtlen;
}

if (gtxuong != macdinh) //Nút xuống
{
  if(gtxuong == 0)
  {
    if (dem == 1) //Xuống ở menu
    {  
      if(dem_tong >= 3)
        dem_tong = 0;
      else 
        dem_tong ++;
      menu_tong();
    }
  delay(100);
  }
  macdinh = gtxuong;
}

if (gtmenu != macdinh) //Nút menu
{
  if(gtmenu == 0)
  {
    if (dem == 1 && dem_tong != 0) //Chọn menu
    {
      chon_menu();
      dem_menu = 1;
    }
    else if (dem == 1 && dem_tong == 0) //Nhấn nút BACK để về màn hình
    {
      lcd.clear();
      manhinh();
      dem = 0;
    }    
  delay(100);
  }
  macdinh = gtmenu;
}
}
