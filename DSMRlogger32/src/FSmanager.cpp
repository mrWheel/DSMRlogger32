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



//-aaw-const char WARNING[] PROGMEM = R"(<h2>Check! Sketch is compiled with "FS:none"!)";
const char WARNING[] = R"(
  <h2>Check! Sketch is compiled with "FS:none"!</h2>
  <br>Do you want to format the filesystem (YOU WILL LOOSE ALL DATA ON IT!)?
  <hr>
  <form method="POST" action="/format" enctype="multipart/form-data">
    <input type='submit' name='SUBMIT' value='Yes (and loose all data)'/>
  </form>
  <br>
  <form method="POST" action="/ReBoot" enctype="multipart/form-data">
    <input type='submit' name='SUBMIT' value='No (reboot)'/>
  </form>
  <hr>
)";

const char HELPER[] = R"(
  <br>You first need to upload these two files:
  <ul>
    <li>FSmanager.html</li>
    <li>FSmanager.css</li>
  </ul>
  <hr>
  <form method="POST" action="/upload" enctype="multipart/form-data">
    <input type="file" name="upload">
    <input type="submit" value="Upload">
  </form>
  <hr>
  <br/><b>or</b> you can use the <i>Flash Utility</i> to flash firmware or SPIFFS!
  <form action='/update' method='GET'>
    <input type='submit' name='SUBMIT' value='Flash Utility'/>
  </form>
)";

// Display the HTML form with JavaScript and dropdown
const char RFUindexHtml[] = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <link rel="stylesheet" type="text/css" href="DSMRindex.css">
            <title>DSMR-logger32  (Remote Update)</title>
            <script>
                async function fetchVersions(type) 
                {
                    try 
                    {
                        const response = await fetch(type === 'firmware' ? '/RFUlistFirmware' : '/RFUlistSpiffs');
                        if (!response.ok) 
                        {
                            throw new Error(`HTTP error! status: ${response.status}`);
                        }
                        const filenames = await response.json();

                        const select = document.getElementById(type === 'firmware' ? 'firmwareSelect' : 'spiffsSelect');
                        select.innerHTML = `<option value="">Select ${type}</option>`;
                        filenames.forEach(file => 
                        {
                            const option = document.createElement('option');
                            option.value = file;
                            option.textContent = file;
                            select.appendChild(option);
                        });
                    } 
                    catch (error) 
                    {
                        console.error(`Error fetching ${type} versions:`, error);
                        document.getElementById(type === 'firmware' ? 'firmwareSelect' : 'spiffsSelect').innerHTML = `<option value="">Error fetching ${type} versions: ${error.message}</option>`;
                    }
                }

                function submitForm(action) 
                {
                    const form = document.getElementById('updateForm');
                    const firmwareSelect = form.elements['firmwareVersion'];
                    const spiffsSelect = form.elements['spiffsVersion'];
                    if (action === 'updateFirmware' && (firmwareSelect.value === '' || firmwareSelect.value === 'none'))
                    {
                        alert('Please select a valid firmware version');
                        return;
                    }
                    if (action === 'updateSpiffs' && (spiffsSelect.value === '' || spiffsSelect.value === 'none')) 
                    {
                        alert('Please select a valid spiffs version');
                        return;
                    }
                    if (action === 'updateSpiffs') 
                    {
                      if (!confirm('Warning: Updating SPIFFS will erase all data on it.\n' +
                                   'Please first download:\n' +
                                   ' - RINGhours.csv\n' +
                                   ' - RINGdays.csv\n' +
                                   ' - RINGmonts.csv\n' +
                                   'and the .json files: DSMRdevSettings.json and DSMRsmSettings.json\n' +
                                   'Are you sure you want to proceed?')) 
                      {
                          return;
                      }
                    }
                    const actionInput = document.createElement('input');
                    actionInput.type = 'hidden';
                    actionInput.name = 'action';
                    actionInput.value = action;
                    form.appendChild(actionInput);
                    form.submit();
                }
            </script>
        </head>
        <body onload='fetchVersions("firmware"); fetchVersions("spiffs")'>
            <h1>DSMR-logger32  (Remote Update)</h1>
            <form id="updateForm" method="POST">
                Select Firmware: <select id="firmwareSelect" name="firmwareVersion"></select> &nbsp; &nbsp; &nbsp;
                    <button type="button" onclick='submitForm("updateFirmware")'>Update Firmware</button><br><br>
                Select Spiffs: <select id="spiffsSelect" name="spiffsVersion"></select> &nbsp; &nbsp; &nbsp;
                    <button type="button" onclick='submitForm("updateSpiffs")'>Update SPIFFS</button><br><br><br>
                <button type="button" onclick='submitForm("Return")'>Return</button>
            </form>
            <br><br>
        </body>
        </html>
            )";

const char Header[] = "HTTP/1.1 303 OK\r\nLocation:FSmanager.html\r\nCache-Control: no-cache\r\n";


//===========================================================================================
void setupFSmanager()
{
  httpServer.serveStatic("/FSmanager", _FSYS, "/FSmanager.html");
  httpServer.on("/format", formatFS);
  httpServer.on("/upload", HTTP_POST, []() {}, handleFileUpload);
  httpServer.on("/listFS", listFS);
  httpServer.on("/ReBoot", reBootESP);
  httpServer.on("/local_update", HTTP_POST, sendResponce, handleLocalUpdate);  // Changed from "/upload" to "/local_update"
  httpServer.on("/RFUupdate", handleRemoteUpdate);  //-- route for remote update
  //httpServer.on("/RFUlistFiles", RFUlistFiles);     //-- route for list Firmware
  httpServer.on("/RFUlistFirmware", []() { RFUlistFiles("firmware"); });
  httpServer.on("/RFUlistSpiffs",   []() { RFUlistFiles("spiffs"); });

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


//===========================================================================================
//-- Senden aller Daten an den Client
bool handleList()
{
  _catStruct catalog[_FSYS_MAX_FILES];
  char thisDir[35];
  int catPos = 0;

  memset(catalog, 0, _FSYS_MAX_FILES);

  File root = _FSYS.open("/");

  File dir  = root.openNextFile();
  while (dir && (catPos < (_FSYS_MAX_FILES-2)) )
  {
    yield();
    //-- Ordner und Dateien zur Liste hinzufügen
    if (dir.isDirectory())
    {
      DebugTf("Found Directory [%s]\r\n", dir.name());
      uint8_t ran {0};
      snprintf(thisDir, sizeof(thisDir), "/%s/", dir.name());
      File fold = _FSYS.open(thisDir);
      fold.openNextFile();
      while (fold && (catPos < (_FSYS_MAX_FILES-2)) )
      {
        yield();
        //DebugTf("[%s] Found file [%s]\r\n", thisDir, fold.name());
        ran++;
        //dirList.emplace_back(String(dir.name()), String(fold.name()), fold.size());
        snprintf(catalog[catPos].fDir,  sizeof(catalog[0].fDir), "%s", dir.name());
        snprintf(catalog[catPos].fName, sizeof(catalog[0].fName), "%s", fold.name());
        catalog[catPos].fSize = fold.size();
        catPos++;
        fold = dir.openNextFile();
      }
      if (catPos > (_FSYS_MAX_FILES-3) )
      {
        snprintf(catalog[catPos].fDir,  sizeof(catalog[0].fDir), "");
        snprintf(catalog[catPos].fName, sizeof(catalog[0].fName), "  TO-MANY-FILES-ON-FILESYSTEM ");
        catalog[catPos].fSize = 0;
        catPos++;
        snprintf(catalog[catPos].fDir,  sizeof(catalog[0].fDir), "/");
        snprintf(catalog[catPos].fName, sizeof(catalog[0].fName), "  NOT-ALL-FILES-ARE-SHOWN ");
        catalog[catPos].fSize = 0;
        DebugTf("To many files in the FileSystem (only %d shown)!\r\n", (catPos-2));
        catPos++;
      }
      if (!ran)
      {
        snprintf(catalog[catPos].fDir,  sizeof(catalog[0].fDir), "%s", dir.name());
        snprintf(catalog[catPos].fName, sizeof(catalog[0].fName), "");
        catalog[catPos].fSize = 0;
        catPos++;
      }
    }
    else
    {
      //DebugTf("Found file [%s]\r\n", dir.name());
      snprintf(catalog[catPos].fDir, sizeof(catalog[0].fDir), "");
      snprintf(catalog[catPos].fName, sizeof(catalog[0].fName), "%s", dir.name());
      catalog[catPos].fSize = dir.size();
      catPos++;
    }
    dir = root.openNextFile();
  }
  #ifdef _SPIFFS
    //-- SPIFFS hack. it works but why???
    catPos--;
  #endif
  qsort(catalog, catPos, sizeof(catalog[0]), sortFunction);

  String temp = "[";
  for (int i=0; i<catPos; i++)
  {
    if (temp != "[") temp += "\n,";
    temp += "{\"folder\":\"" + String(catalog[i].fDir)
            + "\",\"name\":\"" + String(catalog[i].fName)
            + "\",\"size\":\"" + formatBytes(catalog[i].fSize) + "\"}";
  }
  //-- totals ---
  temp += ",{\"usedBytes\":\"" + formatBytes(_FSYS.usedBytes()) +                      // Berechnet den verwendeten Speicherplatz
          "\",\"totalBytes\":\"" + formatBytes(_FSYS.totalBytes()) +                   // Zeigt die Größe des Speichers
          "\",\"freeBytes\":\"" + (_FSYS.totalBytes() - _FSYS.usedBytes()) + "\"}]";   // Berechnet den freien Speicherplatz

  httpServer.send(200, "application/json", temp);
  return true;

} //  handleList()


//===========================================================================================
void deleteRecursive(const char *path)
{
  char mName[33] = {};
  char fName[33] = {};

  DebugTf("path is [%s]\r\n", path);
  if (_FSYS.remove(path) )
  {
    DebugTf("Looks like [%s] was a file! Removed!\r\n", path);
    writeToSysLog("File [%s] removed by user!", path);
    return;
  }

  //-- is path a "folder"??
  if (String(path).lastIndexOf('/') == 0)  //-- yes! it's a folder
  {
    snprintf(mName, sizeof(mName), "%s", path);
    DebugTf("Remove folder [%s]\r\n", mName);
    writeToSysLog("Folder [%s] removed by user", mName);
    if (mName[0] != '/') snprintf(mName, sizeof(mName), "%s", path);
    File map = _FSYS.open(mName);
    File file = map.openNextFile();
    while(file)
    {
      snprintf(cBuff, sizeof(cBuff), "%s/%s", mName, file.name());
      file.close();
      deleteRecursive(cBuff);
      file  = map.openNextFile();
    }
    if (_FSYS.rmdir(mName))
          DebugTf("OK! [%s] removed\r\n", mName);
    else  DebugTf("ERROR! [%s] NOT removed\r\n", mName);
    return;
  }

  //-- it's a file!
  snprintf(mName, sizeof(mName), "%s", path);
  if (mName[0] != '/') snprintf(mName, sizeof(mName), "/%s", path);
  snprintf(fName, sizeof(fName), "%s", String(path).substring(String(path).lastIndexOf('/')+1));
  DebugTf("Remove file [%s][%s]\r\n", mName, fName);
  if (_FSYS.remove(mName) )
  {
    DebugTf("File [%s] removed\r\n", mName);
    writeToSysLog("File [%s] removed by user", mName);
  }
  else
  {
    DebugTf("ERROR! File [%s] is NOT  removed!\r\n", mName);
    writeToSysLog("ERROR trying to remove File [%s]", mName);
  }

} //  deleteRecursive()


//===========================================================================================
bool handleFile(String &&path)
{

  if (httpServer.hasArg("new"))
  {
    char folderName[50] = {};

    snprintf(folderName, sizeof(folderName), "/%s", httpServer.arg("new").c_str());
    DebugTf("New folderName [%s]\r\n", folderName);
    if (_FSYS.mkdir(folderName))
    {
      strlcat(folderName, "/dummy.txt", sizeof(folderName));
      DebugTf("Dummy file [%s] created\r\n", folderName);
      //-- folder won't stick without a file in it????
      File dummy = _FSYS.open(folderName, FILE_WRITE);
      if (dummy)
      {
        dummy.println("Dummy");
        dummy.close();
        DebugTf("Dummy file [%s] created\r\n", folderName);
      }
      else
      {
        DebugTf("ERROR: creating Dummy file [%s]\r\n", folderName);
      }
    }
    else
    {
      DebugTf("ERROR: mkdir(%s) failed!\r\n", folderName);
    }
  } //  create folder
  if (httpServer.hasArg("sort")) return handleList();
  if (httpServer.hasArg("delete"))
  {
    deleteRecursive(httpServer.arg("delete").c_str());
    sendResponce();
    return true;
  }
  if (!_FSYS.exists("/FSmanager.html"))
  {
    //-- ermöglicht das hochladen der FSmanager.html
    httpServer.send(200, "text/html", _FSYS.begin() ? HELPER : WARNING);
  }
  if (path.endsWith("/")) path += "/index.html";
  //-- Vorrübergehend für den Admin Tab
  //if (path == "/FSmanager.html") sendResponce();
  //return _FSYS.exists(path) ? ({File f = _FSYS.open(path, "r"); httpServer.streamFile(f, mime::getContentType(path)); f.close(); true;}) : false;
  return _FSYS.exists(path) ? ({File f = _FSYS.open(path, "r"); httpServer.streamFile(f, getContentType(path)); f.close(); true;}) : false;

} //  handleFile()


//=====================================================================================
void handleFileUpload()
{
  static File fsUploadFile;
  HTTPUpload &upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    //-- Datei geselecteerd
    if (upload.filename.length() < 2)
    {
      DebugTln("No file selected! Abort.");
      return;
    }
    //-- Dateinamen auf 30 Zeichen kürzen
    if (upload.filename.length() > 30)
    {
      upload.filename = upload.filename.substring(upload.filename.length() - 30, upload.filename.length());
    }
    DebugTln("FileUpload Name: " + upload.filename);
    fsUploadFile = SPIFFS.open("/" + httpServer.urlDecode(upload.filename), "w");
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    DebugTln("FileUpload Data: " + (String)upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
      fsUploadFile.close();
    DebugTln("FileUpload Size: " + (String)upload.totalSize);
    httpServer.sendContent(Header);
  }

} // handleFileUpload()


//===========================================================================================
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
} // handleLocalUpdate()


//===========================================================================================
// Custom reverse string search function
const char* reverse_strstr(const char* haystack, const char* needle, const char* haystack_end) 
{
  size_t needle_len = strlen(needle);
  const char* p;

  if (needle_len == 0) 
  {
    return haystack_end;
  }

  for (p = haystack_end - needle_len; p >= haystack; --p) 
  {
    if (strncmp(p, needle, needle_len) == 0) 
    {
      return p;
    }
  }

  return NULL;
} // reverse_strstr()


//===========================================================================================
void RFUlistFiles(const char* startWith) 
{
  uint8_t nrFiles = 0;

  DebugTln("RFUlistFiles() .. ");
  DebugTf("Looking for files starting with [%s] in [%s]\r\n", startWith, _REMOTE_UPDATESERVER);

  HTTPClient http;
  http.begin(_REMOTE_UPDATESERVER);
  int httpCode = http.GET();
  
  DynamicJsonDocument doc(1024);
  JsonArray fileArray = doc.to<JsonArray>();
  
  if (httpCode > 0) 
  {
    int contentLength = http.getSize();
    if (contentLength > 0) 
    {
      char* htmlContent = (char*)malloc(contentLength + 1);
      if (htmlContent) 
      {
        int bytesRead = http.getStream().readBytes(htmlContent, contentLength);
        htmlContent[bytesRead] = '\0'; // Null-terminate the string

        const char* pos = htmlContent;
        const char* htmlEnd = htmlContent + bytesRead;
        while ((pos = strstr(pos, ".bin")) != NULL) 
        {
          const char* hrefPos = reverse_strstr(htmlContent, "href=\"", pos);
          if (hrefPos != NULL && hrefPos < pos) 
          {
            const char* startPos = hrefPos + 6; // Move past 'href="'
            const char* endPos = strchr(startPos, '"');
            if (endPos != NULL && endPos > startPos) 
            {
              char fileName[256];
              size_t fileNameLen = endPos - startPos;
              if (fileNameLen < sizeof(fileName)) 
              {
                strncpy(fileName, startPos, fileNameLen);
                fileName[fileNameLen] = '\0';
                if (strstr(fileName, ".bin") == fileName + strlen(fileName) - 4 &&
                    strncmp(fileName, startWith, strlen(startWith)) == 0) 
                {
                  // Check for duplicates
                  bool isDuplicate = false;
                  for (JsonVariant v : fileArray) 
                  {
                    if (strcmp(v.as<const char*>(), fileName) == 0) 
                    {
                      isDuplicate = true;
                      break;
                    }
                  }
                  if (!isDuplicate) 
                  {
                    fileArray.add(fileName);
                    nrFiles++;
                  }
                }
              }
            }
          }
          pos += 4; // Move past ".bin"
        }
        free(htmlContent);
      }
      else 
      {
        DebugTln("Memory allocation failed");
        fileArray.add("Memory allocation failed");
      }
    }
    else 
    {
      DebugTln("Empty content received");
      fileArray.add("Empty content received");
    }
  } 
  else 
  {
    DebugTln("Error fetching files ...");
    fileArray.add("Error fetching files");
  }
  http.end();

  if (nrFiles == 0)
  {
    DebugTln("No matching files found ...");
    fileArray.add("No matching files found");
  }

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  DebugTln(jsonBuff);
  httpServer.send(200, "application/json", jsonBuff);

} // RFUlistFiles()


//===========================================================================================
void handleRemoteUpdate()
{
  bool SPIFFSfile = false;
  File file;
  char updateServerURI[100] = {};
  DebugTln("handleRemoteUpdate() ...");

  if (httpServer.method() == HTTP_GET) 
  {
      httpServer.send(200, "text/html", RFUindexHtml);
  } 
  else if (httpServer.method() == HTTP_POST) 
  {
    char action[16] = {};
    char newVersionNr[32] = {};

    for(int a=0; a<httpServer.args(); a++)
    {
      if (Verbose1) DebugTf("arg[%d]: [%s] = [%s]\r\n", a, httpServer.argName(a).c_str(), httpServer.arg(a).c_str());
    }

    strlcpy(action, httpServer.arg("action").c_str(), sizeof(action));
    DebugTf("action: [%s]\r\n", action);

    if (strncmp(action, "Return", 6) == 0) 
    {
      DebugTln("Return requested. No update performed.");
      doRedirect("Back to FSmanager", 2, "/FSmanager.html", false);
    }

    if (strncmp(action, "updateFirmware", 14) == 0) 
        strlcpy(newVersionNr, httpServer.arg("firmwareVersion").c_str(), sizeof(newVersionNr));
    else if (strncmp(action, "updateSpiffs", 12) == 0) 
        strlcpy(newVersionNr, httpServer.arg("spiffsVersion").c_str(), sizeof(newVersionNr));
    else
    {
      DebugTln("Invalid POST data received");
      doRedirect("Back to FSmanager", 2, "/FSmanager.html", false);
      return;
    }
    
    DebugTf("Update requested. New version: %s\r\n", newVersionNr);
    if (strncmp(newVersionNr, "No Updates Found", 16) == 0)
    {
      DebugTf("(%s) No Firmware Update!\r\n", __FUNCTION__);
      doRedirect("Wait for redirect ...", 5, "/FSmanager.html", false);
      return;
    }
    snprintf(updateServerURI, sizeof(updateServerURI), "%s/%s", _REMOTE_UPDATESERVER, newVersionNr);

    DebugTf("updateServerURI(): newVersionNr [%s]\r\n", newVersionNr);
    
    UpdateManager updateManager;

    DebugTf("(%s) Starting %s upload!\r\n", __FUNCTION__, action);

    if (strncmp(action, "updateFirmware", 14) == 0)
    {
      doRedirect("Wait for firmware update to complete ...", 120, "/", false);      
      //-- Shorthand
      updateManager.updateFirmware(updateServerURI, [](u_int8_t progress) 
      {
        if ((progress % 70) == 0) 
        {
          Debugln('.');
          pulseHeart();
        }
        else Debug('.');
      });
    }
    else if (strncmp(action, "updateSpiffs", 12) == 0)
    {
      doRedirect("Wait for spiffs update to complete ...", 80, "/", false);      
      //-- Shorthand
      updateManager.updateSpiffs(updateServerURI, [](u_int8_t progress) 
      {
        if ((progress % 70) == 0) 
        {
          Debugln('.');
          pulseHeart();
        }
        else Debug('.');
      });
    }
    else
    {
      DebugTf("(%s) Invalid update action!\r\n", __FUNCTION__);
      doRedirect("Back to FSmanager", 2, "/FSmanager.html", false);
      return;
    }

    if (updateManager.feedback(UPDATE_FEEDBACK_UPDATE_ERROR)) 
    { 
      DebugTf("\r\n(%s) Update ERROR\r\n", __FUNCTION__);
      httpServer.send(200, "text/html", "Update ERROR!");
      delay(1000);
      ESP.restart();
      delay(3000);
    }
    if (updateManager.feedback(UPDATE_FEEDBACK_UPDATE_OK)) 
    { 
      Debugf("\r\n(%s) Update OK\r\n", __FUNCTION__);
      httpServer.send(200, "text/html", "Update Succesfull!");
      delay(1000);
      ESP.restart();
      delay(3000);
    }
  }

  doRedirect("Back to FSmanager", 2, "/FSmanager.html", false);

} //  handleRemoteUpdate()

//===========================================================================================
void formatFS()      // Formatiert das Filesystem
{
  DebugTln("formatting Filesystem ..");
  _FSYS.format();
  sendResponce();

} //  formatFS()


//===========================================================================================
void listFS()
{
  DebugTln("send Filesystem data ..");
  sendResponce();

} //  listFS()


//===========================================================================================
void sendResponce()
{
  httpServer.sendHeader("Location", "/FSmanager.html");
  httpServer.send(303, "message/http");

} //  sendResponse()


//===========================================================================================
const String formatBytes(size_t const &bytes)
{
  // lesbare Anzeige der Speichergrößen
  return bytes < 1024 ? static_cast<String>(bytes) + " Byte" : bytes < 1048576 ? static_cast<String>(bytes / 1024.0) + " KB" : static_cast<String>(bytes / 1048576.0) + " MB";

} //  formatBytes()

//=====================================================================================
void reBootESP()
{
  DebugTln("Redirect and ReBoot ..");
  doRedirect("Reboot DSMRlogger32 ..", 50, "/", true);

} // reBootESP()

//=====================================================================================
void doRedirect(String msg, int wait, const char *URL, bool reboot)
{
  String redirectHTML =
    "<!DOCTYPE HTML><html lang='en-US'>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<style type='text/css'>"
    "body {background-color: lightblue;}"
    "</style>"
    "<title>Redirect to Main Program</title>"
    "</head>"
    "<body><h1>"+String(_DEFAULT_HOSTNAME)+"</h1>"
    "<h3>"+msg+"</h3>";
  if (String(URL).indexOf("/updateIndex") == 0)
  {
    redirectHTML += "<br>If this does not work just type \"<b>http://"+String(_DEFAULT_HOSTNAME)+".local/update\"";
    redirectHTML += "</b>as URL!<br>";
  }
  redirectHTML += "<br><div style='width: 500px; position: relative; font-size: 25px;'>"
                  "  <div style='float: left;'>Redirect over &nbsp;</div>"
                  "  <div style='float: left;' id='counter'>"+String(wait)+"</div>"
                  "  <div style='float: left;'>&nbsp; seconden ...</div>"
                  "  <div style='float: right;'>&nbsp;</div>"
                  "</div>"
                  "<!-- Note: don't tell people to `click` the link, just tell them that it is a link. -->"
                  "<br><br><hr>If you are not redirected automatically, click this <a href='/'>Main Program</a>."
                  "  <script>"
                  "      setInterval(function() {"
                  "          var div = document.querySelector('#counter');"
                  "          var count = div.textContent * 1 - 1;"
                  "          div.textContent = count;"
                  "          if (count <= 0) {"
                  "              window.location.replace('"+String(URL)+"'); "
                  "          } "
                  "      }, 1000); "
                  "  </script> "
                  "</body></html>\r\n";

  DebugTln(msg);
  httpServer.send(200, "text/html", redirectHTML);
  if (reboot)
  {
    writeToSysLog("Redirect to [%s]", URL);
    delay(5000);
    ESP.restart();
    delay(5000);
  }

} // doRedirect()


//===========================================================================================
String getContentType(String filename)
{
  if (httpServer.hasArg("download"))    return "application/octet-stream";
  else if (filename.endsWith(".htm"))   return "text/html";
  else if (filename.endsWith(".html"))  return "text/html";
  else if (filename.endsWith(".css"))   return "text/css";
  else if (filename.endsWith(".js"))    return "application/javascript";
  else if (filename.endsWith(".png"))   return "image/png";
  else if (filename.endsWith(".gif"))   return "image/gif";
  else if (filename.endsWith(".jpg"))   return "image/jpeg";
  else if (filename.endsWith(".ico"))   return "image/x-icon";
  else if (filename.endsWith(".xml"))   return "text/xml";
  else if (filename.endsWith(".pdf"))   return "application/x-pdf";
  else if (filename.endsWith(".zip"))   return "application/x-zip";
  else if (filename.endsWith(".gz"))    return "application/x-gzip";
  return "text/plain";

} // getContentType()


//---------------------------------------------------------
// qsort requires you to create a sort function
int sortFunction(const void *cmp1, const void *cmp2)
{
  struct _catStruct *ia = (struct _catStruct *)cmp1;
  struct _catStruct *ib = (struct _catStruct *)cmp2;
  #ifdef _SPIFFS
      return strcmp(ia->fName, ib->fName);
  #else
      return strcmp(ia->fDir, ib->fDir);
  #endif

} //  sortFunction()

/*eof*/