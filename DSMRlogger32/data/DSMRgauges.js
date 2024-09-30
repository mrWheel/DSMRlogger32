const APIGW=window.location.protocol+'//'+window.location.host+'/api/';
const AMPS =25
const PHASES = 3

var AmpG = [4];
var PhaseAmps = [4];
var MaxAmps = [4];
var TotalAmps = 0.0, 
    minKW = [4], 
    maxKW = [4];
var timeTimer  = 0;
var currentUnit = 'Amps';


var gaugeOptions = {

    chart: {
        type: 'solidgauge'
    },

    title: null,

    pane: {
        center: ['50%', '75%'],
        size: '100%',
        startAngle: -90,
        endAngle: 90,
        background: {
            backgroundColor:
                Highcharts.defaultOptions.legend.backgroundColor || '#EEE',
            innerRadius: '80%',
            outerRadius: '105%',
            shape: 'arc'
            //opacity: '30%'
        }
    },

    tooltip: {
        enabled: false
    },

    // the value axis
    yAxis: {
        stops: [
            [0.1, '#55BF3B'], // green
            [0.5, '#DDDF0D'], // yellow
            [0.9, '#DF5353'] // red
        ],
        lineWidth: 1,
        //minorTickInterval: 'auto',
        minorTicks: true,
        // minorTicksWidth: 3px,
        //tickAmount: 0,
        title: {
            y: -70
        },
        labels: {
            y: 25
        }
    },

    plotOptions: {
        solidgauge: {
            innerRadius: '85%',
            opacity: '60%',
            dataLabels: {
                y: 5,
                borderWidth: 0,
                useHTML: true
            }
        }
    }
};

var AmpOptions = {
    yAxis: {
        min: -25,
        max: 25,
    },

    credits: {
        enabled: false
    },

    series: [{
        name: 'A',
        data: [0],
        dataLabels: {
            format:
                '<div style="text-align:center">' +
                '<span style="font-weight:lighter;font-size:16px;font-family: Dosis">{y} A</span><br/>' +
                '</div>'
        }
    },{

        name: 'Max',
        data: [1],
        innerRadius:'100%',
        radius: '105%',
        dataLabels: {
            enabled: false
       }
    }]

};

  window.onload=bootsTrapMain;
    
  //============================================================================  
  function bootsTrapMain() 
  {
    console.log("bootsTrapMain()");
    document.getElementById('bReturn').addEventListener('click',function() 
                                                { console.log("newTab: Return");
                                                  location.href = "/DSMRindex.html";
                                                });
    // Add event listeners for radio buttons
    document.querySelectorAll('input[name="displayUnit"]').forEach((elem) => {
        elem.addEventListener("change", function(event) {
            currentUnit = event.target.value;
            updateGauges();
        });
    });
    
    refreshDevInfo();
    refreshDevTime();
    
    clearInterval(timeTimer);  
    timeTimer = setInterval(refreshDevTime, 10 * 1000); // repeat every 10s

    console.log("..exit bootsTrapMain()!");
      
  } // bootsTrapMain()
  
  
  //============================================================================  
  function refreshDevInfo() {
    console.log("Refreshing api2dev/info...");
    fetch(APIGW + "v2/dev/info")
        .then(response => {
            console.log("Received response:", response);
            return response.json();
        })
        .then(json => {
            //console.log("Parsed JSON:", JSON.stringify(json));

            // Check if the 'devinfo' field exists in the JSON
            if (!json || !json.devinfo) {
                console.error("Error: 'devinfo' field is missing in the JSON response.");
                return;
            }

            let data = json.devinfo;
            //console.log("Data keys:", Object.keys(data));

            let fldValue, fldUnit;

            // Log each field and value in data
            for (let field in data) {
                //console.log(`Processing field [${field}] with value [${data[field]}]..`);
                fldValue = data[field];

                if (field === 'hostname') {
                    console.log("Field == 'hostname', value:", fldValue);
                    if (document.getElementById('devName')) {
                        document.getElementById('devName').innerHTML = fldValue;
                    } else {
                        console.error("Element with id 'devName' not found.");
                    }
                } else if (field === 'fwversion') {
                    console.log("Field == 'fwversion', value:", fldValue);
                    if (document.getElementById('devVersion')) {
                        document.getElementById('devVersion').innerHTML = fldValue;
                    } else {
                        console.error("Element with id 'devVersion' not found.");
                    }

                    let tmpFW = fldValue;
                    let firmwareVersion_dspl = tmpFW;
                    let tmpX = tmpFW.substring(1, tmpFW.indexOf(' '));
                    let tmpN = tmpX.split(".");
                    let firmwareVersion = tmpN[0] * 10000 + tmpN[1] * 1;

                    console.log(`Firmware Version: [${firmwareVersion}]`);
                } 
                /***
                else {
                    console.log(`Field [${field}] is not specifically handled.`);
                }
                ***/
            }
        })
        .catch(function (error) {
            console.error('Error:', error.message);
            var p = document.createElement('p');
            p.appendChild(
                document.createTextNode('Error: ' + error.message)
            );
            document.body.appendChild(p);
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
      
    //document.getElementById('message').innerHTML = newVersionMsg;

  } // refreshDevTime()
  
  
//============================================================================  
function abs(x)
{
    return (x < 0 ? -x : x);
}

//============================================================================  
function getFieldByName(data, prefix, factor = 1) 
{
    //console.log("["+prefix+"]->["+data[prefix]+"]");
    return {
      name: prefix,
      value: factor * Number(data[prefix])
    };
}

//============================================================================  
function updateGaugeScale(maxValue) {
    let scale;
    if (currentUnit === 'Amps') {
        if (maxValue < 5) {
            scale = 5;
        } else if (maxValue < 15) {
            scale = 15;
        } else {
            scale = 25;
        }
    } else { // Watts
        if (maxValue < 1150) {
            scale = 1150;
        } else if (maxValue < 3450) {
            scale = 3450;
        } else {
            scale = 5750;
        }
    }

    for (let i = 1; i <= PHASES + 1; i++) {
        AmpG[i].yAxis[0].update({
            min: -scale,
            max: scale
        });
    }
}

//============================================================================  
function updateGauges() {
    document.querySelectorAll('.unit').forEach((elem) => {
        elem.textContent = currentUnit === 'Amps' ? 'Ampère' : 'Watt';
    });

    for (let phase = 1; phase <= PHASES + 1; phase++) {
        let chart = AmpG[phase];
        let point = chart.series[0].points[0];
        let maxPoint = chart.series[1].points[0];
        
        let value = currentUnit === 'Amps' ? PhaseAmps[phase] : PhaseAmps[phase] * 230;
        let maxValue = currentUnit === 'Amps' ? MaxAmps[phase] : MaxAmps[phase] * 230;
        
        point.update(Math.round(value * 1000) / 1000);
        maxPoint.update(Math.round(maxValue * 1000) / 1000);
        
        chart.series[0].update({
            dataLabels: {
                format: `<div style="text-align:center"><span style="font-weight:lighter;font-size:16px;font-family: Dosis">{y} ${currentUnit === 'Amps' ? 'A' : 'W'}</span><br/></div>`
            }
        });
    }
    
    updateGaugeScale(currentUnit === 'Amps' ? Math.max(...MaxAmps) : Math.max(...MaxAmps) * 230);
}

//============================================================================  
function update()
{
    var phase;
    let maxAmps = 0;
    //console.log("update() ..");
    fetch(APIGW+"v2/sm/actual")
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
    })
    .then(json => {
        const data = json.actual;

        // Check relay_state and change background color of the fourth gauge
        console.log("relay_state:", data.relay_state);
        console.log("AmpG[4] type:", typeof AmpG[4]);
        console.log("AmpG[4] chart type:", AmpG[4].options ? AmpG[4].options.chart.type : "undefined");
        
        if (data.relay_state === "1") {
            console.log("Attempting to change background to light green");
            try {
                const chartDiv = document.getElementById('container-t');
                if (chartDiv) {
                    chartDiv.style.backgroundColor = 'lightgreen';
                } else {
                    console.error("Chart container not found");
                }
            } catch (error) {
                console.error("Error updating gauge background:", error);
            }
        } else {
            console.log("Attempting to change background to default");
            try {
                const chartDiv = document.getElementById('container-t');
                if (chartDiv) {
                    chartDiv.style.backgroundColor = '';
                } else {
                    console.error("Chart container not found");
                }
            } catch (error) {
                console.error("Error updating gauge background:", error);
            }
        }

        // Rest of the function remains unchanged
        for (let phase = 1; phase <= PHASES; phase++) {
            const deliveredField = "power_delivered_l" + phase;
            const returnedField = "power_returned_l" + phase;
            let nvKW = -data[deliveredField] + data[returnedField]; // Combine delivered (negative) and returned (positive) power

            let nvA = nvKW * 1000.0 / 230.0; // estimated amps using fixed voltage

            // update view
            const element = document.getElementById(deliveredField);
            if (element) {
                element.innerHTML = currentUnit === 'Amps' ? nvA.toFixed(1) : nvKW.toFixed(1);
            }

            if (nvKW < minKW[phase]) {
                minKW[phase] = nvKW;
                const minElement = document.getElementById(`power_delivered_${phase}min`);
                if (minElement) {
                    minElement.innerHTML = currentUnit === 'Amps' ? (nvKW * 1000.0 / 230.0).toFixed(2) : nvKW.toFixed(2);
                }
            }
            if (nvKW > maxKW[phase]) {
                maxKW[phase] = nvKW;
                const maxElement = document.getElementById(`power_delivered_${phase}max`);
                if (maxElement) {
                    maxElement.innerHTML = currentUnit === 'Amps' ? (nvKW * 1000.0 / 230.0).toFixed(2) : nvKW.toFixed(2);
                }
            }

            // update gauge with actual values
            var chart = AmpG[phase];
            var point = chart.series[0].points[0];   
            var newValue = currentUnit === 'Amps' ? Math.round(nvA*1000.0 ) / 1000.0 : Math.round(nvKW * 1000.0);
            point.update(newValue);
            
            if (abs(nvA) > abs(MaxAmps[phase])) { //new record
                MaxAmps[phase] = nvA;
                point = AmpG[phase].series[1].points[0];
                point.update(currentUnit === 'Amps' ? Math.round(MaxAmps[phase] * 1000) / 1000 : Math.round(MaxAmps[phase] * 230));
            } 
            
            // trend coloring
            const headerElement = document.getElementById(deliveredField+"h");
            if (headerElement) {
                const cvKW = parseFloat(element.innerHTML) || 0;
                if(abs(cvKW - nvKW) < 0.15) {// don't highlight small changes < 150W
                    headerElement.style.background="#314b77";
                } else if( nvKW < cvKW ) {
                    headerElement.style.background="Red";
                } else {
                    headerElement.style.background="Green";
                }
            }
            PhaseAmps[phase] = nvA;
            
            maxAmps = Math.max(maxAmps, abs(nvA));
        }

        TotalAmps = PhaseAmps[1] + PhaseAmps[2] + PhaseAmps[3];
        let TotalKW = TotalAmps * 230.0 / 1000.0;

        if (TotalKW < minKW[4]) {
            minKW[4] = TotalKW;
            document.getElementById("power_delivered_tmin").innerHTML = currentUnit === 'Amps' ? (TotalKW * 1000.0 / 230.0).toFixed(2) : TotalKW.toFixed(2);
        }
        if (TotalKW > maxKW[4]) {
            maxKW[4] = TotalKW;
            document.getElementById("power_delivered_tmax").innerHTML = currentUnit === 'Amps' ? (TotalKW * 1000.0 / 230.0).toFixed(2) : TotalKW.toFixed(2);
        }
        
        document.getElementById("power_delivered_t").innerHTML = currentUnit === 'Amps' ? TotalAmps.toFixed(1) : TotalKW.toFixed(1);
        point = AmpG[4].series[0].points[0];
        point.update(currentUnit === 'Amps' ? Math.round(TotalAmps*1000.0 ) /1000.0 : Math.round(TotalKW * 1000.0));

        if (abs(TotalAmps) > abs(MaxAmps[4])){
            MaxAmps[4] = TotalAmps;
            point = AmpG[4].series[1].points[0];
            point.update(currentUnit === 'Amps' ? Math.round(TotalAmps*1000.0 ) /1000.0 : Math.round(TotalKW * 1000.0));
        }

        updateGaugeScale(currentUnit === 'Amps' ? maxAmps : maxAmps * 230);
    })
    .catch(error => {
        console.error(`Error in update function:`, error);
        // Optionally, update the UI to show that there was an error
        for (let phase = 1; phase <= PHASES; phase++) {
            const headerElement = document.getElementById(`power_delivered_l${phase}h`);
            const valueElement = document.getElementById(`power_delivered_l${phase}`);
            if (headerElement) {
                headerElement.style.background = "gray";
            }
            if (valueElement) {
                valueElement.innerHTML = "Error";
            }
        }
    });
} // end update()


//============================================================================  
AmpG[1] = Highcharts.chart('container-1', Highcharts.merge(gaugeOptions, AmpOptions));
AmpG[2] = Highcharts.chart('container-2', Highcharts.merge(gaugeOptions, AmpOptions));
AmpG[3] = Highcharts.chart('container-3', Highcharts.merge(gaugeOptions, AmpOptions));
AmpG[4] = Highcharts.chart('container-t', Highcharts.merge(gaugeOptions, {
                yAxis: {
                    min: -3*AMPS,
                    max: 3*AMPS,
                    
                },

                credits: {
                    enabled: false
                },

                series: [{
                    name: 'A',
                    data: [0],
                    dataLabels: {
                        format:
                            '<div style="text-align:center">' +
                            '<span style="font-weight:lighter;font-size:16px;font-family: Dosis">{y} A</span><br/>' +
                            '</div>'
                    },
                    dial: {            
                        rearLength: '5%'
                      
                    }
                },{
            
                    name: 'Max',
                    data: [1],
                    innerRadius:'100%',
                    radius: '105%',
                    dataLabels: {
                        enabled: false
                   }
                }]

            }));

for(i=1 ; i <= PHASES+1 ; i++)
{
    MaxAmps[i] = -1 * AMPS;
    PhaseAmps[i] = 0.0;
    minKW [i] = 99.99;
    maxKW [i] = -99.99;
}
MaxAmps[4] = -1 * PHASES * AMPS;

update(); // fill first-time data
var timer = setInterval(update, 2 * 1000); // update every n seconds
