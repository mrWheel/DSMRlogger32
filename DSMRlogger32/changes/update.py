// ****************************************************************
#include "FSmanager.h"
// Sketch Esp8266 Filesystem Manager spezifisch sortiert Modular(Tab)
// created: Jens Fleischer, 2020-06-08
// last mod: Jens Fleischer, 2020-09-02
// For more information visit: https://fipsok.de/Esp8266-Webserver/littlefs-esp8266-270.tab
// ****************************************************************
// Hardware: Esp8266, ESP32
// Software: Esp8266 Arduino Core 2.7.0 - 2.7.4
// Software: ESP32 Arduino Core 2.0.5
// Geprüft: von 1MB bis 2MB Flash
// Getestet auf: Nodemcu
/******************************************************************
  Copyright (c) 2020 Jens Fleischer. All rights reserved.

  This file is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This file is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*******************************************************************/
// Diese Version von LittleFS sollte als Tab eingebunden werden.
// #include <LittleFS.h> #include <WebServer.h> müssen im Haupttab aufgerufen werden
// Die Funktionalität des ESP8266 Webservers ist erforderlich.
// "httpServer.onNotFound()" darf nicht im Setup des ESP8266 Webserver stehen.
// Die Funktion "setupFS();" muss im Setup aufgerufen werden.
/**************************************************************************************/

char cBuff[100] = {};

struct _catStruct
{
  char fDir[35];
  char fName[35];
  int fSize;
} catStruct;

// ... (rest of the constant definitions remain the same)

void setupFSmanager()
{
  httpServer.serveStatic("/FSmanager", _FSYS, "/FSmanager.html");
  httpServer.on("/format", formatFS);
  httpServer.on("/listFS", listFS);
  httpServer.on("/ReBoot", reBootESP);
  httpServer.on("/local_update", HTTP_POST, sendResponce, handleLocalUpdate);  // Changed from "/upload" to "/local_update"
  httpServer.on("/remote_update", handleRemoteUpdate);  // New route for remote update

  httpUpdater.setup(&httpServer);

  httpServer.onNotFound([]()
  {
    if (Verbose1) DebugTf("in 'onNotFound()'!! [%s] => \r\n", String(httpServer.uri()).c_str());

    if (httpServer.uri().indexOf("/api/") == 0) 
    {
      if (Verbose1) DebugTf("next: processAPI(%s)\r\n", String(httpServer.uri()).c_str());
      processAPI();
    }
    else if (httpServer.uri().indexOf("/format") == 0) 
    {
      formatFS();
    }
    else
    {
      DebugTf("next: handleFile(%s)\r\n"
                      , String(httpServer.urlDecode(httpServer.uri())).c_str());

      if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      {
        httpServer.send(404, "text/plain", "FileNotFound");
      }
    }
  });
}

// Changed function name from handleUpload to handleLocalUpdate
void handleLocalUpdate()
{
  // Dateien ins Filesystem schreiben
  static File fsUploadFile;

  HTTPUpload &upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    if (upload.filename.length() > 31)
    {
      // Dateinamen kürzen
      upload.filename = upload.filename.substring(upload.filename.length() - 31, upload.filename.length());
    }
    //-- Poke WatchDog
    pulseHeart(true);

    if (httpServer.arg(0) == "/") //-- root!
    {
      fsUploadFile = _FSYS.open("/" + httpServer.urlDecode(upload.filename), "w");
      DebugTf("FileUpload Name: %s\r\n",  upload.filename.c_str());
      writeToSysLog("FileUpload: [%s]",  upload.filename.c_str());
    }
    else
    {
      fsUploadFile = _FSYS.open("/" + httpServer.arg(0) + "/" + httpServer.urlDecode(upload.filename), "w");
      DebugTf("FileUpload Name: /%s/%s\r\n", httpServer.arg(0), upload.filename.c_str());
      writeToSysLog("FileUpload Name: /%s/%s", httpServer.arg(0), upload.filename.c_str());
    }
    if (!fsUploadFile)
    {
      DebugTf("Failed to open [%s] in [%s]\r\n", upload.filename, httpServer.arg(0));
      writeToSysLog("Failed to open [%s] in [%s]", upload.filename, httpServer.arg(0));
      //-- poke WatchDog
      pulseHeart(true);
      return;
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    DebugTf("FileUpload Data: %u\r\n", upload.currentSize);
    fsUploadFile.write(upload.buf, upload.currentSize);
    //-- poke WatchDog
    pulseHeart(true);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    DebugTf("FileUpload Size: %u\r\n", upload.totalSize);
    fsUploadFile.close();
    //-- poke WatchDog
    pulseHeart(true);
  }
}

// New function to handle remote update
void handleRemoteUpdate()
{
  // Implement the logic for remote update here
  // This is a placeholder and should be replaced with actual remote update logic
  httpServer.send(200, "text/plain", "Remote update functionality not implemented yet");
}

// ... (rest of the functions remain the same)

// The following functions are unchanged:
// handleList()
// deleteRecursive()
// handleFile()
// formatFS()
// listFS()
// sendResponce()
// formatBytes()
// reBootESP()
// doRedirect()
// getContentType()
// sortFunction()

/*eof*/