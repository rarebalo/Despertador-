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

void setup() {
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(button5, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);

    matrix.setIntensity(0);
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
    matrix.write();
}

void loop() {
    // Supongamos que esta es la hora actual en formato de 24 horas
    int hora = 5678; // 12:34

    // Convertir la hora a una cadena de texto
    char strHora[5];
    sprintf(strHora, "%04d", hora);

    // Limpiar el display
    matrix.fillScreen(LOW);

    // Dibujar cada dígito de la hora
    for (int i = 0; i < 4; i++) {
        matrix.drawChar(i * 6, 0, strHora[i], HIGH, LOW, 1);
    }

    // Supongamos que 'M' es el carácter que quieres mostrar
    char caracter = 'M';

    // Dibujar el carácter en el último módulo
    matrix.drawChar(4 * 6 + 2, 0, caracter, HIGH, LOW, 1);

    // Actualizar el display
    matrix.write();

    // Esperar un segundo antes de actualizar la hora
    delay(1000);
}