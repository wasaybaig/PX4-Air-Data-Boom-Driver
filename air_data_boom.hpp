/**
 * @file air_data_boom.hpp
 *
 * Defines for the Air Data Boom Driver
 */

#pragma once
#include <termios.h>
#include <drivers/drv_hrt.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/SubscriptionInterval.hpp>
#include <uORB/topics/air_data_boom.h>
#include <px4_time.h>
#include <uORB/topics/airspeed_validated.h>


#define AIRDATABOOM_DEFAULT_PORT	"/dev/ttyS3"

using namespace time_literals;

class AirDataBoom : public px4::ScheduledWorkItem
{
public:
    AirDataBoom(const char *port);
    virtual ~AirDataBoom();

    bool init();


private:
    char _port[20] {};
    int _fd{-1};

    uORB::Publication<air_data_boom_s>     _air_data_boom_pub{ORB_ID(air_data_boom)};
    uORB::Publication<airspeed_validated_s>          _airspeed_validated_pub{ORB_ID(airspeed_validated)};
    uORB::Subscription      _air_data_boom_sub{ORB_ID(air_data_boom)};
    air_data_boom_s _air_data;
    airspeed_validated_s _airspeed_validated;

    void Run() override;
    int collect();
    int parseData(char data, int count);
    void start();
    void stop();
};
