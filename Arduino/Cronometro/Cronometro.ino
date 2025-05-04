#include <TimerOne.h>

#include <LiquidCrystal.h>   
LiquidCrystal lcd(16, 17, 23, 25, 27, 29); 

#define LCD_PINS_RS 16      // LCD control conectado a GADGETS3D  shield LCDRS
#define LCD_PINS_ENABLE 17  // LCD enable pin conectado a GADGETS3D shield LCDE
#define LCD_PINS_D4 23      // LCD signal pin, conectado a GADGETS3D shield LCD4
#define LCD_PINS_D5 25      // LCD signal pin, conectado a GADGETS3D shield LCD5
#define LCD_PINS_D6 27      // LCD signal pin, conectado a GADGETS3D shield LCD6
#define LCD_PINS_D7 29      // LCD signal pin, conectado a GADGETS3D shield LCD7

#define BTN_ENC 35          // Encoder Click, connected to Gadgets3D shield S_EC

int pulsador_inicio = 35;   

int horas = 0;     
int minutos = 0;     
int segundos = 0;    

int inicio = 0;

void setup() 
{
  pinMode(pulsador_inicio, INPUT_PULLUP);             // Pulsador para iniciar el cronometro
  Timer1.initialize(1000000);                //Configura el TIMER en 1 Segundo
  Timer1.attachInterrupt(Temporizador) ;
  lcd.begin(20, 4);                          // Dimensiones de la pantalla LCD
}

void loop() 
{
    lcd.setCursor(0, 0);
    lcd.print("CRONOMETRO");
  
  if (digitalRead(pulsador_inicio) == false )
  {
    inicio = 1;
    }
  
  if (inicio == 0){
    lcd.setCursor(0, 1);
    lcd.print("00:00:00");
    }
  else{  
    if (segundos == 60)    {
    segundos = 0;
    minutos++;           }
    if (minutos == 60)     {
    minutos = 0;
    horas++;             }
    lcd.setCursor(0, 1);
    if (horas < 10)        {
    lcd.print("0");      }
    lcd.print(horas);
    lcd.print(":");
    lcd.setCursor(3, 1);
    if (minutos < 10)      {
    lcd.print("0");      }
    lcd.print(minutos);
    lcd.print(":");
    lcd.setCursor(6, 1);
    if (segundos < 10)     {
    lcd.print("0");        }
    lcd.print(segundos);
  }

}


void Temporizador()
{
  if (inicio == 1) {
    segundos++;
  }
}
