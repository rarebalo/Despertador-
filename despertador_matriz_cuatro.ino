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

int alarmaMin = 0;
int alarmaHora = 0;

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
bool visualizacionSegundos = false;
int contadorSegundos = 0;
bool entraPrimeraVez = true;
int caminoSegundo = 3;
bool incrementando = true;
bool visualizacionSegundosAnterior = false;

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
    actualizarHora();
    time_t tiempoActual = now();
    miReloj.setMinutos(minute(tiempoActual));
    miReloj.setHora(hour(tiempoActual));
    caracter = diasDeLaSemana[weekday(tiempoActual) - 1];
    if (contadorSegundos < 1) {
      contadorSegundos++;
    } else {
      contadorSegundos = 0;
      visualizacionSegundos = !visualizacionSegundos;
    }
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

  if (miReloj.sonar) {
    pantallaDeError();
  }

  pilotoDelSegundo();
  
  if (dolorDeCabeza) {
    matrix.drawPixel(8, 7, HIGH);
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
  matrix.drawPixel(24, 7, HIGH);
  matrix.drawPixel(30, 7, HIGH);
  matrix.drawPixel(8, 7, HIGH);
  matrix.write();
}

void cambiarModo() {
  if (modo > 5) {
    modo = 0;
  } else {
    modo++;
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
  setup();
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

  if (miReloj.minutosAlarma != miReloj.minutos) {
    miReloj.setSonar(EEPROM.read(20));
  }
}

void guardarAlarmaEeprom() {
  if (alarmaHora != EEPROM.read(0)) {
    EEPROM.put(0, alarmaHora);
  }
  if (alarmaMin != EEPROM.read(10)) {
    EEPROM.put(10, alarmaMin);
  }
  if (miReloj.sonar != EEPROM.read(20)) {
    EEPROM.put(20, miReloj.sonar);
  }
}
void actualizarHora() {
  DateTime now = rtc.now();
  setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
}

void mostrarTemperatura() {
  float temperaturaFloat = rtc.getTemperature();               // Obtener la temperatura como float
  int temperaturaEntera = static_cast<int>(temperaturaFloat);  // Convertir el float a entero

  // Calcular la parte decimal multiplicando por 100 y restando la parte entera
  int temperaturaDecimal = static_cast<int>((temperaturaFloat - temperaturaEntera) * 100);

  char stringAMostrar[6];
  char srtAMostrar[6];

  snprintf(srtAMostrar, sizeof(srtAMostrar), "%02d%02d", temperaturaEntera, temperaturaDecimal);
  strncpy(stringAMostrar, srtAMostrar, sizeof(stringAMostrar));

  matrix.fillScreen(LOW);
  for (int i = 0; i < 5; i++) {
    matrix.drawChar(i * 6, 0, stringAMostrar[i], HIGH, LOW, 1);
  }
  matrix.drawPixel(10, 6, HIGH);
  matrix.drawChar(4 * 6 + 2, 0, 'C', HIGH, LOW, 1);
  matrix.write();
}
void finDeSonido() {
  EasyBuzzer.beep(500, 3);
}

void modificarBrillo() {

  char numeroBrillo[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15' };
  if (presionandoBtn(button4) || entraPrimeraVez) {
    if (!entraPrimeraVez) {
      brillo++;
    }
    entraPrimeraVez = false;
    matrix.fillScreen(LOW);
    matrix.setIntensity(brillo);

    for (int i = 0; i < brillo * 2; i++) {
      for (int o = 0; o < 8; o++) {
        matrix.drawPixel(i, o, HIGH);
      }
    }
    matrix.drawPixel(8, 7, HIGH);
    matrix.drawChar(4 * 6 + 2, 0, numeroBrillo[brillo], HIGH, LOW, 1);
    matrix.write();
    if (brillo == 10) {
      brillo = 0;
      matrix.fillScreen(LOW);
      matrix.setIntensity(brillo);
      matrix.drawPixel(8, 7, HIGH);
      matrix.drawChar(4 * 6 + 2, 0, numeroBrillo[brillo], HIGH, LOW, 1);
      matrix.write();
    }
  }
}

void limpiarPantalla() {
  matrix.fillScreen(LOW);
  matrix.write();
  entraPrimeraVez = true;
}

void pantallaDeError() {
  if (random(0, 2) == 1) {
    matrix.drawPixel(random(0, 32), random(0, 8), HIGH);
  } else {
    matrix.drawPixel(random(0, 32), random(0, 8), LOW);
  }
  matrix.write();
}

void pilotoDelSegundo(){
    if (visualizacionSegundos != visualizacionSegundosAnterior) { 
    visualizacionSegundosAnterior = visualizacionSegundos;       

    if (visualizacionSegundos) {
      matrix.drawPixel(24, caminoSegundo, LOW);  

      if (incrementando) {
        caminoSegundo++;  
      } else {
        caminoSegundo--;  
      }

      if (caminoSegundo == 2 || caminoSegundo == 5) {
        incrementando = !incrementando; 
      }

      matrix.drawPixel(24, caminoSegundo, HIGH); 
    }
  }
  matrix.drawPixel(24, caminoSegundo, HIGH);
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
    alarmaHora = EEPROM.read(0);
    alarmaMin = EEPROM.read(10);
    miReloj.setSonar(EEPROM.read(20));
  }
  if (millis() - esperaEntreSonidos > 3000) {
    esperaEntreSonidos = millis();
    EasyBuzzer.beep(1500, 200, 50, 3, 500, 1, finDeSonido);
  }
}

void loop() {

  if (presionandoBtn(button3)) {
    cambiarModo();
    limpiarPantalla();
  }

  switch (modo) {
    case 0:
      matrix.setIntensity(brillo);
      pantallaHora();
      monitoreoAlarma();
      //pantallaDeError();
      break;
    case 1:
      //no anda ajustar hora
      /*matrix.setIntensity(3);
      ajustarHora();
      pantallaHora();*/
      pantallaDeError();
      break;
    case 2:
      matrix.setIntensity(1);
      ajustarAlarma();
      pantallaAlarma();
      break;
    case 3:
      guardarAlarmaEeprom();
      modo++;
      break;
    case 4:
      mostrarTemperatura();
      break;
    case 5:
      modificarBrillo();
      break;
    default:
      modo = 0;
      matrix.fillScreen(LOW);
      matrix.drawChar(4 * 6 + 2, 0, 'X', HIGH, LOW, 1);
      matrix.write();
      break;
  }
  EasyBuzzer.update();
}