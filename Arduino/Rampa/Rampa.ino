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

#define X1_PIN 3         // PIN para el fin de carrera del carril 1 (1: presionado (true))
#define X2_PIN 2         // PIN para el fin de carrera del carril 2 (1: presionado (true))

#define X_STEP_PIN 54       // PIN de los pasos del controlador DRV8825 del motor paso a paso
#define X_DIR_PIN 55        // PIN de la direccion del controlador DRV8825 del motor paso a paso
#define X_ENABLE_PIN 38     // PIN del enable del controlador DRV8825 del motor paso a paso

#define BTN_EN1 31          // Encoder, conectado a GADGETS3D shield S_E1
#define BTN_EN2 33          // Encoder, cconectado a GADGETS3D shield S_E2
#define BTN_ENC 35          // Encoder Click, connected to Gadgets3D shield S_EC

float vectort_paso[]={9375,4688,3125,2344,1875,1563,1339,1172,1042,938,852,781,721,670,625,586,551,521,493,469,446,426,408,391,375,361,347,335,323,313,302,293,284,276,268,260,253,247,240,234,229,223,218,213,208,204,199,195,191,188,184,180,177,174,170,167,164,162,159,156,154,151,149,146,144,142,140,138,136,134,132,130,128,127,125,123,122,120,119,117,116,114,113,112,110,109,108,107,105,104,103,102,101,100,99,98,97,96,95};
unsigned long t_paso = 0;        // Variable en la que introducimo el tiempo de cada paso

int rata1 = 0;    // Variable que define si la rata ha caido (1: rata caida)
int rata2 = 0;
volatile int segundos = 0;
volatile int v = 1;
int vmax = 99;        // Velocidad máxima del sistema
int vmin = 0;       // Velocidad mínima del sistema
int vfin = 0;   // Velocidad final de la rampa
volatile int Ta = 0;                           // Tasa de aceleración del sistema
int Tamax = 10;
int Tamin = 0;

int boton = 0;
bool btn_en1, btn_en2, btn_enc, btn_en1_prev, btn_en2_prev ;       // Variables de lectura directa del codificador giratorio mecanico
bool direccion = false;                                            // Direccion del codificador
bool derecha, izquierda, pulsador = false;                         // Variables de lectura del codificador interpretadas
int i = 0;                                                         // Contador de pulsos

volatile int nivel = LOW;             // Nivel del motor paso a paso

int inicio = 0;                       // Variable que inicia el experimento cuando comienza en 0 mm
int fin = 0;                          // Variable que para el experimento cuando llega al final de carrera 
int volatile estado = 0;              // Variables del estado

int fila, columna = 0; 

void setup() {

  pinMode(BTN_EN1, INPUT_PULLUP);     // Encoder 1
  pinMode(BTN_EN2, INPUT_PULLUP);     // Encoder 2
  pinMode(BTN_ENC, INPUT_PULLUP);     // Encoder Swith
  
  pinMode(X1_PIN, INPUT_PULLUP);   // Fin de carrera carril 1            
  pinMode(X2_PIN, INPUT_PULLUP);   // Fin de carrera carril 2

  attachInterrupt(digitalPinToInterrupt(X1_PIN),canal_1,RISING);
  attachInterrupt(digitalPinToInterrupt(X2_PIN),canal_2,RISING);

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

  lcd.setCursor(2,0);
  lcd.print("Velocidad:");
  lcd.setCursor(13,0);
  lcd.print(v);
  lcd.setCursor(15,0);
  lcd.print("rpm");
  lcd.setCursor(2,1);
  lcd.print("Tasa ac:");
  lcd.setCursor(11,1);
  lcd.print(Ta);
  lcd.setCursor(13,1);
  lcd.print("rpm/s"); 

  if (pulsador == true and fila < 1)  
  {
    fila++;
  }
  else if (pulsador == true and fila == 1)
  {
    estado = 1;
  }
  
  switch (fila)
  {
    case 0:
      if (derecha == true and v<vmax)
      {
        v++;
        lcd.setCursor(13, 0);
        lcd.print("  ");
        lcd.setCursor(13, 0);
        lcd.print(v);
        vfin = v;  
      }
      else if (izquierda == true and v>vmin)
      {
        v--;
        lcd.setCursor(13, 0);
        lcd.print("  ");
        lcd.setCursor(13, 0);
        lcd.print(v);
        vfin = v;
      }
    break;
    case 1:
      if (derecha == true and Ta<Tamax)
      {
        Ta++;
        lcd.setCursor(11, 1);
        lcd.print("  ");
        lcd.setCursor(11, 1);
        lcd.print(Ta);  
      }
      else if (izquierda == true and Ta>Tamin)
      {
        Ta--;
        lcd.setCursor(11, 1);
        lcd.print("  ");
        lcd.setCursor(11, 1);
        lcd.print(Ta);
      }
    break;
  }   
}

void experimento()
{
  t_paso = ((unsigned long)vectort_paso[v-1]);     
  Timer1.attachInterrupt(Motor);             // Funcion de la interrupcion de los pasos
  Timer1.initialize(t_paso/2);            // Inicializacion de la interrupcion de los pasos        

  Timer3.initialize(1000000);              // Inicialización de la interrupcion del contador de segundos
  Timer3.attachInterrupt(Temporizador);    // Función de la interrupcion del contador de segundos

  Timer4.initialize(1000000);              // Inicialización de la interrupcion de la tasa de aceleracion
  Timer4.attachInterrupt(aceleracion);    // Función de la interrupcion de la tasa de aceleracion
  
  if (rata1 == 0)  //Display velocidad y tiempo de cada carril
  {
    if (segundos < 10)      
    {
      lcd.setCursor(6, 2);
    } 
    if (segundos >= 10)     
    {
      lcd.setCursor(5, 2);       
    }
    if (segundos >= 100)     
    {
      lcd.setCursor(4, 2);      
    }
    lcd.print(segundos);
    if (v<10)
    {
      lcd.setCursor(6,3);
      lcd.print(v);
    }
    else if (v>=10 and v<100)
    {
      lcd.setCursor(5,3);
      lcd.print(v);
    }
    else if (v>=100)
    {
      lcd.setCursor(4,3);
      lcd.print(v);
    }
  }
  if (rata2 == 0)
  {
    if (segundos < 10)      
    {
      lcd.setCursor(15, 2);
    } 
    if (segundos >= 10)     
    {
      lcd.setCursor(14, 2);       
    }
    if (segundos >= 100)     
    {
      lcd.setCursor(13, 2);      
    }
    lcd.print(segundos);
    if (v<10)
    {
      lcd.setCursor(15,3);
      lcd.print(v);
    }
    else if (v>=10 and v<100)
    {
      lcd.setCursor(14,3);
      lcd.print(v);
    }
    else if (v>=100)
    {
      lcd.setCursor(13,3);
      lcd.print(v);
    }
  }
  
  if (rata1 == 1 and rata2 == 1)
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

void aceleracion() // Se podría actualizar cada menos tiempo si el cambio es muy brusco (en realidad dudo que se usen aceleraciones mayores a 1 rpm/s, así que habría que ver como hacerlo,
                   // pues esto conllevaría cambiar el vector velocidad y meterle mayor grado de precision (decimales). A espera de confirmación tras reunion)
{
  if (estado == 2)
  {  
    if (v<vfin)
    {
      v = v + Ta;
    }
    else
    {
      v = vfin;
    }
  }
}

void canal_1()
{
  rata1=1;  
}

void canal_2()
{
  rata2=1;  
}

void loop() {

  switch (estado){

    case 0:
      inicio = 0;
      rata1 = 0;
      rata2 = 0;
      leer_pulso();
      leer_encoder();
      pantalla();
    break;

    case 1:
      v=1;
      lcd.setCursor(1,2);
      lcd.print("C1:"); 
      lcd.setCursor(4, 2);
      lcd.print("000");
      lcd.setCursor(4, 3);
      lcd.print("000");
      lcd.setCursor(7, 3);
      lcd.print("rpm");
      lcd.setCursor(10, 2);
      lcd.print("C2:");
      lcd.setCursor(13, 2);
      lcd.print("000"); 
      lcd.setCursor(13, 3);
      lcd.print("000"); 
      lcd.setCursor(16, 3);
      lcd.print("rpm");      
      estado = 2;
   
    case 2: 
      inicio = 1;
      experimento();
    break;    
  }
  
}
