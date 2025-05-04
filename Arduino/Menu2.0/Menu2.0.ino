#include <TimerOne.h>       // Libreria del timer 1
#include <TimerThree.h>     // Libreria del timer 3
#include <TimerFour.h>      // Libreria del timer 4

#include <LiquidCrystal.h>  // Libreria de la pantalla LCD
LiquidCrystal lcd(16, 17, 23, 25, 27, 29);

//Pines de la shield
#define LCD_PINS_RS 16      // LCD control conectado a GADGETS3D  shield LCDRS
#define LCD_PINS_ENABLE 17  // LCD enable pin conectado a GADGETS3D shield LCDE
#define LCD_PINS_D4 23      // LCD signal pin, conectado a GADGETS3D shield LCD4
#define LCD_PINS_D5 25      // LCD signal pin, conectado a GADGETS3D shield LCD5
#define LCD_PINS_D6 27      // LCD signal pin, conectado a GADGETS3D shield LCD6
#define LCD_PINS_D7 29      // LCD signal pin, conectado a GADGETS3D shield LCD7

#define BTN_EN1 31          // Encoder, conectado a GADGETS3D shield S_E1
#define BTN_EN2 33          // Encoder, cconectado a GADGETS3D shield S_E2
#define BTN_ENC 35          // Encoder Click, connected to Gadgets3D shield S_EC



int fila, columna, Modo = 0;                                             // Variable de fila y columna en la pantalla lcd
bool derecha, izquierda, pulsador = false;                         // Variables de lectura del codificador interpretadas
int i = 0;                                                         // Contador de pulsos
int volatile estado = 0;              // Variables del estado
int v = 1;                            // Velocidad del sistema
int vmax = 99;        // Velocidad máxima del sistema
int vmin = 0;       // Velocidad mínima del sistema
int Ta = 0;                           // Tasa de aceleración del sistema
int Tamax = 40;
int Tamin = 0;
bool btn_en1, btn_en2, btn_enc, btn_en1_prev, btn_en2_prev;

// SIGNOS CREADOS

byte empty[8] =
{
  B00000,
  B00100,
  B01010,
  B10001,
  B10001,
  B01010,
  B00100,
};
byte full[8] =
{
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
};


void setup() {

  pinMode(BTN_EN1, INPUT_PULLUP);     // Encoder 1
  pinMode(BTN_EN2, INPUT_PULLUP);     // Encoder 2
  pinMode(BTN_ENC, INPUT_PULLUP);     // Encoder Swith
           

  lcd.begin(20, 4);   // 20 columnas y 4 filas


  btn_en1 , btn_en1_prev = digitalRead(BTN_EN1);
  btn_en2 , btn_en2_prev = digitalRead(BTN_EN2);
  btn_enc = digitalRead(BTN_ENC);

  lcd.createChar(0, empty);   // 0: numero de carácter; empty: matriz que contiene los pixeles del carácter
  lcd.createChar(1, full);    // 1: numero de carácter; full: matriz que contiene los pixeles del carácter

  
}


void menu()
{
  columna=0;
  lcd.clear();
  lcd.setCursor(7,0);
  lcd.print("MENU");
  lcd.setCursor(1,1);
  lcd.print("Modo");
  lcd.setCursor(1,2);
  lcd.print("Velocidad");
  lcd.setCursor(1,3);
  lcd.print("Iniciar experimento");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(0, 2);
  lcd.write(byte(0));
  lcd.setCursor(0, 3);
  lcd.write(byte(0));
  lcd.setCursor(0, fila+1);
  lcd.write(byte(1)); 

}


void seleccion()
{
  switch (fila)
  {
    case 0: //seleccionar el modo
      if (pulsador == true  and columna < 3) // columna es una variable que avanza en horizontal por la pantalla, cuando vale 0 podemos cambia en vertical con derecha o izquierda, cuando no aumentamos la variable con la que estemos trabajando
      {
        columna++;
      }
      else if (pulsador == true and columna == 2) //hay 3 opciones
      {
        columna = 0;
      }    
      switch (columna)
      {
        case 0: //modo estandar
          if (derecha == true)
          {
            fila = 1;
            lcd.setCursor(0, 1);
            lcd.write(byte(0));
            lcd.setCursor(0, 2);
            lcd.write(byte(1));
            lcd.setCursor(0, 3);
            lcd.write(byte(0));
          }
        break;
        case 1:
          if (pulsador == true)
          {
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("Selecciona el modo");
            lcd.setCursor(2,2);
            lcd.print("Estandar");
            lcd.setCursor(13,2);
            lcd.print("Rampa");
            lcd.setCursor(1,2);
            lcd.write(byte(1));
            lcd.setCursor(12,2);
            lcd.write(byte(0));
          }
          else if (derecha == true)
          {
            lcd.setCursor(1,2);
            lcd.write(byte(0));
            lcd.setCursor(12,2);
            lcd.write(byte(1));
            columna=3;
          }  
        break;
        case 2:    
          if (pulsador == true)
          {
            Modo=0;
            estado=0;
          }
          else if (derecha == true)
          {
            lcd.setCursor(1,2);
            lcd.write(byte(0));
            lcd.setCursor(12,2);
            lcd.write(byte(1));
            columna=3;
          }  
        break;
        case 3: //modo rampa
          if (pulsador == true)
          {
          Modo=1;
          estado=0;
          }
          else if (izquierda == true)
          {
            lcd.setCursor(1,2);
            lcd.write(byte(1));
            lcd.setCursor(12,2);
            lcd.write(byte(0));
            columna=1;
          }
        break;
      }   
    break;
    case 1: // Velocidad
      if (pulsador == true  and columna < 2) // columna es una variable que avanza en horizontal por la pantalla, cuando vale 0 podemos cambia en vertical con derecha o izquierda, cuando no aumentamos la variable con la que estemos trabajando
      {
        columna++;
      }
      else if (pulsador == true and columna == 2)
      {
        columna = 0;
        estado=0;
      }
      switch (columna)
      {
        case 0:
          if (derecha == true)
          {
            fila = 2;
            lcd.setCursor(0, 1);
            lcd.write(byte(0));
            lcd.setCursor(0, 2);
            lcd.write(byte(0));
            lcd.setCursor(0, 3);
            lcd.write(byte(1));
          }
          else if (izquierda == true)
          {
            fila = 0;
            lcd.setCursor(0, 1);
            lcd.write(byte(1));
            lcd.setCursor(0, 2);
            lcd.write(byte(0));
            lcd.setCursor(0, 3);
            lcd.write(byte(0));
          }
        break;
        case 1:
          if (pulsador == true and Modo == 0)
          {
            lcd.clear();
            lcd.setCursor(3,0);
            lcd.print("Selecciona la");
            lcd.setCursor(5,1);
            lcd.print("velocidad");
            lcd.setCursor(2,3);
            lcd.print("Velocidad:000");
            lcd.setCursor(15,3);
            lcd.print("rpm");
          }
    
          if (v<vmax and derecha == true and Modo == 0)    
          {
            v++;
          }
          else if (v>vmin and izquierda == true and Modo == 0)
          {
            v--;
          }
          else if (v<10 and Modo == 0)
          {
            lcd.setCursor(13,3);
            lcd.print("0");
            lcd.setCursor(14,3);
            lcd.print(v);
          }
          else if (v>=10 and Modo == 0)
          {
            lcd.setCursor(13,3);
            lcd.print(v);
          }
          
          else if (pulsador == true and Modo == 1)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Selecciona la Vmax y");
            lcd.setCursor(0,1);
            lcd.print("tasa de aceleracion");
            lcd.setCursor(1,2);
            lcd.print("Vmax:");
            lcd.setCursor(6,2);
            lcd.print("000:");
            lcd.setCursor(9,2);
            lcd.print("rpm");
            lcd.setCursor(1,3);
            lcd.print("Tasa:");
            lcd.setCursor(6,3);
            lcd.print("0.0");
            lcd.setCursor(10,3);
            lcd.print("rpm/s");
            lcd.setCursor(0,2);
            lcd.write(byte(1));
            lcd.setCursor(0,3);
            lcd.write(byte(0));
          }
          if (v<vmax and derecha == true and Modo == 1)
          {
            v++;
          }
          else if (v>vmin and izquierda == true and Modo == 1)
          {
            v--;
          } 
          if (v<10 and Modo == 1)
          {
            lcd.setCursor(7,2);
            lcd.print("0");
            lcd.setCursor(8,2);
            lcd.print(v);
          }
          else if (v>=10 and Modo == 1)
          {
            lcd.setCursor(7,2);
            lcd.print(v);
          }
          break;
          case 2:
            if (pulsador == true and Modo == 0)
            {
              estado=0;
              columna=0;

            }
            else if (pulsador == true and Modo == 1)
            {
              lcd.setCursor(0,2);
              lcd.write(byte(0));
              lcd.setCursor(0,3);
              lcd.write(byte(1));
            }
            if (Ta<Tamax and derecha == true)
            {
              Ta++;
            }
            else if (Ta>Tamin and izquierda == true)
            {
              Ta--;
            } 
            if (Ta<10)
            {
              lcd.setCursor(8,3);
              lcd.print(Ta);
            }
            else if (Ta>=10)
            {
              lcd.setCursor(7,3);
              lcd.write(Ta);
            }
          break;
      }
    break;
    case 2: //Iniciar experimento
      if (pulsador == true)
      {
         
        lcd.clear();
        lcd.setCursor(4,1);
        lcd.print("EXPERIMENTO");
        lcd.setCursor(6,2);
        lcd.print("INICIADO");
        estado = 1;
        
      }
      else if (izquierda == true)
      {
        fila = 1;
        lcd.setCursor(0, 1);
        lcd.write(byte(0));
        lcd.setCursor(0, 2);
        lcd.write(byte(1));
        lcd.setCursor(0, 3);
        lcd.write(byte(0));
      }
    break;
  }
}


void leer_pulso()
{
  btn_enc = digitalRead(BTN_ENC);

  if (btn_enc == false) //Detector de flanco del pulsador
  {
    i++;
  }
  if (i >= 80)
  {
    pulsador = true;
    i = 0;
    delay(200);
  }
  else
  {
    pulsador = false;
  }
}





void leer_encoder() // ECHAR UN OJO A ESTO
{
  btn_en1 = digitalRead(BTN_EN1);
  btn_en2 = digitalRead(BTN_EN2);
  //digitalWrite(X_DIR_PIN, direccion);

  if (btn_en1 != btn_en1_prev || btn_en2 != btn_en2_prev)
  {
    if (btn_en2 == false & btn_en1 == false & btn_en2_prev == true & btn_en1_prev == false)
    {
      derecha = true;
      izquierda = false;
    }
    else if ( btn_en2 == false & btn_en1 == false & btn_en2_prev == false & btn_en1_prev == true )
    {
      derecha = false;
      izquierda = true;
    }

    else
    {
      derecha = false;
      izquierda = false;
    }
  }
  else
  {
    derecha = false;
    izquierda = false;
  }
  btn_en1_prev = btn_en1;
  btn_en2_prev = btn_en2;
}

void experimento()
{
  lcd.clear();
  lcd.setCursor(4,1);
  lcd.print("INICIO  DEL"); 
  lcd.setCursor(4,2);
  lcd.print("EXPERIMENTO");
}


void loop() {
  
  switch (estado)
  {
    case 0:
      menu();
      estado = 1;
    break;

    case 1:
      leer_pulso();
      leer_encoder();
      seleccion();
    break;

    case 2: 
      experimento();
    break;   
  }
}
