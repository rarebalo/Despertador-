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

// Datos de la imagen de la cara feliz
const uint8_t caraFeliz[] PROGMEM = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// Datos de la imagen de la cara triste
const uint8_t caraTriste[] PROGMEM = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

// Datos de la imagen de la cara durmiendo
const uint8_t caraDurmiendo[] PROGMEM = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01011010,
    B00111100
};

// Datos de la imagen de la media luna
const uint8_t mediaLuna[] PROGMEM = {
    B00001100,
    B00011110,
    B00110011,
    B01100001,
    B01100001,
    B00110011,
    B00011110,
    B00001100
};

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
    // Supongamos que esta es la hora actual en formato de 24 horas
    int hora = 1234; // 12:34

    // Convertir la hora a una cadena de texto
    char strHora[5];
    sprintf(strHora, "%04d", hora);

    // Limpiar el display
    matrix.fillScreen(LOW);

    // Dibujar cada dígito de la hora
    for (int i = 0; i < 4; i++) {
        matrix.drawChar(i * 6, 0, strHora[i], HIGH, LOW, 1);
    }

    // Actualizar el display
    matrix.write();

    // Esperar un segundo antes de actualizar la hora
    delay(1000);
}
