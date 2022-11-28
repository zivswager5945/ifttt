#include <ArduinoJson.h>
#include <WiFi.h>

const char* ssid = "皓的iPhone";  // insert your SSID
const char* password = "54311111";  // insert your password
  char jsonRead[600]; //讀取response後儲存JSON資料的變數，必須是全域變數
//const char* host = "api.openweathermap.org";
//const char* resource = "/data/2.5/weather?id=1668341&appid=45520da7ba4809b09ee3a81f1b9f10f5";
//char jsonRead[600]; //讀取response後儲存JSON資料的變數，必須是全域變數

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop() {

  //以char*格式儲存回傳的json
  char* json = ClientRequesta();

  Serial.println(json);

  //將字串轉換為JSON，儲存在doc中
  StaticJsonDocument<600> doc;
  deserializeJson(doc, json);

  const char* location = doc["name"];
  const String weather = doc["weather"][0]["description"];
  double temp = doc["main"]["temp"];
  double humidity = doc["main"]["humidity"];

  

  Serial.print("*** ");
  Serial.print(location);
  Serial.println(" ***");
  Serial.print("Type: ");
  Serial.println(weather);
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Serial.println("--------------------");
  Serial.println(weather.length());
  String weatherfix="";
  
for(int i=0; i<sizeof(weather); i++){
       if(weather[i]!=' ') weatherfix+=weather[i];
       else if(weather[i]==' ')weatherfix+="%20";  //將空白轉換成"%20"，為URL中的空白字符
}
  ClientRequest(String(weatherfix), String(temp-273.0), String(humidity));  //Client傳送資料
  delay(3000); // the OWM free plan API does NOT allow more then 60 calls per minute
}

void ClientRequest(String value1, String value2, String value3) {
  const char* host = "maker.ifttt.com"; //IFTTT server網址
  const char* event = "dinner";  //IFTTT事件名稱
  const char* apiKey = "WRDtW8j2LpYY369gtChBNprFbTIr6d5BZMnf-eiLtV";  //IFTTT Applet key
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(host, httpPort)) {

    //Webhook API
    String url = "/trigger/" + String(event) + "/with/key/" + String(apiKey);
    //Query String
    url += "?value1=" + value1 + "&value2=" + value2 + "&value3=" + value3;

    //Client傳送
    client.println(String("POST ") + url + " HTTP/1.1");
    client.println(String("Host: ") + host);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料
    int timeout = 0;
    while (!client.available() && (timeout++ <= 50)) {
      delay(100);
    }

    //如果無回應
    if (!client.available()) {
      Serial.println("No response...");
    }
    //用while迴圈一字一字讀取Response
    while (client.available()) {
      Serial.write(client.read());
    }

    //停止Client
    client.stop();
  }
}
char* ClientRequesta() {
  const char* host = "api.openweathermap.org";
  const char* resource = "/data/2.5/weather?id=1668341&appid=45520da7ba4809b09ee3a81f1b9f10f5";

  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(host, httpPort)) {

    //Client傳送
    client.println(String("POST ") + resource + " HTTP/1.1");
    client.println(String("Host: ") + host);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料
    int timeout = 0;
    while (!client.available() && (timeout++ <= 50)) {
      delay(100);
    }

    //如果無回應
    if (!client.available()) {
      Serial.println("No response...");
    }

    //Checking for the end of HTTP stream
    while (!client.find("\r\n\r\n")) {
      // wait for finishing header stream reading ...
    }

    //讀取資料並儲存在jsonRead中
    client.readBytes(jsonRead, 600);

    //停止Client
    client.stop();
    //回傳
    return jsonRead;
  }
}
