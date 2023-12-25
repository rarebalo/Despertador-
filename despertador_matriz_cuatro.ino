#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <TimeLib.h>
#include <EasyBuzzer.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>



class Reloj {
public:
  char horaMostrar[5];
  char alarmaMostrar[5];
  int hora;
  int minutos;
  int horaAlarma;
  int minutosAlarma;
  bool sonar;

  Reloj() {
    hora = 0;
    minutos = 0;
    horaAlarma = 0;
    minutosAlarma = 0;
    sonar = false;
  }

  void setDisplay() {
    char strHoraMostrar[5];
    snprintf(strHoraMostrar, sizeof(strHoraMostrar), "%02d%02d", hora, minutos);
    strncpy(horaMostrar, strHoraMostrar, sizeof(horaMostrar));
  }

  void setDisplayAlarma() {
    char strAlarmaMostrar[5];
    snprintf(strAlarmaMostrar, sizeof(strAlarmaMostrar), "%02d%02d", horaAlarma, minutosAlarma);
    strncpy(alarmaMostrar, strAlarmaMostrar, sizeof(alarmaMostrar));
  }

  char* getDisplay() {
    return horaMostrar;
  }

  char* getDisplayAlarma() {
    return alarmaMostrar;
  }

  void setHora(int h) {
    hora = h;
    setDisplay();
  }

  int getHora() {
    return hora;
  }

  void setHoraAlarma(int h) {
    horaAlarma = h;
    setDisplayAlarma();
  }

  int getHoraAlarma() {
    return horaAlarma;
  }

  void setMinutos(int m) {
    minutos = m;
    setDisplay();
  }

  int getMinutos() {
    return minutos;
  }

  void setMinutosAlarma(int m) {
    minutosAlarma = m;
    setDisplayAlarma();
  }

  int getMinutosAlarma() {
    return minutosAlarma;
  }

  void setSonar(bool s) {
    sonar = s;
  }

  bool getSonar() {
    return sonar;
  }
};

const int tiempoRebote = 180;
unsigned long ultimaPresionBtn[] = { 0, 0, 0, 0, 0 };
unsigned long currentMillis = millis();

Reloj miReloj;
RTC_DS3231 rtc;

int min = 0;
int hora = 0;
int alarmaMin = 00;
int alarmaHora = 11;

const int pinCS = 10;
const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const int buzzer = 9;
const int button0 = 3;
const int button1 = 4;
const int button2 = 5;
const int button3 = 6;
const int button4 = 2;
unsigned long tiempoInicio = millis();
unsigned long esperaEntreSonidos = 1000;
int modo = 0;
int brillo = 0;
char caracter = 'E';
char diasDeLaSemana[] = { 'D', 'L', 'M', 'M', 'J', 'V', 'S' };
bool configInicial = true;
bool dolorDeCabeza = false;
bool vueltaUnica = true;
bool ultimaConfigAlarma = false;

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
void mostrarAlarma() {
  for (int i = 0; i < 4; i++) {
    matrix.drawChar(i * 6, 0, miReloj.getDisplayAlarma()[i], HIGH, LOW, 1);
  }
}

bool presionandoBtn(int btn) {
  int nBtn = 0;
  switch (btn) {
    case button0:
      nBtn = 0;
      break;
    case button1:
      nBtn = 1;
      break;
    case button2:
      nBtn = 2;
      break;
    case button3:
      nBtn = 3;
      break;
    case button4:
      nBtn = 4;
      break;
  }
  if (digitalRead(btn) == LOW) {
    currentMillis = millis();
    if (currentMillis - ultimaPresionBtn[nBtn] > tiempoRebote) {
      ultimaPresionBtn[nBtn] = currentMillis;
      return true;
    } else {
      return false;
    }

  } else {
    return false;
  }
}

void ajustarHora() {
  if (presionandoBtn(button4)) {
    adjustTime(3600);
  }

  if (presionandoBtn(button1)) {
    adjustTime(60);
  }

  if (presionandoBtn(button0)) {
    adjustTime(-3600);
  }

  if (presionandoBtn(button2)) {
    adjustTime(-60);
  }
}

void ajustarAlarma() {
  if (presionandoBtn(button4)) {
    miReloj.setSonar(!miReloj.getSonar());
    ultimaConfigAlarma = miReloj.sonar;
  }

  if (presionandoBtn(button1)) {
    if (alarmaHora < 23) {
      alarmaHora++;
    } else {
      alarmaHora = 0;
    }
  }

  if (presionandoBtn(button2)) {
    if (alarmaMin < 59) {
      alarmaMin++;
    } else {
      alarmaMin = 0;
    }
  }

  miReloj.setHoraAlarma(alarmaHora);
  miReloj.setMinutosAlarma(alarmaMin);
}

void pantallaHora() {
  if (ejecutarCada(100)) {
    time_t tiempoActual = now();
    miReloj.setMinutos(minute(tiempoActual));
    miReloj.setHora(hour(tiempoActual));
    caracter = diasDeLaSemana[weekday(tiempoActual) - 1];
  }
  if (dolorDeCabeza) {
    caracter = '*';
  }
  matrix.fillScreen(LOW);
  mostrarHora();
  if (modo == 0) {
    matrix.drawChar(4 * 6 + 2, 0, caracter, HIGH, LOW, 1);
  }
  if (modo == 1) {
    matrix.drawChar(25, 0, 'H', HIGH, LOW, 1);
  }
  matrix.write();
}

void pantallaAlarma() {
  if (ejecutarCada(100)) {
    miReloj.setMinutosAlarma(alarmaMin);
    miReloj.setHoraAlarma(alarmaHora);
    if (miReloj.getSonar()) {
      caracter = 'E';
    } else {
      caracter = 'A';
    }
  }
  matrix.fillScreen(LOW);
  mostrarAlarma();
  matrix.drawChar(25, 0, caracter, HIGH, LOW, 1);
  matrix.write();
}

void cambiarModo() {
  modo++;
  if (modo > 2) {
    modo = 0;
  }
}

bool estadoAlarma() {
  return (miReloj.horaAlarma == miReloj.hora && miReloj.minutosAlarma == miReloj.minutos && miReloj.sonar);
}

void subirBrillo() {
  brillo++;
  if (brillo == 16) {
    brillo = 0;
  }
}

void buzzerBrilloActivado() {
  subirBrillo();
  EasyBuzzer.beep(400, 1);
}

void monitoreoAlarma() {
  if (estadoAlarma()) {
    dolorDeCabeza = true;
    miReloj.setSonar(false);
  }

  if (dolorDeCabeza && presionandoBtn(button4)) {
    dolorDeCabeza = false;
    brillo = 0;
  }

  if (dolorDeCabeza) {
    buzzerBrilloActivado();
  }
}
void setup() {
  if (configInicial) {
    configInicial = false;
    EasyBuzzer.setPin(buzzer);
    pinMode(button0, INPUT_PULLUP);
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);
    matrix.setIntensity(brillo);
    matrix.setPosition(0, 0, 0);
    matrix.setPosition(1, 1, 0);
    matrix.setPosition(2, 2, 0);
    matrix.setPosition(3, 3, 0);
    matrix.setRotation(0, 1);
    matrix.setRotation(1, 1);
    matrix.setRotation(2, 1);
    matrix.setRotation(3, 1);
    matrix.fillScreen(LOW);
    matrix.write();
    Wire.begin();
    rtc.begin();
    //rtc.adjust(DateTime(__DATE__, __TIME__)); //poner en hora el reloj con la pc
    //Serial.begin(9600);
    rtc.disable32K();  //desactiva el pin de 32k
    DateTime now = rtc.now();
    setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
  }
  EasyBuzzer.beep(400, 3);
}

void loop() {

  if (presionandoBtn(button3)) {
    cambiarModo();
  }

  switch (modo) {
    case 0:
      matrix.setIntensity(brillo);
      pantallaHora();
      monitoreoAlarma();
      break;
    case 1:
      matrix.setIntensity(3);
      ajustarHora();
      pantallaHora();
      break;
    case 2:
      matrix.setIntensity(1);
      ajustarAlarma();
      pantallaAlarma();

      break;
    default:
      matrix.fillScreen(LOW);
      matrix.drawChar(4 * 6 + 2, 0, 'X', HIGH, LOW, 1);
      matrix.write();
      break;
  }
  EasyBuzzer.update();
}