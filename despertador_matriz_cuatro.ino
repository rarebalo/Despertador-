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
unsigned long tiempoInicio = millis();
int hora = 0000;
char strHora[5];
 char caracter = 'M';

int ejecutarCada(int tiempo, int valor) {
  if (millis() - tiempoInicio >= tiempo) {
    valor++;
    tiempoInicio = millis();
    return valor;
  } else {
    return valor;
  }
}


void setup() {
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  matrix.setIntensity(0);
  matrix.setPosition(0, 0, 0);
  matrix.setPosition(1, 1, 0);
  matrix.setPosition(2, 2, 0);
  matrix.setPosition(3, 3, 0);

  matrix.setRotation(0, 1);
  matrix.setRotation(1, 1);
  matrix.setRotation(2, 1);
  matrix.setRotation(3, 1);

  // Mostrar la cara feliz al inicio
  matrix.fillScreen(LOW);  // Limpiar el display
  matrix.write();
}

void loop() {

  if(hora>9999){
    hora = 0000;
  }

  hora = ejecutarCada(50, hora);


    // Convertir la hora a una cadena de texto
    
  sprintf(strHora, "%04d", hora);

  // Limpiar el display
  matrix.fillScreen(LOW);

  // Dibujar cada dígito de la hora
  for (int i = 0; i < 4; i++) {
    matrix.drawChar(i * 6, 0, strHora[i], HIGH, LOW, 1);
  }

  // Supongamos que 'M' es el carácter que quieres mostrar
 

  // Dibujar el carácter en el último módulo
  matrix.drawChar(4 * 6 + 2, 0, caracter, HIGH, LOW, 1);

  // Actualizar el display
  matrix.write();

  // Esperar un segundo antes de actualizar la hora
  //delay(10);
}