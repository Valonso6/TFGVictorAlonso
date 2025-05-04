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

#define X1_PIN 2         // PIN para el fin de carrera del carril 1 (1: presionado (true))
#define X2_PIN 3         // PIN para el fin de carrera del carril 2 (1: presionado (true))
#define X3_PIN 18        // PIN para el fin de carrera del carril 3 (1: presionado (true))
#define X4_PIN 19        // PIN para el fin de carrera del carril 4 (1: presionado (true))
#define X5_PIN 20        // PIN para el fin de carrera del carril 5 (1: presionado (true))
#define X6_PIN 21        // PIN para el fin de carrera del carril 6 (1: presionado (true))

#define X_STEP_PIN 54       // PIN de los pasos del controlador DRV8825 del motor paso a paso
#define X_DIR_PIN 55        // PIN de la direccion del controlador DRV8825 del motor paso a paso
#define X_ENABLE_PIN 38     // PIN del enable del controlador DRV8825 del motor paso a paso

#define BTN_EN1 31          // Encoder, conectado a GADGETS3D shield S_E1
#define BTN_EN2 33          // Encoder, cconectado a GADGETS3D shield S_E2
#define BTN_ENC 35          // Encoder Click, connected to Gadgets3D shield S_EC

float vectort_paso[]{9375,4688,3125,2344,1875,1563,1339,1172,1042,938,852,781,721,670,625,586,551,521,493,469,446,426,408,391,375,361,347,335,323,313,302,293,284,276,268,260,253,247,240,234,229,223,218,213,208,204,199,195,191,188,184,180,177,174,170,167,164,162,159,156,154,151,149,146,144,142,140,138,136,134,132,130,128,127,125,123,121.7,119.8,117.8,116};

unsigned long t_paso = 0;        // Variable en la que introducimos el tiempo de cada paso
volatile int nivel = LOW;        // Nivel del motor paso a paso

int rata1 = 0;    // Variable que define si la rata ha caido (1: rata caida)
int rata2 = 0;
int rata3 = 0;
int rata4 = 0;
int rata5 = 0;
int rata6 = 0;


volatile int v = 1;   // Inicializamos como 1 porque al coger un valor del veector_paso lo hacemos como vector_paso(v-1)
int vmax = 80;        // Velocidad máxima del sistema
int vmin = 1;         // Velocidad mínima del sistema
int vini = 0;         // Velocidad inicial de la rampa
int vfin = 0;         // Velocidad final de la rampa
int minuto = 0;
volatile int Ta = 0;  // Tasa de aceleración del sistema
int Tamax = 10;       // Tasa de aceleración máxima del sistema
int Tamin = 0;        // Tasa de aceleración mínima del sistema

int boton = 0;
bool btn_en1, btn_en2, btn_enc, btn_en1_prev, btn_en2_prev ;       // Variables de lectura directa del codificador giratorio mecanico
bool direccion = false;                                            // Direccion del codificador

int fila, columna, Modo = 0;                                       // Variable de fila y columna en la pantalla lcd
bool derecha, izquierda, pulsador = false;                         // Variables de lectura del codificador interpretadas
int i = 0;                                                         // Contador de pulsos
int volatile estado = 0;              // Variables del estado
int inicio = 0;                       // Variable para que el cronómetro comienze a contar
volatile int segundos = 0;

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
           
  pinMode(X1_PIN, INPUT_PULLUP);   // Fin de carrera carril 1            
  pinMode(X2_PIN, INPUT_PULLUP);   // Fin de carrera carril 2
  pinMode(X3_PIN, INPUT_PULLUP);   // Fin de carrera carril 3
  pinMode(X4_PIN, INPUT_PULLUP);   // Fin de carrera carril 4
  pinMode(X5_PIN, INPUT_PULLUP);   // Fin de carrera carril 5
  pinMode(X6_PIN, INPUT_PULLUP);   // Fin de carrera carril 6

  attachInterrupt(digitalPinToInterrupt(X1_PIN),canal_1,RISING);      // Interrupciones de los finales de carrera
  attachInterrupt(digitalPinToInterrupt(X2_PIN),canal_2,RISING);
  attachInterrupt(digitalPinToInterrupt(X3_PIN),canal_3,RISING);
  attachInterrupt(digitalPinToInterrupt(X4_PIN),canal_4,RISING);
  attachInterrupt(digitalPinToInterrupt(X5_PIN),canal_5,RISING);
  attachInterrupt(digitalPinToInterrupt(X6_PIN),canal_6,RISING);

  lcd.begin(20, 4);   // 20 columnas y 4 filas

  pinMode(X_STEP_PIN , OUTPUT);
  pinMode(X_DIR_PIN , OUTPUT);
  pinMode(X_ENABLE_PIN , OUTPUT);

  btn_en1 , btn_en1_prev = digitalRead(BTN_EN1);
  btn_en2 , btn_en2_prev = digitalRead(BTN_EN2);
  btn_enc = digitalRead(BTN_ENC);

  lcd.createChar(0, empty);   // 0: numero de carácter; empty: matriz que contiene los pixeles del carácter
  lcd.createChar(1, full);    // 1: numero de carácter; full: matriz que contiene los pixeles del carácter

  digitalWrite(X_ENABLE_PIN , LOW);         // Habilitación a nivel bajo del motor paso a paso

}



void Temporizador() {
  
  if (inicio == 1) //                             <- INTERRUPCION CRONOMETRO
  {
    segundos++;
  }
  
}



void Motor()
{
  if (inicio == 1)
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
  if (inicio == 1 and Modo == 1) //                                <- INTERRUPCION ACELERACION                       
  {
    minuto++;
    if (v<vfin and vini<vfin and minuto == 60)    // RAMPA
    {   
      v = v + Ta;
      minuto = 0;
    }
    else if (v>vfin and vini>vfin and minuto == 60)    //RAMPA INVERSA                               
    {
      v = v - Ta;
      minuto = 0;
    }
    else if (v == vfin and minuto == 60)
    {
      v = vfin;
    }
  }
}

// INTERRUPCIONES DE FINAL DE CARRERA

void canal_1()
{
  rata1=1;  
}

void canal_2()
{
  rata2=1;  
}

void canal_3()
{
  rata3=1;  
}

void canal_4()
{
  rata4=1;  
}

void canal_5()
{
  rata5=1;  
}

void canal_6()
{
  rata6=1;  
}



void leer_pulso()
{
  btn_enc = digitalRead(BTN_ENC);

  if (btn_enc == false) //Detector de flanco del pulsador
  {
    i++;
  }
  if (i >= 30)
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



void leer_encoder() 
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



void menu()         // Menú principal
{
  columna = 0;
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



void seleccion()      // Funcion que nos permite movernos a través de los menús
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
      if (pulsador == true  and columna < 4) // columna es una variable que avanza en horizontal por la pantalla, cuando vale 0 podemos cambia en vertical con derecha o izquierda, cuando no aumentamos la variable con la que estemos trabajando
      {
        columna++;
      }
      else if (pulsador == true and columna == 4)
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
            lcd.print("Escoge la velocidad");
            lcd.setCursor(0,1);
            lcd.print("inicial y final y la");
            lcd.setCursor(0,2);
            lcd.print("tasa de aceleracion");
            lcd.setCursor(5,3);
            lcd.print("por minuto");
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
            lcd.clear();
            lcd.setCursor(1,1);
            lcd.print("Vini:");
            lcd.setCursor(6,1);
            lcd.print("000:");
            lcd.setCursor(9,1);
            lcd.print("rpm");
            lcd.setCursor(1,2);
            lcd.print("Vfin:");
            lcd.setCursor(6,2);
            lcd.print("000:");
            lcd.setCursor(9,2);
            lcd.print("rpm");
            lcd.setCursor(1,3);
            lcd.print("Pasos:");
            lcd.setCursor(7,3);
            lcd.print("000");
            lcd.setCursor(10,3);
            lcd.print("rpm");
            lcd.setCursor(0,1);
            lcd.write(byte(1));
            lcd.setCursor(0,2);
            lcd.write(byte(0));
            lcd.setCursor(0,3);
            lcd.write(byte(0));
          }
          if (v<vmax and derecha == true and Modo == 1)
          {
            v++;
            vini = v;
          }
          else if (v>vmin and izquierda == true and Modo == 1)
          {
            v--;
            vini = v;
          } 
          if (v<10 and Modo == 1)
          {
            lcd.setCursor(7,1);
            lcd.print("0");
            lcd.setCursor(8,1);
            lcd.print(v);
          }
          else if (v>=10 and Modo == 1)
          {
            lcd.setCursor(7,1);
            lcd.print(v);
          }
            
        break;
        case 3:
          lcd.setCursor(0,1);
          lcd.write(byte(0));
          lcd.setCursor(0,2);
          lcd.write(byte(1));
          lcd.setCursor(0,3);
          lcd.write(byte(0));
          if (v<vmax and derecha == true and Modo == 1)
          {
            v++;
            vfin = v;
          }
          else if (v>vmin and izquierda == true and Modo == 1)
          {
            v--;
            vfin = v;
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
        case 4:
          if (pulsador == true and Modo == 1)
          {
            lcd.setCursor(0,1);
            lcd.write(byte(0));
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
            lcd.print("0");
            lcd.setCursor(9,3);
            lcd.print(Ta);
          }
          else if (Ta>=10)
          {
            lcd.setCursor(8,3);
            lcd.print(Ta);
          }          
        break;
      }
    break;
    case 2: //Iniciar experimento
      if (pulsador == true)
      {
        lcd.clear();
        estado = 2;
        
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

void espera()    // Display de espera para la colocación de las ratas
{
  lcd.setCursor(5,0);
  lcd.print("Prepare el");
  lcd.setCursor(4,1);
  lcd.print("experimento");
  lcd.setCursor(2,2);
  lcd.print("y manten pulsado");
  lcd.setCursor(3,3);
  lcd.print("para continuar");
  if (pulsador == true)
  {
    lcd.clear();
    estado = 3;
  }
}


void experimento()       // Comienzo del experimento según el modo elegido
{ 

  t_paso = ((unsigned long)vectort_paso[v-1]);     
  Timer1.attachInterrupt(Motor);               // Funcion de la interrupcion de los pasos
  Timer1.initialize(t_paso/2);                 // Inicializacion de la interrupcion de los pasos        

  Timer3.initialize(1000000);                  // Inicialización de la interrupcion del contador de segundos
  Timer3.attachInterrupt(Temporizador);        // Función de la interrupcion del contador de segundos

  Timer4.initialize(1000000);                  // Inicialización de la interrupcion de la tasa de aceleracion
  Timer4.attachInterrupt(aceleracion);         // Función de la interrupcion de la tasa de aceleracion
  
  //VELOCIDAD CONSTANTE

  if (Modo == 0) //Falta el resto de carriles
  {

    if (derecha == true and v<vmax)
    {
      v++;
    }
    else if (izquierda == true and v>vmin)
    {
      v--;
    }

    if (rata1 == 0)  //Display velocidad y tiempo de cada carril
    {
      if (segundos < 10)      
      {
        lcd.setCursor(2,0);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(1,0);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(0,0);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(1,1);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(0,1);
        lcd.print(v);
      }
    }

    if (rata2 == 0)
    {
      if (segundos < 10)      
      {
        lcd.setCursor(5,2);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(4,2);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(3,2);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(4,3);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(3,3);
        lcd.print(v);
      }
    }

    if (rata3 == 0)
    {
      if (segundos < 10)      
      {
        lcd.setCursor(8,0);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(7,0);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(6,0);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(7,1);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(6,1);
        lcd.print(v);
      }
    }

    if (rata4 == 0)
    {
      if (segundos < 10)      
      {
        lcd.setCursor(11,2);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(10,2);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(9,2);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(10,3);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(9,3);
        lcd.print(v);
      }
    }

    if (rata5 == 0)
    {
      if (segundos < 10)      
      {
        lcd.setCursor(14,0);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(13,0);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(12,0);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(13,1);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(12,1);
        lcd.print(v);
      }
    }

    if (rata6 == 0)
    {
      if (segundos < 10)      
      {
        lcd.setCursor(17,2);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(16,2);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(15,2);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(16,3);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(15,3);
        lcd.print(v);
      }
    }
    
    if (rata1==1 and rata2==1 and rata3==1 and rata4==1 and rata5==1 and rata6==1)
    {
      estado = 5; 
      segundos = 0; 
    }
  }

  //MODO RAMPA

  else if (Modo == 1)
  {
    if (rata1 == 0)  //Display velocidad y tiempo de cada carril
    {
      if (segundos < 10)      
      {
        lcd.setCursor(2,0);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(1,0);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(0,0);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(1,1);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(0,1);
        lcd.print(v);
      }
    }

    if (rata2 == 0)
    {
      if (segundos < 10)      
      {
        lcd.setCursor(5,2);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(4,2);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(3,2);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(4,3);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(3,3);
        lcd.print(v);
      }
    }

    if (rata3 == 0)  //Display velocidad y tiempo de cada carril
    {
      if (segundos < 10)      
      {
        lcd.setCursor(8,0);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(7,0);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(6,0);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(7,1);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(6,1);
        lcd.print(v);
      }
    }

    if (rata4 == 0)  //Display velocidad y tiempo de cada carril
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
    }

    if (rata5 == 0)  //Display velocidad y tiempo de cada carril
    {
      if (segundos < 10)      
      {
        lcd.setCursor(14,0);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(13,0);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(12,0);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(13,1);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(12,1);
        lcd.print(v);
      }
    }

    if (rata6 == 0)  //Display velocidad y tiempo de cada carril
    {
      if (segundos < 10)      
      {
        lcd.setCursor(17,2);
      } 
      if (segundos >= 10)     
      {
        lcd.setCursor(16,2);       
      }
      if (segundos >= 100)     
      {
        lcd.setCursor(15,2);      
      }
      lcd.print(segundos);
      if (v<10)
      {
        lcd.setCursor(16,3);
        lcd.print(v);
      }
      else if (v>=10 and v<100)
      {
        lcd.setCursor(15,3);
        lcd.print(v);
      }
    }
        
    if (rata1==1 and rata2==1 and rata3==1 and rata4==1 and rata5==1 and rata6==1)
    { 
      estado = 5;
      segundos = 0; 
    }
  }
  
}



void loop() {
  switch (estado)
  {
    case 0:             // Menú principal
      rata1 = 0;
      rata2 = 0;
      rata3 = 0;
      rata4 = 0;
      rata5 = 0;
      rata6 = 0;
      menu();
      estado = 1;
    break;

    case 1:            // Selección de variables a traves del menú
      leer_pulso();
      leer_encoder();
      seleccion();
    break;

    case 2:             // Display del mensaje de preparación del experimento
      leer_pulso();
      leer_encoder();
      espera();
    break;
   
    case 3:             // Display de la pantalla de información sobre el experimento según el modo escogido
      if (Modo == 0)    // Modo Estandar
      {
        lcd.setCursor(0,0);
        lcd.print("000s");
        lcd.setCursor(0,1);
        lcd.print("00rpm"); 
        lcd.setCursor(6,0);
        lcd.print("000s"); 
        lcd.setCursor(6,1);
        lcd.print("00rpm");  
        lcd.setCursor(12, 0);
        lcd.print("000s");
        lcd.setCursor(12, 1);
        lcd.print("00rpm");
        lcd.setCursor(3, 2);
        lcd.print("000s");
        lcd.setCursor(3, 3);
        lcd.print("00rpm");
        lcd.setCursor(9, 2);
        lcd.print("000s"); 
        lcd.setCursor(9, 3);
        lcd.print("00rpm");
        lcd.setCursor(15, 2);
        lcd.print("000s");
        lcd.setCursor(15, 3);
        lcd.print("00rpm");
        
      }
      else if (Modo == 1) // Modo Rampa
      {
        v = vini;
        lcd.setCursor(0,0);
        lcd.print("000s");
        lcd.setCursor(0,1);
        lcd.print("00rpm"); 
        lcd.setCursor(6,0);
        lcd.print("000s"); 
        lcd.setCursor(6,1);
        lcd.print("00rpm");  
        lcd.setCursor(12, 0);
        lcd.print("000s");
        lcd.setCursor(12, 1);
        lcd.print("00rpm");
        lcd.setCursor(3, 2);
        lcd.print("000s");
        lcd.setCursor(3, 3);
        lcd.print("00rpm");
        lcd.setCursor(9, 2);
        lcd.print("000s");
        lcd.setCursor(9, 3);
        lcd.print("00rpm");
        lcd.setCursor(15, 2);
        lcd.print("000s");
        lcd.setCursor(15, 3);
        lcd.print("00rpm");
      }
      estado = 4;
    break;

    case 4:           // Inicio del experimento
      inicio = 1;
      experimento();
      leer_pulso();
      leer_encoder();
    break;

    case 5:           // Display de resultados
      inicio = 0;
      leer_pulso();
      if (pulsador == true)
      {
        Ta = 0;
        fila = 0;
        v = 0;
        estado = 0;
      }
    break;
  }

}
