String getAction(String lecture_, int &pin_, int &value_) {
    String action_;
    int separator_ = lecture_.indexOf(':');

    if (separator_ != -1) {
        action_ = lecture_.substring(0, separator_);
        lecture_ = lecture_.substring(separator_+1);
        separator_ = lecture_.indexOf(',');
        if (separator_ != -1) {
            pin_ = lecture_.substring(0, separator_).toInt();
            value_ = lecture_.substring(separator_+1).toInt();
        }
        else {
            action_ = "Pin and value unrecognized";
        }
    }
    else {
        action_ = "Action unrecognized";
    }

    return action_;
}

String lecture;
String msg;
String action;
int pin;
int value;
int sensor_value;
bool return_msg;

void setup() {
    Serial.begin(9600);
    Serial.setTimeout(50);

    pinMode(7, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
}

void loop() {

    // analog read del sensor y entregar
    // causa conflicto print y available?
    // determinar un protocolo
    //Serial.print();
    sensor_value = analogRead(A3);
    Serial.print("INFO:" + String(sensor_value));

    if (Serial.available() > 0) {
        lecture = Serial.readStringUntil('\n');
        pin = -1;
        value = -1;

        action = getAction(lecture, pin, value);
        if (action == "AW") { // Analog Write
            if (pin >= 9 && pin <= 11 && value >= 0 && value <= 255) {
                analogWrite(pin, value);
                msg = "UPD:Updated action: " + action + ". Pin,Value: " + String(pin) + "," + String(value);
                return_msg = true;
            }
        }
        else if (action == "SW") {
            if (pin == 7 && (value == 0 || value == 1)) {
                digitalWrite(pin, value);
                msg = "UPD:Updated action: " + action + ". Pin,Value: " + String(pin) + "," + String(value);
                return_msg = true;
            }
        }

        if (return_msg) {
            Serial.print(msg);
            return_msg = false;
        }
        Serial.flush();
    }
}
