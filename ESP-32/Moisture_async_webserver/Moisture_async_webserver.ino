/*********
 * Based on Rui Santos
 * see his complete project details at https://randomnerdtutorials.com  
 *
 * Added functionality to push the sensor value periodically to a sensor web app
 *********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
# include <HTTPClient.h>

// Replace with your network credentials
//const char* ssid = "REPLACE_WITH_YOUR_SSID";
//const char* password = "REPLACE_WITH_YOUR_PASSWORD";
#include <WSB_private.h> // set the WLAN credentials outside the source code
const char* ssid = STA_SSID;
const char* password = STA_PASS;

// Assign input variables to GPIO pins
const int analogIn = 36;  // ADC1-0

// ADC parameters
const float adc_ref = 3.33;
const int   adc_range = 4095;

// Sensor Server
const char* serverName = "http://192.168.178.100/sensor_values.php";  // pizero.wlan.local
String token = "35c17e3e-88fe-4978-8930-080264f8c615";
String sensorName = "Moisture-ESP";
String sensorLocation  = "Work Room";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readMoistureLevel() {
  int inputRawValue = analogRead(analogIn);
  float inputValue = inputRawValue * (adc_ref / adc_range);
  String moistureString = String(inputValue, 2);
    
  // Check if any reads failed and exit early (to try again).
//  if (isnan(moistureString)) {    
//    Serial.println("Failed to read analog value");
//    return "--";
//  }
//  else {
    Serial.println("Moisture String: " + moistureString);
    return moistureString;
//  }
}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="moist-labels">Moisture</span> 
    <span id="moisture">%MOISTURE%</span>
    <sup class="units">Volts</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("moisture").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/moisture", true);
  xhttp.send();
}, 10000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "MOISTURE"){
    return readMoistureLevel();
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("** Moisture_async_webserver **");
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/moisture", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readMoistureLevel().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  Serial.println("Pushing actual value to Server...");
  int httpResponseCode; 
  String httpResponsePayload;
  
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization","Bearer " + token);
    
    // Prepare your HTTP POST request data
    String httpRequestData = "sname=" + sensorName
                          + "&slocation=" + sensorLocation + "&svalue=" + readMoistureLevel()
                          + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    httpResponsePayload = http.getString();
    
    delay(10); // to calm down the HTTP client and allow serial prints
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    Serial.print("POST request returned:");
    Serial.println(httpResponsePayload);

    // Free resources
    http.end();
    delay(10);

  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  if (httpResponseCode == 200) {
    Serial.println("HTTP Request successful - sleeping for 30 min.");
    delay(1800*1000);  // 30 min
  }
  else {
    Serial.println("HTTP error received - sleeping for 6 hours");
    delay(6*3600*1000);  // in hours
  }
}
