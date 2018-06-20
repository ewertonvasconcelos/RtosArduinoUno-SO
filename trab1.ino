//----------- Nucleo Defines------------------
#include <TimerOne.h>
#include "avr/wdt.h"
#define TEMPO_MAXIMO_EXECUCAO 500
#define TIME_INTERVAL 1000
#define TIME_INTERVAL2 3000
 
#define SUCCESS 1
#define FAIL    0




//---------------------------------------------------------------------------------------------------------------------

#include <LiquidCrystal.h>
#include <Ultrasonic.h>
#include <QList.h>

// Definições gerais
#define LED_TRAVOU 8
#define LED_BLINK  9
#define LED_BLINK2  13

#define SIM 1
#define NAO 0

#define TASK1_PERIOD 50         // 50ms
#define TASK2_PERIOD 1000       // 1000ms
#define TASK3_PERIOD 5       // 5ms

//Ultrassom
#define pino_trigger 7
#define pino_echo 6

//Variaveis
float distanciaG;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Ultrasonic ultrasonic(pino_trigger, pino_echo);

//---------------------------------------------------------------------------------------------------------------------
//Protótipos de funções
void DispLcdTask();
void MedirDistanciaUltrassom();

//Protótipos Escalonador
typedef void(*ptrFunc)();

typedef struct{
  ptrFunc Function;
  uint32_t period;
}TaskHandle;


QList<TaskHandle> buffer2;
volatile QList<uint32_t> taskCounter2;

 
// Variáveis globais 
volatile int16_t TempoEmExecucao;
volatile uint32_t sysTickCounter = 0;
volatile bool TemporizadorEstourou;
volatile bool TarefaSendoExecutada;

char IniciarEscalonador(void);
char AdicionarTarefaFIFO(ptrFunc _function, uint16_t _period, TaskHandle task); 
char KernelRemoveTask(TaskHandle* task);
void IniciarTarefas(void);
 


//---------------------------------------------------------------------------------------------------------------------

void setup() 
{

  lcd.begin(16,2); 
  lcd.clear();       
  pinMode(LED_TRAVOU, OUTPUT);
  pinMode(LED_BLINK, OUTPUT);
  pinMode(LED_BLINK2, OUTPUT);

// Definição das estruturas para armazenarem as informações das tarefas
  TaskHandle task1; 
  TaskHandle task2;
  TaskHandle task3;
  TaskHandle task4;


 IniciarEscalonador();                   

  AdicionarTarefaFIFO(MedirDistanciaUltrassom,  TASK1_PERIOD, task1);
  AdicionarTarefaFIFO(DispLcdTask,      // Funcao para ser escalonada
                TASK2_PERIOD,           // Periodo do processo (A cada quanto tempo DEVE ser executado)
                task2);                // Estrutura da Tarefa
  AdicionarTarefaFIFO(BlinkTicks, TASK3_PERIOD, task3);
  //AdicionarTarefaFIFO(BlinkTicks2, TASK3_PERIOD, task4);
 
  IniciarTarefas();
}

void loop() {
  //todo
}



//---------------------------------------------------------------------------------------------------------------------
// BlinkTicks(): pisca led baseado nos ticks 
//---------------------------------------------------------------------------------------------------------------------
void BlinkTicks()
{
  static uint32_t previousCounter = 0;
  static int state = 0;
  if((sysTickCounter - previousCounter) > TIME_INTERVAL)
  {
    state = !state;
    previousCounter = sysTickCounter;  // Salva o tempo atual
    digitalWrite(LED_BLINK, state);
    

  }

}

//---------------------------------------------------------------------------------------------------------------------
// BlinkTicks2(): pisca led baseado nos ticks 
//---------------------------------------------------------------------------------------------------------------------
void BlinkTicks2()
{
  static uint32_t previousCounter2 = 0;
  static int state = 0;
  if((sysTickCounter - previousCounter2) > TIME_INTERVAL2)
  {
    state = !state;
    previousCounter2 = sysTickCounter;  // Salva o tempo atual
    digitalWrite(LED_BLINK2, state);
    

  }

}


//---------------------------------------------------------------------------------------------------------------------
// DispLcdTask(): escreve no display LCD o valor de um contador sempre que é executada
//---------------------------------------------------------------------------------------------------------------------
void DispLcdTask()
{
  lcd.setCursor(0, 0);
  lcd.print("   S.O - UFRJ   ");
  lcd.setCursor(0, 1);
  lcd.print("Dist.:");
  lcd.setCursor(6, 1);
  lcd.print(distanciaG,2);
  lcd.setCursor(14, 1);
  lcd.print("cm");
  
}


//---------------------------------------------------------------------------------------------------------------------
// MedirDistanciaUltrassom(): Mede distância com sensor ultrassom
//---------------------------------------------------------------------------------------------------------------------

void MedirDistanciaUltrassom() {
  float cmMsec,distancia;
  long microsec = ultrasonic.timing();
  distanciaG = ultrasonic.convert(microsec, Ultrasonic::CM);
}
//------------------------------------------------------------------
// Função vIniciarEscalonador()
//------------------------------------------------------------------
char IniciarEscalonador()
{
 
  // Inicializa as variáveis de sinalização do kernel
  TemporizadorEstourou = NAO;
  TarefaSendoExecutada = NAO;
 
  // Base de tempo para o escalonador
  Timer1.initialize(1000);                 // 1ms
  Timer1.attachInterrupt(TimerAbsoluto);  // chama TimerAbsoluto() quando o            
                        // timer estoura
 
  return SUCCESS;
}//end vIniciarEscalonador

//------------------------------------------------------------------
void TimerAbsoluto()
{
  int i;
  //Testa se Timer de 1ms estourou
  TemporizadorEstourou = SIM;
  
  //Conta a quantos ms o systema esta rodando
  sysTickCounter++;
  
  // Conta o tempo em que uma tarefa está em execução
  if (TarefaSendoExecutada == SIM)
  {
  //TempoEmExecucao recebe o máximo permitido por uma tarefa
    TempoEmExecucao--;
  
  //Caso esse tempo se esgote, o WhatchDog reinicia o Arduino
    if (!TempoEmExecucao)
    {
    
  //--- Pisca Led Vermelho de HW Reiniciando ----
      digitalWrite(LED_TRAVOU, HIGH);  
      delay(500);
      digitalWrite(LED_TRAVOU, LOW);
  //---------------------------------------------
      
    //Reinicia
    wdt_enable(WDTO_15MS);
      while (1);
    }
  }
}

//------------------------------------------------------------------
// Função AdicionarTarefaFIFO() : Adicionar funcoes na FIFO de execucao
//------------------------------------------------------------------

char AdicionarTarefaFIFO(ptrFunc _function, uint16_t _period, TaskHandle task)
{
  int i;

  task.Function = _function;
  task.period = _period;
  buffer2.push_back(task);
  taskCounter2.push_back(0);
  return SUCCESS;
  
}


//------------------------------------------------------------------
// Função IniciarTarefas() : Escalonar tarefas
//------------------------------------------------------------------
void IniciarTarefas()
{
  int i;

  while(true)
  {
    if (TemporizadorEstourou == SIM)
    {
      for (i = 0; i < buffer2.size(); i++)
      {
        //if (buffer2[i] != NULL)
        //{
          if (((sysTickCounter - taskCounter2[i])>buffer2[i].period))
          {
            TarefaSendoExecutada = SIM;
            TempoEmExecucao = TEMPO_MAXIMO_EXECUCAO;
            buffer2[i].Function();
            TarefaSendoExecutada = NAO;
            taskCounter2[i] = sysTickCounter;
          }
        //}
      }
    }
  }
}
