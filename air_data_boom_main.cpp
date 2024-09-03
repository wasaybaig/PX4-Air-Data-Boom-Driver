/**
 * Local functions in support of the shell command.
 */
#include "air_data_boom.hpp"

namespace air_data_boom
{

AirDataBoom	*g_dev{nullptr};

int start(const char *port);
int status();
int stop();
int usage();

int
start(const char *port)
{
	if (g_dev != nullptr) {
		PX4_ERR("already started");
		return PX4_OK;
	}

	// Instantiate the driver.
	g_dev = new AirDataBoom(port);

	if (g_dev == nullptr) {
		PX4_ERR("driver start failed");
		return PX4_ERROR;
	}

	if (OK != g_dev->init()) {
		PX4_ERR("driver start failed");
		delete g_dev;
		g_dev = nullptr;
		return PX4_ERROR;
	}

	return PX4_OK;
}

int
status()
{
	if (g_dev == nullptr) {
		PX4_ERR("driver not running");
		return 1;
	}
	else{
		PX4_INFO("Driver running");
	}

	return 0;
}

int stop()
{
	if (g_dev != nullptr) {
		PX4_INFO("stopping driver");
		delete g_dev;
		g_dev = nullptr;
		PX4_INFO("driver stopped");

	} else {
		PX4_ERR("driver not running");
		return 1;
	}

	return PX4_OK;
}

}
extern "C" __EXPORT int air_data_boom_main(int argc, char *argv[]){
	const char *device_path = AIRDATABOOM_DEFAULT_PORT;

	if (argc < 2) {
		PX4_ERR("Usage: air_data_boom {start|stop|status}");
		return -1;
    	}
	const char *command = argv[1];

	if (!strcmp(command, "start")) {
		return air_data_boom::start(device_path);

	} else if (!strcmp(command, "stop")) {
		return air_data_boom::stop();

	} else if (!strcmp(command, "status")) {
		return air_data_boom::status();
	}
	return PX4_OK;
}
