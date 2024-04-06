#include <OneWire.h>
#include <DallasTemperature.h>

OneWire ds(8);  // Подключение ds18b20 к ардуино через пин 8. Создаем объект OneWire для шины 1-Wire, с помощью которого будет осуществляться работа с датчиком
int wire_break_analogPin = 1;
int temperature_measure = 8; // измерение температуры
int temperature_switch = 9; // включение нагревателя
int timer = 0;
int buffer_timer = 0; // счетчик для измерения температуры
int timer_step = 5; // промежуток измерения температуры
float temperature = 0;
float temperature_max = 40; // повышенная температура нагрева и выдержки 
  
void setup()
{ 
  pinMode (temperature_measure , OUTPUT); 
  pinMode (temperature_switch, OUTPUT);
  digitalWrite(temperature_switch, HIGH); // предварительное выключение
  Serial.begin(9600);
}


void loop() 
{
  if ( wire_break () ) // Проверка "РАЗРЫВА" соединения
  {
    Serial.print ("РАЗРЫВ СОЕДИНЕНИЯ  ");
    print_time ();
    Serial.print("temperature: "); Serial.println(measure_temperature ());
    }
  timer = millis() / 1000;  
  if (timer >= buffer_timer){
    if (!wire_break ()) {Serial.print ("СОЕДИНЕНИЯ  В НОРМЕ");}
    else {Serial.print ("РАЗРЫВ СОЕДИНЕНИЯ  ");}
    print_time ();
    temperature = measure_temperature ();
    Serial.print("temperature: "); Serial.println(temperature);
    if (temperature < temperature_max) 
    { //время вкл./выкл. определяется переменной time_step
      if ( digitalRead (temperature_switch)) {digitalWrite (temperature_switch, LOW);} // вкл. нагреватель
      else {digitalWrite (temperature_switch, HIGH);} // вsкл. нагреватель
    }
    else {digitalWrite (temperature_switch, HIGH);}
    buffer_timer = buffer_timer + timer_step;
    }
  }

  
  int wire_break () 
  {
    if (analogRead (wire_break_analogPin) >= 1000) return 1;
    else return 0;
    }

  
  int measure_temperature () 
  {
    byte data[2]; 
    //измерение температуры
    ds.reset(); // Начинаем взаимодействие со сброса всех предыдущих команд и параметров
    ds.write(0xCC);  // Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
    ds.write(0x44, 1); // Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
    delay(1000); // Микросхема измеряет температуру, а мы ждем.  
    ds.reset(); // Теперь готовимся получить значение измеренной температуры
    ds.write(0xCC); 
    ds.write(0xBE); // Просим передать нам значение регистров со значением температуры
    // Получаем и считываем ответ
    data[0] = ds.read(); // Читаем младший байт значения температуры
    data[1] = ds.read(); // А теперь старший
    // Формируем итоговое значение: "склеиваем" значение, затем умножаем его на коэффициент, соответсвующий разрешающей способности (для 12 бит по умолчанию - это 0,0625)
    float temperature =  ((data[1] << 8) | data[0]) * 0.0625;
    return (int) temperature;
    }

     
  void print_time () 
  {
    int timer = 0;
    int sec = 0;
    int minute = 0;
    int hour = 0;
    timer = millis() / 1000;
    hour = timer / 3600;
    Serial.print(hour);
    Serial.print (": ");
    hour = hour * 3600;
    minute = timer - hour;
    minute = minute /60;
    Serial.print(minute);
    minute = minute * 60;
    Serial.print (": ");
    sec = timer - hour;
    sec = sec - minute;
    Serial.print(sec);
    Serial.print ("  ");
    }
  
 
