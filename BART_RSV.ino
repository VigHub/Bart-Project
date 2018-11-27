#include <MD25Driver.h>
#include <Ultrasonic.h>
#include <TimerOne.h>

MD25Driver robot;
Ultrasonic sonar_sx(3, 2); // sonar sinistro connesso al digital pin 3 and 2
Ultrasonic sonar_dx(5, 4); // sonar destro connesso al digital pin 5 and 4
int d_dx, d_sx;            // distanza misurata dai sonar
int linea_dx, linea_sx;    // valori dei sensori di luminosità destro e sinistro
int stato;
int contaIncroci; // contiamo da 0

// pin
int pinDestro = 6;   // pin seguiLinea destro
int pinSinistro = 8; // pin seguiLinea sinistro

// COSTANTI
int BIANCO = LOW;
int NERO = HIGH;

bool firstTime; //flag per fare le prime volte ai passaggi di stato
bool terminato; // flag per capire se si è concluso il circuito

const int DISTANZA_MINIMA_MURO_INIZIO_CM = 9;
const int DISTANZA_MASSIMA_MURO_INIZIO_CM = 13;

void seguiMuro()
{
  //d_dx = sonar_dx.Ranging(CM);
  d_sx = sonar_sx.Ranging(CM);
  linea_dx = digitalRead(pinDestro);
  linea_sx = digitalRead(pinSinistro);
  if (linea_dx == NERO && linea_sx == NERO)
  {
    contaIncroci = 1;
    return;
  }

  if (d_sx < DISTANZA_MINIMA_MURO_INIZIO_CM)
  {
    robot.setSpeed(20, 10);
  }
  else if (d_sx > DISTANZA_MASSIMA_MURO_INIZIO_CM)
  {
    robot.setSpeed(10, 20);
  }
  else
  {
    robot.setSpeed(20, 20);
  }
}

void seguiLinea()
{

  linea_dx = digitalRead(pinDestro);
  linea_sx = digitalRead(pinSinistro);
  if (linea_sx == NERO && linea_dx == NERO)
  {
    robot.setSpeed(20, 20);
  }
  else if (linea_sx == NERO && linea_dx == BIANCO)
  {
    robot.setSpeed(20, -15);
  }
  else if (linea_sx == BIANCO && linea_dx == NERO)
  {
    robot.setSpeed(-15, 20);
  }
  else if (linea_sx == BIANCO && linea_dx == BIANCO)
  {
    if (firstTime)
      firstTime = false;
    else
    {
      if (contaIncroci == 3)
      {
        robot.setSpeed(25, -25);
        delay(200);
      }
      if (contaIncroci == 4)
      {
        do
        {
          d_dx = sonar_dx.Ranging(CM);
          robot.setSpeed(25, 25);
        } while (d_dx < 40);
        robot.setSpeed(30, -15);
        delay(100);
        do
        {
          d_dx = sonar_dx.Ranging(CM);
          robot.setSpeed(25, 25);
        } while (d_dx < 15);
        while (true)
        {
          robot.setSpeed(0, 0);
        }
      }
      contaIncroci++;
      firstTime = true;
      return;
    }
  }
}

void setup()
{
  robot.initialize(); // inizializzo BART
  firstTime = true;
  terminato = false;
  stato = 0;
  contaIncroci = 0;
  Serial.begin(9600);
  pinMode(pinSinistro, INPUT);
  pinMode(pinDestro, INPUT);
}

void loop()
{
  switch (contaIncroci)
  {
  case 0:
    seguiMuro();
    break;
  case 1:
    seguiLinea();
    break;
  case 2:
    do
    {
      d_dx = sonar_dx.Ranging(CM);
      robot.setSpeed(25, 25);
    } while (d_dx > 20);
    robot.setSpeed(-25, 25);
    delay(100);
    do
    {
      d_dx = sonar_dx.Ranging(CM);
      robot.setSpeed(25, 25);
    } while (d_dx > 30);
    robot.setSpeed(-25, 25);
    delay(50);
    do
    {
      linea_dx = digitalRead(pinDestro);
      linea_sx = digitalRead(pinSinistro);
      robot.setSpeed(20, 20);
    } while (linea_dx == BIANCO || linea_sx == BIANCO);
    contaIncroci = 3;
    break;
  case 3:
    seguiLinea();
    break;
  case 4:
    seguiLinea();
  }
}
