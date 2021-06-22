/* 
  ELTON S. S. 
  DISCIPLINA: SISTEMAS REALIMENTADOS
  ANO: 2019
  PLATAFORMA: ARDUINO UNO | ATMEGA328P | Clock: 16 MHz
*/
#include<Servo.h>                                       //biblioteca c/ funções p/ usar o servo
#include<PID_v1.h>                                      //biblioteca c/ algoritmo do PID

// ===============================================================================
// Protótipo do Filtro de média móvel
#define      n     3                                    //número de pontos da média móvel 

long moving_average();                                  //Função para filtro de média móvel

int       leituraSensor,                                //obtêm a leitura do sensor de ultrassom
          filtrado;                                     //recebe o resultado do processo de filtragem

int       numbers[n];                                   //vetor com os valores para cálculo da média móvel
int       contador = 0;                                 //para saber se já tem os n valores estocados no vetor

// ===============================================================================


// ===============================================================================
// Dados relativos ao controle PD

float Kp = 0.625;                                             //Proporcional
float Ki = 0.0;                                               //Integral
float Kd = 0.223;                                             //Diferencial
double Setpoint, Input, Output, ServoOutput;                                       



PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);    //Cria uma instância do controlador, que irá calcular o PID

// ===============================================================================

const int servoPin = 10;                                   //declara o pino em que o servo está conectado
Servo myServo;                                             //Cria uma instância do servo

void setup() {

  Serial.begin(9600); 
  myServo.attach(servoPin);
  
  Input = readPosition();                      //chama a função que lê a posição da bolinha

  Setpoint = 19;                               //define a posição onde se quer posicionar a bola
  
  myPID.SetMode(AUTOMATIC);                    //define o modo automático do controlador PID 
  myPID.SetOutputLimits(-37,37);               //limita a saída do controlador para não forçar o servo p/ ângulos indevidos
}

void loop()
{
 

  leituraSensor = readPosition();              //lê a posição instantânea da bolinha
  filtrado = moving_average();                 //filtra os dados de posição c/ a média móvel
  Input = filtrado;                            //define como entrada do PID, o resultado da filtragem
 
  myPID.Compute();                                   //calcula o PID
  
  ServoOutput=77-Output;                             //define o novo ângulo que o servo deve atingir
  myServo.write(ServoOutput);                        //dá o comando para o servo produzir o deslocamento angular
  Serial.println(Output);                            //printa o valor do PID
  
  
}
   
float readPosition() {                          //função que lê a posição da bolinha
  delay(40);                                    //intervalo, em ms, p/ cada leitura de posição                              
  
const int echo = 5;                             //pino do echo do sensor
const int trig = 6;                             //pino do trigger do sensor

long duration, cm;      //variáveis p/ guardar a duração do pulso recebido pela onda refletida, e p/ guardar a distância
unsigned long now = millis();                   //variável q armazena o tempo de execução do código até aqui
  pinMode(trig, OUTPUT);                        //define o trig como saída
  digitalWrite(trig, LOW);                      //nível baixo no trig
  delayMicroseconds(2);                         //espera 2us
  digitalWrite(trig, HIGH);                     //nível alto no trig
  delayMicroseconds(5);                         //espera 5us
  digitalWrite(trig, LOW);                      //nível baixo no trig


  pinMode(echo, INPUT);                         //define como entrada o pino echo
  duration = pulseIn(echo, HIGH);               //calcula a duração do sinal refletido que foi recebido pelo echo

  cm = duration/(29*2);                         //calcula a distância em cm
  
  
  if(cm >= 45)                                  //limita a leitura máxima p/ 45cm
  {cm=45;}

   if(cm <= 10)                                 //limita a leitura mínima p/ 10cm
  {cm=10;}

  if(cm>=16 && cm<=23) cm=19;                   //define um intervalo onde se quer que o controlador PID não atue
  
  return cm;                                    //retorna a distância da bola ao sensor
}

// --- Desenvolvimento da Função do filtro de média móvel---
long moving_average()
{

   //desloca os elementos do vetor de média móvel
   for(int i= n-1; i>0; i--) numbers[i] = numbers[i-1]; //pega a leitura mais antiga e vai deslocando ela até ela ser descartada do vetor

   numbers[0] = leituraSensor; //posição inicial do vetor recebe a leitura mais atual do sensor

   long acc = 0;               //acumulador para somar os pontos da média móvel

   for(int i=0; i<n; i++) acc += numbers[i]; //faz a somatória das amostras dentro do vetor

   long resultado = acc/n;                   //calcula a média aritmética

  if(resultado >= 16 && resultado <= 23) resultado = 19; //define o mesmo intervalo aceitável citado anteriormente

   contador = contador + 1;   //define um contador p/ saber quantas amostras já foram colocadas dentro do vetor

   if(contador >= n) return resultado;  //retorna a média móvel caso o número de amostras oletadas seja maior que n
      else return leituraSensor;        //se não, retorna a leitura do sensor mesmo
 
} //fim
