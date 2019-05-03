# IoTStopwatch

## API documentation

### Read today's data
curl -X GET http://IoTStopwatch.local/

### ISO 8601 date/time
curl -X GET http://IoTStopwatch.local/datetime

### Upload file:
curl -X POST -F "upload=@20190501.csv" http://IoTStopwatch.local/upload

### List files:
curl -X GET http://IoTStopwatch.local/list

### Read file:
curl -X GET http://IoTStopwatch.local/20190501.csv

### Delete file:
curl -X DELETE http://IoTStopwatch.local/20190501.csv

### Delete today's data:
curl -X DELETE http://IoTStopwatch.local/

### Firmware update
curl -F "image=@IoTStopwatch.ino.bin" http://IoTStopwatch.local/firmware
