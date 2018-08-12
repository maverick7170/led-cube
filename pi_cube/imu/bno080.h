#ifndef BNO080_
#define BNO080_

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdio>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <Eigen/Dense>

#include "../gpio/gpio.h"

//The default I2C address for the BNO080 on the SparkX breakout is 0x4B. 0x4A is also possible.
#define BNO080_DEFAULT_ADDRESS 0x4B

//Registers
const uint8_t CHANNEL_COMMAND = 0;
const uint8_t CHANNEL_EXECUTABLE = 1;
const uint8_t CHANNEL_CONTROL = 2;
const uint8_t CHANNEL_REPORTS = 3;
const uint8_t CHANNEL_WAKE_REPORTS = 4;
const uint8_t CHANNEL_GYRO = 5;

//All the ways we can configure or talk to the BNO080, figure 34, page 36 reference manual
//These are used for low level communication with the sensor, on channel 2
#define SHTP_REPORT_COMMAND_RESPONSE 0xF1
#define SHTP_REPORT_COMMAND_REQUEST 0xF2
#define SHTP_REPORT_FRS_READ_RESPONSE 0xF3
#define SHTP_REPORT_FRS_READ_REQUEST 0xF4
#define SHTP_REPORT_PRODUCT_ID_RESPONSE 0xF8
#define SHTP_REPORT_PRODUCT_ID_REQUEST 0xF9
#define SHTP_REPORT_BASE_TIMESTAMP 0xFB
#define SHTP_REPORT_SET_FEATURE_COMMAND 0xFD

//All the different sensors and features we can get reports from
//These are used when enabling a given sensor
#define SENSOR_REPORTID_ACCELEROMETER 0x01
#define SENSOR_REPORTID_GYROSCOPE 0x02
#define SENSOR_REPORTID_MAGNETIC_FIELD 0x03
#define SENSOR_REPORTID_LINEAR_ACCELERATION 0x04
#define SENSOR_REPORTID_ROTATION_VECTOR 0x05
#define SENSOR_REPORTID_GRAVITY 0x06
#define SENSOR_REPORTID_GAME_ROTATION_VECTOR 0x08
#define SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR 0x09
#define SENSOR_REPORTID_TAP_DETECTOR 0x10
#define SENSOR_REPORTID_STEP_COUNTER 0x11
#define SENSOR_REPORTID_STABILITY_CLASSIFIER 0x13
#define SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER 0x1E

//Record IDs from figure 29, page 29 reference manual
//These are used to read the metadata for each sensor type
#define FRS_RECORDID_ACCELEROMETER 0xE302
#define FRS_RECORDID_GYROSCOPE_CALIBRATED 0xE306
#define FRS_RECORDID_MAGNETIC_FIELD_CALIBRATED 0xE309
#define FRS_RECORDID_ROTATION_VECTOR 0xE30B

//Command IDs from section 6.4, page 42
//These are used to calibrate, initialize, set orientation, tare etc the sensor
#define COMMAND_ERRORS 1
#define COMMAND_COUNTER 2
#define COMMAND_TARE 3
#define COMMAND_INITIALIZE 4
#define COMMAND_DCD 6
#define COMMAND_ME_CALIBRATE 7
#define COMMAND_DCD_PERIOD_SAVE 9
#define COMMAND_OSCILLATOR 10
#define COMMAND_CLEAR_DCD 11

#define CALIBRATE_ACCEL 0
#define CALIBRATE_GYRO 1
#define CALIBRATE_MAG 2
#define CALIBRATE_PLANAR_ACCEL 3
#define CALIBRATE_ACCEL_GYRO_MAG 4
#define CALIBRATE_STOP 5

#define MAX_PACKET_SIZE 1024 //Packets can be up to 32k but we don't have that much RAM.
#define MAX_METADATA_SIZE 9 //This is in words. There can be many but we mostly only care about the first 9 (Qs, range, etc)

class I2C {
	public:
		I2C() {
			fid = open("/dev/i2c-1",O_RDWR);
			if (fid < 0) {
				std::cout << "ERROR: I2C open" << std::endl;
			}
			ioctl(fid,I2C_SLAVE,BNO080_DEFAULT_ADDRESS); 
		};
		void start() {}
		void stop() {}
		int write_(uint8_t data[], int length = 1) { 
			//union i2c_smbus_data data;
			//data.block[0] = 1;
			//uint8_t buffer[1] = {data};
			//int res = i2c_smbus_write_block_data(fid,0,length,data);
			int bytes_written = write(fid,data,length);	
			return bytes_written == length; 
			//return 0;
		}
		int read_(int address, uint8_t *data, int length, bool repeated = false) { 
			int bytes_read = read(fid,data,length);
			return !(bytes_read == length);
		}
	public:
		int fid;
}; 

class SENSOR_FLOAT {
	public:
	SENSOR_FLOAT() : x(0),y(0),z(0),w(0),status(-1) {}
	void set(float f[],int cnt = 3) {
		x = f[0];
		y = f[1];
		z = f[2];
		if (cnt <= 3) { w = 0; } 
		else { w = f[3]; }
	}
	void get_xyz(Eigen::Vector3d & rot_x_axis, Eigen::Vector3d &rot_y_axis, Eigen::Vector3d &rot_z_axis);
	float x,y,z,w;
	int status;
};

class BNO080 {
	public:
		enum CHANNEL {COMMAND_ = 0,EXECUTABLE, CONTROL, REPORTS, WAKE_REPORTS, GYRO};
		enum REPORT {MAGNETIC_FIELD_UN = 0x0F, ACCELEROMETER = 0x01,GYROSCOPE, MAGNETIC_FIELD, LINEAR_ACCELERATION, ROTATION_VECTOR, GRAVITY, GAME_ROTATION_VECTOR = 0x08, GEOMAGNETIC_ROTATION_VECTOR, TAP_DETECTOR, STEP_COUNTER };
		enum COMMAND {SAVE_DCD = 0x06, ME_CALIBRATION = 0x07 };

		BNO080(uint8_t deviceAddress_) : deviceAddress(deviceAddress_), failed_reads(0) 
		{
			for (auto ii  = 0; ii < 15; ++ii) { enabled[ii] = false; }
  			set_output_pins(std::vector<int>{9});
  			set_input_pins(std::vector<int>{10});
		}
		bool interrupt() { return !GET_GPIO(10); }
		void wait_ms(int t) { std::this_thread::sleep_for(std::chrono::milliseconds(t)); }	
		int send(CHANNEL channel, uint8_t data[], uint16_t packetLength, bool start = false);
		int read();
		int send_command(COMMAND command, uint8_t op1,uint8_t op2,uint8_t op3 );
		int request_report(REPORT report, uint16_t timeBetweenReports = 50, uint32_t specificConfig = 0);
		bool get_reports();
		void error() { 
			std::cout << "IMU: Error with last command..." << std::endl; 
			//reset();
			//while (1) { wait_ms(1000); }    
		}
		void reset() {
  			GPIO_CLR = 1<<9;
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
  			GPIO_SET = 1<<9;
			std::this_thread::sleep_for(std::chrono::milliseconds(400)); 
		}
		SENSOR_FLOAT accel,gyro,game,rot,mag,linear;
	private:
		I2C i2c;
		uint8_t shtpData[128];
		uint8_t sequenceNumber[6] = {0,0,0,0,0,0};
		uint8_t snumber = 0;
		uint8_t response[2048];
		uint16_t dataLength;
		uint8_t deviceAddress;
		bool enabled[15];
	public:	
  		uint32_t good_reads = 0, failed_reads = 0, failed_interrupts = 0;
};

#endif
