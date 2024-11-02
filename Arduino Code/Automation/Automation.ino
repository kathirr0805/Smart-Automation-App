#include <ESP8266WiFi.h>

// Wi-Fi credentials
const char* ssid = "Kathir WIFI";  // Your Wi-Fi SSID
const char* password = "kathir2005";  // Your Wi-Fi password

WiFiServer server(80);  // Start a web server on port 80

// Relay pin definitions
const int relayPins[10] = { D1, D2, D3, D4, D5, D6, D7, D8, D9, D10 }; // Change pin numbers according to your setup
bool relayStates[10] = { false, false, false, false, false, false, false, false, false, false }; // Initial states

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  WiFi.begin(ssid, password);

  // Wait for the NodeMCU to connect to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();  // Start the server

  // Set all relays to LOW (OFF) initially
  for (int i = 0; i < 10; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW); // Ensure all relays are off
  }
}
void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {
    Serial.println("New client connected");
    String request = client.readStringUntil('\r');  // Read the request
    Serial.println(request);
    client.flush();

    // Check if the client requests to toggle a relay
    for (int i = 0; i < 10; i++) {
      if (request.indexOf("GET /toggle" + String(i)) != -1) {
        relayStates[i] = !relayStates[i]; // Toggle state
        digitalWrite(relayPins[i], relayStates[i] ? HIGH : LOW); // Set relay state
      }
    }

    // Respond to the client with HTML and CSS
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<title>Smart Home Control</title>");
    
    // CSS for dark theme, glassmorphism, and responsive design
    client.println("<style>");
    client.println("body { margin: 0; padding: 0; font-family: Arial, sans-serif; background: url('https://i.pinimg.com/736x/5f/97/14/5f9714029f359ffed22a796513d17e38.jpg') no-repeat center center fixed; background-size: cover; display: flex; justify-content: center; align-items: center; height: 100vh; color: white; }");
    client.println(".glass { background: rgba(255, 255, 255, 0.1); border-radius: 15px; backdrop-filter: blur(15px); -webkit-backdrop-filter: blur(15px); box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3); padding: 30px; text-align: center; }");
    client.println(".container { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-top: 20px; }");
    client.println(".relay-button { width: 100%; padding: 15px; border: none; border-radius: 10px; font-size: 18px; cursor: pointer; transition: transform 0.3s ease; }");
    client.println(".relay-button.on { background-color: #4caf50; color: white; }");
    client.println(".relay-button.off { background-color: #f44336; color: white; }");
    client.println(".relay-button:hover { transform: scale(1.05); }");
    client.println(".title { font-size: 36px; font-weight: bold; margin-bottom: 20px; }");
    client.println("</style>");

    client.println("</head>");
    client.println("<body>");
    client.println("<div class='glass'>");
    client.println("<h1 class='title'>Smart Home Control</h1>");
    client.println("<div class='container'>");

    // Array of home appliances
    String appliances[10] = { "TV", "Refrigerator", "Washing Machine", "AC", "Hall Fan", "Hall Light", "Room Fan", "Room Light", "Bathroom Path Light", "One Connection (Mobile Charger)" };

    // Loop to create the relay buttons with appliance names
    for (int i = 0; i < 10; i++) {
      String buttonClass = relayStates[i] ? "on" : "off"; // Set class based on relay state (ON or OFF)
      client.println("<button class='relay-button " + buttonClass + "' onclick=\"location.href='/toggle" + String(i) + "'\">" + appliances[i] + "</button>");
    }

    client.println("</div>");
    client.println("</div>");
    client.println("</body>");
    client.println("</html>");
    
    client.stop();  // Close the connection
    Serial.println("Client disconnected");
  }
}
