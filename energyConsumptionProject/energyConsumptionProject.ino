 
#define RELAY 2   
#define SENSOR A0
#define SENSITIVITY 66

int input;    
double consumption = 0;
bool on = false;
uint32_t lastTime;
int passedTime;
float remainingTime= 86400;

void setup(){
  Serial.begin(9600);
  pinMode(RELAY, OUTPUT);
  pinMode(SENSOR, INPUT);
  lastTime = millis();
}
 
void loop(){
  readValuesFromAndroid();
  passedTime = millis()-lastTime;
  lastTime = millis();
  consumption += readCurrent()*220*(passedTime)/3600000;
  remainingTime -= passedTime/1000;
  if(remainingTime <= 0){
    remainingTime = 86400;
    on = !on;
    digitalWrite(RELAY, on);
  }
  sendAndroidValues();
}

void sendAndroidValues(){
  //puts # before the values so our app knows what to do with the data
  Serial.print('#');
  Serial.print(consumption);
  Serial.print('~'); //used as an end of transmission character - used in app for string length
  Serial.println();
  delay(10);        //added a delay to eliminate missed transmissions
}

void readValuesFromAndroid(){
  if (Serial.available()) {
    delay(100);
    input = Serial.parseInt();
    if( input == 1 ) on = false;
    else if( input == 0 ) on = true;
    else remainingTime = input;
    digitalWrite(RELAY, on);
  }
}

double readCurrent(){
  int readValue;             //value read from the sensor
  int maxValue = 516;        // store max value here
  double voltage = 0;         
  double voltageRMS = 0;
  double ampsRMS = 0;
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000){
       readValue = analogRead(SENSOR);
       // see if you have a new maxValue
       if (readValue > maxValue) {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
   }
   voltage = ((maxValue - 516) * 5.0)/1024.0;
   voltageRMS = voltage * 0.707; 
   ampsRMS = (voltageRMS*1000)/SENSITIVITY;
   return ampsRMS;
 }
