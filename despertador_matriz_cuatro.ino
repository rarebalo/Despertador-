#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <TimeLib.h>
// Definición de la clase Reloj
class Reloj {
public:
  char horaMostrar[5];
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
  void setDisplay() {
    char strHora[3];
    char strMinutos[3];
    sprintf(strHora, "%02d", hora);
    sprintf(strMinutos, "%02d", minutos);
    String strHoraMostrar = String(strHora) + String(strMinutos);
    sprintf(horaMostrar, "%s", strHoraMostrar.c_str());
  }

  char* getDisplay() {
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

char caracter = 'E';

char diasDeLaSemana[] = {'D', 'L', 'M', 'M', 'J', 'V', 'S'}; 

bool ejecutarCada(int tiempo) {
  if (millis() - tiempoInicio >= tiempo) {
    tiempoInicio = millis();
    return true;
  } else {
    return false;
  }
}

void mostrarHora() {
  for (int i = 0; i < 4; i++) {
    matrix.drawChar(i * 6, 0, miReloj.getDisplay()[i], HIGH, LOW, 1);
  }
}



void setup() {  
  setTime(6, 26, 0, 22, 12, 2023);
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

 
  matrix.fillScreen(LOW);  
  miReloj.setMinutos(minute());
  miReloj.setHora(hour());
  matrix.write();
}

void loop() {
 
  if (ejecutarCada(1000)) {
    time_t tiempoActual = now();    
    miReloj.setMinutos(minute(tiempoActual));
    miReloj.setHora(hour(tiempoActual));
    caracter = diasDeLaSemana[weekday(tiempoActual)-1];
  }
  matrix.fillScreen(LOW);
  mostrarHora();
  matrix.drawChar(4 * 6 + 2, 0, caracter, HIGH, LOW, 1);
  matrix.write();
}