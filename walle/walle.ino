#define INTERVAL_CM   17000  //定义传感器采样时间间隔  597000
#define INTERVAL_NET      17000  //定义发送时间

//传感器
#include <Wire.h>
#include "./ESP8266.h" //wifi模块 需要esp8266头文件
#include "I2Cdev.h" //通信库

//wifi
#define SSID "yekongomega" //cannot be longer than 32 characters!
#define PASSWORD "66343300"

#define IDLE_TIMEOUT_MS  3000      
// Amount of time to wait (in milliseconds) with no data 
// received before closing the connection.  If you know the server
// you're accessing is quick to respond, you can reduce this value.

//api上传
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "20493222"
#define PROJECTID "108071"
#define HOST_PORT   (80)
String apiKey="IqsdqcPQRPnJhnk6VLHlQhTuFFE=";
char buf[10];

#define INTERVAL_cm 2000
unsigned long cmlastTime = millis();

String mCottenData;
String jsonToSend; //用于存储发送的json格式参数
String postString; //用于存储发送数据的字符串

#define TrigPinRen 4  //定义超声波引脚
#define EchoPinRen 5
#define TrigPinTong 6
#define EchoPinTong 7
float cr,ct;
float ccr,cct;
char cr_c[7],ct_c[7];
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(mySerial); //定义一个ESP8266（wifi）的对象
unsigned long net_time1 = millis(); //数据上传服务器时间
unsigned long cm_time = millis();   
int k=0; //判断参数

#define servo_pin1 SDA
#define servo_pin2 SDA
#include <Servo.h>
Servo myservo1;
Servo myservo2;
int pose=180;
bool up=true;

//设置串口
void setup() {
  Serial.begin(115200);//串口初始化
  myservo1.attach(servo_pin1);
  myservo2.attach(servo_pin2);
  Serial.begin(115200);//超声波 115200
  pinMode(TrigPinRen, OUTPUT);
  pinMode(EchoPinRen, INPUT);
  pinMode(TrigPinTong, OUTPUT);
  pinMode(EchoPinTong, INPUT);

  //ESP8266初始化
  Serial.print("setup begin\r\n");   
  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());
  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
    } else {
    Serial.print("to station + softap err\r\n");
    }

  if (wifi.joinAP(SSID, PASSWORD)) {      //加入无线网
    Serial.print("Join AP success\r\n");  
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
    } else {
    Serial.print("single err\r\n");
    }
  Serial.print("setup end\r\n");
 }

//人超声波测距
float cmr(){
  digitalWrite(TrigPinRen, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPinRen, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPinRen, LOW);
  // 检测脉冲宽度，并计算出距离
  cr=pulseIn(EchoPinRen, HIGH)/58.0;
  //通过串口监视器输出
  Serial.print("Ren:"); Serial.println(cr);
  return cr;
}

//桶超声波测距
float cmt(){
  digitalWrite(TrigPinTong, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPinTong, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPinTong, LOW);
  // 检测脉冲宽度，并计算出距离
  ct=pulseIn(EchoPinTong, HIGH)/58.0;
  //通过串口监视器输出
  Serial.print("        Tong:"); Serial.println(ct);
  return ct;
}

//是否启动机械臂判断
int push(float cr,float ct){
  if ((cr>50)&&(ct<15))  //没人
  {
      return 1;
  }
  else{
    return 0;
  }
}

//主函数
void loop() {
  if (cm_time > millis()) cm_time = millis();
  if(millis() - cm_time > INTERVAL_CM){ //传感器采样时间间隔  
    getCmData(); 
    cm_time = millis();
  }  
  if (net_time1 > millis())  net_time1 = millis();
  if (millis() - net_time1 > INTERVAL_NET){ //发送数据时间间隔                
    updateSensorData(); //将数据上传到服务器的函数
    net_time1 = millis();
  }  
  if ((push(cmr(),cmt()) == 1)&&(k==0))
  {
    Serial.print("begin!\n");
    int i=0;
    do{
       if(pose==150){
        up=true;
        Serial.print(pose);
        delay(2000);
        i++;
       }
       else if(pose==30){
        up=false;
        Serial.print(pose);
        delay(2000);
       
       }
       if(up)  pose--;
       else  pose++;
       myservo1.write(pose);
       myservo2.write(pose);
    }while(i<3);
    Serial.print("over!\n");
  }
  if (push(cmr(),cmt()) == 1) {  
    k=1;                   //判断压缩后是否仍然满出
  }
  Serial.print(k);
  if ((cmr()<50)&&(cmr()>0.2)) {
     k=0;
  }
  Serial.print(k);
}

void getCmData(){
  if((cmr()<200)&&(cmt()<50)&&(cmr()>0.2)&&(cmt()>0.2)) {
    ccr=cmr();   
    cct=cmt(); 
    delay(1000);
    dtostrf(ccr, 2, 1, cr_c);
    dtostrf(cct, 2, 1, ct_c);
  }
}

void updateSensorData() {
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");

    jsonToSend="{\"Cmren\":";
    dtostrf(ccr,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Cmtong\":";
    dtostrf(cct,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+="}";



    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
}
