//#######################################################################################################################################
//CENTRÍFUGA AUTOMATIZADA - MICROCONTROLADOR STM32F103C8T6
//ANDRÉ PARRA DOURADO - JESSÉ ANDRADE DA SILVA 
//6º AUTOMAÇÃO INDUSTRIAL - 2º/2019
//#######################################################################################################################################


//#######################################################################################################################################
//INCLUSÃO DAS BIBLIOTECAS AUXILIARES 
//#######################################################################################################################################                        
#include <RotaryEncoder.h> //INCLUSÃO DA BIBLIOTECA DE MANIPULAÇÃO DE ENCODER
#include <LiquidCrystal.h> //INCLUSÃO DE BIBLIOTECA DE MANIPULAÇÃO DE LCD


//#######################################################################################################################################
//MAPEAMENTO DE HARDWARE
//#######################################################################################################################################
#define B12 PB12  //  5V                                              spi2nss                                          
#define B13 PB13  //  5V                          tpwm1/1n             spi2sck                                          
#define B14 PB14  //  5V                          tpwm1/2n             spi2miso                                         
#define B15 PB15  //  5V                          tpwm1/3n             spi2mosi                                         
#define  A8 PA8   //  5V  d7                      tpwm1/1                                                   
#define  A9 PA9   //  5V  d8                      tpwm1/2   serial1tx                             
#define A10 PA10  //  5V  d2                      tpwm1/3   serial1rx                            
#define A11 PA11  //  5V                          tpwm1/4                                                     
#define A12 PA12  //  5V                                                        usb                      
#define A15 PA13  //  5V                          pmw2/1              spi1nss                                          
#define  B3 PB3   //  5V  d3                      tpwm2/2              spi1sck                                        
#define  B4 PB4   //  5V  d5                      tpwm3/1              spi1miso                                       
#define  B5 PB5   //3.3V  d4                      tpwm3/2              spi1mosi                                      
#define  B6 PB6   //  5V  d10            i2c1scl           serial1tx                         
#define  B7 PB7   //  5V                 i2c1sda           serial1rx                             
#define  B8 PB8   //  5V  d15            i2c1scl                                                             
#define  B9 PB9   //  5V  d14            i2c1sda                                                             
#define B11 PB11  //  5V                 i2c2sda  tpwm2/4   serial3rx                              
#define B10 PB10  //  5V  d6             i2c2scl  tpwm2/3   serial3tx                            
#define  B1 PB1   //3.3V       analogIn           tpwm3/4                                             
#define  B0 PB0   //3.3V  a3   analogIn           tpwm3/3                                           
#define  A7 PA7   //3.3V  d11  analogIn           tpwm3/2              spi1mosi                              
#define  A6 PA6   //3.3V  d12  analogIn           tpwm3/1              spi1miso                                     
#define  A5 PA5   //3.3V  d13  analogIn                               spi1sck            
#define  A4 PA4   //3.3V  a2   analogIn                               spi1nss             
#define  A3 PA3   //3.3V  d0   analogIn           tpwm2/4   serial2rx            
#define  A2 PA2   //3.3V  d1   analogIn           tpwm2/3   serial2tx            
#define  A1 PA1   //3.3V  a1   analogIn           tpwm2/2                        
#define  A0 PA0   //3.3V       analogIn                                           
#define C15 PC15  //3.3V                                                         
#define C14 PC14  //3.3V                                                          
#define C13 PC13  //3.3V                                                        led onboard                    


//#######################################################################################################################################
//DECLARAÇÃO DE CONSTANTES GLOBAIS - RÓTULOS
//#######################################################################################################################################
#define LEDX PC13          //VINCULA O LED INTERNO PC13 
#define LCD_RS B12         //VINCULA O PINO B12 DO MICROCONTROLADOR COM O PINO RS DO DISPLAY
#define LCD_EN B13         //VINCULA O PINO B13 DO MICROCONTROLADOR COM O PINO EN DO DISPLAY
#define LCD_D4 B14         //VINCULA O PINO B14 DO MICROCONTROLADOR COM O PINO D4 DO DISPLAY
#define LCD_D5 B15         //VINCULA O PINO B15 DO MICROCONTROLADOR COM O PINO D5 DO DISPLAY
#define LCD_D6 A8          //VINCULA O PINO  A8 DO MICROCONTROLADOR COM O PINO D6 DO DISPLAY
#define LCD_D7 A9          //VINCULA O PINO  A9 DO MICROCONTROLADOR COM O PINO D7 DO DISPLAY
#define RELE A10           //VINCULA O PINO C13 DO MICROCONTROLADOR COM O PINO CHANNEL/TRIGGER DO RELÊ
#define BUZZER A2          //VINCULA O PINO  B6 DO MICROCONTROLADOR COM O PINO DO GATE DO BC548 QUE ACIONA O BUZZER
#define ENC_SW B7          //VINCULA O PINO  B7 DO MICROCONTROLADOR COM O PINO DO DO SWITCH DO ENCODER ROTATIVO
#define DATA_ENC B8        //VINCULA O PINO  B8 DO MICROCONTROLADOR COM O PINO DO DATA DO ENCODER ROTATIVO
#define CLOCK_ENC B9       //VINCULA O PINO  B9 DO MICROCONTROLADOR COM O PINO DO CLOCK DO ENCODER ROTATIVO
#define MENU_MAX_OFF 1     //DEFINE LIMITE DE MENUS DO MODO OFF
#define MENU_MAX_ON 1      //DEFINE LIMITE DE MENUS DO MODO ON
#define MENU_MAX_LIGAR 5   //DEFINE LIMITE DE MENUS DO SUBMENU LIGAR
#define TIMER_RATE 500000  //DEFINE PERÍODO DE ESTOURO DOS TIMERS - 0.5 SEGUNDOS = 500000 MICROSSEGUNDOS


//#######################################################################################################################################
//DECLARAÇÃO DE VARIÁVEIS GLOBAIS 
//#######################################################################################################################################
boolean flag_fdc = 0;                        //FLAG DE FIM DE CICLO
boolean statusBotaoEncoder = 0;              //STATUS DO BOTÃO DO ENCODER: 0 = "SOLTO" || 1 = "PRESSIONADO"
boolean statusMostraTempo = 0;               //STATUS DA FUNÇÃO TEMPO RESTANTE: 0 = DESLIGADO || 1 = EXIBINDO
unsigned short int cont = 0;                 //CONTA ESTOUROS DO Timer2
unsigned short int cont2 = 0;                //CONTA ESTOUROS DO Timer3
unsigned short int cont3 = 0;                //CONTA ESTOUROS DO Timer4
unsigned short int memoriaBotaoEncoder = 0;  //QUANTIDADE DE VEZES QUE O ENCODER FOI PRESSIONADO
unsigned short int nivelMenu = 0;            //ARMAZENA EM QUAL NÍVEL DE MENU O LCD ESTÁ
unsigned short int statusDispositivo = 0;    //STATUS DO DISPOSITIVO:  0 = STAND-BY || 1 = EM FUNCIONAMENTO || 2 = AGUARDANDO CONFIRMAÇÃO
unsigned short int tpwm = 0;                 //TEMPORIZADOR DE ESTÁGIO DO PWM
unsigned short int estagio = 1;              //FLAG QUE INDICA QUAL O ESTAGIO ATUAL SELECIONADO PARA O RELE
signed short int novaPosicaoEncoder = 0;     //ARMAZENA POSIÇÃO MAIS RECENTE DO ENCODER


//#######################################################################################################################################
//VETORES - MATRIZES
//#######################################################################################################################################
String option_line1_col1 = "      STAND-BY      ",    
       option_line1_col2 = "  EM FUNCIONAMENTO  ",
       option_line1_col3 = "    FIM DE CICLO    ",
       option_line1_col4 = "        ERRO        ";
String line1[4][1] = {option_line1_col1, 
                      option_line1_col2,
                      option_line1_col3,
                      option_line1_col4};
String menu_off_option1 = "->LIGAR             ",
       menu_off_option2 = "->SAIR              ";                                   
String menu_off[2][1] = {menu_off_option1, 
                         menu_off_option2};
String menu_ligar_option1 = "->03 MIN            ",
       menu_ligar_option2 = "->06 MIN            ",
       menu_ligar_option3 = "->09 MIN            ",
       menu_ligar_option4 = "->12 MIN            ",
       menu_ligar_option5 = "->15 MIN            ",
       menu_ligar_option6 = "->SAIR              ";  
String menu_ligar[6][1] = {menu_ligar_option1, 
                           menu_ligar_option2, 
                           menu_ligar_option3, 
                           menu_ligar_option4, 
                           menu_ligar_option5, 
                           menu_ligar_option6};
String menu_on_option1 = "->DESLIGAR?         ",
       menu_on_option2 = "->SAIR              ";
String menu_on[2][1] = {menu_on_option1, 
                        menu_on_option2};


//#######################################################################################################################################
//STRUCT DE ARMAZENAMENTO DE INFORMAÇÕES DE TEMPORIZAÇÃO
//#######################################################################################################################################
struct InfoTemporizador{                                           //CRIA STRUCT DO TIPO "InfoTemporizador"
  unsigned short int minutosReferencia  = 0;                       //TEMPO ESCOLHIDO EM MINUTOS
  unsigned short int segundosReferencia = 0;                       //TEMPO ESCOLHIDO EM SEGUNDOS
  unsigned short int minutosDecorridos  = 0;                       //TEMPO DECORRIDO EM MINUTOS
  unsigned short int segundosDecorridos = 0;                       //TEMPO DECORRIDO EM SEGUNDOS
  unsigned short int minutosRestantes   = 0;                       //TEMPO RESTANTE EM MINUTOS
  unsigned short int segundosRestantes  = 0;                       //TEMPO RESTANTE EM SEGUNDOS
  void configuraTemporizador(unsigned short int struct_minutos,
                             unsigned short int struct_segundos){  //FUNÇÃO QUE RECEBE VALORES DE TEMPORIZAÇÃO ESCOLHIDOS PELO USUÁRIO
    minutosReferencia = struct_minutos;                            //TRANSFERE MINUTOS RECEBIDOS PARA A VARIÁVEL UTILIZAVEL
    segundosReferencia = struct_segundos;                          //TRANSFERE SEGUNDOS RECEBIDOS PARA A VARIAVEL UTILIZAVEL
    minutosRestantes = minutosReferencia;                          //DEFINE COMO MINUTOS RESTANTES O VALOR SELECIONADO
  }  
  void resetaTemporizador(){                                       //FUNÇÃO QUE RESETA VALORES DO TEMPORIZADOR
    minutosReferencia = 0;                                         //DEFINA VALOR 0 NA VARIÁVEL
    segundosReferencia = 0;                                        //DEFINA VALOR 0 NA VARIÁVEL
    minutosDecorridos = 0;                                         //DEFINA VALOR 0 NA VARIÁVEL
    segundosDecorridos = 0;                                        //DEFINA VALOR 0 NA VARIÁVEL
    minutosRestantes = 0;                                          //DEFINA VALOR 0 NA VARIÁVEL
    segundosRestantes = 0;                                         //DEFINA VALOR 0 NA VARIÁVEL
  } 
}temporizador;                                                     //CRIA INSTÂNCIA "InfoTemporizador" CHAMADA "temporizador"


//#######################################################################################################################################
//DECLARAÇÃO DE FUNÇÕES
//#######################################################################################################################################
void atualizaDisplayLinha1();  //FUNÇÃO QUE ATUALIZA 1ª LINHA DO DISPLAY
void atualizaDisplayLinha2();  //FUNÇÃO QUE ATUALIZA 2ª LINHA DO DISPLAY
void atualizaDisplayLinha4();  //FUNÇÃO QUE ATUALIZA 3ª LINHA DO DISPLAY
void buzzerOn();               //FUNÇÃO QUE ATIVE BUZZER  
void clockStm();               //FUNÇÃO QUE GERENCIA TEMPORIZAÇÃO
void configuraTimers();        //FUNÇÃO QUE CONFIGURA OS TIMERS NA INICIALIZAÇÃO
void configuraDisplay();       //FUNÇÃO QUE CONFIGURA O DISPLAY NA INICIALIZAÇÃO
void desligaDispositivo();     //FUNÇÃO QUE DESLIGA O APARELHO
void desligaTimers();          //FUNÇÃO QUE DESLIGA TIMERS
void fimDeCiclo();             //FUNÇÃO QUE ATIVA FIM DE CICLO
void ligaDispositivo();        //FUNÇÃO QUE LIGA O APARELHO
void ligaTimers();             //FUNÇÃO QUE LIGA TIMERS
void mostraTempo();            //FUNÇÃO QUE MOSTRA O TEMPO RESTANTE DA APLICAÇÃO
void pegaInfoEncoder();        //FUNÇÃO QUE EXTRAI INFORMAÇÕES DO ENCODER ROTATIVO (POSIÇÃO, DIREÇÃO E BOTÃO)
void resetaMenu();             //FUNÇÃO QUE LEVA MENU A POSIÇÃO INICIAL


//#######################################################################################################################################
//INICIALIZAÇÃO DE HARDWARE
//#######################################################################################################################################
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);  //FUNÇÃO QUE INICIALIZA O DISPLAY LCD
RotaryEncoder encoder(DATA_ENC, CLOCK_ENC);                         //FUNÇÃO QUE INICIALIZA O ENCODER


//#######################################################################################################################################
//VOID SETUP - FUNÇÃO EXECUTADA APENAS UMA VEZ AO LIGAR O DISPOSITIVO
//#######################################################################################################################################
void setup(){
  pinMode(RELE, OUTPUT);      //DEFINE COMO OUTPUT PIN A10 - RELÊ
  pinMode(BUZZER, OUTPUT);    //DEFINE COMO OUTPUT PIN  B6 - BUZZER
  pinMode(ENC_SW, INPUT);     //DEFINE COMO  INPUT PIN  B7 - BOTÃO ENCODER ROTATIVO
  pinMode(LEDX,OUTPUT);       //DEFINE COMO OUTPUT O LED INTERNO
  digitalWrite(RELE, HIGH);   //FORÇA DESLIGAMENTO DO RELE NA INICIALIZAÇÃO
  digitalWrite(BUZZER, LOW);  //FORÇA DESLIGAMENTO DO BUZZER NA INICIALIZAÇÃO
  digitalWrite(LEDX, LOW);    //LIGA O LED INTERNO DA PLACA
  configuraTimers();          //CHAMA FUNÇÃO QUE CONFIGURA OS TIMERS
  configuraDisplay();         //CHAMA FUNÇÃO QUE CONFIGURA O DISPLAY
}


//#######################################################################################################################################
//FUNÇÃO PRINCIPAL - RODA INFINITAMENTE
//#######################################################################################################################################
void loop(){
  if (flag_fdc == 1){                                   //VERIFICA SE UMA EXECUÇÃO CHEGOU NO FIM DE CICLO
    fimDeCiclo();                                         //SE SIM EXECUTA A FUNÇÃO FIM DE CICLO
  }
  pegaInfoEncoder();                                    //EXTRAI INFORMAÇÕES DO ENCODER
  if (statusDispositivo == 0 && nivelMenu == 0) {       //VERIFICA SE O DISPOSITIVO NÃO ESTÁ FUNCIONANDO E SE ESTÁ NO NÍVEL 0 DO MENU
    if (memoriaBotaoEncoder == 1) {                       //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO
      nivelMenu = 1;                                        //SOBE PARA O NÍVEL 1 DO MENU
    }
  }
  if (statusDispositivo == 0 && nivelMenu == 1) {       //VERIFICA SE O DISPOSITIVO NÃO ESTÁ FUNCIONANDO E SE ESTÁ NO NÍVEL 1 DO MENU
    if (memoriaBotaoEncoder == 1) {                       //CAI NESTA CONDIÇÃO SE O BOTÃO DO ENCODER NÃO FOI PRESSIONADO
      if (novaPosicaoEncoder == 0) {                        //CAI NESTA CONDIÇÃO CASO O ENCODER NÃO TENHA SIDO RODADO
        lcd.setCursor(0,1);                                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
        lcd.print(menu_off[0][0]);                            //PRINTA "LIGAR"
      } else if (novaPosicaoEncoder == 1) {                 //CAI NESTA CONDIÇÃO CASO O ENCODER TENHA SIDO RODADO 1x PRA DIREITA
        lcd.setCursor(0,1);                                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
        lcd.print(menu_off[1][0]);                            //PRINTA "SAIR"
      } else if (novaPosicaoEncoder < 0) {                  //CAI NESTA CONDIÇÃO CASO O ENCODER TENHA SIDO RODADO X VEZES PRA ESQUERDA
        encoder.setPosition(MENU_MAX_OFF);                    //SETA A POSIÇÃO DO ENCODER COMO ÚLTIMA POSIÇÃO DO SUBMENU, GERANDO UM LOOP INFINITO
      } else if (novaPosicaoEncoder > MENU_MAX_OFF) {       //CAI NESTA CONDIÇÃO CASO O ENCODER TENHA SIDO RODADO PRA DIREITA ALÉM DO LIMITE DE SUBMENUS
        encoder.setPosition(0);                               //SETA A POSIÇÃO DO ENCODER COMO A POSIÇÃO INICIAL DO SUBMENU, GERANDO UM LOOP INFINITO   
      }
    } else if (memoriaBotaoEncoder == 2) {                //CAI NESTA CONDIÇÃO SE O BOTÃO DO ENCODER FOI PRESSIONADO 
        if (novaPosicaoEncoder == 0) {                      //CAI NESTA CONDIÇÃO CASO O ENCODER NÃO TENHA SIDO RODADO
          nivelMenu = 2;                                      //AVANÇA EM UM SUBMENU
        } else if (novaPosicaoEncoder == 1) {               //CAI NESTA CONDIÇÃO CASO O ENCODER TENHA SIDO RODADO 1x PRA DIREITA
          resetaMenu();                                       //VOLTA PARA O MENU INICIAL
        }
    } else if (memoriaBotaoEncoder >= 3) {                //CONDIÇÃO DE SEGURANÇA
        resetaMenu();                                       //VOLTA PARA O MENU INICIAL
    } 
  } 
  if(statusDispositivo == 0 && nivelMenu == 2){         //VERIFICA SE O DISPOSITIVO NÃO ESTÁ FUNCIONANDO E SE ESTÁ NO NÍVEL 2 DO MENU 
    if(memoriaBotaoEncoder == 2){                         //CAI NESTA CONDIÇÃO SE O BOTÃO DO ENCODER NÃO FOI PRESSIONADO
      lcd.setCursor(0,1);                                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY 
      lcd.print(" SELECIONE O TEMPO  ");                    //PRINTA "05 MIN"
      if(novaPosicaoEncoder == 0){                          //CAI NESTA CONDIÇÃO SE O ENCODER NÃO FOI RODADO
        lcd.setCursor(0,2);                                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY 
        lcd.print(menu_ligar[0][0]);                          //PRINTA "05 MIN"
      } else if(novaPosicaoEncoder == 1){                   //CAI NESTA CONDIÇAO SE O ENCODER FOI RODADO 1x PRA DIREITA
         lcd.setCursor(0,2);                                  //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY  
         lcd.print(menu_ligar[1][0]);                         //PRINTA "10 MIN"
      } else if(novaPosicaoEncoder == 2){                   //CAI NESTA CONDIÇÃO SE O ENCODER FOI RODADO 2x PRA DIREITA
         lcd.setCursor(0,2);                                  //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY  
         lcd.print(menu_ligar[2][0]);                         //PRINTA "15 MIN"
      } else if(novaPosicaoEncoder == 3){                   //CAI NESTA CONDIÇÃO SE O ENCODER FOI RODADO 3x PRA DIREITA
         lcd.setCursor(0,2);                                  //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
         lcd.print(menu_ligar[3][0]);                         //PRINTA "20 MIN"
      } else if(novaPosicaoEncoder == 4){                   //CAI NESTA CONDIÇÃO SE O ENCODER FOI RODADO 4x PRA DIREITA
         lcd.setCursor(0,2);                                  //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
         lcd.print(menu_ligar[4][0]);                         //PRINTA "25 MIN"
      } else if(novaPosicaoEncoder == 5){                   //CAI NESTA CONDIÇÃO SE O ENCODER FOI RODADO 5x PRA DIREITA
         lcd.setCursor(0,2);                                  //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
         lcd.print(menu_ligar[5][0]);                         //PRINTA "SAIR"
      } else if(novaPosicaoEncoder < 0){                    //CAI NESTA CONDIÇÃO SE O ENCODER FOI RODADO X VEZES PRA ESQUERDA
         encoder.setPosition(MENU_MAX_LIGAR);                 //SETA A POSIÇÃO DO ENCODER COMO ÚLTIMA POSIÇÃO DO SUBMENU, GERANDO UM LOOP INFINITO
      } else if(novaPosicaoEncoder > MENU_MAX_LIGAR){       //CAI NESTA CONDIÇÃO SE O ENCODER FOI RODADO RRA DIREITA ALÉM DO LIMITE DE SUBMENUS
         encoder.setPosition(0);                              //SETA A POSIÇÃO DO ENCODER COMO A POSIÇÃO INICIAL DO SUBMENU, GERANDO UM LOOP INFINITO 
      } 
    } else if(memoriaBotaoEncoder == 3){                  //CAI NESTA CONDIÇÃO SE O BOTÃO DO ENCODER FOI PRESSIONADO
        if(novaPosicaoEncoder == 0){                        //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO NA POSIÇÃO 0
          temporizador.configuraTemporizador(3,180);           //GRAVA NA STRUCT DE TEMPO, O TEMPO SELECIONADO EM MINUTOS E SEGUNDOS
          ligaDispositivo();                                  //INICIA O FUNCIONAMENTO DO DISPOSITIVO
        } else if(novaPosicaoEncoder == 1){                 //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO NA POSIÇÃO 1
          temporizador.configuraTemporizador(6,360);         //GRAVA NA STRUCT DE TEMPO, O TEMPO SELECIONADO EM MINUTOS E SEGUNDOS
          ligaDispositivo();                                  //INICIA O FUNCIONAMENTO DO DISPOSITIVO
        } else if(novaPosicaoEncoder == 2){                 //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO NA POSIÇÃO 2
          temporizador.configuraTemporizador(9,540);         //GRAVA NA STRUCT DE TEMPO, O TEMPO SELECIONADO EM MINUTOS E SEGUNDOS
          ligaDispositivo();                                  //INICIA O FUNCIONAMENTO DO DISPOSITIVO
        } else if(novaPosicaoEncoder == 3){                 //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO NA POSIÇÃO 3
          temporizador.configuraTemporizador(12,720);        //GRAVA NA STRUCT DE TEMPO, O TEMPO SELECIONADO EM MINUTOS E SEGUNDOS
          ligaDispositivo();                                  //INICIA O FUNCIONAMENTO DO DISPOSITIVO
        } else if(novaPosicaoEncoder == 4){                 //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO NA POSIÇÃO 4
          temporizador.configuraTemporizador(15,900);        //GRAVA NA STRUCT DE TEMPO, O TEMPO SELECIONADO EM MINUTOS E SEGUNDOS
          ligaDispositivo();                                  //INICIA O FUNCIONAMENTO DO DISPOSITIVO
        } else if(novaPosicaoEncoder == 5){                 //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO NA POSIÇÃO 5
          resetaMenu();                                       //VOLTA PARA O MENU INICIAL 
        }
    } else if(memoriaBotaoEncoder >= 4){                  //CONDIÇÃO DE SEGURANÇA
        resetaMenu();                                       //VOLTA PARA O MENU INICIAL 
    } 
  }  
  if(statusDispositivo == 1 && nivelMenu == 0){         //VERIFICA SE O DISPOSITIVO ESTÁ FUNCIONANDO E SE ESTÁ NO NÍVEL 0 DO MENU 
    if(memoriaBotaoEncoder == 1){                         //CAI NESTA CONDIÇÃO CASO O BOTÃO DO ENCODER TENHA SIDO PRESSIONADO
      nivelMenu = 1;                                        //SOBE PARA O NÍVEL 1 DO MENU
    }
  }
  if(statusDispositivo == 1 && nivelMenu == 1){         //VERIFICA SE O DISPOSITIVO ESTÁ FUNCIONANDO E SE ESTÁ NO NÍVEL 1 DO MENU
    if(memoriaBotaoEncoder == 1){                         //CAI NESTA CONDIÇÃO SE O ENCODER NÃO FOI PRESSIONADO
      if(novaPosicaoEncoder == 0){                          //CAI NESTA CONDIÇÃO SE O ENCODER NÃO FOI ROTACIONADO
        lcd.setCursor(0,1);                                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
        lcd.print(menu_on[0][0]);                             //PRINTA "DESLIGAR?"
      } else if (novaPosicaoEncoder == 1) {                 //CAI NESTA CONDIÇÃO SE O ENCODER FOI ROTACIONADO 1x PRA DIREITA
        lcd.setCursor(0,1);                                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
        lcd.print(menu_on[1][0]);                             //PRINTA "SAIR"
      } else if (novaPosicaoEncoder < 0){                   //CAI NESTA CONDIÇÃO SE O ENCODER FOI ROTACIONADO X VEZES PRA ESQUERDA
        encoder.setPosition(MENU_MAX_ON);                     //SETA A POSIÇÃO DO ENCODER COMO ÚLTIMA POSIÇÃO DO SUBMENU, GERANDO UM LOOP INFINITO
      } else if(novaPosicaoEncoder > MENU_MAX_ON){          //CAI NESTA CONDIÇÃO SE O ENCODER FOI ROTACIONADO PRA DIREITA ALÉM DO LIMITE DE SUBMENUS
        encoder.setPosition(0);                               //SETA A POSIÇÃO DO ENCODER COMO A POSIÇÃO INICIAL DO SUBMENU, GERANDO UM LOOP INFINITO
      }   
    } else if (memoriaBotaoEncoder == 2){                 //CAI NESTA CONDIÇÃO SE O ENCODER FOI PRESSIONADO
      if(novaPosicaoEncoder == 0){                          //CAI NESTA CONDIÇÃO SE O ENCODER FOI PRESSIONADO NA POSIÇÃO 0
        desligaDispositivo();                                 //ENCERRA FUNCIONAMENTO DO DISPOSITIVO
      } else if (novaPosicaoEncoder == 1){                  //CAI NESTA CONDIÇÃO SE O ENCODER FOI PRESSIONADO NA POSIÇÃO 1  
        resetaMenu();                                         //VOLTA PARA O MENU INICIAL
      }
    } else if (memoriaBotaoEncoder >=3){                    //CONDIÇÃO DE SEGURANÇA
      resetaMenu();                                           //VOLTA PARA O MENU INICIAL 
    }   
  }
  if(statusDispositivo == 0){                           //CAI NESTA CONDIÇÃO CASO O DISPOSITIVO ESTEJA DESLIGADO
    digitalWrite(RELE, HIGH);                             //DESLIGA O RELÊ DA LÂMPADA
    analogWrite(RELE,255);
  } else if (statusDispositivo == 1){                   //CAI NESTA CONDIÇÃO CASO O DISPOSITIVO ESTEJA LIGADO
    tpwm++;                                               //INCREMENTA TEMPORIZAÇÃO DE ESTÁGIO DO PWM
    if (estagio == 1) {                                   //SETA PRIMEIRO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O SEGUNDO ESTÁGIO
      analogWrite(RELE, 230); 
      if (tpwm > 200){
        estagio = 2;
        tpwm = 0;  
      }  
    } else if (estagio == 2){                             //SETA SEGUNDO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O TERCEIRO ESTÁGIO
      analogWrite(RELE, 207);
      if (tpwm > 200){
        estagio = 3;
        tpwm = 0;  
      }
    } else if (estagio == 3){                             //SETA TERCEIRO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O QUARTO ESTÁGIO
      analogWrite(RELE, 184);
      if (tpwm > 200){
        estagio = 4;
        tpwm = 0;   
      }
    } else if (estagio == 4){                             //SETA QUARTO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O QUINTO ESTÁGIO
      analogWrite(RELE, 161);
      if (tpwm > 200){
        estagio = 5;
        tpwm = 0;  
      }
    } else if (estagio == 5){                             //SETA QUINTO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O SEXTO ESTÁGIO
      analogWrite(RELE, 138);
      if (tpwm > 200){
        estagio = 6;
        tpwm = 0;  
      }
    } else if (estagio == 6){                             //SETA SEXTO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O SÉTIMO ESTÁGIO
      analogWrite(RELE, 115);
      if (tpwm > 200){
        estagio = 7;
        tpwm = 0;  
      }
    } else if (estagio == 7){                             //SETA SÉTIMO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O OITAVO ESTÁGIO
      analogWrite(RELE, 92);
      if (tpwm > 200){
        estagio = 8;
        tpwm = 0;  
      }
    } else if (estagio == 8){                             //SETA OITAVO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O NONO ESTÁGIO
      analogWrite(RELE, 69);
      if (tpwm > 200){
        estagio = 9;
        tpwm = 0;  
      }
    } else if (estagio == 9){                             //SETA NONO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O DÉCIMO ESTÁGIO
      analogWrite(RELE, 46);
      if (tpwm > 200){
        estagio = 10;
        tpwm = 0;  
      }
    } else if (estagio == 10){                            //SETA DÉCIMO ESTÁGIO, VERIFICA SE JÁ DEU O TEMPO DELE E CASO SIM ACIONA O DÉCIMO PRIMEIRO ESTÁGIO
      analogWrite(RELE, 23);
      if (tpwm > 200){
        estagio = 11;
        tpwm = 0;  
      }
    } else if (estagio == 11){                            //SETA O DÉCIMO PRIMEIRO ESTÁGIO
      analogWrite(RELE, 0);
      if (tpwm > 200){
        tpwm = 0;  
      }
    } 
  } else if (statusDispositivo == 2){                   //CAI NESTA CONDIÇÃO CASO O DISPOSITIVO TENHA CHEGADO NO FIM DE CICLO
    digitalWrite(RELE, HIGH);                             //DESLIGA O RELE
    analogWrite(RELE,255);
    if (cont3 < 500){                                     //VERIFICA TEMPORIZADOR DO BUZZER
      digitalWrite(BUZZER, HIGH);                           //LIGA BUZZER
    } else if (cont3 >= 500 && cont3 < 1000){             //VERIFICA TEMPORIZADOR DO BUZZER
      digitalWrite(BUZZER, LOW);                            //DESLIGA BUZZER
    } else {                                              //FORA DOS VALORES DIMENSIONADOS
      cont3 = 0;                                            //RESETA TEMPORIZADOR DO BUZZER
    } 
    if (memoriaBotaoEncoder > 0) {                      //CAI NESTA CONDIÇÃO SE O BOTÃO DO ENCODER TIVER SIDO PRESSIONADO
      Timer4.pause();                                     //PAUSA O Timer4
      statusDispositivo = 0;                              //DESLIGA O DISPOSITIVO
      digitalWrite(RELE, HIGH);                           //DESLIGA O RELE
      analogWrite(RELE,255);                              //DESLIGA O RELE
      digitalWrite(BUZZER, LOW);                          //DESLIGA O BUZZER
      resetaMenu();                                       //RESETA O MENU
    }
  }
  atualizaDisplayLinha1();  //FUNÇÃO QUE ATUALIZA A LINHA 1 DO DISPLAY
  atualizaDisplayLinha2();  //FUNÇÃO QUE ATUALIZA A LINHA 2 DO DISPLAY
  atualizaDisplayLinha4();  //FUNÇÃO QUE ATUALIZA A LINHA 4 DO DISPLAY 
}


//#######################################################################################################################################
//DESENVOLVIMENTO DE FUNÇÕES
//#######################################################################################################################################
//########################################################################################################
//FUNÇÃO >> resetaMenu - RESETA MENU DO LCD
//#######################################################################################################
void resetaMenu(){
  statusMostraTempo = 0;    //DESLIGA A FUNÇÃO MOSTRA TEMPO
  memoriaBotaoEncoder = 0;  //SETA 0 NA MEMÓRIA DO BOTÃO DO ENCODER
  encoder.setPosition(0);   //SETA 0 NA POSIÇÃO DO ENCODER
  nivelMenu = 0;            //SETA 0 NO NIVEL DO MENU
  lcd.clear();              //LIMPA O DISPLAY INTEIRO  
}


//########################################################################################################
//FUNÇÃO >> atualizaDisplayLinha1 - CONTROLA 1º LINHA DO LCD
//########################################################################################################
void atualizaDisplayLinha1(){
  lcd.setCursor(0,0);                  //POSICIONA O CURSOR NA 1ª COLUNA E 1ª LINHA DO DISPLAY
  if(statusDispositivo == 0){          //CAI NESTA CONDIÇÃO SE O DISPOSITIVO ESTIVER DESLIGADO
    lcd.print(line1[0][0]);              //PRINTA "STAND-BY"
  } else if (statusDispositivo == 1){  //CAI NESTA CONDIÇÃO SE O DISPOSITIVO ESTIVER LIGADO
    lcd.print(line1[1][0]);              //PRINTA "EM FUNCIONAMENTO"
  } else if (statusDispositivo == 2){  //CAI NESTA CONDIÇÃO SE O DISPOSITIVO ESTIER EM MODO DE FIM DE CICLO
    lcd.print(line1[2][0]);              //PRINTA "FIM DE CICLO"
  } else {                             //CONDIÇÃO DE SEGURANÇA
    lcd.print(line1[3][0]);              //PRINTA "ERRO"
  }
}


//########################################################################################################
//FUNÇÃO >> atualizaDisplayLinha2 - CONTROLA 2º LINHA DO LCD
//########################################################################################################
void atualizaDisplayLinha2(){
  if (statusDispositivo == 2){          //CAI NESTA CONDIÇÃO CASO O DISPOSITIVO ESTEJA NO MODO FIM DE CICLO
    lcd.setCursor(0,1);                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
    lcd.print("APERTE!             ");    //PRINTA "APERTE P/ MENU"
  } else if (nivelMenu == 0) {          //CAI NESTA CONDIÇAO SE MENU NÃO FOI REQUISITADO
    lcd.setCursor(0,1);                   //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
    lcd.print("   APERTE P/ MENU   ");    //PRINTA "APERTE P/ MENU"
  }    
}


//########################################################################################################
//FUNÇÃO >> atualizaDisplayLinha2 - CONTROLA 4º LINHA DO LCD
//########################################################################################################
void atualizaDisplayLinha4(){
  if (statusMostraTempo == 1) {                 //CAI NESTA CONDIÇÃO SE A FUNÇÃO MOSTRA TEMPO ESTIVER ATIVA 
    if(cont2 >= 2){                               //CAI NESTA CONDIÇÃO QUANDO O Timer3 TIVER CONTADO 1 SEGUNDO
      cont2 = 0;                                    //ZERA O CONTADOR
      lcd.setCursor(0,3);                           //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY 
      lcd.print("  RESTANTE:  ");                   //PRINTA "RESTANTE:"
      if(temporizador.minutosRestantes < 10){       //CAI NESTA CONDIÇÃO SE OS MINUTOS RESTANTES FOREM MENORES QUE 10
        lcd.print("0");                               //PRINTA "0"
      }
      lcd.print(temporizador.minutosRestantes);     //PRINTA "MINUTOS RESTANTES"
      lcd.print(":");                               //PRINTA ":"
      if (temporizador.segundosRestantes < 10){     //CAI NESTA CONDIÇÃO SE OS SEGUNDOS RESTANTES FOREM MENORES QUE 10
        lcd.print("0");                               //PRINTA "0"
      }
      lcd.print(temporizador.segundosRestantes);    //PRINTA "SEGUNDOS RESTANTES"
      lcd.print("  ");
    } 
  }            
}


//########################################################################################################
//FUNÇÃO >> pegaInfoEncoder - EXTRAI INFORMAÇÕES DO ROTARY ENCODER
//########################################################################################################
void pegaInfoEncoder(){                              
  encoder.tick();                              //CHECA O STATUS DO ENCODER
  novaPosicaoEncoder = encoder.getPosition();  //PEGA A ÚLTIMA POSIÇÃO DO ENCODER
  statusBotaoEncoder = !digitalRead(ENC_SW);   //PEGA O STATUS DO BOTÃO DO ENCODER
  while(!digitalRead(ENC_SW)) {}               //BORDA DE SEGURANÇA
  if(statusBotaoEncoder){                      //VERIFICA SE O BOTÃO FOI PRESSIONADO
    memoriaBotaoEncoder++;                       //SE PRESSIONADO É INCREMENTADO A CONTAGEM DA MEMÓRIA
  }
}


//########################################################################################################
//FUNÇÃO >> ligaDispositivo - LIGA O DISPOSITIVO
//########################################################################################################
void ligaDispositivo(){
  ligaTimers();           //FUNÇÃO QUE LIGA TIMERS
  statusDispositivo = 1;  //LIGA O DISPOSITIVO
  resetaMenu();           //VOLTA PARA O MENU INICIAL
  statusMostraTempo = 1;  //LIGA A FUNÇÃO DE EXIBIR O TEMPO
}


//########################################################################################################
//FUNÇÃO >> ligaTimers - LIGA TIMERS
//########################################################################################################
void ligaTimers(){
  Timer2.refresh();  //RESETA VARIÁVEIS DO TIMER PARA SEUS VALORES PRÉ-DEFINIDOS
  Timer3.refresh();  //RESETA VARIÁVEIS DO TIMER PARA SEUS VALORES PRÉ-DEFINIDOS
  Timer2.resume();   //INICIA Timer2
  Timer3.resume();   //INICIA Timer3
}


//########################################################################################################
//FUNÇÃO >> clockStm - LÓGICA DE GERENCIAMENTO DA TEMPORIZAÇÃO
//########################################################################################################
void clockStm(){
  cont++;                                                     //INCREMENTA O CONTADOR A CADA ESTOURO DE 0.5 SEGUNDOS DO Timer2
  if(cont >= 2){                                              //CAI NESTA CONDIÇÃO QUANDO O Timer2 TIVER CONTADO 1 SEGUNDO
    cont = 0;                                                   //RESETA O CONTADOR
    if(temporizador.segundosRestantes == 0){                    //CAI NESTA CONDIÇÃO QUANDO OS SEGUNDOS RESTANTES SÃO IGUAIS A 0
      temporizador.segundosRestantes = 60;                        //DEFINE SEGUNDOS RESTANTES COMO 60
      temporizador.minutosRestantes--;                            //DECREMENTA MINUTOS RESTANTES
      temporizador.segundosRestantes--;                           //DECREMENTA SEGUNDOS RESTANTES
      temporizador.segundosDecorridos++;                          //INCREMENTA SEGUNDOS DECORRIDOS
    } else {                                                    //CAI NESTA CONDIÇÃO QUUANDO OS SEGUNDOS RESTANTES SÃO DIFERENTES DE 0
      temporizador.segundosRestantes--;                           //DECREMENTA SEGUNDOS RESTANTES
      temporizador.segundosDecorridos++;                          //INCREMENTA SEGUNDOS DECORRIDOS
    }
    if (temporizador.segundosDecorridos >= temporizador.segundosReferencia){  //CAI NESTA CONDIÇÃO QUANDO OS SEGUNDOS DECORRIDOS FOREM MAIORES OU IGUAIS AO SEGUNDOS DE REFERÊNCIA
      flag_fdc = 1;
    }           
  }
}


//########################################################################################################
//FUNÇÃO >> desligaDispositivo - DESLIGA O DISPOSITIVO
//########################################################################################################
void desligaDispositivo(){
  desligaTimers();                    //FUNÇÃO QUE DESLIGA OS TIMERS
  digitalWrite(RELE, HIGH);           //DESLIGA O RELE
  analogWrite(RELE, 255);             //DESLIGA O RELE
  statusDispositivo = 0;              //DESLIGA O FUNCIONAMENTO DO DISPOSITIVO
  temporizador.resetaTemporizador();  //EXECUTA FUNÇÃO QUE RESETA OS DADOS DO TEMPORIZADOR
  estagio = 1;                        //RETORNA PARA O ESTÁGIO PADRÃO - 1
  resetaMenu();                       //VOLTA PARA O MENU INICIAL
}


//########################################################################################################
//FUNÇÃO >> desligaTimers - DESLIGA TIMERS
//########################################################################################################
void desligaTimers(){    
  Timer2.pause();      //PAUSA Timer2
  Timer3.pause();      //PAUSA Timer3
}


//########################################################################################################
//FUNÇÃO >> mostraTempo - MOSTRATEMPO (SERVE PARA ATUALIZAR O DISPLAY A CADA 1 SEGUNDO COM O TEMPO RESTANTE)
//########################################################################################################
void mostraTempo(){   
  cont2++;           //INCREMENTA A CADA ESTOURO DO Timer3
}


//########################################################################################################
//FUNÇÃO >> fimDeCiclo - FUNÇÃO QUE ATIVA O FIM DE CICLO
//########################################################################################################
void fimDeCiclo(){
  desligaTimers();                           //FUNÇÃO QUE DESLIGA OS TIMERS
  digitalWrite(RELE, HIGH);                  //FUNÇÃO QUE DESLIGA A CARGA
  analogWrite(RELE, 255);                    //DESLIGA O RELE
  statusDispositivo = 2;                     //DEFINE O DISPOSITIVO COMO MODO DE FIM DECICLO
  estagio = 1;                               //RETORNA PARA O ESTÁGIO PADRÃO - 1
  Timer4.refresh();                          //RESETA O Timer4
  Timer4.resume();                           //RETOMA O Timer4
  temporizador.resetaTemporizador();         //EXECUTA FUNÇÃO QUE RESETA OS DADOS DO TEMPORIZADOR
  resetaMenu();                              //RESETA O MENU
  flag_fdc = 0;                              //DESLIGA A FLAG DE FIM DE CICLO
}


//########################################################################################################
//FUNÇÃO >> buzzerOn - OSCILA O BUZZER
//########################################################################################################
void buzzerOn(){
  cont3++;                            //INCREMENTA CONTADOR DO TEMPORIZADOR DO BUZZER 
}


//########################################################################################################
//FUNÇÃO >> configuraDisplay - CONFIGURA DISPLAY NA INICIALIZAÇÃO
//########################################################################################################
void configuraDisplay(){
  lcd.begin(20,4);                    //SETA O DISPLAY COMO LCD 20X4
  lcd.setCursor(0,0);                 //POSICIONA O CURSOR NA 1ª COLUNA E 1ª LINHA DO DISPLAY
  lcd.print("CENTRIFUGA          ");  //IMPRIME "CAMARA" NO DISPLAY
  lcd.setCursor(0,1);                 //POSICIONA O CURSOR NA 1ª COLUNA E 2ª LINHA DO DISPLAY
  lcd.print("AUTOMATIZADA        ");  //IMPRIME "REVELADORA" NO DISPLAY
  lcd.setCursor(0,2);                 //POSICIONA O CURSOR NA 1ª COLUNA E 3ª LINHA DO DISPLAY
  lcd.print("> ANDRE PARRA       ");  //IMPRIME "REVELADORA" NO DISPLAY
  lcd.setCursor(0,3);                 //POSICIONA O CURSOR NA 1ª COLUNA E 4ª LINHA DO DISPLAY
  lcd.print("> JESSE ANDRADE     ");  //IMPRIME "JESSE ANDRADE" NO DISPLAY
  delay(2500);                        //AGUARDA 2 SEGUNDOS
  resetaMenu();                       //LIMPA VÁRIAVEIS DE CONTROLE DE MENU
}


//########################################################################################################
//FUNÇÃO >> configuraTimers - CONFIGURA TIMERS NA INICIALIZAÇÃO
//########################################################################################################
void configuraTimers(){
  Timer2.pause();                                   //PAUSA O Timer2
  Timer3.pause();                                   //PAUSA O Timer3
  Timer4.pause();                                   //PAUSA O Timer4
  Timer2.setPeriod(TIMER_RATE);                     //DEFINE PERÍODO DE ESTOURO (SERVE PARA TODOS OS TIMERS)
  Timer2.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);  //ESCOLHE EM QUAL CANAL OPERAR, E FORMA DE OPERAÇÃO
  Timer3.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);  //ESCOLHE EM QUAL CANAL OPERAR, E FORMA DE OPERAÇÃO
  Timer4.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);  //ESCOLHE EM QUAL CANAL OPERAR, E FORMA DE OPERAÇÃO
  Timer2.setCompare(TIMER_CH1, 1);                  //DEFINE FORMA DE COMPARAR
  Timer3.setCompare(TIMER_CH1, 1);                  //DEFINE FORMA DE COMPARAR
  Timer4.setCompare(TIMER_CH1, 1);                  //DEFINE FORMA DE COMPARAR  
  Timer2.attachInterrupt(TIMER_CH1, clockStm);      //ANEXA FUNÇÃO AO DISPARAR O Timer4  
  Timer3.attachInterrupt(TIMER_CH1, mostraTempo);   //ANEXA FUNÇÃO AO DISPARAR O Timer3
  Timer4.attachInterrupt(TIMER_CH1, buzzerOn);      //ANEXA FUNÇÃO AO DISPARAR O Timer4 
}
