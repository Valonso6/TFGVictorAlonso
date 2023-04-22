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

#define X_MIN_PIN 3         // PIN para el fin de carrera colocado al inicio del recorrido (1: presionado (true))
#define X_MAX_PIN 2         // PIN para el fin de carrera colocado al final del recorrido (1: presionado (true))

#define X_STEP_PIN 54       // PIN de los pasos del controlador DRV8825 del motor paso a paso
#define X_DIR_PIN 55        // PIN de la direccion del controlador DRV8825 del motor paso a paso
#define X_ENABLE_PIN 38     // PIN del enable del controlador DRV8825 del motor paso a paso

#define BTN_EN1 31          // Encoder, conectado a GADGETS3D shield S_E1
#define BTN_EN2 33          // Encoder, cconectado a GADGETS3D shield S_E2
#define BTN_ENC 35          // Encoder Click, connected to Gadgets3D shield S_EC

float vectort_paso[]={9375, 4688, 3125, 2344, 1875, 1563, 1339, 1172, 1042, 938, 852, 781, 721, 670, 625, 586, 551, 521, 493, 469, 446, 426, 408, 391, 375, 361, 347, 335, 323, 313, 302, 293, 284, 276, 268, 260, 253, 247, 240, 234, 229, 223, 218, 213, 208, 204, 199, 195, 191, 188, 184, 180 ,177, 174, 170, 167, 164, 162, 159, 156, 154, 151, 149, 146, 144, 142, 140, 138, 136, 134, 132, 130, 128, 127, 125, 123, 122, 120, 119, 117, 116, 114, 113, 112, 110, 109, 108, 107, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 88, 87, 86, 85, 84, 84, 83, 82, 82, 81, 80, 79, 79, 78, 77, 77, 76,  76, 75, 74, 74, 73, 73, 72, 72, 71, 70, 70, 69, 69, 68, 68, 67, 67, 66, 66, 66, 65, 65, 64, 64, 63, 63, 63, 62, 62, 61, 61, 60, 60, 60, 59, 59, 59, 58, 58, 58, 57, 57, 56, 56, 56, 55, 55, 55, 55, 54, 54, 54, 53, 53, 53, 52, 52, 52, 52, 51, 51, 51, 50, 50, 50, 50, 49, 49, 49, 49, 48, 48, 48, 48, 47, 47, 47, 47, 46, 46, 46, 46, 46, 45, 45, 45, 45, 44, 44, 44, 44, 44, 43, 43, 43, 43, 43, 42, 42, 42, 42, 42, 41, 41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 40, 39, 39, 39, 39, 39, 39, 38, 38, 38, 38, 38, 38, 38, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36, 36, 36, 36, 36, 35, 35, 35, 35, 35, 35, 35, 34, 34, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32, 32, 32, 32, 32, 32, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 23, 23, 21, 19, 17, 16};
unsigned long t_paso = 0;        // Variable en la que introducimo el tiempo de cada paso
bool fc = true;
volatile int segundos = 0;
volatile int v = 0;
int vmax = 404;
int vmin = 0;

int fila, columna = 0;

int boton = 0;
bool btn_en1, btn_en2, btn_enc, btn_en1_prev, btn_en2_prev ;       // Variables de lectura directa del codificador giratorio mecanico
bool direccion = false;                                            // Direccion del codificador
bool derecha, izquierda, pulsador = false;                         // Variables de lectura del codificador interpretadas
int i = 0;                                                         // Contador de pulsos

volatile int nivel = LOW;             // Nivel del motor paso a paso

int inicio = 0;                       // Variable que inicia el experimento cuando comienza en 0 mm
int fin = 0;                          // Variable que para el experimento cuando llega al final de carrera 
int volatile estado = 0;              // Variables del estado

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN_EN1, INPUT_PULLUP);     // Encoder 1
  pinMode(BTN_EN2, INPUT_PULLUP);     // Encoder 2
  pinMode(BTN_ENC, INPUT_PULLUP);     // Encoder Swith
  
  pinMode(X_MIN_PIN, INPUT_PULLUP);   // Fin de carrera inicio            

  lcd.begin(20, 4);   // 20 columnas y 4 filas

  pinMode(X_STEP_PIN , OUTPUT);
  pinMode(X_DIR_PIN , OUTPUT);
  pinMode(X_ENABLE_PIN , OUTPUT);

  btn_en1 , btn_en1_prev = digitalRead(BTN_EN1);
  btn_en2 , btn_en2_prev = digitalRead(BTN_EN2);
  btn_enc = digitalRead(BTN_ENC);
 
  digitalWrite(X_ENABLE_PIN , LOW);         // Habilitación a nivel bajo del motor paso a paso


}

      

void Temporizador() {
  
  if (inicio == 1) //                             <- INTERRUPCION CRONOMETRO
  {
    segundos++;
  }
  
}

void leer_encoder()
{
  btn_en1 = digitalRead(BTN_EN1);
  btn_en2 = digitalRead(BTN_EN2);
  digitalWrite(X_DIR_PIN, direccion);

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

void pantalla() 
{

  lcd.setCursor(2,1);
  lcd.print("Velocidad:");
  lcd.setCursor(12,1);
  lcd.print(v);
  lcd.setCursor(15,1);
  lcd.print("rpm");


  if (pulsador == true)  
  {
    estado = 1;
  }
  else if (derecha == true and v<vmax)
  {
    v++;
    lcd.setCursor(12, 1);
    lcd.print("   ");
    lcd.setCursor(12, 1);
    lcd.print(v);  
  }
  else if (izquierda == true and v>vmin)
  {
    v--;
    lcd.setCursor(12, 1);
    lcd.print("  ");
    lcd.setCursor(12, 1);
    lcd.print(v);
  }
    
}

void experimento()
{
  t_paso = ((unsigned long)vectort_paso[v-1]);     
  Timer1.attachInterrupt(Motor);             // Funcion de la interrupcion de los pasos
  Timer1.initialize(t_paso/2);            // Inicializacion de la interrupcion de los pasos        

  Timer3.initialize(1000000);              // Inicialización de la interrupcion del contador de segundos
  Timer3.attachInterrupt(Temporizador);    // Función de la interrupcion del contador de segundos

  
  if (segundos < 10)
  {
    lcd.setCursor(11, 2);
  } 
  if (segundos >= 10)     
  {
    lcd.setCursor(10, 2);       
  }
  if (segundos >= 100)     
  {
    lcd.setCursor(9, 2);      
  }
  lcd.print(segundos);
  
  fc = digitalRead(X_MIN_PIN);

  if (fc == true)
  {
    estado = 0; 
    segundos = 0; 
  }
  
}

void Motor()
{
  if (estado == 2)
  {
    digitalWrite(X_STEP_PIN , nivel);
    if (nivel == LOW)   //                             <- INTERRUPCION MOVIMIENTO MOTOR TIEMPO PASO
    {       
       nivel = HIGH;
    }
    else
    {
       nivel = LOW;
    }  
  }
}



void loop() {

  switch (estado){

    case 0:
      inicio = 0;
      leer_pulso();
      leer_encoder();
      pantalla();
    break;

    case 1:
      lcd.setCursor(9, 2);
      lcd.print("000");      
    estado = 2;
   
    case 2: 
      inicio = 1;
      experimento();
    break;    
  }
  
}
