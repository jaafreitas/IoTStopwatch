#include "settings.h"
#include "debug.h"
#include "ntp.h"
#include "control.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

String fileName;
File UploadFile;

String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

void handleFileList() {
  String message = "<html><body>\n";

  FSInfo fs_info;
  SPIFFS.info(fs_info);
  float fileTotalKB = (float)fs_info.totalBytes / 1024.0; 
  float fileUsedKB = (float)fs_info.usedBytes / 1024.0; 
  message += "<p>File system info\n";
  message += "<br>Used: " + String(fileUsedKB) + "KB / ";
  message += String(100.0 * fileUsedKB / fileTotalKB) + "%\n";
  message += "<br>Free: " + String(fileTotalKB - fileUsedKB) + "KB\n";
  message += "<br>Total: " + String(fileTotalKB) + "KB\n<br>\n";

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    message += "<br><a href=\"" + fileName + "\">" + fileName + "</a> (" + formatBytes(fileSize) + ")\n";
  }
  message += "<p>Upload a file:";
  message += "<form method='POST' action='/upload' enctype='multipart/form-data'>";
  message += "<input type='file' name='upload'>";
  message += "<input type='submit' value='Upload'>";
  message += "</form>";
  
  message += "</body></html>";
  httpServer.send(200, "text/html", message);
}

void handleFile(String path) {
  bool sucess = false;
  if (SPIFFS.exists(path)) {
    if (httpServer.method() == HTTP_DELETE) {
      debugMsg("Resource %s removed.\n", path.c_str());
      SPIFFS.remove(path);
      httpServer.send(200, "text/plain", "Resource removed");
      sucess = true;
    } else if (httpServer.method() == HTTP_PUT) {
      // Not implemented... We are using onFileUpload as a workaround.
    } else if (httpServer.method() == HTTP_GET) {
      debugMsg("Streaming %s...", path.c_str());
      File file = SPIFFS.open(path, "r");
      httpServer.streamFile(file, "text/csv");
      file.close();
      debugMsg(" Ok.\n");
      sucess = true;
    } else {
      sucess = true;
    }
  }
  if (!sucess) {
    httpServer.send(404, "text/plain", "File Not Found");    
  }
}

void handleRoot() {
  String path = currentCSVFile();
  handleFile(String(path));
}

void handleISOTime() {
  httpServer.send(200, "text/plain", getISOTime());
}

void setupSPIFFS() {
  debugMsg("Starting SPIFFS... ");
  if (SPIFFS.begin()) {
    debugMsg(" Ok.\n");
  } else {
    debugMsg(" Error!.\n");
  }  
}

void setupWifi() {
  debugMsg("Starting WiFi Setup...\n");
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.disconnect();

  debugMsg("Connecting %s to SSID %s", HOSTNAME, WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    debugMsg(".");
  }
  debugMsg(" Ok.\n");
  
  debugMsg("IP address: %s\n", WiFi.localIP().toString().c_str());  
}

void setupMDNS() {
  debugMsg("Starting mDNS... ");
  if (MDNS.begin(HOSTNAME)) {
    debugMsg(" Ok.\n");
  } else {
    debugMsg(" Error!.\n");
  }  
}

void setupOTA() {
  debugMsg("Starting Over the Air updater at http://%s%s ... ", WiFi.localIP().toString().c_str(), OTA_UPDATEPATH);
  httpUpdater.setup(&httpServer, OTA_UPDATEPATH);
  debugMsg(" Ok.\n");
}

void setupServer() {
  debugMsg("Starting HTTP server...");
  
  httpServer.on("/", /*HTTP_GET, */handleRoot);
  httpServer.on("/list", HTTP_GET, handleFileList);
  httpServer.on("/datetime", HTTP_GET, handleISOTime);

  httpServer.onNotFound([]() {
    handleFile(httpServer.uri());
  });

  httpServer.onFileUpload([]() {
    if (httpServer.uri() != "/upload") return;
    HTTPUpload& upload = httpServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      fileName = upload.filename;
      debugMsg("Uploading %s...", fileName.c_str());
      String path = "/" + fileName;
      UploadFile = SPIFFS.open(path, "w");
      // already existing file will be overwritten!
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (UploadFile) {
        debugMsg(".");
        UploadFile.write(upload.buf, upload.currentSize);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      debugMsg(" Ok.\n");
      if (UploadFile) {
        UploadFile.close();
      }
    }
    yield();
  });
  
  httpServer.on("/upload", HTTP_POST, []() {
    httpServer.sendHeader("Location", "/list");
    httpServer.send(303);
  });     
  
  httpServer.begin();
  debugMsg(" Ok.\n");
}

void loopServer() {
  httpServer.handleClient();
  MDNS.update();
}
