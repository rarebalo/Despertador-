#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <TimeLib.h>
#include <EasyBuzzer.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>
//#include <IRremote.h>
/*
int pinIR = 2; // Puerto de entrada para el módulo infrarrojo
IRsend irsend;
*/


class Reloj {
public:
  char horaMostrar[5];
  char alarmaMostrar[5];
  byte hora;
  byte minutos;
  byte horaAlarma;
  byte minutosAlarma;
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

  void setHora(byte h) {
    hora = h;
    setDisplay();
  }

  byte getHora() {
    return hora;
  }

  void setHoraAlarma(byte h) {
    horaAlarma = h;
    setDisplayAlarma();
  }

  byte getHoraAlarma() {
    return horaAlarma;
  }

  void setMinutos(byte m) {
    minutos = m;
    setDisplay();
  }

  byte getMinutos() {
    return minutos;
  }

  void setMinutosAlarma(byte m) {
    minutosAlarma = m;
    setDisplayAlarma();
  }

  byte getMinutosAlarma() {
    return minutosAlarma;
  }

  void setSonar(bool s) {
    sonar = s;
  }

  bool getSonar() {
    return sonar;
  }
};

class Jugador {
  public:
    String nombre;
    int partidasJugadas;
    int partidasPerdidas;
    int partidasGanadas;
    int puntosPartidaActual;
    int rachaVictorias;
    int puntosTotales;

    Jugador(String _nombre) {
      nombre = _nombre;
      partidasJugadas = 0;
      partidasPerdidas = 0;
      partidasGanadas = 0;
      puntosPartidaActual = 0;
      rachaVictorias = 0;
      puntosTotales = 0;
    }

    void ganarPartida() {
      if (puntosPartidaActual >= 11 && puntosPartidaActual - puntosPartidaActual >= 2) {
        partidasJugadas++;
        partidasGanadas++;
        rachaVictorias++;
        puntosTotales += puntosPartidaActual;
        reiniciarPuntosPartida();
      }
    }

    void perderPartida() {
      if (puntosPartidaActual <= 9 || puntosPartidaActual - puntosPartidaActual < 2) {
        partidasJugadas++;
        partidasPerdidas++;
        rachaVictorias = 0;
        puntosTotales += puntosPartidaActual;
        reiniciarPuntosPartida();
      }
    }

    void anotarPunto() {
      puntosPartidaActual++;
    }

    void reiniciarPuntosPartida() {
      puntosPartidaActual = 0;
    }

    float porcentajeVictorias() {
      if (partidasJugadas > 0) {
        return (float)partidasGanadas / partidasJugadas * 100;
      } else {
        return 0;
      }
    }
};

const byte tiempoRebote = 180;
unsigned long ultimaPresionBtn[] = { 0, 0, 0, 0, 0 };
unsigned long currentMillis = millis();

Reloj miReloj;
RTC_DS3231 rtc;

byte alarmaMin = 0;
byte alarmaHora = 0;

const byte pinCS = 10;
const byte numberOfHorizontalDisplays = 4;
const byte numberOfVerticalDisplays = 1;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const byte buzzer = 9;
const byte button0 = 3;
const byte button1 = 4;
const byte button2 = 5;
const byte button3 = 6;
const byte button4 = 2;
unsigned long tiempoInicio = millis();
unsigned long esperaEntreSonidos = 1000;
byte modo = 0;
byte brillo = 0;
char caracter = 'E';
char diasDeLaSemana[] = { 'D', 'L', 'M', 'M', 'J', 'V', 'S' };
bool configInicial = true;
bool dolorDeCabeza = false;
bool vueltaUnica = true;
bool ultimaConfigAlarma = false;
bool visualizacionSegundos = false;
byte contadorSegundos = 0;
bool entraPrimeraVez = true;
byte caminoSegundo = 3;
bool incrementando = true;
bool visualizacionSegundosAnterior = false;
int horaAjus;
int minAjus;
int segAjus;
int anioAjus;
int mesAjus;
int diaAjus;
bool cargarVariablesAjusHora = true;
bool configHoraManual = false;
int filaAjus;
int columnaAjus;
unsigned long tiempoTriangulo = millis();
unsigned long tiempoSegundos = millis();
bool seMuestra = true;
unsigned long tiempoCrono = millis();
bool vueta = true;
bool pararCrono = true;
int centecimaCrono = 0;
int segundoCrono = 0;
int minutoCrono = 0;
unsigned long volverInicio = millis();

int trianguloUno = random(23, 32);
int trianguloDos = random(23, 32);
int trianguloTres = random(23, 32);
int trianguloUnoY = random(0, 8);
int trianguloDosY = random(0, 8);
int trianguloTresY = random(0, 8);

char nombreChar[6] = ""; 
int indiceActual = 0; 
char letraActual = 'A';

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
void mostrarAjusHora() {
  char strajusHoraMostrar[5];
  char ajusHoraMostrar[5];
  snprintf(strajusHoraMostrar, sizeof(strajusHoraMostrar), "%02d%02d", horaAjus, minAjus);
  strncpy(ajusHoraMostrar, strajusHoraMostrar, sizeof(ajusHoraMostrar));
  matrix.fillScreen(LOW);
  for (int i = 0; i < 4; i++) {
    matrix.drawChar(i * 6, 0, ajusHoraMostrar[i], HIGH, LOW, 1);
  }
  formasDelTriangulo();
  matrix.drawTriangle(trianguloUno, trianguloUnoY, trianguloDos, trianguloDosY, trianguloTres, trianguloTresY, HIGH);
  matrix.write();
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
    volverInicio = millis();
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

  cargarVarAjusHora();
  if (presionandoBtn(button4)) {
    horaAjus++;
    if (horaAjus > 23) {
      horaAjus = 0;
    }
    configHoraManual = true;
  }

  if (presionandoBtn(button1)) {
    minAjus++;
    if (minAjus > 59) {
      minAjus = 0;
    }
    configHoraManual = true;
  }

  if (presionandoBtn(button0)) {
    horaAjus--;
    if (horaAjus < 0) {
      horaAjus = 23;
    }
    configHoraManual = true;
  }

  if (presionandoBtn(button2)) {
    minAjus--;
    if (minAjus < 0) {
      minAjus = 59;
    }
    configHoraManual = true;
  }
  mostrarAjusHora();
}

void actualizarHoraManual() {
  if (configHoraManual) {
    configHoraManual = false;
    rtc.adjust(DateTime(anioAjus, mesAjus, diaAjus, horaAjus, minAjus, segAjus));
  }
  cargarVariablesAjusHora = true;
}

void cargarVarAjusHora() {
  if (cargarVariablesAjusHora) {
    cargarVariablesAjusHora = false;
    time_t tiempoActual = now();
    anioAjus = year(tiempoActual);
    mesAjus = month(tiempoActual);
    diaAjus = weekday(tiempoActual);
    horaAjus = hour(tiempoActual);
    minAjus = minute(tiempoActual);
    segAjus = second(tiempoActual);
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
    matrix.drawPixel(random(0, 32), random(0, 8), HIGH);
  }

  pilotoDelSegundo();

  if (dolorDeCabeza) {
    matrix.drawPixel(8, 7, HIGH);
  }

  visualizacionSegundosTradi();

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
  if (brillo == 10) {
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
  float temperaturaFloat = rtc.getTemperature();
  int temperaturaEntera = static_cast<int>(temperaturaFloat);

  int temperaturaDecimal = static_cast<int>((temperaturaFloat - temperaturaEntera) * 100);

  char stringAMostrar[6];
  char srtAMostrar[6];

  snprintf(srtAMostrar, sizeof(srtAMostrar), "%02d%02d", temperaturaEntera, temperaturaDecimal);
  strncpy(stringAMostrar, srtAMostrar, sizeof(stringAMostrar));

  matrix.fillScreen(LOW);
  for (int i = 0; i < 5; i++) {
    matrix.drawChar(i * 6, 0, stringAMostrar[i], HIGH, LOW, 1);
  }
  matrix.drawPixel(11, 6, HIGH);
  matrix.drawChar(4 * 6 + 2, 0, 'C', HIGH, LOW, 1);
  matrix.write();
}
void finDeSonido() {
  EasyBuzzer.beep(500, 3);
}

void modificarBrillo() {
  static bool entraPrimeraVez = true;
  static int brillo = 0;

  if (presionandoBtn(button4) || entraPrimeraVez) {
    if (!entraPrimeraVez) {
      brillo = (brillo + 1) % 10;
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
    matrix.drawChar(4 * 6 + 2, 0, '0' + brillo, HIGH, LOW, 1);
    matrix.write();
  }
}

void limpiarPantalla() {
  matrix.fillScreen(LOW);
  matrix.write();
  entraPrimeraVez = true;
}

void pantallaDeError() {
  if (random(2) == 1) {
    matrix.drawPixel(random(0, 32), random(0, 8), HIGH);
  } else {
    matrix.drawPixel(random(0, 32), random(0, 8), LOW);
  }
  matrix.write();
}

void pilotoDelSegundo() {
  static bool visualizacionSegundosAnterior = false;
  static int caminoSegundo = 0;
  static bool incrementando = true;

  if (visualizacionSegundos != visualizacionSegundosAnterior) {
    visualizacionSegundosAnterior = visualizacionSegundos;

    if (visualizacionSegundos) {
      matrix.drawPixel(24, caminoSegundo, LOW);

      caminoSegundo += incrementando ? 1 : -1;

      if (caminoSegundo == 2 || caminoSegundo == 5) {
        incrementando = !incrementando;
      }

      matrix.drawPixel(24, caminoSegundo, HIGH);
    }
  }
  matrix.drawPixel(24, caminoSegundo, HIGH);
}

void formasDelTriangulo() {
  if (millis() - tiempoTriangulo > 500) {
    tiempoTriangulo = millis();
    trianguloUno = puntaDeTriangulo(trianguloUno, 23, 32);
    trianguloDos = puntaDeTriangulo(trianguloDos, 23, 32);
    trianguloTres = puntaDeTriangulo(trianguloTres, 23, 32);
    trianguloUnoY = puntaDeTriangulo(trianguloUnoY, 0, 8);
    trianguloDosY = puntaDeTriangulo(trianguloDosY, 0, 8);
    trianguloTresY = puntaDeTriangulo(trianguloTresY, 0, 8);
  }
}

int puntaDeTriangulo(int triangulo, int limiteBajo, int limiteAlto) {
  if (triangulo > limiteAlto || triangulo < limiteBajo) {
    if (random(0, 2) == 0) {
      return triangulo++;
    } else {
      return triangulo--;
    }
  } else {
    return triangulo = random(limiteBajo, limiteAlto);
  }
}

void visualizacionSegundosTradi() {
  static unsigned long tiempoSegundos = 0;
  static bool seMuestra = false;
  static int columnaRam = 0;

  if (millis() - tiempoSegundos > 1000) {
    tiempoSegundos = millis();
    seMuestra = !seMuestra;
    columnaRam = random(0, 7);
  }

  matrix.drawPixel(11, columnaRam, seMuestra ? HIGH : LOW);
}

void crono() {
  static unsigned long tiempoCrono = 0;
  static int centecimaCrono = 0;
  static int segundoCrono = 0;
  static int minutoCrono = 0;
  static bool pararCrono = false;

  if (presionandoBtn(button4)) {
    pararCrono = !pararCrono;
  }

  if (presionandoBtn(button0)) {
    centecimaCrono = 0;
    segundoCrono = 0;
    minutoCrono = 0;
  }

  if (!pararCrono && millis() - tiempoCrono > 10) {
    tiempoCrono = millis();
    if (++centecimaCrono >= 100) {
      centecimaCrono = 0;
      if (++segundoCrono >= 60) {
        segundoCrono = 0;
        if (++minutoCrono >= 100) {
          minutoCrono = 0;
        }
      }
    }
  }
  pantallaCrono();
}

void pantallaCrono() {
  char strCrono[8];
  snprintf(strCrono, sizeof(strCrono), "%02d%02d%02d", minutoCrono, segundoCrono, centecimaCrono);
  matrix.fillScreen(LOW);
  for (int i = 0; i < 8; i++) {
    matrix.drawChar(i * 6, 0, strCrono[i], HIGH, LOW, 1);
  }
  matrix.write();
}


void regresarPantallaInicial() {
  if (millis() - volverInicio > 15000) {
    volverInicio = millis();
    if (modo == 2) {
      guardarAlarmaEeprom();
    }
    modo = 0;
  }
}


void pantallaTenis(){
  //pregunta, nuevo jugador? y/n
  //y: agrego un nuevo jugador n: selecciono jugadores guardados
  //comenzar partida y/n
  //y: seleccionar jugador 1, jugador 2 n: volver a la primera pregunta
  //comienza el juego el jugador 1 saca
  //finaliza el juego: muestra las estadisticas del jugador ganador pregunta: otra partida? y/n
  //y: repite lo anterios n: vuelve al comienzo
  // debe tener un menu para encontrar un jugador y revisar lasestadisticas

}

char letraNombre(int indice){
   char alfabeto[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  return alfabeto[indice];

}

char nombreGenerado() {
  if (presionandoBtn(button1)) {
    letraActual++;
    if (letraActual > 'Z') {
      letraActual = 'A';
    }
  } else if (presionandoBtn(button2)) {
    if (indiceActual < 5) {
      nombreChar[indiceActual] = letraActual;
      indiceActual++;
      nombreChar[indiceActual] = '\0'; 
    }
  } else if (presionandoBtn(button3)) {
    if (indiceActual > 0) {
      indiceActual--;
      nombreChar[indiceActual] = '\0';
    }
  } else if (presionandoBtn(button4)) {
    return nombreChar;
  }
}

void nuevoJugador(){
  Jugador* nuevoJugador = new Jugador(String(nombreGenerado()));
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
      break;
    case 1:
      ajustarHora();
      regresarPantallaInicial();
      break;
    case 2:
      actualizarHoraManual();
      matrix.setIntensity(1);
      ajustarAlarma();
      pantallaAlarma();
      regresarPantallaInicial();
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
      regresarPantallaInicial();
      break;
    case 6:
      crono();
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