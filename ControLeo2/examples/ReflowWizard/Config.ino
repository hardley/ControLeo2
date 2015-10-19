// Setup menu
// Called from the main loop
// Allows the user to configure the outputs and maximum temperature

// Called when in setup mode
// Return false to exit this mode
boolean Config() {
  static int setupPhase = 0;
  static int output = 4;    // Start with output D4
  static int type = TYPE_TOP_ELEMENT;
  static boolean drawMenu = true;
  static int maxTemperature;
  static int servoDegrees;
  static int servoDegreesIncrement = 5;
  static int selectedServo = SETTING_SERVO_OPEN_DEGREES;
  boolean continueWithSetup = true;
  boolean autoRepeat = false;
  
  switch (setupPhase) {
    case 0:  // Set up the output types
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Output x (Dx) is");
        lcd.setCursor(7, 0);
        lcd.print(output - 3);
        lcd.setCursor(11, 0);
        lcd.print(output);
        type = getSetting(SETTING_D4_TYPE - 4 + output);
        lcdPrintLine(1, outputDescription[type]);
      }
  
      // Was a button pressed?
      // No auto-repeat
      switch (getButton(false)) {
        case CONTROLEO_BUTTON_TOP:
          // Move to the next type
          type = (type+1) % NO_OF_TYPES;
          lcdPrintLine(1, outputDescription[type]);
          break;
        case CONTROLEO_BUTTON_BOTTOM:
          // Save the type for this output
          setSetting(SETTING_D4_TYPE - 4 + output, type);
          // Go to the next output
          output++;
          if (output != 8) {
            lcd.setCursor(7, 0);
            lcd.print(output - 3);
            lcd.setCursor(11, 0);
            lcd.print(output);
            type = getSetting(SETTING_D4_TYPE - 4 + output);
            lcdPrintLine(1, outputDescription[type]);
            break;
          }
          
          // Go to the next phase.  Reset variables used in this phase
          setupPhase++;
          drawMenu = true;
          output = 4;
          break;
      }
      break;
      
    case 1:  // Get the maximum temperature
      if (drawMenu) {
        drawMenu = false;
        lcdPrintLine(0, "Max temperature");
        lcdPrintLine(1, "xxx\1C");
        maxTemperature = getSetting(SETTING_MAX_TEMPERATURE);
        displayMaxTemperature(maxTemperature);
      }
      
      // Was a button pressed?
      // Auto-repeat on Top button
      switch (getButton(true)) {
        case CONTROLEO_BUTTON_TOP:
          // Increase the temperature
          maxTemperature++;
          if (maxTemperature > 280)
            maxTemperature = 175;
          displayMaxTemperature(maxTemperature);
          break;
        case CONTROLEO_BUTTON_BOTTOM:
          if (buttonReleased) { // inhibit auto-repeat on this button
            // Save the temperature
            setSetting(SETTING_MAX_TEMPERATURE, maxTemperature);
            // Go to the next phase.  Reset variables used in this phase
            setupPhase++;
            drawMenu = true; 
          }
      }
      break;
   
    case 2:  // Servo fitted and to be used?
      if (drawMenu) {
        drawMenu = false;
        useServo = getSetting(SETTING_USE_SERVO);
        lcdPrintLine(0, "Use door servo");
        lcdPrintLine(1, useServo == false? "NO" : "YES");
      }
      
      // Was a button pressed?
      // No auto-repeat
      switch (getButton(false)) {
        case CONTROLEO_BUTTON_TOP:
          useServo = !useServo;
          lcdPrintLine(1, useServo == false? "NO" : "YES");          
        break;
        case CONTROLEO_BUTTON_BOTTOM:
          // Save the use-servo setting
          setSetting(SETTING_USE_SERVO, useServo);
          // Go to the next phase.  Reset variables used in this phase
          setupPhase++;
          drawMenu = true;
          // Don't set the servo open and closed settings
          // if no servo is attached
          continueWithSetup = useServo;
        break;
      }
    
    case 3:  // Set the servo open and closed settings 
      if (useServo) {
        if (drawMenu) {     
          drawMenu = false;
          lcdPrintLine(0, "Door servo");
          lcdPrintLine(1, selectedServo == SETTING_SERVO_OPEN_DEGREES? "OPEN:" : "CLOSED:");
          servoDegrees = getSetting(selectedServo);
          if (selectedServo == SETTING_SERVO_OPEN_DEGREES) 
          {
            displayServoDegrees(servoDegrees, 6);            
          } else 
          {
            displayServoDegrees(servoDegrees, 8);              
          }
          // Move the servo to the saved position
          setServoPosition(servoDegrees, 1000);
        }
        
        // Was a button pressed?
	// Auto-repeat on Top button
        switch (getButton(true)) {
          case CONTROLEO_BUTTON_TOP:
            // Should the servo increment change direction?
            if (servoDegrees >= 180)
              servoDegreesIncrement = -5;
            if (servoDegrees == 0)
              servoDegreesIncrement = 5;
            // Change the servo angle
            servoDegrees += servoDegreesIncrement;
            // Move the servo to this new position
            setServoPosition(servoDegrees, 200);
          if (selectedServo == SETTING_SERVO_OPEN_DEGREES) 
          {
            displayServoDegrees(servoDegrees, 6);            
          } else 
          {
            displayServoDegrees(servoDegrees, 8);              
          }
            break;
          case CONTROLEO_BUTTON_BOTTOM:
            if (buttonReleased) { // inhibit auto-repeat on this button
              // Save the servo position
              setSetting(selectedServo, servoDegrees);
              // Go to the next phase.  Reset variables used in this phase
              drawMenu = true;
              if (selectedServo == SETTING_SERVO_OPEN_DEGREES)
                selectedServo = SETTING_SERVO_CLOSED_DEGREES;
              else {
                selectedServo = SETTING_SERVO_OPEN_DEGREES;
                // Done with setup
                continueWithSetup = false;
              }
            break;
          }
        } // Switch getButton
      } // if useServo
  }

  // Is setup complete?
  if (!continueWithSetup) {
    setupPhase = 0;
    lcdPrintLine(0, "     Setup");
    lcdPrintLine(1, "   Completed");
    delay(2000);     
  }
  
  return continueWithSetup;
}


void displayMaxTemperature(int maxTemperature) {
  lcd.setCursor(0, 1);
  lcd.print(maxTemperature);
}


void displayServoDegrees(int degrees, byte position) {
  lcd.setCursor(position, 1);
  lcd.print(degrees);
  lcd.print("\1 ");
}


