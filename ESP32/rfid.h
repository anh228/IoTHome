#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int dem_tong = 0;
int n = 4;
int a;
int gt_zero;
int diachi_zero;
int m = 5;
int moi = 0;
int gt_er = 0;
int o=0;
int id_moi[4]; int id_er[4];
int demco = 0; int demchua = 0; int demmas = 0;
void set(){
  Serial.begin(9600);    
  lcd.init();
  lcd.backlight();
  }
void manhinh()

{
  //lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("HELLO!");
  lcd.setCursor(0,1);
  lcd.print("MOI NHAP THE....");
}
void nhap_mas()
{
  lcd.setCursor(2,0);
  lcd.print("SETUP MASTER");
  lcd.setCursor(0,1);
  lcd.print("MOI NHAP THE....");
}

void menu_tong()
{
  if (dem_tong == 0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">BACK");
    lcd.setCursor(0,1);
    lcd.print(" THEM THE");  
  }
  else if (dem_tong == 1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" BACK");
    lcd.setCursor(0,1);
    lcd.print(">THEM THE");  
  }
  else if (dem_tong == 2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">XOA THE BAT KY");
    lcd.setCursor(0,1);
    lcd.print(" XOA TAT CA THE");  
  }
  else if (dem_tong == 3)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" XOA THE BAT KY");
    lcd.setCursor(0,1);
    lcd.print(">XOA TAT CA THE");  
  }       
}

void chon_menu()
{
  if (dem_tong == 1) //Nhập thẻ mới
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("THEM THE MOI");
    lcd.setCursor(0,1);
    lcd.print("MOI NHAP THE....");     
  }
  else if (dem_tong == 2) //Xóa thẻ bất kỳ
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" XOA THE BAT KY ");
    lcd.setCursor(0,1);
    lcd.print("MOI NHAP THE....");     
  } 
  else if (dem_tong == 3) //XÓA TẤT CẢ THẺ
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("XOA TAT CA THE");    
  }     
}
void tim_zero()
{
  while( n < a) //Tìm ô nhớ có GT 0 đầu tiên
    {
      gt_zero = EEPROM.read(n);
      if (gt_zero == 0)
      {
        diachi_zero = n;
        //Serial.print("Zero: "); Serial.print(diachi_zero); 
        break;
      }      
      //Serial.print("   n: "); Serial.println(n);
      n++;
    }
  //Serial.println(".....................");
  if(diachi_zero==0)
  {
    diachi_zero = a; //Nếu trong đoạn từ 4 đến số ô đã sử dụng không có ô nào có GT 0
                    // thì diachi_zero = a là ô cuối cùng lưu thẻ
    n=0;
  }
  // Serial.print("Zero: "); Serial.print(diachi_zero);   
  // Serial.print("   n: "); Serial.print(n); 
  // Serial.print("   a: "); Serial.println(a);   
}

void ss_epprom()
{
  //Serial.print("Zero: "); Serial.println(diachi_zero);
  //m = 5 ; a lưu số ô cuối được dùng
    while(m < a) //chạy từ ô 5 đến ô sử dụng cuối ... 5 < 20
    {
      moi = m + 4; //moi = 9
      for(m; m<moi; m++)
      {
        gt_er = EEPROM.read(m);
        // Serial.print("m: "); Serial.print(m);
        // Serial.print("   moi: "); Serial.print(moi);
        // Serial.print("   GT eep: "); Serial.print(gt_er);
        // o=0
        if(o<=3) //Lưu giá trị đọc từ eeprom vào id_er
        {
          id_er[o]=gt_er;
          // Serial.print("   id eeprom: "); Serial.print(id_er[o]);
          // Serial.print("   o: "); Serial.println(o);
        }
        o++;
      }   
      // Serial.print("$$$$$$$$$$ o: "); Serial.println(o);
      if(id_moi[0] == id_er[0] && id_moi[1] == id_er[1] && id_moi[2] == id_er[2] && id_moi[3] == id_er[3]) //Nếu thẻ có trong EEPR
      {
        demco = 1; demchua=0; demmas=0;
        //Serial.print("   demco: "); Serial.println(demco);
        break;
      }
      else if(id_moi[0] != id_er[0] or id_moi[1] != id_er[1] or id_moi[2] != id_er[2] or id_moi[3] != id_er[3]) //Nếu thẻ KO có trong EEPR
      { // Dùng or vì chỉ cần  1 byte khác với 1 byte lưu trong eeprom thì thẻ đó là thẻ khác
        // Nếu dùng && có thể có thẻ sẽ trùng 1 đến 2 byte
        demchua += 1; demco=0; demmas=0;
        //Serial.print("   demchua: "); Serial.println(demchua);
      } 
      if(id_moi[0] == EEPROM.read(1) && id_moi[1] == EEPROM.read(2) && 
              id_moi[2] == EEPROM.read(3) && id_moi[3] == EEPROM.read(4)) //THẺ mới nhập là MASTER
      {
        demmas=1; demco=0; demchua=0;
      }           
      o = 0;
      m = moi;
    }  
}
