const APIGW = window.location.protocol + '//' + window.location.host + '/api/';
const PHASES = 3;
const UPDATE_INTERVAL = 5000; // 5 seconds
const MAX_POWER = 3700; // Maximum power in Watts
const MAX_CURRENT = 16; // Maximum current in Amperes

let pwrBars = [];
let currentUnit = 'Amps';
let refreshTimer = UPDATE_INTERVAL
let refreshInterval; // Store the interval ID globally
let maxNetPower = 0;
let barLength = 0; // Variable to store the calculated bar length
let containerPadding = 20; // Padding on each side of the bar container
const OFFSET_PX = -20; // Offset to shift bars slightly to the left (in pixels)

/******************************************************************************************
 * Creates an object representing a power bar element.
 * @param {string} id The id of the HTML element that contains the power bar
 * @returns {Object} An object with the following properties:
 *                  - element: The HTML element
 *                  - bar: The HTML element for the bar
 *                  - value: The HTML element for the value
 *                  - scale: The HTML element for the scale
 */
function createPwrBar(id) 
{
    return {
        element: document.getElementById(id),
        bar: document.querySelector(`#${id} .bar`),
        value: document.querySelector(`#${id} .value`),
        scale: document.querySelector(`#${id} .scale`)
    };
}

/******************************************************************************************
 * Initializes the power bars.
 * This function is called once at startup.
 * It will:
 * - Create a power bar object for each phase and the total
 * - Calculate the length of the bar
 * - Set up the scales
 * @return {void}
 */
function initPwrBars() 
{
    for (let i = 1; i <= PHASES; i++) 
    {
        pwrBars.push(createPwrBar(`pwrbar-${i}`));
    }
    pwrBars.push(createPwrBar('pwrbar-total'));
    calculateBarLength();
    updateScales();
}

/******************************************************************************************
 * Calculates the length of the bar element.
 * This function is called only once at startup, and uses the first power bar element as a reference.
 * It will calculate the length of the bar as the full width of the container minus the sum of the left and right padding.
 * @returns {void}
 */
function calculateBarLength() 
{
    if (pwrBars.length > 0) 
    {
        const containerWidth = pwrBars[0].element.querySelector('.bar-container').offsetWidth;
        barLength = containerWidth - (2 * containerPadding); // Full width minus padding on both sides
        console.log(`Bar length calculated: ${barLength}px`);
    }
}

/******************************************************************************************
 * Determines the maximum scale for a given power value, depending on the current unit (Amps or Watts).
 * The returned value is the maximum scale for the given unit. The values are determined as follows:
 * - For Amps: values below 1 are scaled to 1, values between 1 and 2 are scaled to 2, values between 2 and 5 are scaled to 5, values between 5 and 10 are scaled to 10, and values above 10 are scaled to 16.
 * - For Watts: values below 250 are scaled to 250, values between 250 and 500 are scaled to 500, values between 500 and 1200 are scaled to 1200, values between 1200 and 2500 are scaled to 2500, and values above 2500 are scaled to 3700.
 * @param {number} maxValue The maximum power value
 * @param {boolean} isAmpere Whether the maximum power value is in Amps (true) or Watts (false)
 * @returns {number} The maximum scale for the given power value
 */
function determineMaxScale(maxValue, isAmpere) 
{
  if (isAmpere) 
  {
    //console.log("Determining max scale for Ampere ["+maxValue+"]");
    if (Math.abs(maxValue) < 1) return 1;
    if (Math.abs(maxValue) >= 1 && Math.abs(maxValue) < 2) return 2;
    if (Math.abs(maxValue) >= 2 && Math.abs(maxValue) < 5) return 5;
    if (Math.abs(maxValue) >= 5 && Math.abs(maxValue) < 10) return 10;
    return 16;
  } 
  else 
  {
    //console.log("Determining max scale for Watt ["+maxValue+"]");
    if (Math.abs(maxValue) < 250) return 250;
    if (Math.abs(maxValue) >= 250 && Math.abs(maxValue) < 500) return 500;
    if (Math.abs(maxValue) >= 500 && Math.abs(maxValue) < 1200) return 1200;
    if (Math.abs(maxValue) >= 1200 && Math.abs(maxValue) < 2500) return 2500;
    return 3700;
  }
}

/******************************************************************************************
 * Updates the scales for all power bars by recalculating the maximum scale and regenerating the scale HTML.
 * The scales are updated based on the current unit and the maximum net power value.
 */
function updateScales() 
{
  const isAmpere = currentUnit === 'Amps';
  const maxScale = determineMaxScale(maxNetPower, isAmpere);
  const min = -maxScale;

  //console.log(`Updating scales. Current unit: ${currentUnit}, Min value: ${min}, Max value: ${maxScale}`);
  
  pwrBars.forEach((pwrbar, index) => 
  {
      let scaleHTML = '';
      
      // Generate 11 ticks for the scale (-maxScale to +maxScale)
      for (let i = 0; i <= 10; i++) 
      {
          const tickValue = min + (i / 10) * (maxScale - min); // Value of the tick
          const percentage = ((tickValue - min) / (maxScale - min)) * 100; // Position in percentage
          const pixelPosition = (percentage / 100) * barLength; // Convert percentage to pixels
          
          const isMajor = i === 0 || i === 5 || i === 10; // Major ticks at 0%, 50%, 100%
          
          // Only show labels for major ticks
          let label = '';
          if (isMajor) 
          {
              label = tickValue.toFixed(1); // Label shows the value of the tick (one decimal)
          }

          // Build the tick HTML
          scaleHTML += `
              <div class="scale-tick ${isMajor ? 'major' : 'minor'}" style="left: ${pixelPosition}px;">
                  ${isMajor ? `<span class="scale-label">${label}</span>` : ''}
              </div>
          `;
      }
      
      pwrbar.scale.innerHTML = scaleHTML;
      //console.log(`Scale HTML for pwrbar ${index + 1}:`, scaleHTML);
  });
}

/******************************************************************************************
 * Updates a power bar with the given value.
 * The power bar is updated based on the absolute value of the given power value.
 * The width of the bar is calculated based on the maximum scale for the current unit (Amps or Watts).
 * The position and style of the bar are adjusted accordingly.
 * The displayed value is updated with the given power value.
 * @param {object} pwrbar The power bar object to be updated
 * @param {number} value The power value to be displayed
 * @return {void}
 */
function updatePwrBar(pwrbar, value) 
{
  const isAmpere = currentUnit === 'Amps';
  const maxScale = determineMaxScale(maxNetPower, isAmpere);
  
  // Calculate the width of the bar based on absolute value
  const width = Math.abs(value) / maxScale * 50; // Maximum width is 50% for either -maxScale or +maxScale
  
  // Calculate the starting point (left edge) of the bar
  let startPoint;
  if (value >= 0) 
  {
    startPoint = 50; // Start from the center for positive values
  } 
  else 
  {
    startPoint = 50 - width; // Start left of center for negative values
  }
  
  // Adjust the bar's position and style
  pwrbar.bar.style.width = `${width}%`;
  pwrbar.bar.style.left = `calc(${startPoint}% + ${containerPadding + OFFSET_PX}px)`;
  pwrbar.bar.style.backgroundColor = value < 0 ? 'red' : 'green'; // Red for negative, green for positive
  
  // Update the displayed value
  pwrbar.value.textContent = `${value.toFixed(1)} ${isAmpere ? 'Ampères' : 'Watt'}`;
  
  //console.log(`Updating pwrbar: value = ${value}, width = ${width}%, startPoint = ${startPoint}%, left = ${pwrbar.bar.style.left}`);
}

/******************************************************************************************
 * Fetches data from the API, updates the power bars and scales accordingly.
 * The API endpoint is /api/v2/sm/actual.
 * The data is processed and each phase is updated with the net power (delivered - returned).
 * The total power is also calculated and displayed.
 * The maximum net power is used to update the scale of the power bars.
 * @return {void}
 */
function fetchData() 
{
  console.log('Fetching data from api/v2/sm/actual');
  fetch(`${APIGW}v2/sm/actual`)
      .then(response => response.json())
      .then(json => {
          console.log('Received data:', json);
          const data = json.actual;
          let maxPower = 0;
          for (let i = 1; i <= PHASES; i++) 
          {
              const delivered = data[`power_delivered_l${i}`] || 0;
              const returned = data[`power_returned_l${i}`] || 0;
              const netPower = returned - delivered; // Negative for delivered, positive for returned
              const displayValue = currentUnit === 'Amps' ? (netPower * 1000 / 230) : (netPower * 1000);
              maxPower = Math.max(maxPower, Math.abs(displayValue));
              //console.log(`Phase ${i}: delivered = ${delivered}, returned = ${returned}, netPower = ${netPower}, displayValue = ${displayValue}`);
              updatePwrBar(pwrBars[i - 1], displayValue);
          }
          
          const totalDelivered = data.power_delivered || 0;
          const totalReturned = data.power_returned || 0;
          const totalNetPower = totalReturned - totalDelivered;
          const totalDisplayValue = currentUnit === 'Amps' ? (totalNetPower * 1000 / 230) : (totalNetPower * 1000);
          maxPower = Math.max(maxPower, Math.abs(totalDisplayValue));
          //console.log(`Total: delivered = ${totalDelivered}, returned = ${totalReturned}, netPower = ${totalNetPower}, displayValue = ${totalDisplayValue}`);
          updatePwrBar(pwrBars[3], totalDisplayValue);

          maxNetPower = maxPower;
          updateScales();
      })
      .catch(error => {
          console.error('Error fetching data:', error);
      });
}

/******************************************************************************************
 * Fetches the device information from the API and updates the page with the
 * hostname and firmware version.
 * @return {Promise<void>} A promise that resolves when the device information
 * has been updated.
 */
function refreshDevInfo() 
{
    fetch(`${APIGW}v2/dev/info`)
        .then(response => response.json())
        .then(json => {
            const data = json.devinfo;
            if (data.hostname) 
            {
                document.getElementById('devName').textContent = data.hostname;
            }
            if (data.fwversion) 
            {
                document.getElementById('devVersion').textContent = data.fwversion;
            }
        })
        .catch(error => console.error('Error fetching device info:', error));
}

/******************************************************************************************
 * Fetches the current device time and updates the #theTime element.
 * This function is called once at startup and every 10 seconds after that.
 * @return {Promise<void>} A promise that resolves when the time has been updated.
 */
function refreshDevTime() 
{
    fetch(`${APIGW}v2/dev/time`)
        .then(response => response.json())
        .then(json => {
            if (json.devtime && json.devtime.time) 
            {
                document.getElementById('theTime').textContent = json.devtime.time;
            }
        })
        .catch(error => console.error('Error fetching device time:', error));
}

/******************************************************************************************
 * Initializes the pwrBars module.
 * This function is called once at startup.
 * It will:
 * - Initialize the power bars
 * - Set up event listeners for the return button, unit selection and refresh time selection
 * - Fetch the initial data
 * - Set up a timer to fetch new data every 10 seconds
 * - Set up a timer to fetch the current device time every 10 seconds
 * @return {void}
 */
function init() 
{
    console.log('Initializing pwrBars');
    initPwrBars();
    document.getElementById('bReturn').addEventListener('click', () => {
        location.href = "/DSMRindex.html";
    });
    
    document.querySelectorAll('input[name="displayUnit"]').forEach((elem) => {
        elem.addEventListener("change", function(event) {
            fetchData();
            console.log(`Changing unit to ${event.target.value}`);
            currentUnit = event.target.value;
            updateScales();
            fetchData();
        });
    });
    document.querySelectorAll('input[name="refreshTime"]').forEach((elem) => {
      elem.addEventListener("change", function(event) {
          console.log(`Changing refreshTime to ${event.target.value}`);
          refreshTimer = event.target.value;
          clearInterval(refreshInterval); 
          refreshInterval = setInterval(fetchData, refreshTimer);
      });
    });
  
    refreshDevInfo();
    refreshDevTime();
    
    fetchData();
    console.log(`refreshTimer = ${refreshTimer}`);
    refreshInterval = setInterval(fetchData, refreshTimer);
    setInterval(refreshDevTime, 10000); // Update time every 10 seconds
}

window.onload = init;