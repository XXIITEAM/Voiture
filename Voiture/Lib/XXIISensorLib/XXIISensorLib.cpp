#include "XXIISensorLib.h"


XXIISensorLib::XXIISensorLib()
{
	Serial.begin(115200);
	pinMode(US1_TRIGGER_PIN, OUTPUT);
	pinMode(US1_ECHO_PIN, INPUT);
	pinMode(US2_TRIGGER_PIN, OUTPUT);
	pinMode(US2_ECHO_PIN, INPUT);
	pinMode(US3_TRIGGER_PIN, OUTPUT);
	pinMode(US3_ECHO_PIN, INPUT);
	pinMode(US4_TRIGGER_PIN, OUTPUT);
	pinMode(US4_ECHO_PIN, INPUT);
	pinMode(US5_TRIGGER_PIN, OUTPUT);
	pinMode(US5_ECHO_PIN, INPUT);
	pinMode(US6_TRIGGER_PIN, OUTPUT);
	pinMode(US6_ECHO_PIN, INPUT);
}

float XXIISensorLib::ScanAv(float *capteur_av_g_cm, float *capteur_av_c_cm, float *capteur_av_d_cm) {
	*capteur_av_g_cm = getCM(US1_TRIGGER_PIN, US1_ECHO_PIN);
	*capteur_av_c_cm = getCM(US2_TRIGGER_PIN, US2_ECHO_PIN);
	*capteur_av_d_cm = getCM(US3_TRIGGER_PIN, US3_ECHO_PIN);
}

float XXIISensorLib::ScanAr(float *capteur_ar_d_cm, float *capteur_ar_c_cm, float *capteur_ar_g_cm) {
	*capteur_ar_d_cm = getCM(US4_TRIGGER_PIN, US4_ECHO_PIN);
	*capteur_ar_c_cm = getCM(US5_TRIGGER_PIN, US5_ECHO_PIN);
	*capteur_ar_g_cm = getCM(US6_TRIGGER_PIN, US6_ECHO_PIN);
}

float XXIISensorLib::getCM(int trigPin, int echoPin) {
	float microsec;
		digitalWrite(trigPin, LOW);
		delayMicroseconds(2);
		digitalWrite(trigPin, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigPin, LOW);
		microsec = pulseIn(echoPin, HIGH);
		return (microsec / 58);
}