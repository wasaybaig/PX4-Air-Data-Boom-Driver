#include "air_data_boom.hpp"
#include <lib/drivers/device/Device.hpp>
#include <fcntl.h>

#define SOH_1 0x01
#define SOH_2 0x02
#define SOH_3 0x03
#define CR_1 0x0D
#define CR_2 0x0A

AirDataBoom::AirDataBoom(const char *port)
    : ScheduledWorkItem(MODULE_NAME, px4::serial_port_to_wq(port)),
      _fd(-1)
{
    strncpy(_port, port, sizeof(_port) - 1);
    _port[sizeof(_port) - 1] = '\0';
    memset(&_air_data, 0, sizeof(_air_data));
}

AirDataBoom::~AirDataBoom()
{
    stop();
}

bool AirDataBoom::init()
{
    int ret = 0;

    do{
	_fd = ::open(_port, O_RDONLY | O_NOCTTY | O_NDELAY);
	if (_fd < 0) {
		PX4_ERR("Failed to open port: %s", _port);
		ret = PX4_ERROR;
	}

	termios tty{};
	if (tcgetattr(_fd, &tty) != 0) {
		PX4_ERR("Error from tcgetattr: %d", errno);
		ret = PX4_ERROR;
	}

	cfsetospeed(&tty, B230400);
	cfsetispeed(&tty, B230400);

	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;

	// Set raw input mode
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_oflag &= ~OPOST;

	// No hardware flow control (disable RTS/CTS)
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(_fd, TCSANOW, &tty) != 0) {
		PX4_ERR("Error from tcsetattr: %d", errno);
		ret = PX4_ERROR;
		return PX4_ERROR;
	}


	if (_fd < 0) {
		PX4_ERR("FAIL: laser fd");
		ret = -1;
		break;
	}
    } while(0);

    // close the fd
    ::close(_fd);
    _fd = -1;

    PX4_INFO("RET: %d",ret);
    if (ret == PX4_OK){
	PX4_INFO("Scheduling...");
	start();
    }
    return ret;
}

void AirDataBoom::start()
{
    ScheduleOnInterval(50_ms);
}

void AirDataBoom::stop()
{
    ScheduleClear();
}

void AirDataBoom::Run()
{
	if (_fd < 0) {
		// open fd
		_fd = ::open(_port, O_RDONLY | O_NOCTTY | O_NDELAY);
	}
    	collect();
}

int AirDataBoom::collect()
{   char _buffer;
    int n = ::read(_fd, &_buffer, 1);
    int count = 0;
    if (n > 0) {
	while(count < 12){
        	count = parseData(_buffer, count);
		::read(_fd, &_buffer, 1);
	}
    }
    else{
	PX4_ERR("Unable to read!");
    }
    return PX4_OK;
}

int AirDataBoom::parseData(char data, int count)
{
    static int pos = 0;
    static char buff[12] = {0};

    if (((unsigned char)data == SOH_1) || ((unsigned char)data == SOH_2) || ((unsigned char)data == SOH_3)){
	buff[0] = data;
	pos = 1;
    }

    else if ((pos > 0) && (pos < 12)){
	buff[pos] = (uint8_t)data;
	if (((unsigned char)data == CR_1)){
		if (pos == 10){
		count = count + 1;
		buff[10] = '\0';
		int32_t bits = strtoll(&buff[2], NULL, 16);
		float value = *((float *)&bits);
		uint8_t label = buff[1] & 0x0Fu;
		uint8_t soh = buff[0];

		if (soh == 1){
			switch (label){
			case 1:
				_air_data.qc = value;
				break;
			case 2:
				_air_data.ps = value;
				break;
			case 3:
				_air_data.aoa = value;
				break;
			case 4:
				_air_data.aos = value;
				break;
			case 5:
				_air_data.cas = value;
				break;
			case 6:
				_air_data.tas = value;
				break;
			case 7:
				_air_data.hp = value;
				break;
			case 8:
				_air_data.sat = value;
				break;
			case 9:
				_air_data.tat = value;
				break;
			}
		}
		else if (soh == 2){
			_air_data.cr = value;
		}
		else if (soh == 3){
			switch (label){
			case 1:
				_air_data.qcr = value;
				break;
			case 2:
				_air_data.psr = value;
				break;
			}
		}

		if (count == 12){
			_air_data.timestamp = hrt_absolute_time();
			_air_data_boom_pub.publish(_air_data);
			_airspeed_validated.timestamp = hrt_absolute_time();
			_airspeed_validated.indicated_airspeed_m_s = _air_data.cas;
			_airspeed_validated.calibrated_airspeed_m_s = _air_data.cas;
			_airspeed_validated.true_airspeed_m_s = _air_data.tas;
			_airspeed_validated.calibrated_ground_minus_wind_m_s  = NAN;
			_airspeed_validated.true_ground_minus_wind_m_s = NAN;
			_airspeed_validated.airspeed_sensor_measurement_valid  = true;
			_airspeed_validated.selected_airspeed_index = 1;
			_airspeed_validated_pub.publish(_airspeed_validated);
		}
		}
	    pos = 0;
	}
	else{
	    pos++;
	}

	}
	return count;
}


