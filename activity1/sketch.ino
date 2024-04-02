String getAction(String lecture, int &pin, int &value) {
  
    String action;
    int separator = lecture.indexOf(':');

    if (separator != -1) {
        action = lecture.substring(0, separator);
        lecture = lecture.substring(separator+1);
        separator = lecture.indexOf(',');
        if (separator != -1) {
            pin = lecture.substring(0, separator).toInt();
            value = lecture.substring(separator+1).toInt();
        }
        else {
            action = "Pin and value unrecognized";
        }
    }
    else {
        action = "Action unrecognized";
    }

    return action;
}

String lecture;
String msg;
String action;
int pin;
int value;
bool return_msg;

void setup() {
    Serial.begin(9600);
    Serial.setTimeout(1);

    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
}

void loop() {

    // analog read del sensor y entregar
    // causa conflicto print y available?
    // determinar un protocolo
    Serial.print();

    if (Serial.available() > 0) {
        lecture = Serial.readStringUntil('\n');
        msg;
        pin = -1;
        value = -1;

        action = getAction(lecture, pin, value);

        if (action == "AW") { // Analog Write
            if (pin >= 9 && pin <= 11 && value >= 0 && value <= 255) {
                analogWrite(pin, value);
                msg = "Updated action: " + action + ". Pin,Value: " + String(pin) + "," + String(value);
                return_msg = true;
            }
        }
        else if (action == "SW") {
            if (pin == 7 && (value == 0 || value == 1)) {
                digitalWrite(pin, value);
                return_msg = true;
            }
        }

        if (return_msg) {
            Serial.print("OK");
            return_msg = false;
        }
    }
}
