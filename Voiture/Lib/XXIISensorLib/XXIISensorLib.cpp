/*!
 * @file XXIISensorLib.cpp
 * @brief Librairie voiture arduino
 * @author XXII-TEAM
 * @version 0.1
 * @date 22 juin 2019
 *
 * Librairie Arduino voiture commandée par application Android (BTArdroid).
 *
 */

#include "XXIISensorLib.h"

 /*!
  * @fn XXIISensorLib::XXIISensorLib()
  * @brief Fonction Setup.
  *
  * @param Aucun.
  * @return Aucun
  */
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
/*!
 * @fn float XXIISensorLib::ScanAv(float *capteur_av_g_cm, float *capteur_av_c_cm, float *capteur_av_d_cm)
 * @brief Fonction Scan avant ultrason, l'appel se fait : Sensor.ScanAv(&dist_av_g, &dist_av_c, &dist_av_d); &VARIABLE = récupération du contenu du pointeur.
 *
 * @param float *capteur_av_g_cm, float *capteur_av_c_cm, float *capteur_av_d_cm.
 * @return *capteur_av_g_cm *capteur_av_c_cm *capteur_av_d_cm
 */
float XXIISensorLib::ScanAv(float *capteur_av_g_cm, float *capteur_av_c_cm, float *capteur_av_d_cm) {
	*capteur_av_g_cm = getCM(US1_TRIGGER_PIN, US1_ECHO_PIN);
	*capteur_av_c_cm = getCM(US2_TRIGGER_PIN, US2_ECHO_PIN);
	*capteur_av_d_cm = getCM(US3_TRIGGER_PIN, US3_ECHO_PIN);
}

/*!
 * @fn float XXIISensorLib::ScanAr(float *capteur_ar_d_cm, float *capteur_ar_c_cm, float *capteur_ar_g_cm)
 * @brief Fonction Setup.
 *
 * @param float *capteur_ar_d_cm, float *capteur_ar_c_cm, float *capteur_ar_g_cm
 * @return *capteur_ar_d_cm *capteur_ar_c_cm *capteur_ar_g_cm
 */
float XXIISensorLib::ScanAr(float *capteur_ar_d_cm, float *capteur_ar_c_cm, float *capteur_ar_g_cm) {
	*capteur_ar_d_cm = getCM(US4_TRIGGER_PIN, US4_ECHO_PIN);
	*capteur_ar_c_cm = getCM(US5_TRIGGER_PIN, US5_ECHO_PIN);
	*capteur_ar_g_cm = getCM(US6_TRIGGER_PIN, US6_ECHO_PIN);
}

/*!
 * @fn float XXIISensorLib::getCM(int trigPin, int echoPin)
 * @brief Fonction getCM, récupération distance + moyenne sur 10 resultats.
 *
 * @param int trigPin, int echoPin
 * @return cm_now
 */
float XXIISensorLib::getCM(int trigPin, int echoPin) {
	float microsec, cm_now, microsec_moy, microsec_final;
	for (int i = 0; i < 10; i++) {
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	microsec = pulseIn(echoPin, HIGH);
	microsec_moy = microsec_moy + microsec;
	}
	microsec_final = microsec_moy / 10;
	cm_now = (microsec_final * (331.4 + (0.606 * 20.00) + (0.0124 * 40.00)) / (2 * 10000));
	return cm_now;
}