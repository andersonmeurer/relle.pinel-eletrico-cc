	// gets the state of the SELF_TEST bit
bool adxl345_GetSelfTestBit(void) {
	return adxl345_GetRegisterBit(ADXL345_DATA_FORMAT, 7);
}

// Sets the SELF-TEST bit
// if set to 1 it applies a self-test force to the sensor causing a shift in the output data
// if set to 0 it disables the self-test force
void adxl345_SetSelfTestBit(bool selfTestBit) {
	adxl345_SetRegisterBit(ADXL345_DATA_FORMAT, 7, selfTestBit);
}

// Gets the state of the SPI bit
bool adxl345_GetSpiBit(void) {
	return adxl345_GetRegisterBit(ADXL345_DATA_FORMAT, 6);
}

// Sets the SPI bit
// if set to 1 it sets the device to 3-wire mode
// if set to 0 it sets the device to 4-wire SPI mode
void adxl345_SetSpiBit(bool spiBit) {
	adxl345_SetRegisterBit(ADXL345_DATA_FORMAT, 6, spiBit);
}

// Gets the state of the INT_INVERT bit
bool adxl345_GetInterruptLevelBit(void) {
	return adxl345_GetRegisterBit(ADXL345_DATA_FORMAT, 5);
}

// Sets the INT_INVERT bit
// if set to 0 sets the interrupts to active high
// if set to 1 sets the interrupts to active low
void adxl345_SetInterruptLevelBit(bool interruptLevelBit) {
	adxl345_SetRegisterBit(ADXL345_DATA_FORMAT, 5, interruptLevelBit);
}

// Gets the state of the FULL_RES bit
bool adxl345_GetFullResBit(void) {
	return adxl345_GetRegisterBit(ADXL345_DATA_FORMAT, 3);
}


// Gets the state of the justify bit
bool adxl345_GetJustifyBit(void) {
	return adxl345_GetRegisterBit(ADXL345_DATA_FORMAT, 2);
}








// Sets the JUSTIFY bit
// if sets to 1 selects the left justified mode
// if sets to 0 selects right justified mode with sign extension
void adxl345_SetJustifyBit(bool justifyBit) {
	adxl345_SetRegisterBit(ADXL345_DATA_FORMAT, 2, justifyBit);
}

// Gets the THRESH_TAP u8 value
// return value is comprised between 0 and 255
// the scale factor is 62.5 mg/LSB
int adxl345_GetTapThreshold(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_THRESH_TAP, 1, &_b);
	return (int)(_b);
}

// set/get the gain for each axis in Gs / count
void adxl345_SetAxisGains(double *_gains) {
	int i;
	for(i = 0; i < 3; i++) gains[i] = _gains[i];
}

void adxl345_GetAxisGains(double *_gains) {
	int i;
	for(i = 0; i < 3; i++) _gains[i] = gains[i];
}

// Sets the OFSX, OFSY and OFSZ bytes
// OFSX, OFSY and OFSZ are user offset adjustments in twos complement format with
// a scale factor of 15,6mg/LSB
// OFSX, OFSY and OFSZ should be comprised between
void adxl345_SetAxisOffset(int x, int y, int z) {
	adxl345_WriteReg(ADXL345_OFSX, (u8)(x));
	adxl345_WriteReg(ADXL345_OFSY, (u8)(y));
	adxl345_WriteReg(ADXL345_OFSZ, (u8)(z));
}

// Gets the OFSX, OFSY and OFSZ bytes
void adxl345_GetAxisOffset(int* x, int* y, int*z) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_OFSX, 1, &_b);
	*x = (int)(_b);
	adxl345_ReadBuffer(ADXL345_OFSY, 1, &_b);
	*y = (int)(_b);
	adxl345_ReadBuffer(ADXL345_OFSZ, 1, &_b);
	*z = (int)(_b);
}

// Gets the DUR u8
int adxl345_GetTapDuration(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_DUR, 1, &_b);
	return (int)(_b);
}


// Gets the Latent value
int adxl345_GetDoubleTapLatency(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_LATENT, 1, &_b);
	return (int)(_b);
}

// Gets the Window register
int adxl345_GetDoubleTapWindow(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_WINDOW, 1, &_b);
	return (int)(_b);
}


// Gets the THRESH_ACT u8
int adxl345_GetActivityThreshold(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_THRESH_ACT, 1, &_b);
	return (int)(_b);
}

// Gets the THRESH_INACT u8
int adxl345_GetInactivityThreshold(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_THRESH_INACT, 1, &_b);
	return (int)(_b);
}

// Gets the TIME_INACT register
int adxl345_GetTimeInactivity(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_TIME_INACT, 1, &_b);
	return (int)(_b);
}


// Gets the THRESH_FF register.
int adxl345_GetFreeFallThreshold(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_THRESH_FF, 1, &_b);
	return (int)(_b);
}

// Gets the TIME_FF register.
int adxl345_GetFreeFallDuration(void) {
	u8 _b;
	adxl345_ReadBuffer(ADXL345_TIME_FF, 1, &_b);
	return (int)(_b);
}

bool adxl345_IsActivityXEnabled(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 6);
}

bool adxl345_IsActivityYEnabled(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 5);
}

bool adxl345_IsActivityZEnabled(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 4);
}

bool adxl345_IsInactivityXEnabled(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 2);
}

bool adxl345_IsInactivityYEnabled(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 1);
}

bool adxl345_IsInactivityZEnabled(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 0);
}

bool adxl345_IsActivityAc(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 7);
}

bool adxl345_IsInactivityAc(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_INACT_CTL, 3);
}

// state pdON ou pdOFF
void adxl345_SetActivityAc(bool state) {
	adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 7, state);
}

// state pdON ou pdOFF
void adxl345_SetInactivityAc(bool state) {
	adxl345_SetRegisterBit(ADXL345_ACT_INACT_CTL, 3, state);
}

bool adxl345_GetSuppressBit(void) {
	return adxl345_GetRegisterBit(ADXL345_TAP_AXES, 3);
}

// state pdON ou pdOFF
void adxl345_SetSuppressBit(bool state) {
	adxl345_SetRegisterBit(ADXL345_TAP_AXES, 3, state);
}

bool adxl345_IsTapDetectionOnX(void) {
	return adxl345_GetRegisterBit(ADXL345_TAP_AXES, 2);
}

bool adxl345_IsTapDetectionOnY(void) {
	return adxl345_GetRegisterBit(ADXL345_TAP_AXES, 1);
}


bool adxl345_IsTapDetectionOnZ(void) {
	return adxl345_GetRegisterBit(ADXL345_TAP_AXES, 0);
}

bool adxl345_IsActivitySourceOnX(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 6);
}

bool adxl345_IsActivitySourceOnY(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 5);
}

bool adxl345_IsActivitySourceOnZ(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 4);
}

bool adxl345_IsTapSourceOnX(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 2);
}

bool adxl345_IsTapSourceOnY(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 1);
}

bool adxl345_IsTapSourceOnZ(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 0);
}

bool adxl345_IsAsleep(void) {
	return adxl345_GetRegisterBit(ADXL345_ACT_TAP_STATUS, 3);
}

bool adxl345_IsLowPower(void) {
	return adxl345_GetRegisterBit(ADXL345_BW_RATE, 4);
}

//Used to check if action was triggered in interrupts
//Example triggered(interrupts, ADXL345_SINGLE_TAP);
bool adxl345_Triggered(u8 interrupts, int mask) {
	return ((interrupts >> mask) & 1);
}

//u8 adxl345_getInterruptSource (void) {
//	u8 _b;
//	adxl345_ReadBuffer(ADXL345_INT_SOURCE, 1, &_b);
//	return _b;
//}

bool adxl345_GetInterruptSource(u8 interruptBit) {
	return adxl345_GetRegisterBit(ADXL345_INT_SOURCE,interruptBit);
}

bool adxl345_GetInterruptMapping(u8 interruptBit) {
	return adxl345_GetRegisterBit(ADXL345_INT_MAP,interruptBit);
}

// Set the mapping of an interrupt to pin1 or pin2
// eg: setInterruptMapping(ADXL345_INT_DOUBLE_TAP_BIT,ADXL345_INT2_PIN);
void adxl345_SetInterruptMapping(u8 interruptBit, bool interruptPin) {
	adxl345_SetRegisterBit(ADXL345_INT_MAP, interruptBit, interruptPin);
}

bool adxl345_IsInterruptEnabled(u8 interruptBit) {
	return adxl345_GetRegisterBit(ADXL345_INT_ENABLE,interruptBit);
}

static bool adxl345_GetRegisterBit(u8 regAdress, int bitPos);

bool adxl345_GetRegisterBit(u8 regAdress, int bitPos) {
	u8 _b;
	adxl345_ReadBuffer(regAdress, 1, &_b);
	return ((_b >> bitPos) & 1);
}


//static float Q_angle; // Process noise variance for the accelerometer
//static float Q_bias; // Process noise variance for the gyro bias
//static float R_measure; // Measurement noise variance - this is actually the variance of the measurement noise
//static float angle; // The angle calculated by the Kalman filter - part of the 2x1 state vector
//static float bias; // The gyro bias calculated by the Kalman filter - part of the 2x1 state vector
//static float rate; // Unbiased rate calculated from the rate and the calculated bias - you have to call getAngle to update the rate
//
//static float P[2][2]; // Error covariance matrix - This is a 2x2 matrix
//
//static void kalman_Init(void) {
//    /* We will set the variables like so, these can also be tuned by the user */
//    Q_angle = 0.001f;
//    Q_bias = 0.003f;
//    R_measure = 0.03f;
//
//    angle = 0.0f; // Reset the angle
//    bias = 0.0f; // Reset bias
//
//    P[0][0] = 0.0f; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
//    P[0][1] = 0.0f;
//    P[1][0] = 0.0f;
//    P[1][1] = 0.0f;
//    timer = now();
//};
//
//// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
//// newAngle = angle measured with atan2 using the accelerometer
//// newRate = angle measured using the gyro
//// looptime = loop time in millis()
//static float kalman_GetAngle(float newAngle, float newRate) {
//    // KasBot V2  -  Kalman filter module - http://www.x-firm.com/?page_id=145
//    // Modified by Kristian Lauszus
//    // See my blog post for more information: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it
//
//    // Discrete Kalman filter time update equations - Time Update ("Predict")
//    // Update xhat - Project the state ahead
//    // Step 1
//    rate = newRate - bias;
//    angle += dt * rate;
//
//    // Update estimation error covariance - Project the error covariance ahead
//    // Step 2
//    P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0] + Q_angle);
//    P[0][1] -= dt * P[1][1];
//    P[1][0] -= dt * P[1][1];
//    P[1][1] += Q_bias * dt;
//
//    // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
//    // Calculate Kalman gain - Compute the Kalman gain
//    // Step 4
//    float S = P[0][0] + R_measure; // Estimate error
//    // Step 5
//    float K[2]; // Kalman gain - This is a 2x1 vector
//    K[0] = P[0][0] / S;
//    K[1] = P[1][0] / S;
//
//    // Calculate angle and bias - Update estimate with measurement zk (newAngle)
//    // Step 3
//    float y = newAngle - angle; // Angle difference
//    // Step 6
//    angle += K[0] * y;
//    bias += K[1] * y;
//
//    // Calculate estimation error covariance - Update the error covariance
//    // Step 7
//    float P00_temp = P[0][0];
//    float P01_temp = P[0][1];
//
//    P[0][0] -= K[0] * P00_temp;
//    P[0][1] -= K[0] * P01_temp;
//    P[1][0] -= K[1] * P00_temp;
//    P[1][1] -= K[1] * P01_temp;
//
//	return angle;
//};

#if (ADXL345_USE_INCLINATION == pdON)
//static tTime dt, timer;
static void adxl345_CalcRollPitch(void) {
//	dt = now() - timer;
//	timer = now();

	double fx = (s16)adxl345.x;
	double fy = (s16)adxl345.y;
	double fz = (s16)adxl345.z;

  	//Roll & Pitch Equations
    adxl345.roll  = (atan2(-fy, fz)*180.0)/M_PI;
    adxl345.pitch = (atan2(fx, sqrt(fy*fy + fz*fz))*180.0)/M_PI;
    adxl345.angle = round(adxl345.roll);

	//adxl345.roll = kalman_GetAngle(adxl345.roll, 0);

// 	adxl345.xAngle = atan( fx / (sqrt((fy*fy) + (fz*fz))));
//  adxl345.yAngle = atan( fy / (sqrt((fx*fx) + (fz*fz))));
//  adxl345.zAngle = atan( sqrt((fx*fx) + (fy*fy)) / fz);
//  adxl345.xAngle *= 180.00;
//  adxl345.yAngle *= 180.00;
//  adxl345.zAngle *= 180.00;
//  adxl345.xAngle /= M_PI;
//  adxl345.yAngle /= M_PI;
//  adxl345.zAngle /= M_PI;

//	adxl345.angle = atan2(fz, fy)*180.0f/M_PI;
}
#endif
