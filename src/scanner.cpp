#include "scanner.h"

uint8_t handleBarcodeScanner(char *barcode){

    char newLineChars[3];
	int numCharRead;

    
    if (Serial2.available()) {
        Serial.println("Serial2 available!");
		numCharRead = Serial2.readBytesUntil('\r', barcode, 100);
        Serial.println("numCharRead:");
        Serial.println(numCharRead);
		barcode[numCharRead] = 0;
		Serial2.readBytes(newLineChars, 3);

		/* after switching on scanner sends whitespace, so we need to exclude this by checking if first character is alfanumeric */
		if(isalnum(barcode[0])){

			
			Serial.print("Barcode: ");
			Serial.println(barcode);
            return numCharRead;
			
		}

	}
    return 0;
}