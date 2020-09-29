void checkForUpdates() {

  String fwURL = String( fwBaseUrl );
  fwURL.concat("occu_esp8266");
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );

  Serial.println(F("Checking for firmware updates." ));
  Serial.print(F("Firmware version URL: "));
  Serial.println( fwVersionURL );

  HTTPClient httpClient;
  httpClient.begin(otaClient, fwVersionURL );
  int httpCode = httpClient.GET();
  if ( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print(F("Current firmware version: "));
    Serial.println( FW_VERSION );
    Serial.print(F("Available firmware version: "));
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if ( newVersion > FW_VERSION ) {
      Serial.println(F("Preparing to update"));

      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update(otaClient, fwImageURL );

      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
          break;
        case HTTP_UPDATE_OK:
          Serial.println(F("HTTP_UPDATE_OK"));
          break;
      }
    }
    else {
      Serial.println(F("Already on latest version"));
    }
  }
  else {
    Serial.print(F("Firmware version check failed, got HTTP response code "));
    Serial.println( httpCode );
  }
  httpClient.end();
}
