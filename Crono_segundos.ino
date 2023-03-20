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
    lcd.setCursor(2, 1);
    lcd.print("CRONOMETRO:");
  
  if (digitalRead(pulsador_inicio) == false )
  {
    inicio = 1;
    }
  
  if (inicio == 0){
    lcd.setCursor(14, 1);
    lcd.print("000");
    }
  else{ 
    if (segundos < 10)
    {
      lcd.setCursor(16, 1);
    } 
    if (segundos >= 10)     
    {
      lcd.setCursor(15, 1);       
    }
    if (segundos >= 100)     
    {
      lcd.setCursor(14, 1);      
    }
    lcd.print(segundos);
  }

}


void Temporizador()
{
  if (inicio == 1) {
    segundos++;
  }
}
