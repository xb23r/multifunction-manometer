#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <FreeDefaultFonts.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000
// ALL Touch panels and wiring is DIFFERENT


const int XP = 6, XM = A2, YP = A1, YM = 7; //240x320 ID=0x3229
const int TS_LEFT = 879, TS_RT = 177, TS_TOP = 154, TS_BOT = 898;


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;
Adafruit_GFX_Button on_btn, off_btn, page1_btn, page2_btn, page3_btn;
Adafruit_GFX_Button ok_btn, cncl_btn, plus_btn, minus_btn;
Adafruit_GFX_Button menu_btn, info_btn, back_btn;
int pixel_x, pixel_y;     //Touch_getXY() updates global vars
// Button calibration
int margin = 5;
int btnWidth = 100;
int btnHeight = 40;
int btnY = 200;
// Software variable
bool enable_nuit = false;
int parameter = 50, old_parameter = 50;
long temp0 = 0;
long temp1 = 0;
long temp2 = 0;
long temp3 = 0;
#define BLACK   0x0000
#define GREY    0x5555
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define DARKGREEN   0x05C0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define GOLD    0xDDC0
#define WHITE   0xFFFF
enum pageId {MENU,SENSOR,CMD,PARAM,INFO};
unsigned int currentPage = MENU, oldPage = -1;
void setup(void)
{
  Serial.begin(9600);
  //init TFTTouch
  uint16_t ID = tft.readID();
  Serial.print("TFT ID = 0x");
  Serial.println(ID, HEX);
  Serial.println(F("Calibrate for your Touch Panel"));
  if (ID == 0xD3D3) ID = 0x9486;  //for 3.5" TFT LCD Shield , 0x9341 for 2.8" TFT LCD Shield
  tft.begin(ID);
  tft.setRotation(1); //0-PORTRAIT 1-PAYSAGE 2-REVERSE PORTRAIT 3-REVERSE PAYSAGE

  currentPage = MENU; // Indicates that we are at Home Screen
  Serial.println("Home Page");
}
bool down ;
void loop(void) {
  switch (currentPage) {
    case MENU: //Menu page
      if (currentPage != oldPage)   drawMenuScreen();
      page1_btn.press(down && page1_btn.contains(pixel_x, pixel_y));
      page2_btn.press(down && page2_btn.contains(pixel_x, pixel_y));
      page3_btn.press(down && page3_btn.contains(pixel_x, pixel_y));
      if (page1_btn.justReleased())
        page1_btn.drawButton();
      if (page2_btn.justReleased())
        page2_btn.drawButton();
      if (page3_btn.justReleased())
        page3_btn.drawButton();
      if (page1_btn.justPressed()) {
        page1_btn.drawButton(true);
        currentPage = SENSOR;
      }
      if (page2_btn.justPressed()) {
        page2_btn.drawButton(true);
        currentPage = CMD;
      }
      if (page3_btn.justPressed()) {
        page3_btn.drawButton(true);
        currentPage = PARAM;
      }
      break;
    case SENSOR:
      if (currentPage != oldPage)   drawSensorScreen();
      readSensor();
      updateTemp();
      menu_btn.press(down && menu_btn.contains(pixel_x, pixel_y));
      info_btn.press(down && info_btn.contains(pixel_x, pixel_y));
      if (menu_btn.justReleased())
        menu_btn.drawButton();
      if (info_btn.justReleased())
        info_btn.drawButton();
      if (menu_btn.justPressed()) {
        menu_btn.drawButton(true);
        currentPage = MENU;
      }
      if (info_btn.justPressed()) {
        info_btn.drawButton(true);
        currentPage = INFO;
      }
      break;
    case CMD:
      if (currentPage != oldPage)   drawCmdScreen();
      back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
      if (back_btn.justReleased())
        back_btn.drawButton();
      if (on_btn.justReleased())
        on_btn.drawButton();
      if (off_btn.justReleased())
        off_btn.drawButton();
      if (back_btn.justPressed()) {
        back_btn.drawButton(true);
        currentPage = MENU;
      }

    
      break;
    case PARAM: //consigne
      if (currentPage != oldPage)   drawParamScreen();
      plus_btn.press(down && plus_btn.contains(pixel_x, pixel_y));
      minus_btn.press(down && minus_btn.contains(pixel_x, pixel_y));
      ok_btn.press(down && ok_btn.contains(pixel_x, pixel_y));
      cncl_btn.press(down && cncl_btn.contains(pixel_x, pixel_y));
      if (plus_btn.justReleased())
        plus_btn.drawButton();
      if (minus_btn.justReleased())
        minus_btn.drawButton();
      if (ok_btn.justReleased())
        ok_btn.drawButton();
      if (cncl_btn.justReleased())
        cncl_btn.drawButton();
      if (plus_btn.justPressed()) {
        plus_btn.drawButton(true);
        parameter += 1;
        Serial.print(F("Consigne : "));
        Serial.println(parameter);
        drawTextInRect(tft.width() / 2., 60 + 3 * 4 + 6 * 8, parameter, 4, RED, BLACK);
      }
      if (minus_btn.justPressed()) {
        minus_btn.drawButton(true);
        parameter -= 1;
        Serial.print(F("Consigne : "));
        Serial.println(parameter);
        drawTextInRect(tft.width() / 2., 60 + 3 * 4 + 6 * 8, parameter, 4, RED, BLACK);
      }
      if (ok_btn.justPressed()) {
        ok_btn.drawButton(true);
        currentPage = MENU;
      }
      if (cncl_btn.justPressed()) {
        cncl_btn.drawButton(true);
        parameter = old_parameter;
        currentPage = MENU;
      }
      break;
    case INFO:
      if (currentPage != oldPage) drawInfoScreen();
      if (Touch_getXY()) {
        currentPage = MENU;
      }
      break;
  }
  if (oldPage == currentPage) {
    down = Touch_getXY();
  } else {
    down = false;
  }
}
/************************************************************************************
    SCREENS DEFINTION
 ************************************************************************************/
void drawMenuScreen() {
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  // Title
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(0, 10);
  tft.print("manometre"); // Prints the string on the screen
  tft.drawLine(0, 32, tft.width() * 0.6, 32, GREY); // Draws the red line
  tft.setTextColor(WHITE, BLACK);//((255, 255, 255), (0,0,0));
  tft.setCursor(0, 80);
  tft.setTextColor(GREEN, BLACK);//((255, 255, 255), (0,0,0));
  // Button
  page1_btn.initButton(&tft,  tft.width() / 2. , tft.height() / 2. - (1.*btnHeight + margin), 2 * btnWidth, btnHeight, WHITE, GREY, BLACK, "sondes", 2);
  page2_btn.initButton(&tft, tft.width() / 2., tft.height() / 2., 2 * btnWidth, btnHeight, WHITE, GREY, BLACK, "recul", 2);
  page3_btn.initButton(&tft, tft.width() / 2., tft.height() / 2. + (1.*btnHeight + margin), 2 * btnWidth, btnHeight, WHITE, GREY, BLACK, "tools", 2);
  page1_btn.drawButton(false);
  page2_btn.drawButton(false);
  page3_btn.drawButton(false);
  //Button frame
  tft.drawRoundRect(tft.width() / 2. - 1.5 * btnWidth, tft.height() / 2. - (1.5 * btnHeight + 2 * margin), 2 * btnWidth + btnWidth, 3 * btnHeight + 4 * margin, 10, GREY);
  oldPage = currentPage;
}
void readSensor() {
temp0 = analogRead(A5);
temp1 = analogRead(A4);
temp2 = analogRead(A3);
temp3 = analogRead(A2);

}
void drawSensorScreen() {
  tft.fillScreen(BLACK);
  tft.drawLine(tft.width() / 2., 0, tft.width() / 2., tft.height(), WHITE);
  tft.drawLine(0, tft.height() / 2., tft.width(), tft.height() / 2., WHITE);
  updateTemp();
  tft.setTextSize(1);
  // bouton centré X,Y
  menu_btn.initButton(&tft,  tft.width() / 2. - btnWidth - margin , tft.height() - btnHeight / 2., btnWidth, btnHeight, WHITE, GREY, BLACK, "MENU", 2);
  info_btn.initButton(&tft, tft.width() / 2. + btnWidth + margin, tft.height() - btnHeight / 2., btnWidth, btnHeight, WHITE, GREY, BLACK, "INFO", 2);
  menu_btn.drawButton(false);
  info_btn.drawButton(false);
  oldPage = currentPage;
}
void updateTemp() {
  //temp1
  if (parameter < temp0) { //(abs(parameter-temp0)<1){
    drawTextInRect(tft.width() / 4., tft.height() / 4., temp0, 5, GREEN, 255);
  } else {
    drawTextInRect(tft.width() / 4., tft.height() / 4., temp0, 5, RED, 255);
  }
  drawTextInRect(3 * tft.width() / 4., tft.height() / 4., temp1, 5, RED, 255);
  drawTextInRect(tft.width() / 4., 3 * tft.height() / 4., temp2, 5, RED, 255);
  drawTextInRect(3 * tft.width() / 4., 3 * tft.height() / 4., temp3, 5, RED, 255);
}
void drawCmdScreen() {
  tft.setRotation(1);            //PORTRAIT
  tft.setTextSize(1);
  tft.fillScreen(BLACK);
  back_btn.initButton(&tft,  60 , 20, btnWidth, btnHeight, BLACK, BLACK, WHITE, "<- Back", 2);
  // bouton centré X,Y
  back_btn.drawButton(false);
  on_btn.drawButton(false);
  off_btn.drawButton(false);
  oldPage = currentPage;
}
void drawParamScreen() {
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  //Title
  tft.setTextSize(3);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(tft.width() / 2. - 9 * 3 * 3, 50);
  tft.print("Parameter");
  ok_btn.initButton(&tft,  2 + btnWidth / 2., btnHeight / 2. + margin, btnWidth, btnHeight, WHITE, DARKGREEN, BLACK, "Valider", 2);
  cncl_btn.initButton(&tft, tft.width() - btnWidth / 2 - 2, btnHeight / 2. + margin, btnWidth, btnHeight, WHITE, RED, BLACK, "Annuler", 2);
  ok_btn.drawButton(false);
  cncl_btn.drawButton(false);
  drawTextInRect(tft.width() / 2., 60 + 3 * 4 + 6 * 8, parameter, 4, RED, BLACK);
  plus_btn.initButton(&tft,  tft.width() / 2. - btnWidth / 2. , 60 + 3 * 4 + 6 * 8 + (btnWidth - 30), btnWidth - 20, btnWidth - 30, WHITE, GREEN, BLACK, "+", 5);
  minus_btn.initButton(&tft, tft.width() / 2. + btnWidth / 2. + margin, 60 + 3 * 4 + 6 * 8 + (btnWidth - 30), btnWidth - 20, btnWidth - 30, WHITE, GREEN, BLACK, "-", 5);
  plus_btn.drawButton(false);
  minus_btn.drawButton(false);
  oldPage = currentPage;
}
void drawInfoScreen() {
  tft.setRotation(1);
  tft.fillScreen(BLACK);//(100, 155, 203)
  tft.drawRoundRect(10, 50, tft.width() - 20, tft.height() - 60, 5, GREY); //tft.fillRect (10, 10, 60, 36);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(70, 18);
  tft.print("Nom des sondes");
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(25, 80);
  tft.print("en devellopement");
  tft.setCursor(20, 245);
  tft.print(" ");
  oldPage = currentPage;
}
/************************************************************************************
    UTILITY FUNCTION
*************************************************************************************/
void drawTextInRect(int x, int y, long temp, int tsize, unsigned int fColor, unsigned int bColor) {
  int marg = 10;
  char buf[12];
  int nbChar = strlen(itoa(temp, buf, 10)) + 2;
  if (bColor != 255) tft.fillRect(x - nbChar * 3 * tsize - marg, y - nbChar * 1 * tsize - marg, nbChar * 6 * tsize + 2 * marg, nbChar * 2 * tsize + 2 * marg, bColor);
  tft.setTextSize(tsize);
  tft.setTextColor(fColor, BLACK);
  //tft.setCursor(x-strlen(*text)*3*tsize+marg, y+rheight/2.+marg);
  tft.setCursor(x - nbChar * 3 * tsize, y - nbChar * 1 * tsize);
  tft.print(temp);
  //while(*text) tft.print(*text++);
  tft.write(0xF7);
  tft.print("C");
}
bool Touch_getXY(void)
{
  p = ts.getPoint();
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    if (tft.width() <= tft.height()) { //Portrait
      pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
      pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    } else {
      pixel_x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
      pixel_y = map(p.x, TS_RT, TS_LEFT, 0, tft.height());
    }
  }
  return pressed;
}
