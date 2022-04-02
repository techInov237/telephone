#include <String.h>

// module GSM
#include <SoftwareSerial.h>
SoftwareSerial sim(6, 7);
String _buffer;
String number = "+237656068502"; //-> Numero d'envoie du message
char Received_SMS;
String RSMS;  
int taille_du_message =0;

// ecran LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);

// cablage des entrées du clavier
#define ligne_0 2
#define ligne_1 3
#define ligne_2 4
#define ligne_3 5

#define colonne_0 8
#define colonne_1 9
#define colonne_2 10
#define colonne_3 11

// buzzer
const int buzzer = 12; 


void setup() {


  //delay(7000); // delay de 7 secondes pour s'assurer que les modules reçoivent le signal
  Serial.begin(9600);
  _buffer.reserve(50);
  Serial.println("Système En marche...");
  sim.begin(9600);
  delay(1000);
  ReceiveMode();      //Calling the function that puts the SIM800L moduleon receiving SMS mode
  Serial.println("Tape A pour l'envoi d'un SMS, B pour recevoir un SMS et C pour passer un appel");
  
  // initialisation du LCD
  lcd.init();
  lcd.init();
  lcd.backlight();

// message de initial sur le LCD
  lcd.setCursor(0,0);
  lcd.print("Appel: A    SMS:");
  lcd.setCursor(0,1);
  lcd.print("B-reçois C-send ");
  
  // Initialise le port série
  Serial.begin(9600);

  // On configure les lignes en sorties
  pinMode(ligne_0, OUTPUT);
  pinMode(ligne_1, OUTPUT);
  pinMode(ligne_2, OUTPUT);
  pinMode(ligne_3, OUTPUT);

  // On configure les colonnes en entrées avec pull-up
  pinMode(colonne_0, INPUT_PULLUP);
  pinMode(colonne_1, INPUT_PULLUP);
  pinMode(colonne_2, INPUT_PULLUP);
  pinMode(colonne_3, INPUT_PULLUP);  

  pinMode(buzzer, OUTPUT);

}



void loop() {

  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Appel: A    SMS:");
  lcd.setCursor(0,1);
  lcd.print("B-receive C-send ");

// interface GSM
 
  char mode_appel_ou_sms;
  mode_appel_ou_sms = renvoie_le_buton_presse();
  Serial.println(mode_appel_ou_sms);
  sim.println(mode_appel_ou_sms);
  
    //if (Serial.available() < 0)
   //{
    //Serial.println("ok");
    //switch (Serial.read())
    switch (mode_appel_ou_sms)
    {
      Serial.println("ok");
      case 'C':
        SendMessage();
        break;

        case 'A':
        callNumber();
        break;
        
      case 'B':
        String mot = message_recu();
        //mot = message_recu();
        lcd.init();
        lcd.init(); 
        lcd.setCursor(0,0);
        
        for (int i=50; i<60; i++)
          {
          
          lcd.print(mot[i]);    
          }
          
          
        lcd.print(mot);
         
        break;
        
      
    }
    if (sim.available() > 0)
    Serial.write(sim.read());

   //}

//String num = numero_de_telph();
//Serial.println(num);
}






//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// fonction du module GSM
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$




void SendMessage()
{
  //$$$$$$$$$$$$$$$$$$$$$$$
  //Serial.println ("Envoi du message");
  sim.println("AT+CMGF=1");    //Définit le module GSM en mode texte
  delay(200);
  //Serial.println ("Définir le numéro de SMS");
  number = numero_de_telph();
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Numéro de téléphone portable pour envoyer le message
  delay(200);
  String SMS = message();
  sim.println(SMS);
  
  //LCD affiche le message
  lcd.init();
  lcd.init();
  lcd.setCursor(0,1);
  lcd.print("Envoie du SMS...");
  delay(500);
  sim.println((char)26);
  delay(200);
  _buffer = _readSerial();

  
}

///////////////
void Serialcom() //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
{
  delay(500);
  while(Serial.available())                                                                      
  {
    sim.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim.available())                                                                      
  {
    Serial.write(sim.read());//Forward what Software Serial received to Serial Port
  }
  
}
void ReceiveMode(){       //Set the SIM800L Receive mode
  
  sim.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  sim.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  sim.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}


//////////////////////////////////////////////////////////////
String _readSerial() {
  int _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  { 
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}

////////////////////////////////////////////////////////////////

String message_recu(){
  String RSMS;
  taille_du_message = 1;
  while(sim.available()<0){
     //ReceiveMode();             
     while(sim.available()>0){
        
     Received_SMS=sim.read();  
     Serial.print(Received_SMS); 
     RSMS.concat(Received_SMS); 
     //RSM[taille_du_message] = Received_SMS;
     taille_du_message ++; 
    }
    }
    return RSMS; 
}

////////////////////////////////////////////////////////////////


void callNumber() {

  number = numero_de_telph();
  
  sim.print (F("ATD")); 
  sim.print (number);
  sim.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);

  lcd.init();
  lcd.init();
  lcd.setCursor(5,0);
  lcd.print("CALL...");
  //delay(10000);
  
}





//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// code du clavier
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


char renvoie_le_buton_presse()
{
  do
   {
    // put your main code here, to run repeatedly:

  int etat_colonne_0 = HIGH;
  int etat_colonne_1 = HIGH;
  int etat_colonne_2 = HIGH;
  int etat_colonne_3 = HIGH;

  // On met la ligne 0 à 0V et les autres au 5V
  digitalWrite(ligne_0, LOW);
  digitalWrite(ligne_1, HIGH);
  digitalWrite(ligne_2, HIGH);
  digitalWrite(ligne_3, HIGH);

  // On recupère l'état des colonnes
  etat_colonne_0 = digitalRead(colonne_0);
  etat_colonne_1 = digitalRead(colonne_1);
  etat_colonne_2 = digitalRead(colonne_2);
  etat_colonne_3 = digitalRead(colonne_3);

  // Si on a la colonne 0 à LOW, c'est que le bouton 1 est pressé
  if(etat_colonne_0 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '1';
  }

  // Si on a la colonne 1 à LOW, c'est que le bouton 2 est pressé
  if(etat_colonne_1 == LOW) {
     delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '2';  
  }

    // Si on a la colonne 2 à LOW, c'est que le bouton 3 est pressé
  if(etat_colonne_2 == LOW) {
     delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '3';   
  }

    // Si on a la colonne 3 à LOW, c'est que le bouton A est pressé
  if(etat_colonne_3 == LOW) {
     delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return 'A';    
  }

  // On met la ligne 1 à 0V et les autres au 5V
  digitalWrite(ligne_0, HIGH);
  digitalWrite(ligne_1, LOW);
  digitalWrite(ligne_2, HIGH);
  digitalWrite(ligne_3, HIGH);

  // On recupère l'état des colonnes
  etat_colonne_0 = digitalRead(colonne_0);
  etat_colonne_1 = digitalRead(colonne_1);
  etat_colonne_2 = digitalRead(colonne_2);
  etat_colonne_3 = digitalRead(colonne_3);

  // Si on a la colonne 0 à LOW, c'est que le bouton 4 est pressé
  if(etat_colonne_0 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '4';
  }

  // Si on a la colonne 1 à LOW, c'est que le bouton 5 est pressé
  if(etat_colonne_1 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '5';   
  }

    // Si on a la colonne 2 à LOW, c'est que le bouton 6 est pressé
  if(etat_colonne_2 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '6';   
  }

    // Si on a la colonne 3 à LOW, c'est que le bouton B est pressé
  if(etat_colonne_3 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return 'B';  
  }

  // On met la ligne 2 à 0V et les autres au 5V
  digitalWrite(ligne_0, HIGH);
  digitalWrite(ligne_1, HIGH);
  digitalWrite(ligne_2, LOW);
  digitalWrite(ligne_3, HIGH);

  // On recupère l'état des colonnes
  etat_colonne_0 = digitalRead(colonne_0);
  etat_colonne_1 = digitalRead(colonne_1);
  etat_colonne_2 = digitalRead(colonne_2);
  etat_colonne_3 = digitalRead(colonne_3);

  // Si on a la colonne 0 à LOW, c'est que le bouton 7 est pressé
  if(etat_colonne_0 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '7';
  }

  // Si on a la colonne 1 à LOW, c'est que le bouton 8 est pressé
  if(etat_colonne_1 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '8';    
  }

    // Si on a la colonne 2 à LOW, c'est que le bouton 9 est pressé
  if(etat_colonne_2 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '9';  
  }

    // Si on a la colonne 3 à LOW, c'est que le bouton C est pressé
  if(etat_colonne_3 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return 'C';  
  }  

  // On met la ligne 3 à 0V et les autres au 5V
  digitalWrite(ligne_0, HIGH);
  digitalWrite(ligne_1, HIGH);
  digitalWrite(ligne_2, HIGH);
  digitalWrite(ligne_3, LOW);

  // On recupère l'état des colonnes
  etat_colonne_0 = digitalRead(colonne_0);
  etat_colonne_1 = digitalRead(colonne_1);
  etat_colonne_2 = digitalRead(colonne_2);
  etat_colonne_3 = digitalRead(colonne_3);

  // Si on a la colonne 0 à LOW, c'est que le bouton * est pressé
  if(etat_colonne_0 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '*';
  }

  // Si on a la colonne 1 à LOW, c'est que le bouton 0 est pressé
  if(etat_colonne_1 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '0';   
  }

    // Si on a la colonne 2 à LOW, c'est que le bouton # est pressé
  if(etat_colonne_2 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);
    return '#';    
  }

    // Si on a la colonne 3 à LOW, c'est que le bouton D est pressé
  if(etat_colonne_3 == LOW) {
    delay(300);
    tone (buzzer, 1000, 250);
    delay(100);   
    return 'D';
  }  
  /*else 
  {
    return NULL;
  }*/

  } while (1);
   
}




//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$





String numero_de_telph() {
  String numero = "+237" ;
  int i=1;

  lcd.init();
  lcd.init();
  lcd.setCursor(0,0);
  lcd.print("Entrez votre");
  lcd.setCursor(0,1);
  lcd.print("numero");

  numero += renvoie_le_buton_presse();
    Serial.println(numero);
    lcd.init();
    lcd.init();
    lcd.setCursor(0,0);
    lcd.print(numero);
  
  while (i <=8)
  {
    numero += renvoie_le_buton_presse();
    i++;
    Serial.println(numero);
    lcd.setCursor(0,0);
    lcd.print(numero);
  }

  return numero;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

String message() {
  String message ;
  int i=1;

  lcd.init();
  lcd.init();
  lcd.setCursor(0,0);
  lcd.print("Entrez votre");
  lcd.setCursor(0,1);
  lcd.print("message");

   message += renvoie_le_buton_presse();
    Serial.println(message);
    lcd.init();
    lcd.init();
    lcd.setCursor(0,0);
    lcd.print(message);
  
  while (i <=4)
  {
    message += renvoie_le_buton_presse();
    i++;
    Serial.println(message);
    lcd.setCursor(0,0);
    lcd.print(message);
  }

  return message;
}
