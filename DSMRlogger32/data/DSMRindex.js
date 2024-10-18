/*
***************************************************************************  
**  Program  : DSMRindex.js, part of DSMRfirmware32
**  Version  : v5.n
**
**  Copyright (c) 2021, 2022, 2023, 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
const APIGW=window.location.protocol+'//'+window.location.host+'/api/';

"use strict";

let needBootsTrapMain     = true;
let needBootsTrapSettings = true;
let activePage            = "mainPage";
let activeTab             = "none";
let presentationType      = "TAB";
let tabTimer              = 0;
let actualTimer           = 0;
let actualHist            = false;
let timeTimer             = 0;
var GitHubVersion         = 0;
var GitHubVersion_dspl    = "-";
var firmwareVersion       = 0;
var firmwareVersion_dspl  = "-";
var newVersionMsg         = "";

var tlgrmInterval         = 10;
var ed_tariff1            = 0;
var ed_tariff2            = 0;
var er_tariff1            = 0;
var er_tariff2            = 0;
var gd_tariff             = 0;
var electr_netw_costs     = 0;
var gas_netw_costs        = 0;
var hostName              = "-";
var pre_dsmr40            = 0;
var dailyreboot           = 0;
var mbus_nr_gas           = 1;

var data       = [];
                
let monthType        = "ED";
let settingBgColor   = 'deepskyblue';
let settingFontColor = 'white'
                  
var monthNames = [ "indxNul","Januari","Februari","Maart","April","Mei","Juni"
                  ,"Juli","Augustus","September","Oktober","November","December"
                  ,"\0"
                 ];

window.onload=bootsTrapMain;
/*
window.onfocus = function() 
{
  if (needBootsTrapMain) 
  {
    window.location.reload(true);
  }
};
*/
  
//============================================================================  
function bootsTrapMain() 
{
  console.log("bootsTrapMain()");
  needBootsTrapMain = false;
  
  document.getElementById('bActualTab').addEventListener('click',function()
                                              {openTab('ActualTab');});
  document.getElementById('bHoursTab').addEventListener('click',function() 
                                              {openTab('HoursTab');});
  document.getElementById('bDaysTab').addEventListener('click',function() 
                                              {openTab('DaysTab');});
  document.getElementById('bMonthsTab').addEventListener('click',function() 
                                              {openTab('MonthsTab');});
  document.getElementById('bFieldsTab').addEventListener('click',function() 
                                              {openTab('FieldsTab');});
  document.getElementById('bTelegramTab').addEventListener('click',function() 
                                              {openTab('TelegramTab');});
  document.getElementById('bSyslogTab').addEventListener('click',function() 
                                              {openTab('SyslogTab');});
  document.getElementById('bSysInfoTab').addEventListener('click',function() 
                                              {openTab('SysInfoTab');});
  document.getElementById('bAPIdocTab').addEventListener('click',function() 
                                              {openTab('APIdocTab');});
  document.getElementById('FSmanager').addEventListener('click',function() 
                                              { console.log("newTab: goFSmanager");
                                                location.href = "/FSmanager.html";
                                              });
  document.getElementById('bPwrBars').addEventListener('click',function() 
                                              { console.log("Event: goDSMRpwrBars");
                                                location.href = "/DSMRpwrBars.html";
                                              });
  document.getElementById('Settings').addEventListener('click',function() 
                                              {openPage('settingsPage');});
  
  document.getElementById('mCOST').checked = false;
  setMonthTableType();
  refreshDevTime();
  getSmSettings();
  getDevSettings();
  refreshDevInfo();
  
  clearInterval(timeTimer);  
  timeTimer = setInterval(refreshDevTime, 10 * 1000); // repeat every 10s

  openPage("mainPage");
  actualHist = false;
  initActualGraph();
  openTab("ActualTab");
  setPresentationType('TAB');
  readGitHubVersion();
  
  console.log("..exit bootsTrapMain()!");
    
} // bootsTrapMain()

  
function bootsTrapSettings() 
{
  console.log("bootsTrapSettings()");
  needBootsTrapSettings = false;
  
  document.getElementById('bTerug').addEventListener('click',function()
                                              {openPage('mainPage');});
  document.getElementById('bEditMonths').addEventListener('click',function()
                                              {openTab('tabEditMonths');});
  document.getElementById('bEditDevSettings').addEventListener('click',function()
                                              {openTab('tabEditDevSettings');});
  document.getElementById('bEditSmSettings').addEventListener('click',function()
                                              {openTab('tabEditSmSettings');});
  document.getElementById('bEditShieldSettings').addEventListener('click',function()
                                              {openTab('tabEditShieldSettings');});
  document.getElementById('bUndo').addEventListener('click',function() 
                                              {undoReload();});
  document.getElementById('bSave').addEventListener('click',function() 
                                              {saveData();});
  refreshDevTime();
  refreshDevInfo();
  
  openPage("settingsPage");

  //openTab("tabEditSmSettings");
  
  //---- update buttons in navigation bar ---
  let x = document.getElementsByClassName("editButton");
  for (var i = 0; i < x.length; i++) 
  {
    x[i].style.background     = settingBgColor;
    x[i].style.border         = 'none';
    x[i].style.textDecoration = 'none';  
    x[i].style.outline        = 'none';  
    x[i].style.boxShadow      = 'none';
  }

} // bootsTrapSettings()


//============================================================================  
function openTab(tabName) 
{
  activeTab = tabName;
  actualHist = false;

  clearInterval(tabTimer);  
  clearInterval(actualTimer);  
  
  let bID = "b" + tabName;
  let i;
  console.log("openTab("+bID+")..");
  //---- update buttons in navigation bar ---
  let x = document.getElementsByClassName("tabButton");
  for (i = 0; i < x.length; i++) 
  {
    x[i].style.background     = 'deepskyblue';
    x[i].style.border         = 'none';
    x[i].style.textDecoration = 'none';  
    x[i].style.outline        = 'none';  
    x[i].style.boxShadow      = 'none';
  }
  //--- hide canvas -------
  document.getElementById("dataChart").style.display = "none";
  document.getElementById("gasChart").style.display  = "none";
  //--- hide all tab's -------
  x = document.getElementsByClassName("tabName");
  for (i = 0; i < x.length; i++) 
  {
    x[i].style.display    = "none";  
  }
  //--- and set active tab to 'block'
  console.log("now set ["+bID+"] to block ..");
  //document.getElementById(bID).style.background='lightgray';
  document.getElementById(tabName).style.display = "block";  
  if (tabName != "ActualTab") 
  {
    clearInterval(actualTimer);
    actualHist  = false;
    actualTimer = setInterval(refreshSmActual, 60 * 1000);                  // repeat every 60s
  }
  
  if (tabName == "ActualTab") 
  {
    console.log("newTab: ActualTab");
    refreshSmActual();
    clearInterval(actualTimer);
    if (tlgrmInterval < 10)
          actualTimer = setInterval(refreshSmActual, 10 * 1000);            // repeat every 10s
    else  actualTimer = setInterval(refreshSmActual, tlgrmInterval * 1000); // repeat every tlgrmInterval seconds

  } else if (tabName == "HoursTab") 
  {
    console.log("newTab: HoursTab");
    refreshHours();
    clearInterval(tabTimer);
    tabTimer = setInterval(refreshHours, 58 * 1000); // repeat every 58s

  } else if (tabName == "DaysTab") 
  {
    console.log("newTab: DaysTab");
    refreshDays();
    clearInterval(tabTimer);
    tabTimer = setInterval(refreshDays, 58 * 1000); // repeat every 58s

  } else if (tabName == "MonthsTab") 
  {
    console.log("newTab: MonthsTab");
    refreshMonths();
    clearInterval(tabTimer);
    tabTimer = setInterval(refreshMonths, 118 * 1000); // repeat every 118s
  
  } else if (tabName == "SysInfoTab") 
  {
    console.log("newTab: SysInfoTab");
    refreshDevInfo();
    clearInterval(tabTimer);
    tabTimer = setInterval(refreshDevInfo, 58 * 1000); // repeat every 58s

  } else if (tabName == "FieldsTab") 
  {
    console.log("newTab: FieldsTab");
    refreshSmFields();
    clearInterval(tabTimer);
    tabTimer = setInterval(refreshSmFields, 58 * 1000); // repeat every 58s

  } else if (tabName == "TelegramTab") 
  {
    console.log("newTab: TelegramTab");
    refreshSmTelegram();
    clearInterval(tabTimer); // do not repeat!

  } else if (tabName == "SyslogTab") 
  {
    console.log("newTab: SyslogTab");
    refreshDevSyslog();
    clearInterval(tabTimer); // do not repeat!

  } else if (tabName == "APIdocTab") 
  {
    console.log("newTab: APIdocTab");
    showAPIdoc();
    
  } else if (tabName == "tabEditMonths") 
  {
    console.log("newTab: tabEditMonths");
    document.getElementById('tabMaanden').style.display = 'block';
    getMonths();

  } else if (tabName == "tabEditSmSettings") 
  {
    console.log("newTab: tabEditSmSettings");
    document.getElementById('tabEditSmSettings').style.display = 'block';
    refreshSmSettings();

  } else if (tabName == "tabEditDevSettings") 
  {
    console.log("newTab: tabEditDevSettings");
    document.getElementById('tabEditDevSettings').style.display = 'block';
    refreshDevSettings();
  
  } else if (tabName == "tabEditShieldSettings") 
    {
      console.log("newTab: tabEditShieldSettings");
      document.getElementById('tabEditShieldSettings').style.display = 'block';
      refreshShieldSettings();
    
    }

} // openTab()


//============================================================================  
function openPage(pageName) 
{
  console.log("openPage("+pageName+")");
  activePage = pageName;
  if (pageName == "mainPage") 
  {
    document.getElementById("settingsPage").style.display = "none";
    data = {};
    needBootsTrapSettings = true;
    openTab("ActualTab");
    if (needBootsTrapMain)       bootsTrapMain();
  }
  else if (pageName == "settingsPage") 
  {
    document.getElementById("mainPage").style.display = "none";  
    data = {};
    needBootsTrapMain = true;
    openTab('tabEditDevSettings');
    if (needBootsTrapSettings)   bootsTrapSettings();
  }
  else if (pageName == "systemPage") 
  {
    document.getElementById("mainPage").style.display = "none";  
    data = {};
    needBootsTrapMain = true;
    openTab('tabEditDevSettings');
    if (needBootsTrapSystem)   bootsTrapSystem();
  }
  document.getElementById(pageName).style.display = "block";  

} // openPage()
  

//============================================================================  
function refreshDevInfo()
{
  fetch(APIGW+"v2/dev/info")
    .then(response => response.json())
    .then(json => {
      //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
      data = json.devinfo;
      let fldValue, fldUnit;
      
      for( let field in data )
      {
        //console.log("Processing ["+field+"]..");
          //console.log("["+field+"]->["+fldValue+"]->["+fldUnit+"]");
          fldValue = data[field];
          var tableRef = document.getElementById('devInfoTable').getElementsByTagName('tbody')[0];
          bName = translateToHuman(field);
          humanName = bName.replace("<2>", "<br>");
          
          //-- element bestaat nog niet, add it!
          if( !document.getElementById("devInfoTable_"+field) )
          {
            var newRow   = tableRef.insertRow();
            newRow.setAttribute("id", "devInfoTable_"+field, 0);
            // Insert a cell in the row at index 0
            var newCell  = newRow.insertCell(0);
            var newText  = document.createTextNode('');
            newCell.appendChild(newText);
            newCell  = newRow.insertCell(1);
            newCell.appendChild(newText);
            newCell  = newRow.insertCell(2);
            newCell.appendChild(newText);
          }
          tableCells = document.getElementById("devInfoTable_"+field).cells;
          tableCells[0].innerHTML = humanName;
          if (isFloat(fldValue))
          {
                tableCells[1].innerHTML = fldValue.toFixed(3);
                tableCells[1].style.textAlign = "right";
          }
          else if (!isNaN(fldValue))
          {
                tableCells[1].innerHTML = fldValue;
                tableCells[1].style.textAlign = "right";
          }
          else  tableCells[1].innerHTML = fldValue;

          if (field == "fwversion")
          {
            document.getElementById('devVersion').innerHTML = fldValue;
            var tmpFW = fldValue;
            firmwareVersion_dspl = tmpFW;
            tmpX = tmpFW.substring(1, tmpFW.indexOf(' '));
            tmpN = tmpX.split(".");
            firmwareVersion = tmpN[0]*10000 + tmpN[1]*1;
            console.log("firmwareVersion["+firmwareVersion+"] >= GitHubVersion["+GitHubVersion+"]");
            if (GitHubVersion == 0 || firmwareVersion >= GitHubVersion)
                  newVersionMsg = "";
            else  newVersionMsg = firmwareVersion_dspl + " nieuwere versie ("+GitHubVersion_dspl+") beschikbaar";
            document.getElementById('message').innerHTML = newVersionMsg;
            console.log(newVersionMsg);

          } else if (field == 'hostname')
          {
            document.getElementById('devName').innerHTML = fldValue;
          } else if (field == 'tlgrm_interval')
          {
            tlgrmInterval = value;
          } else if (field == "compileoptions" && fldValue.length > 50) 
          {
            tableCells[1].innerHTML = fldValue.substring(0,50);
            var lLine = value.substring(50);
            while (lLine.length > 50)
            {
              tableCells[1].innerHTML += "<br>" + lLine.substring(0,50);
              lLine = lLine.substring(50);
            }
            tableCells[1].innerHTML += "<br>" + lLine;
            tableCells[0].setAttribute("style", "vertical-align: top");
          }
       }  // for ..
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
} // refreshDevInfo()


//============================================================================  
function refreshDevTime()
{
  //console.log("Refresh api2dev/time ..");
  fetch(APIGW+"v2/dev/time")
    .then(response => response.json())
    .then(json => {
      //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
      data = json.devtime;
      for( let field in data )
      {
        //console.log("dev/time: field["+field+"], value["+data[field]+"]");
        if (field == "time")
        {
          //console.log("Got new time ["+data[field]+"]");
          document.getElementById('theTime').innerHTML = data[field];
        }
      }
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
    
  document.getElementById('message').innerHTML = newVersionMsg;

} // refreshDevTime()


//============================================================================  
function refreshSmActual()
{
  console.log("refreshSmActual: presentationType["+presentationType+"]");
  if (presentationType == "GRAPH" && (actualHist == false))
  {
    fetch(APIGW+"v2/hist/actual")
      .then(response => response.json())
      .then(json => {
          //console.log("parsing fields in ["+ JSON.stringify(json)+"]");
          store = json.store;
          for (var key in store) 
          {
            if (store.hasOwnProperty(key)) 
            {
              num = json.store[key];
              //console.log("["+key+"] parsed ==> ["+ JSON.stringify(num)+"]");
              for (var actual in num) 
              {
                //console.log("Found ["+JSON.stringify(num[actual].actual)+"]");
                data   = num[actual].actual;
                copyActualToChart(data);
              }
            }
          }
          actualHist = true;
          if (presentationType == "TAB")
                showActualTable(data);
          else  showActualGraph(data);
          //console.log("-->done..");
      })
      .catch(function(error) 
      {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });
  }
  else
  {
    fetch(APIGW+"v2/sm/actual")
      .then(response => response.json())
      .then(json => {
          //console.log("parsed .., fields is ["+ JSON.stringify(json)+"]");
          data = json.actual;
          copyActualToChart(data);
          if (presentationType == "TAB")
          {
            actualHist = false;
            showActualTable(data);
          }
          else  showActualGraph(data);
          //console.log("-->done..");
      })
      .catch(function(error) 
      {
        var p = document.createElement('p');
        p.appendChild(
          document.createTextNode('Error: ' + error.message)
        );
      });
  } 
};  // refreshSmActual()


//============================================================================  
function refreshSmFields()
{
  fetch(APIGW+"v2/sm/fields")
    .then(response => response.json())
    .then(json => {
        data = json.fields;
        let fldValue;
      
        for( let field in data )
        {
          bName = translateToHuman(field);
          humanName = bName.replace("<2>", "<br>");
          fldValue = data[field];
          console.log("bName["+field+"], fldValue["+fldValue+"]")
          var tableRef = document.getElementById('fieldsTable').getElementsByTagName('tbody')[0];
          if( ( document.getElementById("fieldsTable_"+field)) == null )
          {
            var newRow   = tableRef.insertRow();
            newRow.setAttribute("id", "fieldsTable_"+field, 0);
            var newCell  = newRow.insertCell(0);                  // name
            var newText  = document.createTextNode('');
            newCell.appendChild(newText);
            newCell  = newRow.insertCell(1);                      // humanName
            newCell.appendChild(newText);
            newCell  = newRow.insertCell(2);                      // value
            newCell.appendChild(newText);
            //newCell  = newRow.insertCell(3);                      // unit
            //newCell.appendChild(newText);
          }
          tableCells = document.getElementById("fieldsTable_"+field).cells;
          tableCells[0].innerHTML = field;
          tableCells[1].innerHTML = humanName;
          if (field == "electricity_failure_log" && fldValue.length > 50) 
          {
            tableCells[2].innerHTML = fldValue.substring(0,50);
            var lLine = fldValue.substring(50);
            while (lLine.length > 50)
            {
              tableCells[2].innerHTML += "<br>" + lLine.substring(0,50);
              lLine = lLine.substring(50);
            }
            tableCells[2].innerHTML += "<br>" + lLine;
            tableCells[0].setAttribute("style", "vertical-align: top");
            tableCells[1].setAttribute("style", "vertical-align: top");
          }
          else
          {
            if (field.includes("equipment"))
            {
              tableCells[2].innerHTML = fldValue;
            }
            //-- format floats and voltage_lx
            else if (isFloat(fldValue))
            {
              if (field.includes("voltage"))
              {
                tableCells[2].innerHTML = fldValue.toFixed(1);
                tableCells[2].style.textAlign = "right";
              }
              else  
              {
                tableCells[2].innerHTML = fldValue.toFixed(3);
                tableCells[2].style.textAlign = "right";
              }
            }
            //-- format integers
            else if (!isNaN(fldValue))
            {
                tableCells[2].innerHTML = fldValue;
                tableCells[2].style.textAlign = "right";
            }
            else  tableCells[2].innerHTML = fldValue;
          }
        }
        //console.log("-->done..");
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    }); 
};  // refreshSmFields()


//============================================================================  
function refreshHours()
{
  console.log("fetch("+APIGW+"v2/hist/hours/asc)");
  fetch(APIGW+"v2/hist/hours/asc", {"setTimeout": 2000})
    .then(response => response.json())
    .then(json => {
      //console.log("refreshHours: json is ["+ JSON.stringify(json)+"]");
      data = json.hours;
      //console.log("refreshHours: data is ["+ JSON.stringify(data)+"]");
      expandData(data);
      console.log("now, showHist::["+presentationType+"]");
      if (presentationType == "TAB")
            showHistTable(data, "Hours");
      else  showHistGraph(data, "Hours");
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    }); 
} // resfreshHours()


//============================================================================  
function refreshDays()
{
  console.log("fetch("+APIGW+"v2/hist/days/asc)");
  fetch(APIGW+"v2/hist/days/asc", {"setTimeout": 2000})
    .then(response => response.json())
    .then(json => {
      data = json.days;
      expandData(data);
      if (presentationType == "TAB")
            showHistTable(data, "Days");
      else  showHistGraph(data, "Days");
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });
} // resfreshDays()


//============================================================================  
function refreshMonths()
{
  console.log("fetch("+APIGW+"v2/hist/months/25/asc)");
  fetch(APIGW+"v2/hist/months/25/asc/", {"setTimeout": 2000})
    .then(response => response.json())
    .then(json => {
      //console.log(response);
      data = json.months;
      expandData(data);
      if (presentationType == "TAB")
      {
        if (document.getElementById('mCOST').checked)
              showMonthsCosts(data);
        else  showMonthsHist(data);
      }
      else  showMonthsGraph(data);
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });
} // resfreshMonths()

  
//============================================================================  
function refreshSmTelegram()
{
  fetch(APIGW+"v2/sm/telegram")
    .then(response => response.text())
    .then(response => {
      //console.log("parsed .., data is ["+ response+"]");
      //console.log('-------------------');
      var divT = document.getElementById('rawTelegram');
      if ( document.getElementById("TelData") == null )
      {
          console.log("CreateElement(pre)..");
          var preT = document.createElement('pre');
          preT.setAttribute("id", "TelData", 0);
          preT.setAttribute('class', 'telegram');
          preT.textContent = response;
          divT.appendChild(preT);
      }
      preT = document.getElementById("TelData");
      preT.textContent = response;
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
} // refreshSmTelegram()

  
//============================================================================  
function refreshDevSyslog()
{
  fetch(APIGW+"v2/dev/syslog")
    .then(response => response.text())
    .then(response => {
      //console.log("parsed .., data is ["+ response+"]");
      //console.log('-------------------');
      var divT = document.getElementById('rawSyslog');
      if ( document.getElementById("LogData") == null )
      {
          console.log("CreateElement(pre)..");
          var preT = document.createElement('pre');
          preT.setAttribute("id", "LogData", 0);
          preT.setAttribute('class', 'syslog');
          preT.textContent = response;
          divT.appendChild(preT);
      }
      preT = document.getElementById("LogData");
      preT.textContent = response;
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
} // refreshDevSyslog()

  
//============================================================================  
function expandData(data)
{
  //--- first check op volgordelijkheid ------    
  if (activeTab == "HoursTab") 
  {  
    for(let i=0; i<data.length-1; i++)
    {
      if (data[i].edt1 < data[i+1].edt1 || data[i].edt2 < data[i+1].edt2)
      {
        data[i].edt1 = data[i+1].edt1 * 1.0;
        data[i].edt2 = data[i+1].edt2 * 1.0;
        data[i].ert1 = data[i+1].ert1 * 1.0;
        data[i].ert2 = data[i+1].ert2 * 1.0;
        data[i].gdt  = data[i+1].gdt  * 1.0;
      }
    } // for ...
    console.log("done with for .. hours");
  } // hours
  
  console.log("calculate costs ..");
  for (let i=0; i<data.length; i++)
  {
    var     costs     = 0;
    data[i].p_ed      = {};
    data[i].p_edw     = {};
    data[i].p_er      = {};
    data[i].p_erw     = {};
    data[i].p_gd      = {};
    data[i].costs_e   = {};
    data[i].costs_g   = {};
    data[i].costs_nw  = {};
    data[i].costs_tt  = {};

    if (i < (data.length -1))
    {
      data[i].p_ed  = ((data[i].edt1 +data[i].edt2)-(data[i+1].edt1 +data[i+1].edt2)).toFixed(3);
      data[i].p_edw = (data[i].p_ed * 1000).toFixed(0);
      data[i].p_er  = ((data[i].ert1 +data[i].ert2)-(data[i+1].ert1 +data[i+1].ert2)).toFixed(3);
      data[i].p_erw = (data[i].p_er * 1000).toFixed(0);
      data[i].p_gd  = (data[i].gdt  -data[i+1].gdt).toFixed(3);
      //-- calculate Energy Delivered costs
      costs = ( (data[i].edt1 - data[i+1].edt1) * ed_tariff1 );
      costs = costs + ( (data[i].edt2 - data[i+1].edt2) * ed_tariff2 );
      //-- subtract Energy Returned costs
      costs = costs - ( (data[i].ert1 - data[i+1].ert1) * er_tariff1 );
      costs = costs - ( (data[i].ert2 - data[i+1].ert2) * er_tariff2 );
      data[i].costs_e = costs;
      //-- add Gas Delivered costs
      data[i].costs_g = ( (data[i].gdt  - data[i+1].gdt)  * gd_tariff );
      //-- compute network costs
      data[i].costs_nw = (electr_netw_costs + gas_netw_costs);
      //-- compute total costs
      data[i].costs_tt = ( (data[i].costs_e + data[i].costs_g + data[i].costs_nw) * 1.0);
    }
    else
    {
      costs             = 0;
      data[i].p_ed      = (data[i].edt1 +data[i].edt2).toFixed(3);
      data[i].p_edw     = (data[i].p_ed * 1000).toFixed(0);
      data[i].p_er      = (data[i].ert1 +data[i].ert2).toFixed(3);
      data[i].p_erw     = (data[i].p_er * 1000).toFixed(0);
      data[i].p_gd      = (data[i].gdt).toFixed(3);
      data[i].costs_e   = 0.0;
      data[i].costs_g   = 0.0;
      data[i].costs_nw  = 0.0;
      data[i].costs_tt  = 0.0;
    }
  } // for i ..
  console.log("Total Costs ["+data[0].costs_tt+"]");
  console.log("leaving expandData() ..");

} // expandData()

  
//============================================================================  
function showActualTable(data)
{ 
  if (activeTab != "ActualTab") return;

  console.log("showActual()");

  for (var field in data) 
  {
    fldValue = data[field];
    humanName = translateToHuman(field);
    var tableRef = document.getElementById('actualTable').getElementsByTagName('tbody')[0];
    if( ( document.getElementById("actualTable_"+field)) == null )
    {
      var newRow   = tableRef.insertRow();
      newRow.setAttribute("id", "actualTable_"+field, 0);
      // Insert a cell in the row at index 0
      var newCell  = newRow.insertCell(0);            // (short)name
      var newText  = document.createTextNode('');
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(1);                // value
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(2);                // unit
      newCell.appendChild(newText);
    }
    tableCells = document.getElementById("actualTable_"+field).cells;
    tableCells[0].innerHTML = humanName;
    if (isFloat(fldValue))
    {
      if (field.includes("voltage"))
            tableCells[1].innerHTML = fldValue.toFixed(1);
      else  tableCells[1].innerHTML = fldValue.toFixed(3);
      tableCells[1].style.textAlign = "right";
    }
    else if(!isNaN(fldValue))
    {
      tableCells[1].innerHTML = fldValue;
      tableCells[1].style.textAlign = "right";
    }
    else tableCells[1].innerHTML = fldValue;
  }

  //--- hide canvas
  document.getElementById("dataChart").style.display = "none";
  document.getElementById("gasChart").style.display  = "none";
  //--- show table
  document.getElementById("actual").style.display    = "block";

} // showActualTable()

  
//============================================================================  
function showHistTable(data, type)
{ 
  console.log("showHistTable("+type+")");
  // the last element has the metervalue, so skip it
  for (let i=0; i<(data.length -1); i++)
  {
    var tableRef = document.getElementById('last'+type+'Table').getElementsByTagName('tbody')[0];
    if( ( document.getElementById(type +"Table_"+type+"_R"+i)) == null )
    {
      var newRow   = tableRef.insertRow();
      newRow.setAttribute("id", type+"Table_"+type+"_R"+i, 0);
      // Insert a cell in the row at index 0
      var newCell  = newRow.insertCell(0);
      var newText  = document.createTextNode('-');
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(1);
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(2);
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(3);
      newCell.appendChild(newText);
      if (type == "Days")
      {
        newCell  = newRow.insertCell(4);
        newCell.appendChild(newText);
      }
    }

    tableCells = document.getElementById(type+"Table_"+type+"_R"+i).cells;
    tableCells[0].style.textAlign = "right";
    tableCells[0].innerHTML = formatDate(type, data[i].recid);
    tableCells[1].style.textAlign = "right";
    if (data[i].p_edw >= 0)
          tableCells[1].innerHTML = data[i].p_edw;
    else  tableCells[1].innerHTML = "-";
    tableCells[2].style.textAlign = "right";
    if (data[i].p_erw >= 0)
          tableCells[2].innerHTML = data[i].p_erw;
    else  tableCells[2].innerHTML = "-";
    tableCells[3].style.textAlign = "right";
    if (data[i].p_gd >= 0)
          tableCells[3].innerHTML = data[i].p_gd;
    else  tableCells[3].innerHTML = "-";
    if (type == "Days")
    {
      tableCells[4].style.textAlign = "right";
      tableCells[4].innerHTML = ( (data[i].costs_e + data[i].costs_g) * 1.0).toFixed(2);
    }
  };

  //--- hide canvas
  document.getElementById("dataChart").style.display = "none";
  document.getElementById("gasChart").style.display  = "none";
  //--- show table
  document.getElementById("lastHours").style.display = "block";
  document.getElementById("lastDays").style.display  = "block";

} // showHistTable()

  
//============================================================================  
function showMonthsHist(data)
{ 
  console.log("now in showMonthsHist() ..");
  var showRows    = 0;
  var received_0  = 0;
  var received_1  = 0;
  var delivered_0 = 0;
  var delivered_1 = 0;
  var gas_0       = 0;
  var gas_1       = 0;
  if (data.length > 24) showRows = 12;
  else                  showRows = data.length / 2;
  //console.log("showRows is ["+showRows+"]");
  for (let i=0; i<showRows; i++)
  {
    //console.log("showMonthsHist(): data["+i+"] => data["+i+"].name["+data[i].recid+"]");
    var tableRef = document.getElementById('lastMonthsTable').getElementsByTagName('tbody')[0];
    if( ( document.getElementById("lastMonthsTable_R"+i)) == null )
    {
      var newRow   = tableRef.insertRow();
      newRow.setAttribute("id", "lastMonthsTable_R"+i, 0);
      // Insert a cell in the row at index 0
      var newCell  = newRow.insertCell(0);          // maand
      var newText  = document.createTextNode('-');
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(1);              // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(2);              // verbruik
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(3);              // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(4);              // verbruik
      newCell.appendChild(newText);

      newCell  = newRow.insertCell(5);              // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(6);              // opgewekt
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(7);              // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(8);             // opgewekt
      newCell.appendChild(newText);
      
      newCell  = newRow.insertCell(9);             // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(10);             // gas
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(11);             // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(12);             // gas
      newCell.appendChild(newText);
    }
    var mmNr = parseInt(data[i].recid.substring(2,4), 10);

    tableCells = document.getElementById("lastMonthsTable_R"+i).cells;
    tableCells[0].style.textAlign = "right";
    tableCells[0].innerHTML = monthNames[mmNr];                           // maand
    
    tableCells[1].style.textAlign = "center";
    tableCells[1].innerHTML = "20"+data[i].recid.substring(0,2);          // jaar
    tableCells[2].style.textAlign = "right";
    if (data[i].p_ed >= 0)
          tableCells[2].innerHTML = data[i].p_ed;                         // verbruik
    else  tableCells[2].innerHTML = "-";     
    tableCells[3].style.textAlign = "center";
    tableCells[3].innerHTML = "20"+data[i+12].recid.substring(0,2);       // jaar
    tableCells[4].style.textAlign = "right";
    if (data[i+12].p_ed >= 0)
          tableCells[4].innerHTML = data[i+12].p_ed;                      // verbruik
    else  tableCells[4].innerHTML = "-";     

    tableCells[5].style.textAlign = "center";
    tableCells[5].innerHTML = "20"+data[i].recid.substring(0,2);          // jaar
    tableCells[6].style.textAlign = "right";
    if (data[i].p_er >= 0)
          tableCells[6].innerHTML = data[i].p_er;                         // opgewekt
    else  tableCells[6].innerHTML = "-";     
    tableCells[7].style.textAlign = "center";
    tableCells[7].innerHTML = "20"+data[i+12].recid.substring(0,2);       // jaar
    tableCells[8].style.textAlign = "right";
    if (data[i+12].p_er >= 0)
          tableCells[8].innerHTML = data[i+12].p_er;                      // opgewekt
    else  tableCells[8].innerHTML = "-";     

    tableCells[9].style.textAlign = "center";
    tableCells[9].innerHTML = "20"+data[i].recid.substring(0,2);          // jaar
    tableCells[10].style.textAlign = "right";
    if (data[i].p_gd >= 0)
          tableCells[10].innerHTML = data[i].p_gd;                        // gas
    else  tableCells[10].innerHTML = "-";     
    tableCells[11].style.textAlign = "center";
    tableCells[11].innerHTML = "20"+data[i+12].recid.substring(0,2);      // jaar
    tableCells[12].style.textAlign = "right";
    if (data[i+12].p_gd >= 0)
          tableCells[12].innerHTML = data[i+12].p_gd;                     // gas
    else  tableCells[12].innerHTML = "-";     

    delivered_0 += (data[i].p_ed *1);
    received_0  += (data[i].p_er *1);
    gas_0       += (data[i].p_gd *1);
    delivered_1 += (data[i+12].p_ed *1);
    received_1  += (data[i+12].p_er *1);
    gas_1       += (data[i+12].p_gd *1);
    
  };

  if( ( document.getElementById("12MonthsTotal")) == null )
  {
    var newRow   = tableRef.insertRow();          // add new row
    newRow.setAttribute("id", "12MonthsTotal", 0);
    // Insert a cell in the row at index 0
    var newCell  = newRow.insertCell(0);          // totaal
    var newText  = document.createTextNode('-');
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(1);              // Spare
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(2);              // Delivered +0
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(3);              // Spare
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(4);              // Delivered +12
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(5);              // Spare
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(6);              // Received +0
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(7);              // Spare
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(8);              // Recieved +12
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(9);              // Spare
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(10);             // Gas +0
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(11);             // Spare
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(12);             // Gas +12
    newCell.appendChild(newText);
  }

  console.log("del["+delivered_0+"] rec["+received_0+"]");
  tableCells = document.getElementById("12MonthsTotal").cells;
  tableCells[0].style.textAlign = "right";
  tableCells[0].style.fontWeight = 'bold';
  tableCells[0].innerHTML = "Totaal"
  //tableCells[1].innerHTML = ""
  tableCells[2].style.textAlign = "right";
  tableCells[2].style.fontWeight = 'bold';
  tableCells[2].innerHTML = delivered_0.toFixed(3)+"";
  //tableCells[3].innerHTML = "";
  tableCells[4].style.textAlign = "right";
  tableCells[4].style.fontStyle = 'italic';
  tableCells[4].innerHTML = delivered_1.toFixed(3);
  //tableCells[5].innerHTML = "";
  tableCells[6].style.textAlign = "right";
  tableCells[6].style.fontWeight = 'bold';
  tableCells[6].innerHTML = received_0.toFixed(3);
  //tableCells[7].innerHTML = ""
  tableCells[8].style.textAlign = "right";
  tableCells[8].style.fontStyle = 'italic';
  tableCells[8].innerHTML = received_1.toFixed(3);
  //tableCells[9].innerHTML = ""
  tableCells[10].style.textAlign = "right";
  tableCells[10].style.fontWeight = 'bold';
  tableCells[10].innerHTML = gas_0.toFixed(3);
  //tableCells[11].innerHTML = ""
  tableCells[12].style.textAlign = "right";
  tableCells[12].style.fontStyle = 'italic';
  tableCells[12].innerHTML = gas_1.toFixed(3);

  if( ( document.getElementById("12MonthsSaldo")) == null )
  {
    var newRow   = tableRef.insertRow();                                // voorschot regel
    newRow.setAttribute("id", "12MonthsSaldo", 0);
    // Insert a cell in the row at index 0
    var newCell  = newRow.insertCell(0);          // Saldo
    var newText  = document.createTextNode('-');
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(1);              // Spare
    newCell  = newRow.insertCell(2);              // Saldo +0
    newCell  = newRow.insertCell(3);              // Spare
    newCell  = newRow.insertCell(4);              // Saldo +12
  }

  tableCells = document.getElementById("12MonthsSaldo").cells;
  tableCells[0].style.textAlign = "right";
  tableCells[0].style.fontWeight = 'bold';
  tableCells[0].innerHTML = "Saldo"
  tableCells[1].innerHTML = ""
  tableCells[2].style.textAlign = "right";
  tableCells[2].style.fontWeight = 'bold';
  tableCells[2].innerHTML = (delivered_0 - received_0).toFixed(3);

  tableCells[3].innerHTML = "";
  tableCells[4].style.textAlign = "right";
  tableCells[4].style.fontStyle = 'italic';
  tableCells[4].innerHTML = (delivered_1 - received_1).toFixed(3);
  
  //--- hide canvas
  document.getElementById("dataChart").style.display  = "none";
  document.getElementById("gasChart").style.display   = "none";
  //--- show table
  document.getElementById("lastMonths").style.display = "block";

} // showMonthsHist()

  
//============================================================================  
function showMonthsCosts(data)
{ 
  console.log("now in showMonthsCosts() ..");
  var totalCost   = 0;
  var totalCost_1 = 0;
  var showRows    = 0;
  if (data.length > 24) showRows = 12;
  else                  showRows = data.length / 2;
  //console.log("showRows is ["+showRows+"]");
  for (let i=0; i<showRows; i++)
  {
    //console.log("showMonthsHist(): data["+i+"] => data["+i+"].name["+data[i].recid+"]");
    var tableRef = document.getElementById('lastMonthsTableCosts').getElementsByTagName('tbody')[0];
    if( ( document.getElementById("lastMonthsTableCosts_R"+i)) == null )
    {
      var newRow   = tableRef.insertRow();
      newRow.setAttribute("id", "lastMonthsTableCosts_R"+i, 0);
      // Insert a cell in the row at index 0
      var newCell  = newRow.insertCell(0);          // maand
      var newText  = document.createTextNode('-');
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(1);              // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(2);              // kosten electra
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(3);              // kosten gas
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(4);              // vast recht
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(5);              // kosten totaal
      newCell.appendChild(newText);

      newCell  = newRow.insertCell(6);              // jaar
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(7);              // kosten electra
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(8);              // kosten gas
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(9);              // vast recht
      newCell.appendChild(newText);
      newCell  = newRow.insertCell(10);              // kosten totaal
      newCell.appendChild(newText);
    }
    var mmNr = parseInt(data[i].recid.substring(2,4), 10);

    tableCells = document.getElementById("lastMonthsTableCosts_R"+i).cells;
    tableCells[0].style.textAlign = "right";
    tableCells[0].innerHTML = monthNames[mmNr];                           // maand
    
    tableCells[1].style.textAlign = "center";
    tableCells[1].innerHTML = "20"+data[i].recid.substring(0,2);          // jaar
    tableCells[2].style.textAlign = "right";
    tableCells[2].innerHTML = (data[i].costs_e *1).toFixed(2);            // kosten electra
    tableCells[3].style.textAlign = "right";
    tableCells[3].innerHTML = (data[i].costs_g *1).toFixed(2);            // kosten gas
    tableCells[4].style.textAlign = "right";
    tableCells[4].innerHTML = (data[i].costs_nw *1).toFixed(2);           // netw kosten
    tableCells[5].style.textAlign = "right";
    tableCells[5].style.fontWeight = 'bold';
    tableCells[5].innerHTML = "€ " + (data[i].costs_tt *1).toFixed(2);    // kosten totaal
    //--- omdat de actuele maand net begonnen kan zijn tellen we deze
    //--- niet mee, maar tellen we de laatste maand van de voorgaand periode
    if (i > 0)
          totalCost += data[i].costs_tt;
    else  totalCost += data[i+12].costs_tt;

    tableCells[6].style.textAlign = "center";
    tableCells[6].innerHTML = "20"+data[i+12].recid.substring(0,2);         // jaar
    tableCells[7].style.textAlign = "right";
    tableCells[7].innerHTML = (data[i+12].costs_e *1).toFixed(2);           // kosten electra
    tableCells[8].style.textAlign = "right";
    tableCells[8].innerHTML = (data[i+12].costs_g *1).toFixed(2);           // kosten gas
    tableCells[9].style.textAlign = "right";
    tableCells[9].innerHTML = (data[i+12].costs_nw *1).toFixed(2);          // netw kosten
    tableCells[10].style.textAlign = "right";
    tableCells[10].style.fontWeight = 'bold';
    tableCells[10].innerHTML = "€ " + (data[i+12].costs_tt *1).toFixed(2);  // kosten totaal
    totalCost_1 += data[i+12].costs_tt;

  };

  if( ( document.getElementById("periodicCosts")) == null )
  {
    var newRow   = tableRef.insertRow();                                // voorschot regel
    newRow.setAttribute("id", "periodicCosts", 0);
    // Insert a cell in the row at index 0
    var newCell  = newRow.insertCell(0);                                // maand
    var newText  = document.createTextNode('-');
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(1);              // description
    newCell.setAttribute("colSpan", "4");
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(2);              // voorschot
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(3);              // description
    newCell.setAttribute("colSpan", "4");
    newCell.appendChild(newText);
    newCell  = newRow.insertCell(4);              // voorschot
    newCell.appendChild(newText);
  }
  tableCells = document.getElementById("periodicCosts").cells;
  tableCells[1].style.textAlign = "right";
  tableCells[1].innerHTML = "Voorschot Bedrag"
  tableCells[2].style.textAlign = "right";
  tableCells[2].innerHTML = "€ " + (totalCost / 12).toFixed(2);
  tableCells[3].style.textAlign = "right";
  tableCells[3].innerHTML = "Voorschot Bedrag"
  tableCells[4].style.textAlign = "right";
  tableCells[4].innerHTML = "€ " + (totalCost_1 / 12).toFixed(2);

  
  //--- hide canvas
  document.getElementById("dataChart").style.display  = "none";
  document.getElementById("gasChart").style.display   = "none";
  //--- show table
  if (document.getElementById('mCOST').checked)
  {
    document.getElementById("lastMonthsTableCosts").style.display = "block";
    document.getElementById("lastMonthsTable").style.display = "none";
  }
  else
  {
    document.getElementById("lastMonthsTable").style.display = "block";
    document.getElementById("lastMonthsTableCosts").style.display = "none";
  }
  document.getElementById("lastMonths").style.display = "block";

} // showMonthsCosts()


//============================================================================  
function getSmSettings()
{
  fetch(APIGW+"v2/sm/settings")
    .then(response => response.json())
    .then(json => {
      //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
      for( let i in json.settings )
      {
          if (json.settings[i].name == "ed_tariff1")
          {
            ed_tariff1 = json.settings[i].value;
          }
          else if (json.settings[i].name == "ed_tariff2")
          {
            ed_tariff2 = json.settings[i].value;
          }
          else if (json.settings[i].name == "er_tariff1")
          {
            er_tariff1 = json.settings[i].value;
          }
          else if (json.settings[i].name == "er_tariff2")
          {
            er_tariff2 = json.settings[i].value;
          }
          else if (json.settings[i].name == "gd_tariff")
          {
            gd_tariff = json.settings[i].value;
          }
          else if (json.settings[i].name == "electr_netw_costs")
          {
            electr_netw_costs = json.settings[i].value;
          }
          else if (json.settings[i].name == "gas_netw_costs")
          {
            gas_netw_costs = json.settings[i].value;
          }
          else if (json.settings[i].name == "mbus_nr_gas")
          {
            mbus_nr_gas = json.settings[i].value;
          }
          else if (json.settings[i].name == "pre_dsmr40")
          {
            pre_dsmr40 = json.settings[i].value;
          }
        }
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
    
} // getSmSettings()


//============================================================================  
function getDevSettings()
{
  fetch(APIGW+"v2/dev/settings")
    .then(response => response.json())
    .then(json => {
      //console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
      for( let i in json.settings )
      {
        if (json.settings[i].name == "dailyreboot")
        {
          dailyreboot = json.settings[i].value;
        }
        else if (json.settings[i].name == "hostname")
        {
          hostName = json.settings[i].value;
        }
      }
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
    
} // getDevSettings()


//============================================================================  
function getDevSystem()
{
  fetch(APIGW+"v2/dev/system")
    .then(response => response.json())
    .then(json => {
      console.log("parsed .., data is ["+ JSON.stringify(json)+"]");
      for( let i in json.system )
      {
        if (json.system[i].name == "hostname")
        {
          hostName = json.system[i].value;
        }
        else if (json.settings[i].name == "dailyreboot")
        {
          dailyreboot = json.settings[i].value;
        }
      }
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     
} // getDevSystem()

  
//============================================================================  
function setPresentationType(pType) 
{
  if (pType == "GRAPH") {
    console.log("Set presentationType to GRAPHICS mode!");
    presentationType = pType;
    document.getElementById('aGRAPH').checked = true;
    document.getElementById('aTAB').checked   = false;
    initActualGraph();
    document.getElementById('hGRAPH').checked = true;
    document.getElementById('hTAB').checked   = false;
    document.getElementById('dGRAPH').checked = true;
    document.getElementById('dTAB').checked   = false;
    document.getElementById('mGRAPH').checked = true;
    document.getElementById('mTAB').checked   = false;
    document.getElementById('mCOST').checked  = false;
    document.getElementById("lastMonthsTable").style.display      = "block";
    document.getElementById("lastMonthsTableCosts").style.display = "none";

  } else if (pType == "TAB") {
    console.log("Set presentationType to Tabular mode!");
    presentationType = pType;
    document.getElementById('aTAB').checked   = true;
    document.getElementById('aGRAPH').checked = false;
    document.getElementById('hTAB').checked   = true;
    document.getElementById('hGRAPH').checked = false;
    document.getElementById('dTAB').checked   = true;
    document.getElementById('dGRAPH').checked = false;
    document.getElementById('mTAB').checked   = true;
    document.getElementById('mGRAPH').checked = false;
    document.getElementById('mCOST').checked  = false;

  } else {
    console.log("setPresentationType to ["+pType+"] is quite shitty! Set to TAB");
    presentationType = "TAB";
  }

  document.getElementById("APIdocTab").style.display = "none";

  if (activeTab == "ActualTab")  refreshSmActual();
  if (activeTab == "HoursTab")   refreshHours();
  if (activeTab == "DaysTab")    refreshDays();
  if (activeTab == "MonthsTab")  refreshMonths();

} // setPresenationType()

  
//============================================================================  
function setMonthTableType() 
{
  console.log("Set Month Table Type");
  if (presentationType == 'GRAPH') 
  {
    document.getElementById('mCOST').checked = false;
    return;
  }
  if (document.getElementById('mCOST').checked)
  {
    document.getElementById("lastMonthsTableCosts").style.display = "block";
    document.getElementById("lastMonthsTable").style.display      = "none";
  }
  else
  {
    document.getElementById("lastMonthsTable").style.display      = "block";
    document.getElementById("lastMonthsTableCosts").style.display = "none";
  }
  document.getElementById('lastMonthsTableCosts').getElementsByTagName('tbody').innerHTML = "";
  refreshMonths();
    
} // setMonthTableType()

  
//============================================================================  
function showAPIdoc() 
{
  console.log("Show API doc ..@["+location.host+"]");
  document.getElementById("APIdocTab").style.display = "block";
  addAPIdoc("v2/dev/info",      "Device info in JSON format", true);
  addAPIdoc("v2/dev/time",      "Device time (epoch) in JSON format", true);

  addAPIdoc("v2/dev/settings",  "System settings in JSON format", true);
  addAPIdoc("v2/dev/settings{jsonObj}", "[POST] update System Settings in JSON format\
      <br>test with:\
      <pre>curl -X POST -H \"Content-Type: application/json\" --data '{\"name\":\"mqtt_broker\",\"value\":\"hassio.local\"}' \
http://DSMR-ESP32.local/api/v2/dev/settings</pre>", false);
  
  addAPIdoc("v2/sm/info",       "Smart Meter info in JSON format", true);
  addAPIdoc("v2/sm/actual",     "Smart Meter Actual data in JSON format", true);
  addAPIdoc("v2/sm/fields",     "Smart Meter all fields data in JSON format", true);
  addAPIdoc("v2/sm/fields/{fieldName}", "Smart Meter one field data in JSON format", false);

  addAPIdoc("v2/sm/settings",  "Slimme Meter Settings in JSON format", true);
  addAPIdoc("v2/sme/settings{jsonObj}", "[POST] update Slimme Meter Settings in JSON format\
      <br>test with:\
      <pre>curl -X POST -H \"Content-Type: application/json\" --data '{\"name\":\"ed_tariff2\",\"value\":\"1.54\"}' \
http://DSMR-ESP32.local/api/v2/sm/settings</pre>", false);

  addAPIdoc("v2/sm/telegram",   "raw telegram as send by the Smart Meter including all \"\\r\\n\" line endings", false);

  addAPIdoc("v2/shield/info",      "Device info in JSON format", true);
  addAPIdoc("v2/shield/settings",  "System settings in JSON format", true);

  addAPIdoc("v2/hist/actual",   "History data last reads in JSON format", true);
  addAPIdoc("v2/hist/hours",    "History data per hour in JSON format", true);
  addAPIdoc("v2/hist/days",     "History data per day in JSON format", true);
  addAPIdoc("v2/hist/months",   "History data per month in JSON format", true);

} // showAPIdoc()

  
//============================================================================  
function addAPIdoc(restAPI, description, linkURL) 
{
  if (document.getElementById(restAPI) == null)
  {
    var topDiv = document.getElementById("APIdocTab");
    var br = document.createElement("BR"); 
    br.setAttribute("id", restAPI, 0);
    br.setAttribute("style", "clear: left;");
    
    var div1 = document.createElement("DIV"); 
    div1.setAttribute("class", "div1", 0);
    var aTag = document.createElement('a');
    if (linkURL)
    {
      aTag.setAttribute('href',APIGW +restAPI);
      aTag.target = '_blank';
    }
    else
    {
      aTag.setAttribute('href',"#");
    }
    aTag.innerText = "/api/"+restAPI;
    aTag.style.fontWeight = 'bold';
    div1.appendChild(aTag);

    // <div class='div2'>Device time (epoch) in JSON format</div>
    var div2 = document.createElement("DIV"); 
    div2.setAttribute("class", "div2", 0);
    //var t2 = document.createTextNode(description);                   // Create a text node
    var t2 = document.createElement("p");
    t2.innerHTML = description;                   // Create a text node
    div2.appendChild(t2);     

    topDiv.appendChild(br);    // Append <br> to <div> with id="myDIV"
    topDiv.appendChild(div1);  // Append <div1> to <topDiv> with id="myDIV"
    topDiv.appendChild(div2);  // Append <div2> to <topDiviv> with id="myDIV"
  }
  
  
} // addAPIdoc()

  
//============================================================================  
function refreshSmSettings()
{
  console.log("refreshSmSettings() ..");
  data = {};
  fetch(APIGW+"v2/sm/settings")
    .then(response => response.json())
    .then(json => {
      console.log("then(json => ..)");
      stngsData = json.settings;
      //console.log("refreshSmSettings():["+JSON.stringify(data)+"]");
      for( let i in stngsData )
      {
        let fieldName=stngsData[i].name;
        console.log("refreshSmSettings("+fieldName+")..["+JSON.stringify(stngsData[i])+"]");
        var settings = document.getElementById('settings');
        if( ( document.getElementById("settingR_"+fieldName)) == null )
        {
          var rowDiv = document.createElement("div");
          rowDiv.setAttribute("class", "settingDiv");
          rowDiv.setAttribute("id", "settingR_"+fieldName);
          rowDiv.setAttribute("style", "text-align: right;");
          rowDiv.style.marginLeft = "10px";
          rowDiv.style.marginRight = "10px";
          rowDiv.style.width = "450px";
          rowDiv.style.border = "thick solid lightblue";
          rowDiv.style.background = "lightblue";
          //--- field Name ---
            var fldDiv = document.createElement("div");
                fldDiv.setAttribute("style", "margin-right: 10px;");
                fldDiv.style.width = "250px";
                fldDiv.style.float = 'left';
                fldDiv.textContent = translateToHuman(fieldName);

                rowDiv.appendChild(fldDiv);
          //--- input ---
            var inputDiv = document.createElement("div");
                inputDiv.setAttribute("style", "text-align: left;");

                  var sInput = document.createElement("INPUT");
                  sInput.setAttribute("id", "setFld_"+fieldName);

                  if (stngsData[i].type == "s")
                  {
                    sInput.setAttribute("type", "text");
                    sInput.setAttribute("maxlength", stngsData[i].maxlen);
                  }
                  else if (stngsData[i].type == "f")
                  {
                    sInput.setAttribute("type", "number");
                    sInput.max = stngsData[i].max;
                    sInput.min = stngsData[i].min;
                    sInput.step = (stngsData[i].min + stngsData[i].max) / 1000;
                  }
                  else if (stngsData[i].type == "i")
                  {
                    sInput.setAttribute("type", "number");
                    sInput.max = stngsData[i].max;
                    sInput.min = stngsData[i].min;
                    sInput.step = (stngsData[i].min + stngsData[i].max) / 1000;
                    sInput.step = 1;
                  }
                  sInput.setAttribute("value", stngsData[i].value);
                  sInput.addEventListener('change',
                              function() { setBackGround("setFld_"+fieldName, "lightgray"); },
                                          false
                              );
                inputDiv.appendChild(sInput);
                
          rowDiv.appendChild(inputDiv);
          settings.appendChild(rowDiv);
        }
        else
        {
          document.getElementById("setFld_"+fieldName).style.background = "white";
          document.getElementById("setFld_"+fieldName).value = stngsData[i].value;
        }
      }
      //console.log("-->done..");
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     

    document.getElementById('message').innerHTML = newVersionMsg;

} // refreshSmSettings()
  

//============================================================================  
function refreshDevSettings()
{
  console.log("refreshDevSettings() ..");
  data = {};
  fetch(APIGW+"v2/dev/settings")
    .then(response => response.json())
    .then(json => {
      console.log("then(json => ..)");
      sysData = json.system;
      for( let i in sysData )
      {
        let fieldName=sysData[i].name;
        console.log("["+fieldName+"]=>["+sysData[i].value+"]");
        var system = document.getElementById('system');
        if( ( document.getElementById("systemR_"+fieldName)) == null )
        {
          var rowDiv = document.createElement("div");
          rowDiv.setAttribute("class", "systemDiv");
          rowDiv.setAttribute("id", "systemR_"+fieldName);
          rowDiv.setAttribute("style", "text-align: right;");
          rowDiv.style.marginLeft = "10px";
          rowDiv.style.marginRight = "10px";
          rowDiv.style.width = "450px";
          rowDiv.style.border = "thick solid lightblue";
          rowDiv.style.background = "lightblue";
          //--- field Name ---
            var fldDiv = document.createElement("div");
                fldDiv.setAttribute("style", "margin-right: 10px;");
                fldDiv.style.width = "250px";
                fldDiv.style.float = 'left';
                fldDiv.textContent = translateToHuman(fieldName);

                rowDiv.appendChild(fldDiv);
          //--- input ---
            var inputDiv = document.createElement("div");
                inputDiv.setAttribute("style", "text-align: left;");

                  var sInput = document.createElement("INPUT");
                  sInput.setAttribute("id", "setFld_"+fieldName);

                  if (sysData[i].type == "s")
                  {
                    sInput.setAttribute("type", "text");
                    sInput.setAttribute("maxlength", sysData[i].maxlen);
                  }
                  else if (sysData[i].type == "f")
                  {
                    sInput.setAttribute("type", "number");
                    sInput.max = sysData[i].max;
                    sInput.min = sysData[i].min;
                    sInput.step = (sysData[i].min + sysData[i].max) / 1000;
                    sInput.setAttribute("maxlength", 10); //-- 21-12-2022
                  }
                  else if (sysData[i].type == "i")
                  {
                    sInput.setAttribute("type", "number");
                    sInput.max = sysData[i].max;
                    sInput.min = sysData[i].min;
                    sInput.step = (sysData[i].min + sysData[i].max) / 1000;
                    sInput.step = 1;
                    sInput.setAttribute("maxlength", 10); //-- 21-12-2022
                  }
                  sInput.setAttribute("value", sysData[i].value);
                  sInput.addEventListener('change',
                              function() { setBackGround("setFld_"+fieldName, "lightgray"); },
                                          false
                              );
                inputDiv.appendChild(sInput);
                
          rowDiv.appendChild(inputDiv);
          system.appendChild(rowDiv);
        }
        else
        {
          document.getElementById("setFld_"+fieldName).style.background = "white";
          document.getElementById("setFld_"+fieldName).value = sysData[i].value;
        }
      }
      //console.log("-->done..");
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     

    document.getElementById('message').innerHTML = newVersionMsg;

} // refreshDevSettings()
  

//============================================================================  
function refreshShieldSettings()
{
  console.log("refreshShieldSettings() ..");
  data = {};
  fetch(APIGW+"v2/dev/shield")
    .then(response => response.json())
    .then(json => {
      console.log("then(json => ..)");
      sysData = json.shield;
      for( let i in sysData )
      {
        let fieldName=sysData[i].name;
        console.log("["+fieldName+"]=>["+sysData[i].value+"]");
        var system = document.getElementById('shield');
        if( ( document.getElementById("shieldR_"+fieldName)) == null )
        {
          var rowDiv = document.createElement("div");
          rowDiv.setAttribute("class", "shieldDiv");
          rowDiv.setAttribute("id", "shieldR_"+fieldName);
          rowDiv.setAttribute("style", "text-align: right;");
          rowDiv.style.marginLeft = "10px";
          rowDiv.style.marginRight = "10px";
          rowDiv.style.width = "450px";
          rowDiv.style.border = "thick solid lightblue";
          rowDiv.style.background = "lightblue";
          //--- field Name ---
            var fldDiv = document.createElement("div");
                fldDiv.setAttribute("style", "margin-right: 10px;");
                fldDiv.style.width = "250px";
                fldDiv.style.float = 'left';
                fldDiv.textContent = translateToHuman(fieldName);

                rowDiv.appendChild(fldDiv);
          //--- input ---
            var inputDiv = document.createElement("div");
                inputDiv.setAttribute("style", "text-align: left;");

                  var sInput = document.createElement("INPUT");
                  sInput.setAttribute("id", "setFld_"+fieldName);

                  if (sysData[i].type == "s")
                  {
                    sInput.setAttribute("type", "text");
                    sInput.setAttribute("maxlength", sysData[i].maxlen);
                  }
                  else if (sysData[i].type == "f")
                  {
                    sInput.setAttribute("type", "number");
                    sInput.max = sysData[i].max;
                    sInput.min = sysData[i].min;
                    sInput.step = (sysData[i].min + sysData[i].max) / 1000;
                    sInput.setAttribute("maxlength", 10); //-- 21-12-2022
                  }
                  else if (sysData[i].type == "i")
                  {
                    sInput.setAttribute("type", "number");
                    sInput.max = sysData[i].max;
                    sInput.min = sysData[i].min;
                    sInput.step = (sysData[i].min + sysData[i].max) / 1000;
                    sInput.step = 1;
                    sInput.setAttribute("maxlength", 10); //-- 21-12-2022
                  }
                  sInput.setAttribute("value", sysData[i].value);
                  sInput.addEventListener('change',
                              function() { setBackGround("setFld_"+fieldName, "lightgray"); },
                                          false
                              );
                inputDiv.appendChild(sInput);
                
          rowDiv.appendChild(inputDiv);
          system.appendChild(rowDiv);
        }
        else
        {
          document.getElementById("setFld_"+fieldName).style.background = "white";
          document.getElementById("setFld_"+fieldName).value = sysData[i].value;
        }
      }
      //console.log("-->done..");
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });     

    document.getElementById('message').innerHTML = newVersionMsg;

} // refreshShieldSettings()


//============================================================================  
function getMonths()
{
  console.log("fetch("+APIGW+"v2/hist/months/asc/25)");
  fetch(APIGW+"v2/hist/months/asc/25", {"setTimeout": 2000})
    .then(response => response.json())
    .then(json => {
      //console.log(response);
      data = json.months;
      expandDataSettings(data);
      showMonths(data, monthType);
    })
    .catch(function(error) 
    {
      var p = document.createElement('p');
      p.appendChild(
        document.createTextNode('Error: ' + error.message)
      );
    });

    document.getElementById('message').innerHTML = newVersionMsg;
    
} // getMonths()


//============================================================================  
function showMonths(data, type)
{ 
  console.log("showMonths("+type+")");
  //--- first remove all Children ----
  var allChildren = document.getElementById('editMonths');
  while (allChildren.firstChild) {
    allChildren.removeChild(allChildren.firstChild);
  }
  
  console.log("Now fill the DOM!");    
  for (let i=0; i<data.length; i++)
  {
    //console.log("["+i+"] >>>["+data[i].EEYY+"-"+data[i].MM+"]");
    var em = document.getElementById('editMonths');

    if( ( document.getElementById("em_R"+i)) == null )
    {
      var div1 = document.createElement("div");
          div1.setAttribute("class", "settingDiv");
          div1.setAttribute("id", "em_R"+i);
          div1.style.borderTop = "thick solid lightblue";
          if (i == (data.length -1))  // last row
          {
            div1.style.borderBottom = "thick solid lightblue";
          }
          div1.style.marginLeft = "150px";
          div1.style.marginRight = "400px";
          var span2 = document.createElement("span");
          span2.style.borderTop = "thick solid lightblue";
            //--- create input for EEYY
            var sInput = document.createElement("INPUT");
            sInput.setAttribute("id", "em_YY_"+i);
            sInput.setAttribute("type", "number");
            sInput.setAttribute("min", 2000);
            sInput.setAttribute("max", 2099);
            sInput.size              = 5;
            sInput.style.marginLeft  = '10px';
            sInput.style.marginRight = '20px';
            sInput.addEventListener('change',
                    function() { setNewValue(i, "EEYY", "em_YY_"+i); }, false);
            span2.appendChild(sInput);
            //--- create input for months
            var sInput = document.createElement("INPUT");
            sInput.setAttribute("id", "em_MM_"+i);
            sInput.setAttribute("type", "number");
            sInput.setAttribute("min", 1);
            sInput.setAttribute("max", 12);
            sInput.size              = 3;
            sInput.style.marginRight = '20px';
            sInput.addEventListener('change',
                    function() { setNewValue(i, "MM", "em_MM_"+i); }, false);
            span2.appendChild(sInput);
            //--- create input for data column 1
            sInput = document.createElement("INPUT");
            sInput.setAttribute("id", "em_in1_"+i);
            sInput.setAttribute("type", "number");
            sInput.setAttribute("step", 0.001);
            sInput.style.marginRight = '20px';
            
            if (type == "ED")
            {
              sInput.addEventListener('change',
                  function() { setNewValue(i, "edt1", "em_in1_"+i); }, false );
            }
            else if (type == "ER")
            {
              sInput.addEventListener('change',
                  function() { setNewValue(i, "ert1", "em_in1_"+i); }, false);
            }
            else if (type == "GD")
            {
              sInput.addEventListener('change',
                  function() { setNewValue(i, "gdt", "em_in1_"+i); }, false);
            }
            
            span2.appendChild(sInput);
            //--- if not GD create input for data column 2
            if (type == "ED")
            {
              //console.log("add input for edt2..");
              var sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_in2_"+i);
              sInput.setAttribute("type", "number");
              sInput.setAttribute("step", 0.001);
              sInput.style.marginRight = '20px';
              sInput.addEventListener('change',
                    function() { setNewValue(i, "edt2", "em_in2_"+i); }, false);
              span2.appendChild(sInput);
            }
            else if (type == "ER")
            {
              //console.log("add input for ert2..");
              var sInput = document.createElement("INPUT");
              sInput.setAttribute("id", "em_in2_"+i);
              sInput.setAttribute("type", "number");
              sInput.setAttribute("step", 0.001);
              sInput.style.marginRight = '20px';
              sInput.addEventListener('change',
                    function() { setNewValue(i, "ert2", "em_in2_"+i); }, false);
              span2.appendChild(sInput);
            }
            div1.appendChild(span2);
            em.appendChild(div1);
    } // document.getElementById("em_R"+i)) == null 

    //--- year
    document.getElementById("em_YY_"+i).value = data[i].EEYY;
    document.getElementById("em_YY_"+i).style.background = "white";
    //--- month
    document.getElementById("em_MM_"+i).value = data[i].MM;
    document.getElementById("em_MM_"+i).style.background = "white";
    
    if (type == "ED")
    {
      document.getElementById("em_in1_"+i).style.background = "white";
      document.getElementById("em_in1_"+i).value = (data[i].edt1 *1).toFixed(3);
      document.getElementById("em_in2_"+i).style.background = "white";
      document.getElementById("em_in2_"+i).value = (data[i].edt2 *1).toFixed(3);
    }
    else if (type == "ER")
    {
      document.getElementById("em_in1_"+i).style.background = "white";
      document.getElementById("em_in1_"+i).value = (data[i].ert1 *1).toFixed(3);
      document.getElementById("em_in2_"+i).style.background = "white";
      document.getElementById("em_in2_"+i).value = (data[i].ert2 *1).toFixed(3);
    }
    else if (type == "GD")
    {
      document.getElementById("em_in1_"+i).style.background = "white";
      document.getElementById("em_in1_"+i).value = (data[i].gdt *1).toFixed(3);
    }
    
  } // for all elements in data
  
  console.log("Now sequence EEYY/MM values ..(data.length="+data.length+")");
  //--- sequence EEYY and MM data
  var changed = false;
  for (let i=0; i<(data.length -1); i++)
    {
    //--- month
    if (data[i+1].MM == 0)
    {
      data[i+1].MM    = data[i].MM -1;
      changed = true;
      if (data[i+1].MM < 1) {
        data[i+1].MM   = 12;
        if (data[i+1].EEYY == 2000) {
          data[i+1].EEYY = data[i].EEYY -1;
          document.getElementById("em_YY_"+(i+1)).value = data[i+1].EEYY;
          //document.getElementById("em_YY_"+(i+1)).style.background = "lightgray";
        }
      }
      document.getElementById("em_MM_"+(i+1)).value = data[i+1].MM;
      //document.getElementById("em_MM_"+(i+1)).style.background = "lightgray";
    }
    if (data[i+1].EEYY == 2000) {
      data[i+1].EEYY = data[i].EEYY;
      changed = true;
      document.getElementById("em_YY_"+(i+1)).value = data[i+1].EEYY;
      //document.getElementById("em_YY_"+(i+1)).style.background = "lightgray";
    }
    if (changed) sendPostReading((i+1), data);

  } // sequence EEYY and MM

} // showMonths()


//============================================================================  
function expandDataSettings(data)
{ 
  for (let i=0; i<data.length; i++)
  {
    data[i].EEYY = {};
    data[i].MM   = {};
    data[i].EEYY = parseInt("20"+data[i].recid.substring(0,2));
    data[i].MM   = parseInt(data[i].recid.substring(2,4));
  }

} // expandDataSettings()

    
//============================================================================  
function undoReload()
{
  if (activeTab == "tabEditMonths") {
    console.log("getMonths");
    getMonths();
  } else if (activeTab == "tabEditSmSettings") {
    console.log("undoReload(): reload Settings..");
    data = {};
    refreshSmSettings();
  } else if (activeTab == "tabEditDevSettings") {
    console.log("undoReload(): reload System..");
    data = {};
    refreshDevSettings();

  } else if (activeTab == "tabEditShieldSettings") {
    console.log("undoReload(): reload System..");
    data = {};
    refreshShieldSettings();

  } else {
    console.log("undoReload(): I don't know what to do ..");
  }

} // undoReload()


//============================================================================  
function saveData() 
{
  document.getElementById('message').innerHTML = "Gegevens worden opgeslagen ..";

  if (activeTab == "tabEditSmSettings")
  {
    saveSmSettings();
  } 
  else if (activeTab == "tabEditDevSettings")
  {
    saveDevSettings();
  }
  else if (activeTab == "tabEditShieldSettings")
    {
      saveShieldSettings();
    }
    else if (activeTab == "tabEditMonths")
  {
    saveMeterReadings();
  }
  
} // saveData()


//============================================================================  
function saveSmSettings() 
{
  for(var i in stngsData)
  {
    var fldId  = stngsData[i].name;
    var newVal = document.getElementById("setFld_"+fldId).value;
    if (stngsData[i].value != newVal)
    {
      console.log("save stngsData ["+fldId+"] => from["+stngsData[i].value+"] to["+newVal+"]");
      sendPostSmSetting(fldId, newVal);
    }
  }    
  // delay refresh as all fetch functions are asynchroon!!
  setTimeout(function() 
  {
    refreshSmSettings();
  }, 1000);
  
} // saveSmSettings()


//============================================================================  
function saveDevSettings() 
{
  for(var i in sysData)
  {
    var fldId  = sysData[i].name;
    var newVal = document.getElementById("setFld_"+fldId).value;
    if (sysData[i].value != newVal)
    {
      console.log("save data ["+fldId+"] => from["+sysData[i].value+"] to["+newVal+"]");
      sendPostDevSetting(fldId, newVal);
      if (fldId == "alter_ring_slots") 
      {
        document.getElementById("setFld_"+fldId).value = 0;
        bootsTrapMain();
      }
    }
  }    
  // delay refresh as all fetch functions are asynchroon!!
  setTimeout(function() 
  {
    refreshDevSettings();
  }, 1000);
  
} // saveDevSettings()


//============================================================================  
function saveShieldSettings() 
{
  for(var i in sysData)
  {
    var fldId  = sysData[i].name;
    var newVal = document.getElementById("setFld_"+fldId).value;
    if (sysData[i].value != newVal)
    {
      console.log("save data ["+fldId+"] => from["+sysData[i].value+"] to["+newVal+"]");
      sendPostShieldSetting(fldId, newVal);
    }
  }    
  // delay refresh as all fetch functions are asynchroon!!
  setTimeout(function() 
  {
    refreshShieldSettings();
  }, 1000);
  
} // saveShieldSettings()


//============================================================================  
function saveMeterReadings() 
{
  console.log("Saving months-data ..");
  let changes = false;
  
  //--- has anything changed?
  for (i in data)
  {
    //console.log("saveMeterReadings["+i+"] ..");
    changes = false;

    if (getBackGround("em_YY_"+i) == "lightgray")
    {
      setBackGround("em_YY_"+i, "white");
      changes = true;
    }
    if (getBackGround("em_MM_"+i) == "lightgray")
    {
      setBackGround("em_MM_"+i, "white");
      changes = true;
    }

    if (document.getElementById("em_in1_"+i).style.background == 'lightgray')
    {
      changes = true;
      document.getElementById("em_in1_"+i).style.background = 'white';
    }
    if (monthType != "GD")
    {
      if (document.getElementById("em_in2_"+i).style.background == 'lightgray')
      {
        changes = true;
        document.getElementById("em_in2_"+i).style.background = 'white';
      }
    }
    if (changes) {
      console.log("Changes where made in ["+i+"]["+data[i].EEYY+"-"+data[i].MM+"]");
      //processWithTimeout([(data.length -1), 0], 2, data, sendPostReading);
      sendPostReading(i, data);
    }
  } 

} // saveMeterReadings()

  
//============================================================================  
function sendPostSmSetting(field, value) 
{
  const jsonString = {"name" : field, "value" : value};
  //console.log("send JSON:["+jsonString+"]");
  const other_params = {
      headers : { "content-type" : "application/json; charset=UTF-8"},
      body : JSON.stringify(jsonString),
      method : "POST",
      mode : "cors"
  };

  //-fetch(APIGW+"v2/dev/settings", other_params)
  fetch(APIGW+"v2/sm/settings", other_params)
    .then(function(response) {
          //console.log(response.status );    //=> number 100–599
          //console.log(response.statusText); //=> String
          //console.log(response.headers);    //=> Headers
          //console.log(response.url);        //=> String
          //console.log(response.text());
          //return response.text()
    }, function(error) 
    {
      console.log("Error["+error.message+"]"); //=> String
    });
    
} // sendPostSmSetting()

  
//============================================================================  
function sendPostDevSetting(field, value) 
{
  const jsonString = {"name" : field, "value" : value};
  //console.log("send JSON:["+jsonString+"]");
  const other_params = {
      headers : { "content-type" : "application/json; charset=UTF-8"},
      body : JSON.stringify(jsonString),
      method : "POST",
      mode : "cors"
  };

  //-fetch(APIGW+"v2/dev/system", other_params)
  fetch(APIGW+"v2/dev/settings", other_params)
    .then(function(response) {
          //console.log(response.status );    //=> number 100–599
          //console.log(response.statusText); //=> String
          //console.log(response.headers);    //=> Headers
          //console.log(response.url);        //=> String
          //console.log(response.text());
          //return response.text()
    }, function(error) 
    {
      console.log("Error["+error.message+"]"); //=> String
    });
    
} // sendPostDevSetting()

  
//============================================================================  
function sendPostShieldSetting(field, value) 
{
  const jsonString = {"name" : field, "value" : value};
  //console.log("send JSON:["+jsonString+"]");
  const other_params = {
      headers : { "content-type" : "application/json; charset=UTF-8"},
      body : JSON.stringify(jsonString),
      method : "POST",
      mode : "cors"
  };

  //-fetch(APIGW+"v2/dev/system", other_params)
  fetch(APIGW+"v2/dev/shield", other_params)
    .then(function(response) {
          //console.log(response.status );    //=> number 100–599
          //console.log(response.statusText); //=> String
          //console.log(response.headers);    //=> Headers
          //console.log(response.url);        //=> String
          //console.log(response.text());
          //return response.text()
    }, function(error) 
    {
      console.log("Error["+error.message+"]"); //=> String
    });
    
} // sendPostShieldSetting()

  
//============================================================================  
function sendEraseRequest() 
{
  const jsonString = {"name" : "eraseRequest"};
  //console.log("send JSON:["+jsonString+"]");
  const other_params = {
      headers : { "content-type" : "application/json; charset=UTF-8"},
      body : JSON.stringify(jsonString),
      method : "POST",
      mode : "cors"
  };
    
  fetch(APIGW+"v2/dev/erase", other_params)
    .then(function(response) {
    }, function(error) {
      console.log("Error["+error.message+"]"); //=> String
    });
    
  refreshDevSyslog();
  
} // sendEraseRequest()

  
//============================================================================  
function validateReadings(type) 
{
  let withErrors = false;
  let prevMM     = 0;
  let lastBG     = "white";
      
  console.log("validate("+type+")");
  
  for (let i=0; i<(data.length -1); i++)
  {
    //--- reset background for the years
    if (getBackGround("em_YY_"+i) == "red")
    {
      setBackGround("em_YY_"+i, "lightgray");
    }
    //--- zelfde jaar, dan prevMM := (MM -1)
    if ( data[i].EEYY == data[i+1].EEYY )
    {
      prevMM = data[i].MM -1;
      //console.log("["+i+"].EEYY == ["+(i+1)+"].EEYY => ["+data[i].EEYY+"] prevMM["+prevMM+"]");
    }
    //--- jaar == volgend jaar + 1
    else if ( data[i].EEYY == (data[i+1].EEYY +1) )
    {
      prevMM = 12;
      //console.log("["+i+"].EEYY == ["+(i+1)+"].EEYY +1 => ["+data[i].EEYY+"]/["+data[i+1].EEYY+"] ("+prevMM+")");
    }
    else
    {
      setBackGround("em_YY_"+(i+1), "red");
      withErrors = true;
      prevMM = data[i].MM -1;
      //console.log("["+i+"].EEYY == ["+(i+1)+"].EEYY +1 => ["+data[i].EEYY+"]/["+data[i+1].EEYY+"] (error)");
    }
    
    //--- reset background for the months
    if (getBackGround("em_MM_"+(i+1)) == "red")
    {
      setBackGround("em_MM_"+(i+1), "lightgray");
    }
    //--- if next month != prevMM and this MM != next MM
    if (data[i+1].MM != prevMM && data[i].MM != data[i+1].MM)
    {
      setBackGround("em_MM_"+(i+1), "red");
      withErrors = true;
      //console.log("(["+(i+1)+"].MM != ["+prevMM+"].prevMM) && => ["+data[i].MM+"]/["+data[i+1].MM+"] (error)");
    }
    else
    {
      //setBackGround("em_MM_"+i, "lightgreen");
    }
    if (type == "ED")
    {
      if (getBackGround("em_in1_"+(i+1)) == "red")
      {
        setBackGround("em_in1_"+(i+1), "lightgray");
      }
      if (data[i].edt1 < data[i+1].edt1)
      {
        setBackGround("em_in1_"+(i+1), "red");
        withErrors = true;
      }
      if (getBackGround("em_in2_"+(i+1)) == "red")
      {
        setBackGround("em_in2_"+(i+1), "lightgray");
      }
      if (data[i].edt2 < data[i+1].edt2)
      {
        setBackGround("em_in2_"+(i+1), "red");
        withErrors = true;
      }
    }
    else if (type == "ER")
    {
      if (getBackGround("em_in1_"+(i+1)) == "red")
      {
        setBackGround("em_in1_"+(i+1), "lightgray");
      }
      if (data[i].ert1 < data[i+1].ert1)
      {
        setBackGround("em_in1_"+(i+1), "red");
        withErrors = true;
      }
      if (getBackGround("em_in2_"+(i+1)) == "red")
      {
        setBackGround("em_in2_"+(i+1), "lightgray");
      }
      if (data[i].ert2 < data[i+1].ert2)
      {
        setBackGround("em_in2_"+(i+1), "red");
        withErrors = true;
      }
    }
    else if (type == "GD")
    {
      if (getBackGround("em_in1_"+(i+1)) == "red")
      {
        setBackGround("em_in1_"+(i+1), "lightgray");
      }
      if (data[i].gdt < data[i+1].gdt)
      {
        setBackGround("em_in1_"+(i+1), "red");
        withErrors = true;
      }
    }
    
  }
  if (withErrors)  return false;

  return true;
  
} // validateReadings()

  
//============================================================================  
function sendPostReading(i, row) 
{
  console.log("sendPostReadings["+i+"]..");
  let sYY = (row[i].EEYY - 2000).toString();
  let sMM = "00";
  if ((row[i].MM *1) < 1 || (row[i].MM *1) > 12)
  {
    console.log("send: ERROR MM["+row[i].MM+"]");
    return;
  }
  if (row[i].MM < 10)
        sMM = "0"+(row[i].MM).toString();
  else  sMM = ((row[i].MM * 1)).toString();
  let sDDHH = "0101";
  let recId = sYY + sMM + sDDHH;
  console.log("send["+i+"] => ["+recId+"]");
  
  const jsonString = {"recid": recId, "edt1": row[i].edt1, "edt2": row[i].edt2,
                       "ert1": row[i].ert1,  "ert2": row[i].ert2, "gdt":  row[i].gdt };

  const other_params = {
      headers : { "content-type" : "application/json; charset=UTF-8"},
      body : JSON.stringify(jsonString),
      method : "POST",
      mode : "cors"
  };
  
  fetch(APIGW+"v2/hist/months", other_params)
    .then(function(response) {
    }, function(error) {
      console.log("Error["+error.message+"]"); //=> String
    });

    
} // sendPostReading()


//============================================================================  
function readGitHubVersion()
{
  if (GitHubVersion != 0) return;
  
  fetch("https://cdn.jsdelivr.net/gh/mrWheel/DSMRloggerAPI@master/data/DSMRversion.dat", {cache: "no-store"})
    .then(response => {
      if (response.ok) {
        return response.text();
      } else {
        console.log('Something went wrong');
        return "";
      }
    })
    .then(text => {
      var tmpGHF     = text.replace(/(\r\n|\n|\r)/gm, "");
      console.log("parsed: tmpGHF is ["+tmpGHF+"]");
      GitHubVersion_dspl = tmpGHF;
      //console.log("parsed: GitHubVersion is ["+GitHubVersion_dspl+"]");
      tmpX = tmpGHF.substring(1, tmpGHF.indexOf(' '));
      //console.log("parsed: tmpX is ["+tmpX+"]");
      tmpN = tmpX.split(".");
      console.log("parsed: tmpN is ["+tmpN[0]+"|"+tmpN[1]+"|"+tmpN[2]+"]");
      GitHubVersion = tmpN[0]*10000 + tmpN[1]*1;
      
      console.log("firmwareVersion["+firmwareVersion+"] >= GitHubVersion["+GitHubVersion+"]");
      if (firmwareVersion == 0 || firmwareVersion >= GitHubVersion)
            newVersionMsg = "";
      else  newVersionMsg = firmwareVersion_dspl + " nieuwere versie ("+GitHubVersion_dspl+") beschikbaar";
      document.getElementById('message').innerHTML = newVersionMsg;
      console.log(newVersionMsg);

    })
    .catch(function(error) 
    {
      console.log(error);
      GitHubVersion_dspl   = "";
      GitHubVersion        = 0;
    });     

} // readGitHubVersion()

  
//============================================================================  
function setEditType(eType) 
{
  if (eType == "ED") {
    console.log("Edit Energy Delivered!");
    monthType = eType;
    getMonths()
    showMonths(data, monthType);
  } else if (eType == "ER") {
    console.log("Edit Energy Returned!");
    monthType = eType;
    getMonths()
    showMonths(data, monthType);
  } else if (eType == "GD") {
    console.log("Edit Gas Delivered!");
    monthType = eType;
    getMonths()
    showMonths(data, monthType);
  } else {
    console.log("setEditType to ["+eType+"] is quit shitty!");
    monthType = "";
  }

} // setEditType()

 
//============================================================================  
function setNewValue(i, dField, field) 
{
  document.getElementById(field).style.background = "lightgray";
  //--- this is ugly!!!! but don't know how to do it better ---
  if (dField == "EEYY")       data[i].EEYY = document.getElementById(field).value;
  else if (dField == "MM")    data[i].MM   = document.getElementById(field).value;
  else if (dField == "edt1")  data[i].edt1 = document.getElementById(field).value;
  else if (dField == "edt2")  data[i].edt2 = document.getElementById(field).value;
  else if (dField == "ert1")  data[i].ert1 = document.getElementById(field).value;
  else if (dField == "ert2")  data[i].ert2 = document.getElementById(field).value;
  else if (dField == "gdt")   data[i].gdt  = document.getElementById(field).value;
  
} // setNewValue()

 
//============================================================================  
function setBackGround(field, newColor) 
{
  console.log("setBackGround("+field+")");
  document.getElementById(field).style.background = newColor;
  
} // setBackGround()

 
//============================================================================  
function getBackGround(field) 
{
  return document.getElementById(field).style.background;
  
} // getBackGround()


//============================================================================  
function validateNumber(field) 
{
  console.log("validateNumber(): ["+field+"]");
  if (field == "EDT1" || field == "EDT2" || field == "ERT1" || field == "ERT2" || field == "GAS") 
  {
    var pattern = /^\d{1,1}(\.\d{1,5})?$/;
    var max = 1.99999;
  } else 
  {
    var pattern = /^\d{1,2}(\.\d{1,2})?$/;
    var max = 99.99;
  }
  var newVal = document.getElementById(field).value;
  newVal = newVal.replace( /[^0-9.]/g, '' );
  if (!pattern.test(newVal)) 
  {
    document.getElementById(field).style.color = 'orange';
    console.log("wrong format");
  } else 
  {
    document.getElementById(field).style.color = settingFontColor;
    console.log("valid number!");
  }
  if (newVal > max) 
  {
    console.log("Number to big!");
    document.getElementById(field).style.color = 'orange';
    newVal = max;
  }
  document.getElementById(field).value = newVal * 1;
  
} // validateNumber()


//============================================================================  
function translateToHuman(longName) 
{
  for(var index = 0; index < translateFields.length; index++) 
  {
      if (translateFields[index][0] == longName)
      {
        return translateFields[index][1];
      }
  };
  return longName;
  
} // translateToHuman()


//============================================================================  
function formatDate(type, dateIn) 
{
  let dateOut = "";
  if (type == "Hours")
  {
    //date = "20"+dateIn.substring(0,2)+"-"+dateIn.substring(2,4)+"-"+dateIn.substring(4,6)+" ["+dateIn.substring(6,8)+"]";
    dateOut = "("+dateIn.substring(4,6)+") ["+dateIn.substring(6,8)+":00 - "+dateIn.substring(6,8)+":59]";
  }
  else if (type == "Days")
    dateOut = recidToWeekday(dateIn)+" "+dateIn.substring(4,6)+"-"+dateIn.substring(2,4)+"-20"+dateIn.substring(0,2);
  else if (type == "Months")
    dateOut = "20"+dateIn.substring(0,2)+"-["+dateIn.substring(2,4)+"]-"+dateIn.substring(4,6)+":"+dateIn.substring(6,8);
  else
    dateOut = "20"+dateIn.substring(0,2)+"-"+dateIn.substring(2,4)+"-"+dateIn.substring(4,6)+":"+dateIn.substring(6,8);
  return dateOut;
}


//============================================================================  
function recidToEpoch(dateIn) 
{
  var YY = "20"+dateIn.substring(0,2);
  console.log("["+YY+"]["+(dateIn.substring(2,4)-1)+"]["+dateIn.substring(4,6)+"]");
  //-------------------YY-------------------(MM-1)----------------------DD---------------------HH--MM--SS
  var epoch = Date.UTC(YY, (dateIn.substring(2,4)-1), dateIn.substring(4,6), dateIn.substring(6,8), 1, 1);
  //console.log("epoch is ["+epoch+"]");

  return epoch;
  
} // recidToEpoch()


//============================================================================  
function recidToWeekday(dateIn)
{
  var YY = "20"+dateIn.substring(0,2);
  //-------------------YY-------------------(MM-1)----------------------DD---------------------HH--MM--SS
  var dt = new Date(Date.UTC(YY, (dateIn.substring(2,4)-1), dateIn.substring(4,6), 1, 1, 1));

  return dt.toLocaleDateString('nl-NL', {weekday: 'long'});
  
} // epochToWeekday()

  
//============================================================================  
function round(value, precision) 
{
  var multiplier = Math.pow(10, precision || 0);
  return Math.round(value * multiplier) / multiplier;
}

  
//============================================================================  
function isFloat(x) 
{
  // check if the passed value is a number
  if(typeof x == 'number' && !isNaN(x))
  {
    // check if it is integer
    if (Number.isInteger(x))
          return false;
    else  return true;
  } 
  else 
  {
    return false;
  }
  
} //  isFloat()
  
  
//============================================================================  
var translateFields = [
         [ "author",                    "Auteur" ]
        ,[ "boardtype",                 "Board Type" ]
        ,[ "chip_model",                "Chip type" ]
        ,[ "chipid",                    "Chip ID" ]
        ,[ "compile_options",           "Compiler Opties" ]
        ,[ "compiled",                  "Gecompileerd" ]
        ,[ "coreversion",               "Core Versie" ]
        ,[ "cpu_freq",                  "CPU Frequency [MHz]" ]
        ,[ "current_l1",                "Current l1" ]
        ,[ "current_l2",                "Current l2" ]
        ,[ "current_l3",                "Current l3" ]
        ,[ "daily_reboot",               "Dagelijkse Reboot [0=Nee, 1=Ja]" ]
        
        ,[ "ed_tariff1",                "Energie Verbruik Tarief-1/kWh" ]
        ,[ "ed_tariff2",                "Energie Verbruik Tarief-2/kWh" ]
        ,[ "electr_netw_costs",         "Netwerkkosten Energie/maand" ]
        ,[ "electricity_failure_log",   "Electricity Failure log" ]
        ,[ "electricity_failures",      "Electricity Failures" ]
        ,[ "electricity_long_failures", "Electricity Long Failures" ]
        ,[ "electricity_sags_l1",       "Electricity Sags l1" ]
        ,[ "electricity_sags_l2",       "Electricity Sags l2" ]
        ,[ "electricity_sags_l3",       "Electricity Sags l3" ]
        ,[ "electricity_swells_l1",     "Electricity Swells l1" ]
        ,[ "electricity_swells_l2",     "Electricity Swells l2" ]
        ,[ "electricity_swells_l3",     "Electricity Swells l3" ]
        ,[ "electricity_switch_position","Electricity Switch Position" ]
        ,[ "electricity_tariff",        "Electriciteit tarief" ]
        ,[ "electricity_threshold",     "Electricity Threshold" ]
        ,[ "energy_delivered_tariff1",  "Energie Gebruikt tarief 1" ]
        ,[ "energy_delivered_tariff2",  "Energie Gebruikt tarief 2" ]
        ,[ "energy_returned_tariff1",   "Energie Opgewekt tarief 1" ]
        ,[ "energy_returned_tariff2",   "Energie Opgewekt tarief 2" ]
        ,[ "er_tariff1",                "Energie Opgewekt Tarief-1/kWh" ]
        ,[ "er_tariff2",                "Energie Opgewekt Tarief-2/kWh" ]
        
        ,[ "filesystem_type",           "File Systeem" ]
        ,[ "filesystem_size",           "Grootte littleFS [bytes]" ]
        ,[ "flashchip_mode",            "Flash Chip Mode" ]
        ,[ "flashchip_speed",           "Flash Chip Freq. [Hz]" ]
        ,[ "flashchipid",               "Flash Chip ID" ]
        ,[ "flashchiprealsize",         "Flash Chip Real Size" ]
        ,[ "flashchipsize",             "Flash Chip Size" ]
        ,[ "free_heap",                 "Free Heap Space [bytes]" ]
        ,[ "free_psram_size",           "Free Psram [SPI-RAM] [bytes]" ]
        ,[ "free_sketch_space",         "Free Sketch Space [bytes]" ]
        ,[ "fwversion",                 "Firmware Versie" ]
        
        ,[ "gas_delivered",             "Gas Gebruikt" ]
        ,[ "gas_netw_costs",            "Netwerkkosten Gas/maand" ]
        ,[ "gd_tariff" ,                "Gas Verbruik Tarief/m3" ]
        ,[ "hostname",                  "HostName" ]
        ,[ "identification",            "Slimme Meter ID" ]
        ,[ "index_page",                "Te Gebruiken index.html Pagina" ]
        ,[ "indexfile",                 "Te Gebruiken index.html Pagina" ]
        ,[ "ipaddress",                 "IP adres" ]
        ,[ "last_reset",                "Laatste Reset reden" ]
        
        ,[ "macaddress",                "MAC adres" ]
        ,[ "mbus1_delivered",           "MBus-1 Gebruikt" ]
        ,[ "mbus1_delivered_dbl",       "MBus-1 Gebruikt" ]
        ,[ "mbus1_delivered_ntc",       "MBus-1 Gebruikt [ntc]" ]
        ,[ "mbus1_device_type",         "MBus-1 Type meter [0=geen]" ]
        ,[ "mbus1_equipment_id_ntc",    "MBus-1 Equipm. ID [ntc]" ]
        ,[ "mbus1_equipment_id_tc",     "MBus-1 Equipm. ID [tc]" ]
        ,[ "mbus1_type",                "MBus-1 Type meter [0=geen]" ]
        ,[ "mbus1_valve_position",      "MBus-1 Klep Positie" ]
        ,[ "mbus2_delivered",           "MBus-2 Gebruikt" ]
        ,[ "mbus2_delivered_dbl",       "MBus-2 Gebruikt" ]
        ,[ "mbus2_delivered_ntc",       "MBus-2 Gebruikt [ntc]" ]
        ,[ "mbus2_device_type",         "MBus-2 Type meter [0=geen]" ]
        ,[ "mbus2_equipment_id_ntc",    "MBus-2 Equipm. ID [ntc]" ]
        ,[ "mbus2_equipment_id_tc",     "MBus-2 Equipm. ID [tc]" ]
        ,[ "mbus2_type",                "MBus-2 Type meter [0=geen]" ]
        ,[ "mbus2_valve_position",      "MBus-2 Klep Positie" ]
        ,[ "mbus3_delivered",           "MBus-3 Gebruikt" ]
        ,[ "mbus3_delivered_dbl",       "MBus-3 Gebruikt" ]
        ,[ "mbus3_delivered_ntc",       "MBus-3 Gebruikt [ntc]" ]
        ,[ "mbus3_device_type",         "MBus-3 Type meter [0=geen]" ]
        ,[ "mbus3_equipment_id_ntc",    "MBus-3 Equipm. ID [ntc]" ]
        ,[ "mbus3_equipment_id_tc",     "MBus-3 Equipm. ID [tc]" ]
        ,[ "mbus3_type",                "MBus-3 Type meter [0=geen]" ]
        ,[ "mbus3_valve_position",      "MBus-3 Klep Positie" ]
        ,[ "mbus4_delivered",           "MBus-4 Gebruikt" ]
        ,[ "mbus4_delivered_dbl",       "MBus-4 Gebruikt" ]
        ,[ "mbus4_delivered_ntc",       "MBus-4 Gebruikt [ntc]" ]
        ,[ "mbus4_device_type",         "MBus-4 Type meter [0=geen]" ]
        ,[ "mbus4_equipment_id_ntc",    "MBus-4 Equipm. ID [ntc]" ]
        ,[ "mbus4_equipment_id_tc",     "MBus-4 Equipm. ID [tc]" ]
        ,[ "mbus4_type",                "MBus-4 Type meter [0=geen]" ]
        ,[ "mbus4_valve_position",      "MBus-4 Klep Positie" ]
        ,[ "message_long",              "Lange Boodschap" ]
        ,[ "message_short",             "Korte Boodschap" ]
        ,[ "min_free_heap",             "Min. Free Heap ever [bytes]" ]
        ,[ "mqtt_broker",               "MQTT Broker IP/URL" ]
        ,[ "mqtt_broker_connected",     "MQTT broker connected [1=Ja]" ]
        ,[ "mqtt_broker_port",          "MQTT Broker Poort" ]
        ,[ "mqtt_interval",             "Verzend MQTT Berichten [Sec.]" ]
        ,[ "mqtt_passwd",               "Password MQTT Gebruiker" ]
        ,[ "mqtt_toptopic",             "MQTT Top Topic" ]
        ,[ "mqtt_user",                 "MQTT Gebruiker" ]
        ,[ "mqttbroker",                "MQTT Broker IP/URL" ]
        ,[ "mqttbrokerport",            "MQTT Broker Poort" ]
        ,[ "mqttinterval",              "Verzend MQTT Berichten [Sec.]" ]
        ,[ "mqttpasswd",                "Password MQTT Gebruiker" ]
        ,[ "mqtttoptopic",              "MQTT Top Topic" ]
        ,[ "mqttuser",                  "MQTT Gebruiker" ]
        
        ,[ "no_hour_slots",             "Uren aan historie" ]
        ,[ "no_day_slots",              "Dagen aan historie" ]
        ,[ "no_month_slots",            "Maanden aan historie [in jaren]" ]
        ,[ "alter_ring_slots",          "Historie aanpassen [1=Ja]" ]
        ,[ "neo_brightness",            "Brightness NeoPixels" ]
        
        ,[ "oled_flip_screen",          "Flip OLED scherm [0=Nee, 1=Ja]" ]
        ,[ "oled_screen_time",          "Oled Screen Time [Min., 0=infinite]" ]
        ,[ "oled_type",                 "OLED type [0=None, 1=SDD1306, 2=SH1106]" ]
        ,[ "p1_version",                "P1 Versie" ]
        ,[ "p1_version_be",             "P1 Versie [BE]" ]
        ,[ "power_delivered",           "Vermogen Gebruikt" ]
        ,[ "power_delivered_l1",        "Vermogen Gebruikt l1" ]
        ,[ "power_delivered_l2",        "Vermogen Gebruikt l2" ]
        ,[ "power_delivered_l3",        "Vermogen Gebruikt l3" ]
        ,[ "power_returned",            "Vermogen Opgewekt" ]
        ,[ "power_returned_l1",         "Vermogen Opgewekt l1" ]
        ,[ "power_returned_l2",         "Vermogen Opgewekt l2" ]
        ,[ "power_returned_l3",         "Vermogen Opgewekt l3" ]
        ,[ "pre_dsmr40",                "Pré DSMR 40 [0=Nee, 1=Ja]" ]
        ,[ "psram_size",                "Psram size [bytes]" ]
        
        ,[ "reboots",                   "Aantal keer opnieuw opgestart" ]
        ,[ "run_as_ap",                 "run als AccessPoint [0=Nee, 1=Ja]" ]
        ,[ "sdk_version",               "SDK versie" ]
        ,[ "sketch_size",               "Sketch Size [bytes]" ]
        ,[ "sm_has_fase_info",          "SM Has Fase Info [0=Nee, 1=Ja]" ]
        ,[ "smhasfaseinfo",             "SM Has Fase Info [0=Nee, 1=Ja]" ]
        ,[ "ssid",                      "WiFi SSID" ]
        ,[ "shld_GPIOpin0",             "SW-0 GPIO pin [-1=geen, 13, 14]" ]
        ,[ "shld_inversed0",            "SW-0 Inversed Logic [0=Nee, 1=Ja]" ]
        ,[ "shld_activeStart0",         "SW-0 Actief start tijd" ]
        ,[ "shld_activeStop0",          "SW-0 Actief stop tijd" ]
        ,[ "shld_onThreshold0",         "SW-0 'Aan' drempel" ]
        ,[ "shld_offThreshold0",        "SW-0 'Uit' drempel'" ]
        ,[ "shld_onDelay0",             "SW-0 'Aan' vertraging [sec.]" ]
        ,[ "shld_offDelay0",            "SW-0 'Uit' vertraging [sec.]" ]
        ,[ "shld_GPIOpin1",             "SW-1 GPIO pin [-1=geen, 13, 14]" ]
        ,[ "shld_inversed1",            "SW-1 Inversed Logic [0=Nee, 1=Ja]" ]
        ,[ "shld_activeStart1",         "SW-1 Actief start tijd" ]
        ,[ "shld_activeStop1",          "SW-1 Actief stop tijd" ]
        ,[ "shld_onThreshold1",         "SW-1 'Aan' drempel" ]
        ,[ "shld_offThreshold1",        "SW-1 'Uit' drempel'" ]
        ,[ "shld_onDelay1",             "SW-1 'Aan' vertraging [sec.]" ]
        ,[ "shld_offDelay1",            "SW-1 'Uit' vertraging [sec.]" ]
        ,[ "telegram_count",            "Aantal verwerkte Telegrammen" ]
        ,[ "telegram_errors",           "Aantal Foutieve Telegrammen" ]
        ,[ "telegram_interval",         "Telegram Lees Interval [Sec.]" ]
        ,[ "tlgrm_interval",            "Telegram Lees Interval [Sec.]" ]
        
        ,[ "uptime",                    "Up Time [dagen] - [hh:mm]" ]
        ,[ "uptime_sec",                "Up Time in Seconden" ]
        ,[ "used_psram_size",           "Psram in gebruik [bytes]" ]
        ,[ "voltage_l1",                "Voltage l1" ]
        ,[ "voltage_l2",                "Voltage l2" ]
        ,[ "voltage_l3",                "Voltage l3" ]
        ,[ "wifi_rssi",                 "WiFi RSSI" ]
        
                      ];

/*
***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************
*/
