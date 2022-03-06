#include "pca9685servo.h"
#include <iostream>

#define MAX_12BIT	(0xFFF)
#define MAX_8BIT	(0xFF)
#define MAX_ANGLE	(180)

#define MID_COUNT	static_cast<uint16_t>(.5f + ((204.8f * SERVO_CENTER_DEFAULT_US) / 1000))

PCA9685Servo::PCA9685Servo(uint8_t nAddress): PCA9685(nAddress) {
	SetInvert(false);
	SetOutDriver(true);
	SetFrequency(50);
	CalcLeftCount();
	CalcRightCount();
	CalcCenterCount();
}

void PCA9685Servo::SetLeftUs(uint16_t nLeftUs) {
	assert(nLeftUs < m_nRightUs);
	assert(nLeftUs < m_nCenterUs);

	m_nLeftUs = nLeftUs;
	CalcLeftCount();
}

uint16_t PCA9685Servo::GetLeftUs() const {
	return m_nLeftUs;
}

void PCA9685Servo::SetRightUs(uint16_t nRightUs) {
	assert(m_nLeftUs < nRightUs);
	assert(m_nCenterUs < nRightUs);

	m_nRightUs = nRightUs;
	CalcRightCount();
}

uint16_t PCA9685Servo::GetRightUs() const {
	return m_nRightUs;
}

void PCA9685Servo::SetCenterUs(uint16_t nCenterUs) {
	assert(nCenterUs < m_nRightUs);
	assert(m_nLeftUs < nCenterUs);

	m_nCenterUs = nCenterUs;
	CalcCenterCount();
}

uint16_t PCA9685Servo::GetCenterUs() const {
	return m_nCenterUs;
}

void PCA9685Servo::CalcLeftCount() {
	m_nLeftCount = static_cast<uint16_t>((.5f + ((204.8f * m_nLeftUs) / 1000)));
}

void PCA9685Servo::CalcRightCount() {
	m_nRightCount = static_cast<uint16_t>((.5f + ((204.8f * m_nRightUs) / 1000)));
}

void PCA9685Servo::CalcCenterCount() {
	m_nCenterCount = static_cast<uint16_t>((.5f + ((204.8f * m_nCenterUs) / 1000)));
}

void PCA9685Servo::Set(uint8_t nChannel, uint16_t nData) {
	if (nData > m_nRightCount) {
		nData = m_nRightCount;
	} else if (nData < m_nLeftCount) {
		nData = m_nLeftCount;
	}
	Write(nChannel, nData);
}

void PCA9685Servo::Set(uint8_t nChannel, uint8_t nData) {
	if (nData == 0) {
		Write(nChannel, m_nLeftCount);
	} else if (nData == (MAX_8BIT + 1) / 2) {
		Write(nChannel, m_nCenterCount);
	}  else if (nData == MAX_8BIT) {
		Write(nChannel, m_nRightCount);
	} else {
		const auto nCount = static_cast<uint16_t>(m_nLeftCount + (.5f + (static_cast<float>((m_nRightCount - m_nLeftCount)) / MAX_8BIT) * nData));
		Write(nChannel, nCount);
	}
}

void PCA9685Servo::SetAngle(uint8_t nChannel, uint8_t nAngle) {
	if (nAngle == 0) {
		Write(nChannel, m_nLeftCount);
	} else if (nAngle == 90) {
		Write(nChannel, m_nCenterCount);
	}  else if (nAngle >= 180) {
		Write(nChannel, m_nRightCount);
	} else if (nAngle < 90) {
		const auto nCount = static_cast<uint16_t>(m_nLeftCount + (.5f + (static_cast<float>((m_nCenterCount - m_nLeftCount)) / 90) * nAngle));
		Write(nChannel, nCount);
	} else {
		const auto nCount = static_cast<uint16_t>((2.0f * m_nCenterCount) - m_nRightCount + (.5f + (static_cast<float>((m_nRightCount - m_nCenterCount)) / 90) * nAngle));
		Write(nChannel, nCount);
	}
}
