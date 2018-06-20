# Rtos no Arduino Uno
# Sistemas Operacionais UFRJ 2018-1
Ewerton Vasconcelos - João Brandão - Andrews Monzato

O RTOS
Um RTOS (Sigla do inglês para "Sistema Operacional de Tempo Real" ) é um sistema operacional, geralmente ultilizado em hardwares simples  de  sistemas embarcados, que tem como objetivo principal a execução de múltiplas tarefas onde o tempo de resposta a um evento é pré definido. Esse tempo de resposta é chamado de prazo da tarefa e a perda de um prazo, isto é, o não cumprimento de uma tarefa dentro do prazo esperado, caracteriza uma falha do sistema.


Os objetivos do projeto
Os objetivos do projeto são os seguintes:


  Desenvolver um kernel capaz de armazenar em uma lista dinâmica escalável de tarefas a serem executadas pelo microcontrolador, ultilizando do recurso de Watchdog para controlar o tempo máximo disponível das tarefas. Com dinâmica, queremos dizer que o usuário do sistema poderar submeter tantas tarefas o quanto ele queira, respeitando as limitações de hardware do arduíno.
  
  Ultilizar um buffer circular para as tarefas, sendo assim as tarefas serão executadas seguindo a ordem de uma fila. As tarefas serão submetidas à lista fornecendo um tempo máximo de execução, para que o RTOS possa encerrar as tarefas que não finalizarem até o tempo máximo pré definido.
  
  Desenvolver duas rotinas a serem executadas pelo RTOS como forma de teste. Essas rotinas serão submetidas ao buffer de funções a serem esecutadas e poderemos acompanahar o sistemas executando-as concomitantemente.
   Rotina 1 - fazer um contador em um display LCD.
   Rotina 2 - Medir a distância a um obstáculo com um sensor ultrasom.
   Rotina 3 - Piscar LED em frequência definida.
 Simular e montar em um ambiente de protótipos o o projeto.
