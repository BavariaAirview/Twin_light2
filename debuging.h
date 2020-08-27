#ifdef debug
//______________________________________________________________________________________
void debug_print() {

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

  Serial.print("Battery         : ");
  Serial.println(vbat);

  Serial.print("dist to home    : ");
  Serial.println(distanceToHome);

  Serial.print("GPS                 : ");
  Serial.print(gps_lon, 6);
  Serial.print(" ");
  Serial.println(gps_lat, 6);
  
  Serial.print("GPS  Home           : ");
  Serial.print(gps_home_lon, 6);
  Serial.print(" ");
  Serial.print(gps_home_lat, 6);

  Serial.println(" ");

}
#endif
