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
	*capteur_av_g_cm = getCM(US1_TRIGGER_PIN, US1_ECHO_PIN, US2_ECHO_PIN, US3_ECHO_PIN);
	*capteur_av_c_cm = getCM(US2_TRIGGER_PIN, US2_ECHO_PIN, US1_ECHO_PIN, US3_ECHO_PIN);
	*capteur_av_d_cm = getCM(US3_TRIGGER_PIN, US3_ECHO_PIN, US2_ECHO_PIN, US1_ECHO_PIN);
}

float XXIISensorLib::ScanAr(float *capteur_ar_d_cm, float *capteur_ar_c_cm, float *capteur_ar_g_cm) {
	*capteur_ar_d_cm = getCM(US4_TRIGGER_PIN, US4_ECHO_PIN, US5_ECHO_PIN, US6_ECHO_PIN);
	*capteur_ar_c_cm = getCM(US5_TRIGGER_PIN, US5_ECHO_PIN, US4_ECHO_PIN, US6_ECHO_PIN);
	*capteur_ar_g_cm = getCM(US6_TRIGGER_PIN, US6_ECHO_PIN, US4_ECHO_PIN, US5_ECHO_PIN);
}

float XXIISensorLib::getCM(int trigPin, int echoPin, int echoPin2, int echoPin3) {
	float microsec, cm_now;
		digitalWrite(trigPin, LOW);
		delayMicroseconds(2);
		digitalWrite(trigPin, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigPin, LOW);
		
			microsec = pulseIn(echoPin, HIGH);
		/*if (pulseIn(echoPin2, HIGH)) {
			//indirect1
			microsec = pulseIn(echoPin2, HIGH);
		}
		if (pulseIn(echoPin3, HIGH)) {
			//indirect2
			microsec = pulseIn(echoPin3, HIGH);
		}*/
			// A prévoir calcul avec temp et humi : cm_now = microsec * (331.4 + (0.606 * TEMP) + (0.0124 * HUMI)) / (2 * 10000);
		cm_now = (microsec * (331.4 + (0.606 * 20.00) + (0.0124 * 40.00)) / (2 * 10000));
		return cm_now;
}