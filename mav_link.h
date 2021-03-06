//______________________________________________________________________________________
void mavl_receive()
{

  mavlink_message_t msg;
  mavlink_status_t status;

  while (mavlinkSerial.available() > 0 )
  {

    uint8_t c = mavlinkSerial.read();

    if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {

      switch (msg.msgid)
      {

        case MAVLINK_MSG_ID_SYS_STATUS:  // SYS_STATUS
          {
            mavlink_sys_status_t sys_status;
            mavlink_msg_sys_status_decode(&msg, &sys_status);

            vbat = (uint8_t)(sys_status.voltage_battery / 100);
          }
          break;

        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:  // MAVLINK_MSG_ID_GLOBAL_POSITION_INT
          {
            mavlink_global_position_int_t global_position_int;
            mavlink_msg_global_position_int_decode(&msg, &global_position_int);

            relative_alt = global_position_int.relative_alt;
            gps_lat = global_position_int.lat;
            gps_lon = global_position_int.lon;
            gps_alt = global_position_int.alt;

          }
          break;

        case MAVLINK_MSG_ID_GPS_RAW_INT:  // MAVLINK_MSG_ID_GPS_RAW_INT
          {
            mavlink_gps_raw_int_t gps_raw_int;
            mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int);

            fix_type = gps_raw_int.fix_type;
            numSat = gps_raw_int.satellites_visible;

          }
          break;

        case MAVLINK_MSG_ID_HEARTBEAT:  // HEARTBEAT
          {
            mavlink_heartbeat_t heartbeat;
            mavlink_msg_heartbeat_decode(&msg, &heartbeat);

            base_mode = heartbeat.base_mode;
            system_status = heartbeat.system_status;
          }
          break;

        default:
          break;
      }
    }

  }
}


//______________________________________________________________________________________
void set_flight_mode_flags()
{
  if (base_mode & MAV_MODE_FLAG_SAFETY_ARMED) {
    arm = true;
  }
  else {
    arm = false;
  }
  if (system_status == MAV_STATE_CRITICAL || system_status == MAV_STATE_EMERGENCY) {
    failsafe = true;
  }
  else {
    if (arm == false && arm_alt == true) {
      failsafe = true;
    } else {
      failsafe = false;
    }
  }
}
