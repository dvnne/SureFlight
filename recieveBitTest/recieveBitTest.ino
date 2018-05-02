int readPins[] = {30, 32, 34, 36, 38, 40, 42, 44};

void setup(){
    Serial.begin(9600);
    for (int i=0; i < 8; i++){
        pinMode(readPins[i], INPUT);
    }
}

void loop(){
    delay(40);
    read();
}

void read(){
    unsigned int n = 0;
    for (int i=7; i > -1; i--){
        int b = digitalRead(readPins[i]);
        Serial.print(b);
        if (b){
            float t = pow(2, i);
            t += 0.5;
            t = int(t);
            n += t;
        }
    }
    Serial.print(" I recieved ");
    Serial.println(n);
}