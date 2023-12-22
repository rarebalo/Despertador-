#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

const int pinCS = 10;
const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const int buzzer = 9;
const int button1 = 3;
const int button2 = 4;
const int button3 = 5;
const int button4 = 6;
const int button5 = 2;

bool durmiendo = false;
bool triste = false;
int brillo = 0;
int incrementoBrillo = 1;

void setup() {
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(button5, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);

    matrix.setIntensity(brillo);
    matrix.setPosition(0 ,0 ,0 );
    matrix.setPosition(1 ,1 ,0 );
    matrix.setPosition(2 ,2 ,0 );
    matrix.setPosition(3 ,3 ,0 );

    matrix.setRotation(0 ,1 );
    matrix.setRotation(1 ,1 );
    matrix.setRotation(2 ,1 );
    matrix.setRotation(3 ,1 );

    // Mostrar la cara feliz al inicio
    matrix.fillScreen(LOW); // Limpiar el display
    matrix.drawBitmap(0,0,caraFeliz ,8 ,8 ,HIGH );
    matrix.write();
}

void loop() {
     // Cambiar el brillo del display gradualmente
     if (!triste) {
         brillo += incrementoBrillo;
         if (brillo >= (durmiendo ? -3 :15)) {
             incrementoBrillo = -1;
         } else if (brillo <= (durmiendo ? -7 :-7)) {
             incrementoBrillo = +1;
         }
         matrix.setIntensity(brillo + (durmiendo ? -3 :0));
     } else {
         matrix.setIntensity(-15);
     }
     
     // Verificar si se presionó el botón para cambiar a la cara triste
     if (digitalRead(button5) == LOW && !durmiendo) {
         delay(20); // Agregar un pequeño retardo para estabilizar las señales
         if (digitalRead(button5) == LOW) {
             // Mostrar la cara triste
             triste = true;
             matrix.fillScreen(LOW); // Limpiar el display
             matrix.drawBitmap(0,0,caraTriste ,8 ,8 ,HIGH );
             matrix.write();
             // Emitir un sonido triste
             tone(buzzer ,300 ,500 );
             delay(500);
             tone(buzzer ,250 ,500 );
             delay(500); 
             // Esperar durante cinco segundos
             delay(5000);
             // Volver a mostrar la cara feliz
             triste = false;
             matrix.fillScreen(LOW); // Limpiar el display
             matrix.drawBitmap(0,0,caraFeliz ,8 ,8 ,HIGH );
             matrix.write();
             // Emitir un sonido feliz
             tone(buzzer ,500 ,500 );
             delay(500);
             tone(buzzer ,600 ,500 );
             delay(500); 
         }
     }
     // Verificar si se presionó el botón para cambiar a la cara durmiendo o para despertar
     if (digitalRead(button4) == LOW) {
         delay(20); // Agregar un pequeño retardo para estabilizar las señales
         if (digitalRead(button4) == LOW) {
             if (!durmiendo) {
                 // Mostrar la cara durmiendo
                 matrix.fillScreen(LOW); // Limpiar el display
                 matrix.drawBitmap(0,0,caraDurmiendo ,8 ,8 ,HIGH );
                 matrix.drawBitmap(16, 0, mediaLuna, 8, 8, HIGH);
                 matrix.write();
                 // Emitir un sonido de dormir
                 tone(buzzer ,200 ,500 );
                 delay(500);
                 tone(buzzer ,150 ,500 );
                 delay(500); 
                 durmiendo = true;
             } else {
                 // Mostrar la cara feliz
                 matrix.fillScreen(LOW); // Limpiar el display
                 matrix.drawBitmap(0,0,caraFeliz ,8 ,8 ,HIGH );
                 matrix.write();
                 // Emitir un sonido feliz
                 tone(buzzer ,500 ,500 );
                 delay(500);
                 tone(buzzer ,600 ,500 );
                 delay(500); 
                 durmiendo = false;
             }
         }
     }
     delay(durmiendo ? 100 : 10);
}
