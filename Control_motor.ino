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

float vectort_paso[]={300000, 150000, 100000, 75000, 60000, 50000, 42857, 37500, 33333, 30000, 27273, 25000, 23077, 21429, 20000, 18750, 17647, 16667, 15789, 15000, 14286, 13636, 13043, 12500, 12000, 11538, 11111, 10714, 10345, 10000, 9677, 9375, 9091, 8824, 8571, 8333, 8108, 7895, 7692, 7500, 7317, 7143, 6977, 6818, 6667, 6522, 6383, 6250, 6122, 6000, 5882, 5769, 5660, 5556, 5455, 5357, 5263, 5172, 5085, 5000, 4918, 4839, 4762, 4688, 4615,  4545, 4478, 4412, 4348, 4286, 4225, 4167, 4110, 4054, 4000, 3947, 3896, 3846, 3797, 3750, 3704, 3659, 3614, 3571, 3529, 3488, 3448, 3409, 3371, 3333, 3297, 3261, 3226, 3191, 3158, 3125, 3093, 3061, 3030};
unsigned long t_paso = 0;        // Variable en la que introducimo el tiempo de cada paso
bool fc = true;
volatile int segundos = 0;
volatile int v = 0;

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
  lcd.setCursor(13,1);
  lcd.print(v);
  lcd.setCursor(15,1);
  lcd.print("rpm");


  if (pulsador == true)  
  {
    estado = 1;
  }
  else if (derecha == true and v<99)
  {
    v++;
    lcd.setCursor(13, 1);
    lcd.print("  ");
    lcd.setCursor(13, 1);
    lcd.print(v);  
  }
  else if (izquierda == true and v>0)
  {
    v--;
    lcd.setCursor(13, 1);
    lcd.print("  ");
    lcd.setCursor(13, 1);
    lcd.print(v);
  }
    
}

void experimento()
{
  t_paso = ((unsigned long)vectort_paso[v-1]);     
  Timer1.attachInterrupt(Motor);             // Funcion de la interrupcion de los pasos
  Timer1.initialize(t_paso);            // Inicializacion de la interrupcion de los pasos        

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
  if (estado == 1)
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
