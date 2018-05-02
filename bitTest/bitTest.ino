int outPins[] = {0, 1, 2, 3, 4, 5, 6, 7};
int readPins[] = {30, 32, 34, 36, 38, 40, 42, 44};

void setup(){
    Serial.begin(9600);
    for(int pin=0; pin < 8; pin++){
        pinMode(outPins[pin], OUTPUT);
    }
}

void loop(){
    static unsigned int n = 0;
    delay(200);
    if (n<1023) n++;
    else n=0;
    Serial.println(n);
    write(n);
}

void write(unsigned int n){
    int bin;
    for (int i=0; i< 8; i++){
        bin = (n / pow(2, i));
        bin = (int) bin;
        bin = bin % 2;
        digitalWrite(outPins[i], bin);
        Serial.print(bin);
    }
    Serial.print('\n');
}