#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

// Definición de la clase Reloj
class Reloj {
public:
  int horaMostrar;
  int hora;
  int minutos;
  int alarma;
  bool sonar;

  // Constructor
  Reloj() {
    hora = 0;
    minutos = 0;
    alarma = 0;
    sonar = false;
  }

  // Métodos para establecer y obtener los valores
  void setDisplay(){
    String strHora = String(hora);
    String strMinutos = String(minutos);
    String strHoraMostrar = strHora + strMinutos;
    horaMostrar = strHoraMostrar.toInt();
  }

  int getDisplay(){
    return horaMostrar;
  }

  void setHora(int h) {
    hora = h;
    setDisplay();
  }

  int getHora() {
    return hora;
  }

  void setMinutos(int m) {
    minutos = m;
    setDisplay();
  }

  int getMinutos() {
    return minutos;
  }

  void setAlarma(int a) {
    alarma = a;
  }

  int getAlarma() {
    return alarma;
  }

  void setSonar(bool s) {
    sonar = s;
  }

  bool getSonar() {
    return sonar;
  }
};

Reloj miReloj;

int min = 0;
int hora = 0;




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
//int hora = 00;
char strHora[5];
char caracter = 'M';

bool ejecutarCada(int tiempo) {
  if (millis() - tiempoInicio >= tiempo) {
    tiempoInicio = millis();
    return true;
  } else {
    return false;
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


  if (ejecutarCada(100)) {
    if (min >= 60) {
      min = 0;
    } else {
      min++;
    }
    miReloj.setMinutos(min);
  }









  // Convertir la hora a una cadena de texto

  sprintf(strHora, "%04d", miReloj.getDisplay());

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