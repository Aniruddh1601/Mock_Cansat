#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // I2C Interface
#include <Servo.h>
Servo servop; Servo servoh;

int par=11;int heat=9;
float mp=A0;
String temp,alti,pres,ve;
float te,pr,rho,Pa,al,P;
// parameters for averaging and offset
int offset = 0;
int offset_size = 10;
int veloc_mean_size = 20;
int zero_span = 2;
int t=0,i=1;

void setup() {
  Serial.begin(9600);
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  for (int ii=0;ii<offset_size;ii++){
    offset += analogRead(A0)-(1023/2);
  }
  offset /= offset_size;
  servop.attach(par);servoh.attach(heat);
}

void alt()
{
    te=bmp.readTemperature();temp=String(te);
    pr=(bmp.readPressure()/100);pres=String(pr);
    al=bmp.readAltitude(P);alti=String(al);
}

void mpx()
{
  float adc=0.0; float vel=0.0;
   for (int ii=0;ii<veloc_mean_size;ii++){
    adc+= analogRead(mp)-offset;
  }
  adc/=veloc_mean_size;te+=273;
  rho=(pr*100)/(287.058*te);
  if (adc<512){
      vel= -sqrt((-10000.0*((adc/1023.0)-0.5))/rho);
    } 
    else{
      vel=sqrt((10000.0*((adc/1023.0)-0.5))/rho);
    }
    ve=String(vel);
}

void loop() 
{
 if(t==0)
 {
  while(Serial.available()==0);
  P=Serial.read();
  t++;
 }
 else if(t==1)
 {
  while(Serial.available()==0);
  if (Serial.read()=="START")
  t++;
 }
 else if(t==2)
 {
  alt();
  mpx(); 
  String i1=String(i);
  String sp=i1+",SP,"+alti+","+temp+","+ve;
  Serial.println(sp);
  i++;
  if(t==2)
  {
    if(al<=500)
    {
      servoh.write(45);
      servop.write(45);
      t++;
    }
  }
  if(al<=5)
  t++;
  
  delay(1000);
 }

}
