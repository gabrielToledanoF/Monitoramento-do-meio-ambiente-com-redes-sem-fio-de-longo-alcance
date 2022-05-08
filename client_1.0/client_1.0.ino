//Conexoes:
//O SX127x Arduino Shield Dragino v.1.4 deve estar devidamente conectado ao Arduino
//O botao deve estar conectado ao pin 7 

//Includes para a utilização da RadioHead
#include <SPI.h>
#include <RH_RF95.h>

//Criação de variáveis globais
RH_RF95 rf95;                        //Instancia do modulo de transmissao
int sf[] = {6, 7, 8, 9, 10, 11, 12}; //Valores do Spreading Factor
int cr[] = {5, 6, 7, 8};             //Valores do Coding Rate
int i = 2, j = 0;                    //Variáveis para caminharmos pelo sf e cr
int tam = 100;                       //Tamanho do experimento, quantas mensagens serao enviadas
int contador;                        //Contador do número da mensagem atual
uint8_t msg[251];                    //Base da mensagem a ser enviada
int F;                               //Armazena a quantidade de mensagens que falharam em ser recebidas
  


void setup(){
  //Inicializa a serial do Arduino
  Serial.begin(9600);
  while(!Serial); //Espera o Serial estar disponível;

  //Inicialização do modulo
  if (!rf95.init()){//Caso o módulo falhe a inicialização
    Serial.println("Falha na inicialização, verifique o equipamento");
    while(1);
  } 
  Serial.println("Módulo Inicializado com sucesso!");


  
  //Criacao da mensagem
  Serial.println("Preparando base da mensagem para o envio.");
  for(int x = 0; x < 250; x++){
    msg[x] = 'a';
  }
  msg[250] = '\0';
  Serial.println("Mensagem preparada!");

  //Fim do Set Up
  Serial.println("Para inicializar o experimento, pressione o botão.");
  Serial.println("");
}

void loop(){
  //(Re)Inicializacao dos valores das variaveis
  F = 0;
  contador = 0;
  
  //Espera o botao ser pressionado para prosseguir a execucao
  while(!digitalRead(7));
  
  //Configuração dos valores de Spreading Factor e Coding Rate
  rf95.setSpreadingFactor(sf[i]);//valore entre 6 e 12
  Serial.println("Spreading Factor definido para " + String(sf[i]) + ".");
  
  rf95.setCodingRate4(cr[j]);//  fração com denominador de 5 até 8
  Serial.println("Coding Rate definido para " + String(cr[j]) + ".");
  //Fim das configuracoes
  
  Serial.println("Inicializando o envio de mensagens.");
  //Envio das mensagens
  while(contador < tam){
      sendMessage();
      contador ++;
    }
  Serial.println("");
  Serial.println("Sequência Finalizada.");
  Serial.println("Para esta sequência " + String(F) + " mensagens não foram respondiddas.");
  Serial.println("Para prosseguir os testes, pressione o botão.");
  Serial.println("");
  while(!digitalRead(7));
  iteracao(); //Caminha pelos parametros do Lora
  Serial.println("");
  
}

//Funcao que caminha pela combinacao dos valores do Spreading Factor e Coding Rate
void iteracao(){
  if(i == 6){
    Serial.println("Fim da Bateria de Testes!");
    while(1);
  }
  if(j == 3){
    j = 0;
    i += 1;
  } else{ j+= 1;}
}

//Funcao responsavel pelo envio e captura de mensagens
void sendMessage(){
  rf95.send(msg, sizeof(msg));
  rf95.waitPacketSent();
  Serial.println(",");
  
  //Retorno
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
        
  if (rf95.waitAvailableTimeout(3000)){
    if (rf95.recv(buf, &len)){
      //Caso a mensagem seja respondida normalmente
      Serial.print(",True,");
      Serial.print(rf95.lastRssi(), DEC);
      Serial.println(";");
     }
     else{
      //Caso a mensagem seja respondida, porem haja erro na sua captacao
      Serial.print(",Fail,");
      Serial.print(rf95.lastRssi(), DEC);
      Serial.println(";");
      F ++;}
     }
   
   else{
    //Caso a resposta nao tenha sido respondida
    Serial.println(",False,0;");
    F ++;}
   delay(400);
}
