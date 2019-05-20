//Bibliotecas
#include <LinkedList.h>
#include <MFRC522.h>  //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI
#include <LiquidCrystal_I2C.h>

//Pinos
#define SS_PIN    4
#define RST_PIN   2
#define RELE_PIN 32
//#define BUTTON_PIN 27
#define VERDE 26
#define VERMELHO 33
#define AZUL 27
#define LED_AZUL 35

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int lcdAddr = 39;

// set LCD address, number of columns and rows
LiquidCrystal_I2C lcd(lcdAddr, lcdColumns, lcdRows);  

String IDtag = ""; //Variável que armazenará o ID da Tag
bool Permitido = false; //Variável que verifica a permissão

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Cria uma nova instância para o leitor e passa os pinos como parâmetro

//Declaração da classe "Pessoa"
class Pessoa {
  public:
    String nome;
    String key;

  Pessoa(){
    this->nome = "";
    this->key = "";
  }
  
  Pessoa(String id){
    this->key = id;
  }

  Pessoa(String id, String nomeID){
    this->key = id;
    this->nome = nomeID;
  }
};

//Lista encadeada responsável por armazenar os ID's das Tag's cadastradas
LinkedList<Pessoa> myLinkedList;

void setup() {
  Serial.begin(9600);             // Inicializa a comunicação Serial
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  SPI.begin();                    // Inicializa comunicacao SPI
  mfrc522.PCD_Init();             // Inicializa o leitor RFID
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println(); 
  pinMode(AZUL, OUTPUT);
  pinMode(VERDE, OUTPUT);     // Declara o pino do led verde como saída
  pinMode(VERMELHO, OUTPUT);  // Declara o pino do led vermelho como saída
  pinMode(LED_AZUL, OUTPUT);
  pinMode(RELE_PIN, OUTPUT);
//  pinMode(BUTTON_PIN, INPUT);     // Declara o pino do botão como entrada
}

void loop() {
  digitalWrite(LED_AZUL, HIGH);
  //pushButton();
  telaInicial();  
  Leitura();  //Chama a função responsável por fazer a leitura das Tag's
}

void telaInicial(){
//  lcd.clear();
  lcd.setCursor(2, 0);
  // print message
  lcd.print("APROXIME SEU");
  lcd.setCursor(5, 1);
  // print message
  lcd.print("CARTAO");
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
    Serial.print("\n");
    Serial.print(String(mfrc522.uid.uidByte[i], HEX));
  }

  if (IDtag.equalsIgnoreCase("23A4D483")) //UID 1 - Chaveiro
  {
    lcd.clear();
    // set cursor to first column, first row
    lcd.setCursor(0, 0);
    // print message
    lcd.print("     MODO DE");
    // set cursor to first column, first row
    lcd.setCursor(0, 1);
    // print message
    lcd.print("CADASTRAMENTO...");
    brancoLiga();
    delay(2000);
    lcd.clear();
    cadastrar();
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

//void pushButton() {
//  if (digitalRead(BUTTON_PIN)) {
//    lcd.print("    LIBERADO");
//    liberar();
//  }
//}

void acessoLiberado(){
  Permitido = false;  //Seta a variável Permitido como false novamente
  verdeLiga();
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("ACESSO LIBERADO,");
  // set cursor to first column, first row
  lcd.setCursor(3, 1);
  // print message
  lcd.print("BEM VINDO!");
  digitalWrite(RELE_PIN, HIGH);
  delay(1000);
  digitalWrite(RELE_PIN, LOW);
  delay(3000);
  // clears the display to print new message
  lcd.clear();
  verdeDesliga();
}

void acessoNegado() {
  vermelhoLiga();
  // set cursor to first column, first row
  lcd.setCursor(1, 0);
  // print message
  lcd.print("ACESSO NEGADO!");
  // set cursor to first column, first row
  lcd.setCursor(0, 1);
  // print message
  lcd.print(" NAO CADASTRADO");
  delay(4000);
  // clears the display to print new message
  vermelhoDesliga();
  lcd.clear();
}

void cadastrar(){
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("    APROXIME");
  // set cursor to first column, first row
  lcd.setCursor(0, 1);
  // print message
  lcd.print("   SEU CARTAO");
  delay(1000);
  while (!mfrc522.PICC_IsNewCardPresent())  {
    delay(100);
  }
  while (!mfrc522.PICC_ReadCardSerial()){
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
    brancoDesliga();
    amareloLiga();
    lcd.clear();
    // set cursor to first column, first row
    lcd.setCursor(0, 0);
    // print message
    lcd.print("   USUARIO JA");
    // set cursor to first column, first row
    lcd.setCursor(0, 1);
    // print message
    lcd.print("   CADASTRADO");
    delay(3000);
    amareloDesliga();
    lcd.clear();
  }
  else{
    Pessoa novo(conteudo);
    myLinkedList.add(novo);
    brancoDesliga();
    verdeLiga();
    lcd.clear();
    // set cursor to first column, first row
    lcd.setCursor(1, 0);
    // print message
    lcd.print("CADASTRADO COM");
    // set cursor to first column, first row
    lcd.setCursor(4, 1);
    // print message
    lcd.print("SUCESSO!");
    delay(3000);
    verdeDesliga();
    lcd.clear();
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

void brancoLiga(){
  digitalWrite(AZUL, HIGH);
  digitalWrite(VERDE, HIGH);
  digitalWrite(VERMELHO, HIGH);
}
void brancoDesliga(){
  digitalWrite(AZUL, LOW);
  digitalWrite(VERDE, LOW);
  digitalWrite(VERMELHO, LOW);
}
void vermelhoLiga(){
  digitalWrite(AZUL, LOW);
  digitalWrite(VERDE, LOW);
  digitalWrite(VERMELHO, HIGH);
}
void vermelhoDesliga(){
  digitalWrite(VERMELHO, LOW);
}
void verdeLiga(){
  digitalWrite(AZUL, LOW);
  digitalWrite(VERDE, HIGH);
  digitalWrite(VERMELHO, LOW);
}
void verdeDesliga(){
  digitalWrite(VERDE, LOW);
}
void amareloLiga(){
  digitalWrite(AZUL, LOW);
  digitalWrite(VERDE, HIGH);
  digitalWrite(VERMELHO, HIGH);
}
void amareloDesliga(){
  digitalWrite(VERDE, LOW);
  digitalWrite(VERMELHO, LOW);
}
