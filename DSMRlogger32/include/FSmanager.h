#ifndef FSMANAGER_H
#define FSMANAGER_H

//============ Includes ====================
#include "arduinoGlue.h"
#include <HTTPClient.h>
#include "UpdateManager.h"

//============ Prototypes ==================
bool handleList();                                          
void deleteRecursive(const char *path);                     
bool handleFile(String &&path);              
void handleFileUpload();               
//void handleUpload();        
void handleLocalUpdate();
void handleRemoteUpdate();
void RFUlistFirmware();
void formatFS();                                            
void listFS();                                              
void sendResponce();                                        
const String formatBytes(size_t const &bytes);              
void reBootESP();                                           
void doRedirect(String msg, int wait, const char *URL, bool reboot);
String getContentType(String filename);                     

//============ Added by Convertor ==========
#endif // FSMANAGER_H
