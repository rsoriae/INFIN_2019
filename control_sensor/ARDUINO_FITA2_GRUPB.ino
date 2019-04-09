const int Pinsensor= A0;
int opcio=0;
int temps;
int comptador=0;
float mytemp[10];
int i=0;
int parat=1;
String data;
String data1;
int mostres;
int tempsd;
int tempsu;
int tempsentre;
int sortidau;
int sortidad;
int sortida;
int entradau;
int entradad;
int entrada;
int valorentrada;




//respostes
String msgM="AMEZ";
String msgC="ACE0000Z";
String msgS="ASEZ";
String msgE="AE0EZ";


void setup()
{

  
  //Freqüència d'execució del programa
  Serial.begin(9600);

  //Se configuran los pins del 0 al 13 como salidas.
  //No s'han de configurar com a entrada els analogics
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  
/////////////INTERRUPCIONS///////////////  
   //Desactivar totes les interrupcions  
   noInterrupts();
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;
   OCR1A = 62500; // compare match register 16MHz/256/2Hz
   TCCR1B |= (1 << WGM12); // CTC mode
   TCCR1B |= (1 << CS12); // 256 prescaler
   TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
   interrupts(); // enable all interrupts
}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service

{
  //Cridada del void adquisició
  adquisicio();

}
////////////////////////////////////////////


void loop()
{
   
  switch (opcio) {
    
    //Case de parada
     case 0:
      
      if(parat == 1){
        
        Serial.println("");
        Serial.println("PARAT");
        parat=0;
      }
      parada();
      break;
      
      
    //Case de marxa i cridada de la funció marxa
    case 1:
     
      
      marxa();
      break;   
  }
}



// VOID QUE PREN LES DADES
void adquisicio() {
      //Si es troba en marxa, executará aquest if
      if(opcio == 1){
        
        comptador++;    

        //Comparació del comptador amb el temps determinat per codi de l'adquisició
        
        if(comptador == temps){

            //Llegirà el valor de l'entrada analògica (A0) i la mostra per pantalla
            int value = analogRead(Pinsensor);
            float millivolts = (value / 1023.0) * 5000; //conversió de milivolsts a celsius
            float celsius = millivolts / 10; 
            Serial.print(celsius);
            Serial.println(" ºC");
            
            //Crea l'array de valors de temperatura per poder fer la mitjana
            
            mytemp[i]=celsius;
            i++;
            comptador=0;
            
            //Funció per fer l'array en bucle, amb un màxim de 9 mostres enmmagatzemades
            
            if(i == 9){
  
              i=0;          
            }
        }
    }
}


// VOID QUE S'EXECUTA SI ESTEM EN EL CAS 0 DE PARADA
void parada() {

      //comprovació de la funció Marxa
      if((data[0]=='A')&&(data[1]=='M')&&(data[2]=='1')&&(data[6]=='Z')){
        
        //concersió del número de string (ASCII) a enter
        tempsd=data[3]-48;
        tempsu=data[4]-48;
        
        //conversió a número enter de 2 dígits
        temps=tempsd*10+tempsu; 
        mostres=data[5]-48;
        
        //temps entre mostres en segon
        tempsentre=temps*1000;

        //Comprovació del si el temps supera els 20 segons o les mostres són superiors a 9
        if((temps <= 20)&&(temps > 0)&&(mostres <= 9)&&(mostres > 0)){
          
         //Igualació per evitar mostrar continuament el missatge introduït
          data1=data;
          
          //Canvi de número a caràcter ASCII, sumant 48 (valor 0 en ASCII)
          msgM[2]=0+'0';
          
          //Missatga de codi correcte, el temps i les mostres.
          Serial.println("");
          Serial.println(data);
          Serial.println("");
          Serial.println(msgM);
          Serial.println("");
          Serial.print("TEMPS: ");
          Serial.println(temps);
          Serial.print("MOSTRES: ");
          Serial.println(mostres);
          Serial.println("");
          
          //es manté en situació marxa
          opcio=1;
        }

         else{
           
        //en cas contrari, s'envia un 2, error de paràmetres
          msgM[2]=2+'0';
          Serial.println("");
          Serial.println(msgM);
         }
      }

      //Comprovació d'error de protocol
      if((data[0]=='A') && (data[1]=='M') && (data[6]!='Z')){
     
          msgM[2]=1+'0';
          Serial.println("");
          Serial.println(msgM);

     }


     //Conversió analógic digital
      if((data[0]=='A') && (data[1]=='C') && (data[2]!='Z')){
          
          //Mostrar per pantalla error de protocol
          msgC[2]=1+'0';
                    
          Serial.println("");
          Serial.println(msgC);
      }


      //Comprovació comanda Sortida digital
      if((data[0]=='A') && (data[1]=='S') && (data[5]=='Z')){

        Serial.println("");
        Serial.println(data);
        
       //Conversió en números enters
        sortidad=data[2]-48;
        sortidau=data[3]-48;
        sortida=sortidad*10+sortidau;
        
        //comprovació del nº de pin de sortida dins del rang de l'arduino i que s'activi o es desactivi (1 o 0)
          if((sortida <= 13) && ((data[4]=='1')||(data[4]=='0'))){
            
              //Mode pin activat
              if(data[4]=='1'){
                
              //Activació sortida i declaració de sortida com la variable "sortida"
                pinMode(sortida,OUTPUT);
                digitalWrite(sortida,HIGH);
                
              //Mostra per pantalla codi correcte
                msgS[2]=0+'0';
                Serial.println("");
                Serial.println(msgS);
              }
              //Mode pin desactivat
              if(data[4]=='0'){
                
              //Desactivació sortida i declaració de sortida com la variable "sortida"
                pinMode(sortida,OUTPUT);
                digitalWrite(sortida,LOW);
              //Mostrar per pantalla codi correcte
                msgS[2]=0+'0';
                Serial.println("");
                Serial.println(msgS);
              }           
            }
            else{
            //Mostrar error de paràmetres
              msgS[2]=2+'0';
              Serial.println("");
              Serial.println(msgS);
            }
            
        }


        //Comprovació d'error de protocol Sortides
      if((data[0]=='A') && (data[1]=='S') && (data[5]!='Z')){

          
          msgS[2]=1+'0';
         
          Serial.println("");
          Serial.println(msgS);
      }

      //Comprovació Codi entrades, mateix funcionament que el codi de sortides
      if((data[0]=='A') && (data[1]=='E') && (data[4]=='Z')){
        
        Serial.println("");
        Serial.println(data);
        
        entradad=data[2]-48;
        entradau=data[3]-48;
        entrada=entradad*10+entradau;

        if(entrada <= 13){
          
        //Mode pullup per evitar rebots
        pinMode(entrada,INPUT_PULLUP);
        
        //Lectura del valor de l'entrada
        valorentrada= digitalRead(entrada);
        //Mostra del valor de lectura
        Serial.println("");
        Serial.println(valorentrada);
        
        msgE[3]=valorentrada+'0';
        Serial.println("");
        Serial.println(msgE);
        }
      }


      //Comprovació error de protocol d'entrades
      if((data[0]=='A') && (data[1]=='E') && (data[4]!='Z')){

          msgE[2]=1+'0';
          msgE[3]=0+'0';
          Serial.println("");
          Serial.println(msgE);
      }  
}


// VOID QUE S'EXECUTA SI ESTEM EN EL CAS 1 DE MARXA
void marxa() {
  
}
