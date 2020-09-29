xcopy /s C:\Users\Tamojit\Documents\Arduino\mqtt_esp8266_ota D:\git\esp8266_secure_ota_aws_mqtt

ren mqtt_esp8266_ota.ino esp8266_secure_ota_aws_mqtt.ino
del mqtt_esp8266_ota.ino
ren *.bin occu_esp8266.bin
del mqtt_esp8266_ota.ino.generic.bin
mv .\occu_esp8266.bin .\fota