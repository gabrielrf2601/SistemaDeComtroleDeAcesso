//Bibliotecas
#include <LinkedList.h>
#include <MFRC522.h>  //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI

//Pinos
#define SS_PIN    21
#define RST_PIN   22
//#define RELE_PIN 32
#define BUTTON_PIN 27
#define LED_VERDE     12
#define LED_VERMELHO  32

String IDtag = ""; //Variável que armazenará o ID da Tag
bool Permitido = false; //Variável que verifica a permissão

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Cria uma nova instância para o leitor e passa os pinos como parâmetro

//Declaração da classe "Pessoa"
class Pessoa {
  public:
    String key;

  Pessoa(){
    this->key = "";
  }
  
  Pessoa(String id){
    this->key = id;
  }
};

//Lista encadeada responsável por armazenar os ID's das Tag's cadastradas
LinkedList<Pessoa> myLinkedList;

void setup() {
  Serial.begin(9600);             // Inicializa a comunicação Serial
  SPI.begin();                    // Inicializa comunicacao SPI
  mfrc522.PCD_Init();             // Inicializa o leitor RFID
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println(); 
  pinMode(LED_VERDE, OUTPUT);     // Declara o pino do led verde como saída
  pinMode(LED_VERMELHO, OUTPUT);  // Declara o pino do led vermelho como saída
  pinMode(BUTTON_PIN, INPUT);     // Declara o pino do botão como entrada
}

void loop() {
  pushButton();
  Leitura();  //Chama a função responsável por fazer a leitura das Tag's
}

void Leitura() {

  IDtag = ""; //Inicialmente IDtag deve estar vazia.

  // Verifica se existe uma Tag presente
  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  // Pega o ID da Tag através da função mfrc522.uid e Armazena o ID na variável IDtag
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    IDtag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  if (IDtag.equalsIgnoreCase("23A4D483")) //UID 1 - Chaveiro
  {
    Serial.print("Modo Cadastramento");
    delay(2000);
    cadastrar();
    delay(3000);
    return;
  }

  //Compara o valor do ID lido com os IDs armazenados no vetor TagsCadastradas[]
  for (int i = 0; i < (myLinkedList.size()); i++) {
    if (IDtag.equalsIgnoreCase(myLinkedList.get(i).key)) {
      Permitido = true; //Variável Permitido assume valor verdadeiro caso o ID Lido esteja cadastrado
    }
  }

  if (Permitido == true) acessoLiberado(); //Se a variável Permitido for verdadeira será chamada a função acessoLiberado()
  else acessoNegado(); //Se não será chamada a função acessoNegado()

  delay(2000); //aguarda 2 segundos para efetuar uma nova leitura
}

void pushButton() {
  if (digitalRead(BUTTON_PIN)) {
    Serial.println("LIBERADO!");
    liberar();
  }
}

void acessoLiberado() {
  Serial.println("Tag Cadastrada: " + IDtag); //Exibe a mensagem "Tag Cadastrada" e o ID da tag não cadastrada
  liberar();
  Permitido = false;  //Seta a variável Permitido como false novamente
}

void liberar() {
  digitalWrite(LED_VERDE, HIGH);
  delay(1000);
  digitalWrite(LED_VERDE, LOW);
}

void acessoNegado() {
  Serial.println("Tag NAO Cadastrada: " + IDtag); //Exibe a mensagem "Tag NAO Cadastrada" e o ID da tag cadastrada
  digitalWrite(LED_VERMELHO, HIGH);
  delay(1000);
  digitalWrite(LED_VERMELHO, LOW);
}

void cadastrar(){
  delay(1000);
  while ( ! mfrc522.PICC_IsNewCardPresent())  {
    delay(100);
  }
  while ( ! mfrc522.PICC_ReadCardSerial()){
    delay(100);
  }
  delay(500);
  String conteudo = "";
  for (byte i = 0; i < mfrc522.uid.size; i++){
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.println();
  conteudo.toUpperCase();
  if (ehcadastrado(conteudo)){
    Serial.println("TAG JÁ CADASTRADA!!");
  }
  else{
    Pessoa novo(conteudo);
    myLinkedList.add(novo);
    Serial.println("TAG CADASTRADA!");
    Serial.println(conteudo);
  }
}

bool ehcadastrado(String conteudo) {
  for (int i = 0; i < myLinkedList.size(); i++) {
    Pessoa pessoa = myLinkedList.get(i);
    if (conteudo == pessoa.key)
    {
      return true;
    }
  }

  return false;
}
