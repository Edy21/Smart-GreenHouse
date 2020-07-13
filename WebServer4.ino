#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#include <ctype.h>
int MOTOR1_PIN1 = 6;
int MOTOR1_PIN2 = 9;

int var[6] = {0, 0, 0, 0, 0, 0};
int d = 0;
int stropit = 0;
int lMin = 220, tempMax = 25, umaMin = 0, umsMin = 80, durata = 0;
Servo myservo;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);

EthernetServer server(80);


float readTempInCelsius(int count, int pin) {
  float temperaturaMediata = 0;
  float sumaTemperatura = 0;
  for (int i =0; i < count; i++) {
    int reading = analogRead(pin);
    float voltage = reading * 5.0;
    voltage /= 1024.0;
    float temperatureCelsius = (voltage - 0.5) * 100 ;
    sumaTemperatura = sumaTemperatura + temperatureCelsius;
  }
  return sumaTemperatura / (float)count;
} 

void setup() {
  Serial.begin(9600);
  
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  myservo.attach(14);
  myservo.write(10);
}

int a = 0, b = 0, part, total;
float percentage;
void loop() {
  //cod pentru luminozitate ajustabila
  int l = analogRead(A2)/ 4;
  Serial.print(l);
  Serial.print("-");
  Serial.println(lMin);
  if(l < lMin)
  {
    analogWrite(7, (lMin - l) % 255);
  
  }else
    digitalWrite(7, 0);
  /*int t = readTempInCelsius(10, 3);
  if(t > tempMax){
    myservo.write(90);
    Serial.print(t);
    Serial.print("-");
    Serial.print(tempMax);
    Serial.println();
  
  }else myservo.write(10);
  */
/*
  int u = analogRead(A1);
 if(u < umsMin && stropit == 0){
      Serial.println(stropit);
      analogWrite(MOTOR1_PIN1, 255);
      analogWrite(MOTOR1_PIN2, 100);
      delay(3000);
      analogWrite(MOTOR1_PIN1, 0);
      analogWrite(MOTOR1_PIN2, 0);
      stropit = 1;
      Serial.println("STOP");        
  }else stropit = 0;
 */
  EthernetClient client = server.available();
  if (client) {
    int i = 0;
    Serial.println("new client");
   
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // aici iei valoarea de la action intr-o variabila
        if (isdigit(c) && d == 1){
          int j = c - '0';
          var[i] = var[i] * 10 + j;
        }
        if (!(isdigit(c)) && d == 1){
          i++;
          d = 0;
        }
        if (b == 1 && isdigit(c)){
          var[i] = c - '0';
          a = 0;
          b = 0;
          d = 1;
          
        }
        if (b == 1 && !(isdigit(c))){
          i++;
          b = 0;
        }
        if (c == '?' || c == '&'){
          a = 1;
        }
        if(a == 1 && c == '='){
          b = 1;
        }
        if (c == '\n' && currentLineIsBlank) {
          //AICI PUI CE Raspunde         
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println("Connection: close"); 
          client.println();
          for(i = 0; i < 6; i++){
            Serial.println(var[i]);
          }
          Serial.println();
          durata = var[1];
          lMin = var[2];
          tempMax = var[3];
          umaMin = var[4];
          umsMin = var[5];
          Serial.println(lMin);
          Serial.println(tempMax);
          Serial.println(umaMin);
          Serial.println(umsMin);
          switch(var[0]){
            case 0:
                analogWrite(MOTOR1_PIN1, 255);
                analogWrite(MOTOR1_PIN2, 100);
                delay(durata);
                analogWrite(MOTOR1_PIN1, 0);
                analogWrite(MOTOR1_PIN2, 0);
                
                client.println("done");
            	break;
            case 1:
                                   
                myservo.write(10);
                myservo.write(90);
                delay(durata);
                myservo.write(10);
		            client.println("done");
                break;
            case 2:
		            client.println("done");
                break;
            case 3:
                part = analogRead(A2);
                total = 1024;
                percentage = (float)part/total * 100.0;
                client.print(percentage);
                client.println('%');
            break;
            case 4:
                client.print(readTempInCelsius(10, 3));
                client.println("°C");
            break;
            /*
            case 5:
                  part = analogRead(A4) - 300;
                  total = 600;
                  while(part > 600){
                    part--;
                  }
                  while(part < 0){
                    part++;
                  }
                  percentage = (float)part/total * 100.0;
                  client.println(percentage);
                  client.print('%');
            break;
            */
            case 6:
                  part = analogRead(A1);
                  total = 880;
                  while(part > 880){
                    part--;
                  }
                  while(part < 0){
                    part++;
                  }
                  percentage = (float)part/total * 100.0;
                  client.print(percentage);
                  client.println('%');
            break;
          }
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disonnected");
  }
}

