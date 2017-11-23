#include "MHZ19.h"

MHZ19::MHZ19(HardwareSerial * serial)
{
	_hs = serial;
}

MHZ19::MHZ19(SoftwareSerial * serial)
{
	_ss = serial;
}

MHZ19::~MHZ19()
{
	_hs = nullptr;
	_ss = nullptr;
}

int MHZ19::getCO2()
{
	if (_result == 0)
	{
		return bytes2int(_response[2], _response[3]);
	}
	return _result;
}

int MHZ19::getTemperature()
{
	if (_result == 0)
	{
		int value = static_cast<int>(_response[4]);
		return value - 40;
	}
	return _result;
}

int MHZ19::getAccuracy()
{
	if (_result == MHZ19_RESULT_OK)
	{
		int value = static_cast<int>(_response[4]);
		return value;
	}
	return _result;
}

MHZ19_RESULT MHZ19::retrieveData()
{
	byte cmd[9] = { 0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79 };
	for (int i = 0; i < 9; i++) {
		_response[i] = 0;
	}

	if (_hs)
	{
		while (_hs->available()) { _hs->read(); }
		_hs->write(cmd, 9);
	}
	else
	{
		while (_ss->available()) { _ss->read(); }
		_ss->write(cmd, 9);
	}
	
	memset(_response, 0, 9);

	if (_hs)
	{
		_hs->readBytes(_response, 9);
	}
	else
	{
		_ss->readBytes(_response, 9);
	}

	byte crc = calcResponseCRC();

	_result = MHZ19_RESULT_OK;
	if (_response[0] != 0xFF)
		_result = MHZ19_RESULT_ERR_FB;
	if (_response[1] != 0x86)
		_result = MHZ19_RESULT_ERR_SB;
	if (_response[8] != crc)
		_result = MHZ19_RESULT_ERR_CRC;
	
	return _result;
}

int MHZ19::bytes2int(byte h, byte l)
{
	int high = static_cast<int>(h);
	int low = static_cast<int>(l);
	return (256 * high) + low;
}

byte MHZ19::calcResponseCRC()
{
	byte i;
	byte crc = 0;
	for (i = 1; i < 8; i++)
	{
		crc += _response[i];
	}
	crc = 255 - crc;
	crc++;

	return crc;
}