#ifdef debug
//______________________________________________________________________________________
void debug_print(){
  if (debugtime < currenttime && debugsend == false) {
    debugtime = currenttime + debugtimeout;
    debugsend == true;
  }


  if (debugsend == true) {
    Serial.println(" ");

    Serial.print("Status          : ");
    Serial.println(ST);

    Serial.print("SAT Fix         : ");
    Serial.println(fix_type);

    Serial.print("arm             : ");
    Serial.println(arm);

    Serial.print("FS              : ");
    Serial.println(failsafe);

    Serial.print("base Mode       : ");
    Serial.println(base_mode);

    Serial.print("System Status   : ");
    Serial.println(system_status);
    
    Serial.println(" ");
  }


}
#endif
