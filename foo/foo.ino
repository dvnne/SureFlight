
void setup(){
  Serial.begin(9600);
}

void loop(){
  if (Serial.available() > 0){
    char flag = Serial.peek();
    if (flag == 'm') int bytes = 2;
    int input = Serial.parseInt();
    Serial.print("Integer: ");
    Serial.println(input);

  }
}