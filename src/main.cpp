#include <Arduino.h>
#include <Notecard.h>
#include <NotecardPseudoSensor.h>
int getSensorInterval();


#define usbSerial Serial
#define productUID "com.gmail.miguelrmza98:rhota"

Notecard notecard;
using namespace blues;
NotecardPseudoSensor sensor(notecard);



//Test swan conectado con la Notecard
//con este codigo conecte la notecard con el swan al notehub y mande datos, tambien conecte el notehub con la note card para mandar variables
//lo que no entendi fue lo del inbound le puse cada 5 minutos pero se actualiza como cada 30s eso no le entendi


void setup()
{
  delay(2500);
  usbSerial.begin(115200);

  notecard.begin();
  notecard.setDebugOutputStream(usbSerial);

  
  J *req = notecard.newRequest("hub.set");
  if (req != NULL) {
    JAddStringToObject(req, "product", productUID);
    JAddStringToObject(req, "mode", "continuous");
    JAddNumberToObject(req, "inbound", 5); // Revisa cada 5 munutos (o no, cambio)
    //JAddBoolToObject(req, "sync", true);
    notecard.sendRequest(req);
  
}

}

// the loop function runs over and over again forever
void loop()
{
float temperature = sensor.temp();
float humidity = sensor.humidity();

usbSerial.print("Temperature = ");
usbSerial.print(temperature);
usbSerial.println(" *C");
usbSerial.print("Humidity = ");
usbSerial.print(humidity);
usbSerial.println(" %");


{
  J *req = notecard.newRequest("note.add");
  if (req != NULL) {
    JAddStringToObject(req, "file", "sensors.qo");
    JAddBoolToObject(req, "sync", true);
    J *body = JAddObjectToObject(req, "body");
    if (body) {
      JAddNumberToObject(body, "temp", temperature);
      JAddNumberToObject(body, "humidity", humidity);
    }
    notecard.sendRequest(req);
  }
}

int sensorIntervalSeconds = getSensorInterval();
usbSerial.print("Delaying ");
usbSerial.print(sensorIntervalSeconds);
usbSerial.println(" seconds");
delay(sensorIntervalSeconds * 1000);
}


// This function assumes you’ll set the reading_interval environment variable to
// a positive integer. If the variable is not set, set to 0, or set to an invalid
// type, this function returns a default value of 60.
int getSensorInterval() {
  int sensorIntervalSeconds = 60;
  J *req = notecard.newRequest("env.get");
  if (req != NULL) {
    JAddStringToObject(req, "name", "reading_interval");
    J* rsp = notecard.requestAndResponse(req);
    int readingIntervalEnvVar = atoi(JGetString(rsp, "text"));
    if (readingIntervalEnvVar > 0) {
      sensorIntervalSeconds = readingIntervalEnvVar;
    }
    notecard.deleteResponse(rsp);
  }
  return sensorIntervalSeconds;
}