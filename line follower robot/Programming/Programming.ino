// motors pins //
int RF=9;
int RB=8;
int LF=11;
int LB=10;

// line pins //
int line1=3;
int leftline;
int line2=2;
int rightline;

void setup() 
{
Serial.begin(9600);
pinMode(RF,OUTPUT);
pinMode(RB,OUTPUT);
pinMode(LF,OUTPUT);
pinMode(LB,OUTPUT);
}
void loop() 
{
leftline=digitalRead(line1);
rightline=digitalRead(line2); 


 if((leftline==1)&&(rightline==1))
{
digitalWrite(RF,LOW);
digitalWrite(RB,LOW);
digitalWrite(LF,LOW);
digitalWrite(LB,LOW);
}
 if((leftline==0)&&(rightline==0))
{
digitalWrite(RF,HIGH);
digitalWrite(RB,LOW);
digitalWrite(LF,HIGH);
digitalWrite(LB,LOW);
}
 if((leftline==1)&&(rightline==0))
{
digitalWrite(RF,HIGH);
digitalWrite(RB,LOW);
digitalWrite(LF,LOW);
digitalWrite(LB,HIGH);
}
 if((leftline==0)&&(rightline==1))
{
digitalWrite(RF,LOW);
digitalWrite(RB,HIGH);
digitalWrite(LF,HIGH);
digitalWrite(LB,LOW);
}
}