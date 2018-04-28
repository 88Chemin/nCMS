int i_loop=0;
int meanSum=0;
int peakValue=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue=analogRead(A0);
  sensorValue-=256;
  //if(sensorValue>200){
    //digitalWrite(LED_BUILTIN,HIGH);
    //delay(5);
    //digitalWrite(LED_BUILTIN,LOW);    
    //}
    
  i_loop++;
  meanSum=meanSum+sensorValue;
  if(sensorValue>peakValue)peakValue=sensorValue;
  if (i_loop>2000){
    Serial.println(peakValue);
    i_loop=0;
    meanSum=0;
    peakValue=0;
  }
  delay(0.2);
}
