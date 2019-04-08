const int Pinsensor= A0;
int opcio=0;
int temps;
int comptador=0;
float mytemp[10];
int i=0;
int parat=1;





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
  
}


// VOID QUE S'EXECUTA SI ESTEM EN EL CAS 1 DE MARXA
void marxa() {
  
}
 
