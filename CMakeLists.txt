px4_add_module(
	MODULE drivers__air_data_boom
	MAIN air_data_boom
	SRCS
		air_data_boom.cpp
		air_data_boom.hpp
		air_data_boom_main.cpp
	DEPENDS
		px4_work_queue
	)
