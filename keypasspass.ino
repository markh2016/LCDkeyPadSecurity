
#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>


#define STRING_TERMINATOR '\0'
#define _LOCK   10 


/*MD Harrington Tuesday 29th May 2018  */

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
const int ROWS = 4 ;
const int COLS = 4 ;
const int MAX_INDEX  = 7 ;
const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 511;



char key ;
int State = 0;
int col  = 0 ; 
int index = 0 ;
boolean locked = false ; 


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};


char  defkey[MAX_INDEX]="123456"  ;

 
char  _oldkey[MAX_INDEX] ;
char  _newkey[MAX_INDEX] ;

                    /* display message strings for lcd */
                    const char *msg0 = " M.D. Harrington" ;
                    const char *msg1 = "Enter*||Retype#" ;
                    const char *msg2 = "Codekey Entered" ;
                    const char *msg3 = "User  Cancelled" ; 
                    const char *msg4 = "Please Re Enter" ;
                    const char *msg5 = "key C  for Menu";
                    const char *msg6 = "ArduinoEmbedded" ;
                    const char *msg7 = " Key.Entry.Mode" ;
                    const char *msg8 = " Lock B Unlock D";
                    const char *msg9 = " Validating Key ";
                    const char *msgA = "PassKey Accepted";
                    const char *msgB = "Passkey  Invalid";
                    const char *msgC = "Lock --- Engaged";
                    const char *msgD = "Lock Dis-engaged";

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


void setup() {


  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
   
  }

  pinMode(_LOCK, OUTPUT) ;   
 

  lcd.begin(16, 2);
  lcd.clear();
  showMsg(msg6 ,1,0) ;
  showMsg(msg0,0,1);

  Serial.println(defkey);
 


  
//Serial.print(" Default pass written to EEPROM asserted ") ; 
//Serial.println( eeprom_write_bytes(0,defkey,MAX_INDEX));

  // read from the EEPROM back into _oldKey
 Serial.print(" Reading back from EEPROM ") ;
 Serial.println(eeprom_read_string(0, _oldkey,MAX_INDEX));
 Serial.println("default pass is now in _newkey  returned ") ;

 
 Serial.println(_oldkey);



}


/****************************************************************************************************************/
// Returns true if the address is between the
// minimum and maximum allowed values, false otherwise.

boolean eeprom_is_addr_ok(int addr) {
return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}   


/***************************************************************************************************************/
      
/* Writes a sequence of bytes to eeprom starting at the specified address.

                Returns true if the whole array is successfully written.
                Returns false if the start or end addresses aren't between
                the minimum and maximum allowed values.
                When returning false, nothing gets written to eeprom. */

 
    boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
    // counter
    int i;
    
    // both first byte and last byte addresses must fall within
    // the allowed range 
          
          if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
          return false;
          }
              for (i = 0; i < numBytes; i++) {
              EEPROM.write(startAddr + i, array[i]);
              }
    
    return true;
    }

/***********************************************************************************************************/

                      
      boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
      byte ch; // byte read from eeprom
      int bytesRead; // number of bytes read so far
      
      
      
            if (!eeprom_is_addr_ok(addr)) { // check start address
            return false;
            }
      
                if (bufSize == 0) { // how can we store bytes in an empty buffer ?
                return false;
      
                
                }
            // is there is room for the string terminator only, no reason to go further
            if (bufSize == 1) {
            buffer[0] = 0;
            return true;
            }
            
                  bytesRead = 0; // initialize byte counter
                  ch = EEPROM.read(addr + bytesRead); // read next byte from eeprom
                  buffer[bytesRead] = ch; // store it into the user buffer
                  bytesRead++; // increment byte counter
                                     
                                      
                                      /* stop conditions:                                
                                       
                                       the character just read is the string terminator one (0x00)
                                       we have filled the user buffer
                                       we have reached the last eeprom address
                                       
                                       */
                                      
                                       
                                     
                                      
                    while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
                          // if no stop condition is met, read the next byte from eeprom
                          ch = EEPROM.read(addr + bytesRead);
                          buffer[bytesRead] = ch; // store it into the user buffer
                          bytesRead++; // increment byte counter
                    }
                        // make sure the user buffer has a string terminator, (0x00) as its last byte
                              if ((ch != 0x00) && (bytesRead >= 1)) {
                              buffer[bytesRead - 1] = 0;
                              }
                    
            return true;
      }


/**********************************************************************************************************/
boolean checkPasswords(char old[], char submitted[] )
{

   char *p1 ; 
   char *p2; 
   
   p1 = old ;
   p2 = submitted ; 


      while(*p1 == *p2)
      {
          if ( *p1 == '\0' || *p2 == '\0' )
          break;
          
          p1++;
          p2++;
      }
          if( *p1 == '\0' && *p2 == '\0' )
          return true;
          else
          return false;
   
         
          
}

/***********************************************************************************************************/
     boolean lockUnlock(){
          char _key  ;
          bool flag  ;
             

             // B ascii dec 66 gives us locked , D ascii dec 68 gives us unlocked 
             while (!((_key == 66)||(_key == 68)))
             {
                    _key = keypad.getKey(); // get the next key press untill we have one or the other 
                    ;
                         switch(_key){
                                           Serial.println(_key) ;
                              case 'B' :
                                          digitalWrite(_LOCK , HIGH) ; // turn on the solonoid 

                                          flag = true ; 
                                          
                                          break ;
                
                              case 'D' :
                                          digitalWrite(_LOCK , LOW) ; // turn on the solonoid 

                                          flag = false ; 
                                          break ;
                              default:

                                          break ; 

                        

                         }
             }
      
      
        return  flag ; 
       }




/**********************************************************************************************************/ 

void showMsg(char * message,int top , int bot)
{
      // press accept to enter key 
         lcd.setCursor(top,bot);
            lcd.print(message);
              
              
}


/***********************************************************************************************************/
        

             

      void getUserPasskey(){

          int i = 0 ;
          char *p = _newkey ;
          char *initial; 
          initial = p ;
          bool RedoFlag = true  ; 
          char counter[1] ;
          String counterstr = String("Entries Left = ") ;
          State = 2 ;

          
          
          
                    
                     
          lcd.clear();
          // initialise the lcd position 
          // write menu for get user passord 
          showMsg(msg7,0,1) ;
          
          lcd.setCursor(5,0) ; //top row 5th column 

          // check to see if we are still entering letter keys 
         
          
          if((key>= 65)&&(key <= 68)){
            // key is letter and not digit 
                     
                     
                     while((key>= 65)&&(key <= 68))
                     {
                        key = keypad.getKey(); // get the next key  keep doing this untill we have digit 
                     }
          } // end if 

                    // we have digits only ;  we want to grab all digits 
                     
            do {
                   
                     


                    // we have digits only ;  we want to grab all digits 

              
                  if(!((key>= 65)&&(key <= 68))){ // looking for capital A,B,C,D  We dont want these We only want digits 
                       key = keypad.getKey(); // get next key 

                        // only concerened with digits  ascii  48 to ascii 59 
                        if((key <=59)&&(key >= 48))
                        {
                            lcd.setCursor((5+i),0) ; //top row 5th column 
                            lcd.print(key) ;
                            
                            lcd.setCursor((5+i),0); // relocate  cursor back to same position
                            delay(200) ;             // delay another 200ms 
                            
                            lcd.print('*');          // hide the  digit entered 
                            
                            lcd.setCursor(0,1);     // locate bottom row to indicate how many digits client has entered 
                            
                            *p = key ; // put this into the array holding old password ;  pointer is initialised at array[0] 
                             p++ ;
                             i+=1 ;  // increment i  by one 
                            
                            itoa((6-i),counter,10); // convert integer to chararray
                            
                            lcd.print(counterstr + counter);// print display message for client showing entries 
                           
                        } // end if
                        
                  }// end if 

                  // we now have all digits  now we want a yes or no as acceptance for passkey entered 

                  if (i==6)
                  {
                     delay(400);
                     key = keypad.getKey();
                     
                                          
                     showMsg(msg1,1,0); 
                     showMsg(msg5,0,1) ;

                     
                     
                     do {

                          
                           switch(key)
                           {
                               case '*' : 
                                          lcd.clear();
                                          showMsg(msg2,1,0); 
                                          RedoFlag = false ;
                                          showMsg(msg9,0,1);
                                          State = 2;  // accept state 
                                              
                                               
                                               if (checkPasswords(_oldkey,_newkey) )
                                              {
                                                  State = 1 ; // password accepted state 
                                                  showMsg(msgA,0,1);
                                                      delay(1000);
                                                  showMsg(msg8,0,1);

                                                          if (locked=lockUnlock()) {
                                                           showMsg(msgC,0,1) ; 
                                                           showMsg(msg5,1,0); 
                                                          }
                                                          else 
                                                              {
                                                                showMsg(msg5,1,0); 
                                                                showMsg(msgD,0,1) ;
                                                              }
                                                       
                                              }else
                                                  {
                                                            showMsg(msgB,0,1);
                                                            delay(1000);
                                                            State = 3 ; // exit back to main menu
                                                  }
                                          
                                         
                                          
                                    break ; 
                               case '#':
                                          lcd.clear();
                                          showMsg(msg3,1,0); 
                                          RedoFlag = false ;
                                          State = 3 ;
                                    break ;

                               case 'C':  
                                          
                                          Serial.println(key);
                                        
                                          RedoFlag = true ;
                                          State = 3; // exit state 
                                          Serial.print("State =" ) ;
                                          Serial.println(State,DEC) ; 
                                    break ; 
                               
                               
                               default :

                                    break ; 
                                    
                            
                           }

                      
                          key = keypad.getKey();
                         
                        
                     } while(((key!=42)||(key!=35))&&(State !=3));
                     
                    
                    
                     
                  
                  } // end if i == 6 

                  

                  // check to see if client rejected his entry 
                 
                   
                  


            }while ((i < 6)&&(RedoFlag==true)) ; // end  do while (i < 6)

         
        
            // add our '/0' to the array 
          *p = STRING_TERMINATOR ;

          Serial.println(_oldkey ); // this is for debug puposes so i can see if password entry was complete  
          int  dimension = (sizeof(_oldkey)/sizeof(char)); //array size  
         
          /* debuggging 
             i= 0; // initialise i 
             while (i < 6){
              
              Serial.println( _oldkey[i]);
              i++ ;
             }
            */ 

         
          
                
                     lcd.setCursor(1,0) ;
                     lcd.clear();
                     showMsg(msg6,1,0); 
                     showMsg(msg0,0,1);
                
                     

      }// end method 
    
    
    
   /*****************************************************************************************************************/


   

      void loop() {
      
     

      
            key = keypad.getKey();

            /* debug purposes 
             *  
             */
              
             
             switch (key)
             {
                 
                 /*using letter keys to obtain different fucntionality of main menu */
                
                case 'A':
                         // Serial.println("Enter Password ") ;  // for debug purposes only 
                         
                        getUserPasskey() ;
                  break ;
                
                
                case 'B': 

                    break ;


                case 'C': 

                    break ; 

                case 'D':

                    break ; 

                default:
                         
                    break ;
                    

                
               


             } 


    
      
            
        
       } // end loop    
