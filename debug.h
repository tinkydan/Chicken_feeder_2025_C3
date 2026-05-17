#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTln(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTln(x)
#endif


//#define DEBUG_WAIT


#ifdef DEBUG_WAIT
#define DEBUG_PRINT_WAIT(x) 
  Serial.println(x); 
  //delay(250);
#else
#define DEBUG_PRINT_WAIT(x)
#endif
