//For WiFi
#include <WiFi.h>

//For API request
#include <HTTPClient.h>

//For response format 
#include <ArduinoJson.h>

//Pini segmente
#define A 4
#define B 16
#define C 17
#define D 5
#define E 18  //D0 too dim
#define F 12
#define G 13

#define D1 15
#define D2 2

char characters[36][7] = 
{
	{ 1, 1, 1, 0, 1, 1, 1 },  //A
	{ 0, 0, 1, 1, 1, 1, 1 },  //b
	{ 1, 0, 0, 1, 1, 1, 0 },  //C
	{ 0, 1, 1, 1, 1, 0, 1 },  //d
	{ 1, 0, 0, 1, 1, 1, 1 },  //E
	{ 1, 0, 0, 0, 1, 1, 1 },  //F
	{ 1, 0, 1, 1, 1, 1, 0 },  //G
	{ 0, 0, 1, 0, 1, 1, 1 },  //h
	{ 0, 0, 0, 0, 1, 1, 0 },  //I
	{ 0, 1, 1, 1, 0, 0, 0 },  //J
	{ 1, 0, 1, 0, 1, 1, 1 },  //k
	{ 0, 0, 0, 1, 1, 1, 0 },  //L
	{ 1, 0, 1, 0, 1, 0, 1 },  //M
	{ 0, 0, 1, 0, 1, 0, 1 },  //n
	{ 0, 0, 1, 1, 1, 0, 1 },  //o
	{ 1, 1, 0, 0, 1, 1, 1 },  //P
	{ 1, 1, 1, 0, 0, 1, 1 },  //q
	{ 0, 0, 0, 0, 1, 0, 1 },  //r
	{ 1, 0, 1, 1, 0, 1, 0 },  //S
	{ 0, 0, 0, 1, 1, 1, 1 },  //t
	{ 0, 1, 1, 1, 1, 1, 0 },  //U
	{ 0, 0, 1, 1, 1, 0, 0 },  //v
	{ 0, 1, 0, 1, 0, 1, 1 },  //w
	{ 0, 1, 1, 0, 1, 1, 1 },  //X
	{ 0, 1, 1, 1, 0, 1, 1 },  //y
	{ 1, 1, 0, 1, 1, 0, 0 },  //Z

	{ 1, 1, 1, 1, 1, 1, 0 }, //0
	{ 0, 1, 1, 0, 0, 0, 0 }, //1
	{ 1, 1, 0, 1, 1, 0, 1 }, //2
	{ 1, 1, 1, 1, 0, 0, 1 }, //3
	{ 0, 1, 1, 0, 0, 1, 1 }, //4
	{ 1, 0, 1, 1, 0, 1, 1 }, //5
	{ 1, 0, 1, 1, 1, 1, 1 }, //6
	{ 1, 1, 1, 0, 0, 0, 0 }, //7
	{ 1, 1, 1, 1, 1, 1, 1 }, //8
	{ 1, 1, 1, 1, 0, 1, 1 }  //9
};

int mapping[7] = 
{
  A,
  B,
  C,
  D,
  E,
  F,
  G
};

void setup() 
{

    //Begin serial
    Serial.begin(115200);

    //Clear serial
    for (int i = 0; i < 50; i++)
      Serial.println();

    
    Serial.println("Initializing WiFi...");
    WiFi.mode(WIFI_STA);
    Serial.println("Setup done!");

    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);
    pinMode(E, OUTPUT);
    pinMode(F, OUTPUT);
    pinMode(G, OUTPUT);

    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);



}

//c = [A-Z] or [a-z] or [0-9]
//slot = D1 or D2
void printLetter(char c, unsigned char slot) 
{
	
    //reset cathodes
	digitalWrite(D1, HIGH);
	digitalWrite(D2, HIGH);

	//reset segments
	for (int i = 0; i < 7; i++)
		digitalWrite(mapping[i], LOW);

	//if this no display
	if (c == ' ' || c == '-'|| c == ':')
		return;


	
	//set slot
	digitalWrite(slot, LOW);

	//convert to lowercase if character is uppercase
	if (c <= 'Z' && c >= 'A')
		c = c + 32;

	//get index in characters array
	int idx = c - 'a';

	if (c >= '0' && c <= '9') //number
		idx = 26 + c - '0';


	for (int i = 0; i < 7; i++) 
	{
		if (characters[idx][i] == 1)
			digitalWrite(mapping[i], HIGH);
	}
}



#define TIME_MULTIPLEX 5   //ms
#define TIME_GROUP 330     //ms
#define TIME_FIELD 1000	   //ms

#define CONNECTION_TIMEOUT 10000 //10s

void loop() {

	//Connection phase
	static bool found = false;
  	static String networkName;
  	static String networkPass = "";
	static bool connecting = false;
  	static bool connected = false;

	//API phase
	static int id = -1;
	static DynamicJsonDocument jsonDoc(2048);
	static char c1[] = "Species";
	static char c2[] = "Age";
	static char c3[] = "Planet";
	static char s[] = "Profession";
	static char idStr[] = "Id";

	//Display phase
	static bool display = false;
	static unsigned char current = 1;
	static String str;
	static char window[2] = {' ', ' '};

	static size_t timeM = millis();
	static size_t timeG = millis();
	static size_t idx = 0;
	static unsigned int digit = D1;

	static char serialChars[] = ".-*";

	//Fallback
	static String fallbackJSON = "{\"Species\":\"Robot\",\"Age\":\"4-10\",\"Planet\":\"Earth\",\"Profession\":\"Formerly: Girder-bender, Wrestler, SoldierCurrently: Planet Express Worker\",\"Id\": 3}";
	static bool fallback = false;

	if (!fallback && !found)
	{
		Serial.println("Scanning...");

		// WiFi.scanNetworks will return the number of networks found
		int n = WiFi.scanNetworks();
		Serial.println("Scan done!");
		if (n == 0) 
		{
			Serial.println("No networks found.");
		} else 
		{
			Serial.println();
			Serial.print(n);
			Serial.println(" networks found");
			found = true;
			for (int i = 0; i < n; ++i) 
			{
				// Print SSID and RSSI for each network found
				Serial.print(i);
				Serial.print(": ");
				Serial.print(WiFi.SSID(i));
				Serial.print(" (");
				Serial.print(WiFi.RSSI(i));
				Serial.print(")");
				Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
				delay(10);
			}
			
			Serial.println("Choose network to connect to");
			Serial.println();

			while (Serial.available() == 0);
			int opt = Serial.parseInt();

			if (opt == -1) //use fallback
			{
				Serial.println("Fallback option activated!");
				Serial.println();
				fallback = true;
				return;
			}

			networkName = WiFi.SSID(opt);
			Serial.print("Network chosen: ");
			Serial.print(networkName);
			Serial.print(" (");
			Serial.print(WiFi.RSSI(opt));
			Serial.println(")");
			bool networkOpen = WiFi.encryptionType(opt) == WIFI_AUTH_OPEN;
			if (!networkOpen)
			{
				Serial.println("Selected network not open!");
				Serial.print("Enter password for ");
				Serial.print(networkName);

				while (Serial.available() == 0);
				networkPass = Serial.readString();
			}

			connecting = true;
			WiFi.begin(networkName.c_str(), networkPass.c_str());
			Serial.print("Connecting to: ");
			Serial.println(networkName);
			Serial.println("");

			size_t connectionStart = millis();
			while(WiFi.status() != WL_CONNECTED)
			{
				Serial.print("#");
				delay(500);
				if (millis() - connectionStart >= CONNECTION_TIMEOUT)
					break;
			}
			Serial.println("");
		}
	}


	if (!fallback && connecting && !connected)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			Serial.println("Connection failed! Try again.");
			found = false;
			connecting = false;
		} else
		{
			Serial.println("Connection established!");

			connected = true;

			Serial.print("Connected to network: ");
			Serial.println(WiFi.SSID());
			Serial.print("Local IP address: ");
			Serial.println(WiFi.localIP()); 
			Serial.println();

		}
	}
	
	if ((connected || fallback) && id == -1)
	{
		Serial.println(fallback ? "Send anything to start fallback display" : "Input desired character id");

		while (Serial.available() == 0);

		if (!fallback)
		{
			id = Serial.parseInt();
		} else
		{
			while (Serial.available() > 0)  //Flush buffer
				Serial.read();
			id = 3; //Fallback data is from id 3
		}
			
		Serial.print("Character id: ");
		Serial.println(id);
		Serial.println();

		Serial.print(fallback ? "Fallback to data from " : "Sending API request to ");
		String url = "http://proiectia.bogdanflorea.ro/api/futurama/character?Id=" + String(id);

		Serial.print(url);
		Serial.println(" ...");

		//Begin HTTP request
		HTTPClient http;
		http.begin(url);
		http.setConnectTimeout(30000);
		http.setTimeout(30000); 

		int code = fallback ? 200 : http.GET();

		if (code == 200)
		{
			Serial.println("200 - request successful");


			DeserializationError error = deserializeJson(jsonDoc, fallback ? fallbackJSON : http.getString());
			if (error) 
			{
				Serial.print("JSON deserialization failed: ");
				Serial.println(error.c_str());
			} else
			{
				display = true;
				str = jsonDoc[c1].as<String>() + "  "; //Trailing spaces
				String serial;
				serializeJsonPretty(jsonDoc, serial);

				Serial.println(serial);
				Serial.println();
				Serial.print("Id: ");
				Serial.println(jsonDoc[idStr].as<String>());
				Serial.print("Ocupatie: ");
				Serial.println(jsonDoc[s].as<String>());
				Serial.println();
			}
		} else
		{
			Serial.print("Request returned code: ");
			Serial.println(code);
		}

		http.end();
	}

	if (display)
	{
		
		//Multiplexing
		if (millis() - timeM >= TIME_MULTIPLEX)
		{
			if (digit == D1)
			{
				digit = D2;
				printLetter(window[1], digit);
			}
			else
			{
				digit = D1;
				printLetter(window[0], digit);
			}

			timeM = millis();
		}

		//Letter Grouping
		if (millis() - timeG >= TIME_GROUP)
		{
			window[0] = window[1];

			//Slide window,
			if (idx < str.length()) 
			{
				window[1] = str[idx];
				if (idx < str.length() - 2) //Account for trailing spaces
				Serial.print(serialChars[current - 1]);
			}
			else
			{
				current++;
				if (current == 2)
					str = jsonDoc[c2].as<String>() + "  "; //Trailing spaces
				else if (current == 3)
					str = jsonDoc[c3].as<String>() + "  "; //Trailing spaces
				else if (current > 3) //Restart character selection:
				{
					id = -1;
					current = 1;
					display = false;
					Serial.println();
					Serial.println();
					Serial.println();

				}
				idx = -1;
				Serial.println();
				
				delay(TIME_FIELD);

			}

			idx++;
			timeG = millis();
		}
	}

	// Wait a bit before scanning again
	if (!found)
		delay(5000);
}