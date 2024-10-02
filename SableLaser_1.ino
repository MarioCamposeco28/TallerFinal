#include <Wire.h>                   // Libreria para usar el protocolo i2c
#include <SparkFun_ADXL345.h>        // Libreria para usar el acelerometro ADXL345
#include <SoftwareSerial.h>          // Libreria para emular otro puerto serie
#include <DFRobotDFPlayerMini.h>     // Libreria para usar el reproductor MP3
#include <Adafruit_NeoPixel.h>       // Libreria para usar los NeoPixels

// Constructores
ADXL345 acc_sismo = ADXL345();           // Constructor para usar el acelerometro
SoftwareSerial mp3_sismo(3, 4);          // RX 3 y TX 4
DFRobotDFPlayerMini reproductor_MP3;     // Constructor para el reproductor MP3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(50, 11, NEO_GRB + NEO_KHZ800); 

// Variables
unsigned int acc_X;         // Aceleración en el eje X
unsigned int acc_Y;         // Aceleración en el eje Y
unsigned int acc_Z;         // Aceleración en el eje Z
float magnitud_acc;         // Magnitud del vector aceleración
int last_mapeo = -1;        // Almacena el último valor de mapeo para evitar repetir canciones
const int switchPin = 6;    // Pin donde se configura el interruptor
bool cancion1_reproducida = false; // Variable para verificar si la canción 1 ya se reprodujo

void setup() {
  Serial.begin(9600);             // Inicio la comunicación serial del arduino hacia la computadora
  mp3_sismo.begin(9600);          // Inicio la comunicación serial emulada para el mp3
  acc_sismo.powerOn();            // Enciendo el acelerómetro
  acc_sismo.setRangeSetting(8);    // Configuro la sensibilidad del acelerómetro

  pinMode(switchPin, INPUT);       

  
  strip.begin();
  strip.show();                    

  // Compruebo si hay conexión con el módulo MP3
  if (!reproductor_MP3.begin(mp3_sismo)) { 
    Serial.println("No he podido comunicarme con el módulo MP3");
    while (1);
  }
  reproductor_MP3.volume(30);      
  Serial.println("Comunicación exitosa con el reproductor MP3");
}

void loop() {
  int switchState = digitalRead(switchPin);  // Leo el estado del interruptor

  if (switchState == HIGH) {
    encenderNeoPixels();            

    if (!cancion1_reproducida) {
      reproductor_MP3.play(1);     
      Serial.println("Reproduzco la primera canción");
      cancion1_reproducida = true; 
      delay(2500);                 
    }

    
    acc_sismo.readAccel(&acc_X, &acc_Y, &acc_Z);
    
    magnitud_acc = sqrtf((acc_X * acc_X) + (acc_Y * acc_Y) + (acc_Z * acc_Z));
    int mapeo_intensidad = map(magnitud_acc, 47, 75, 0, 10);

    
    Serial.print("Magnitud: ");
    Serial.print(magnitud_acc);
    Serial.print(" | Mapeo Intensidad: ");
    Serial.println(mapeo_intensidad);

    // Solo ejecuta el código si el mapeo cambió
    if (mapeo_intensidad != last_mapeo) {
      if (mapeo_intensidad >= 0 && mapeo_intensidad < 6) {
        reproductor_MP3.play(2);    
        Serial.println("Reproduzco la segunda canción");
      } 
      else if (mapeo_intensidad >= 6 && mapeo_intensidad < 8) {
        reproductor_MP3.play(3);    
        Serial.println("Reproduzco la tercera canción");
      }
      last_mapeo = mapeo_intensidad;  // Actualiza el último mapeo
    } 
    else {
      Serial.println("Mismo rango, no se cambia la canción.");
    }
  } 
  else {
    reproductor_MP3.stop();         // Detener cualquier reproducción
    apagarNeoPixels();              
    cancion1_reproducida = false;   // Resetea el estado para que se pueda reproducir la canción 1 de nuevo
    Serial.println("Sistema en espera, interruptor en LOW. MP3 apagado.");
  }

  delay(500);  
}

void encenderNeoPixels() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0));  
    strip.show();
    delay(100);  
  }
}

void apagarNeoPixels() {
  for (int i = strip.numPixels() - 1; i >= 0; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));    
    strip.show();
    delay(100);  
  }
}














