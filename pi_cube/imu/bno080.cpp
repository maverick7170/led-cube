#include "bno080.h"

void SENSOR_FLOAT::get_xyz(Eigen::Vector3d & rot_x_axis, Eigen::Vector3d &rot_y_axis, Eigen::Vector3d &rot_z_axis) { 
		Eigen::Quaterniond p,q(w,x,y,z);
                q.normalize();
		Eigen::Vector3d x_axis(1,0,0),y_axis(0,1,0),z_axis(0,0,1);
		p.w() = 0; p.vec() = x_axis;
		rot_x_axis = (q*p*q.inverse()).vec();
		p.w() = 0; p.vec() = y_axis;
		rot_y_axis = (q*p*q.inverse()).vec();
		p.w() = 0; p.vec() = z_axis;
		rot_z_axis = (q*p*q.inverse()).vec();
}

int BNO080::send(CHANNEL channel, uint8_t data[], uint16_t packetLength, bool start)
{
    if (start) {
        i2c.start();
    }
    int ack;
    packetLength += 4;
    //uint8_t header[5] = {deviceAddress,packetLength & 0xFF,packetLength >> 8,channel,sequenceNumber[channel]++};
    std::vector<uint8_t> header = {uint8_t(packetLength & 0xFF),uint8_t((packetLength >> 8) & 0xFF),channel,sequenceNumber[channel]++};
    for (uint16_t ii = 0; ii < packetLength-4; ++ii) {
	header.push_back(data[ii]);
    }
    ack = i2c.write_(header.data(),header.size());  
    return ack;
}


int BNO080::read() {
    int total_bytes = 0, next_bytes = 4;
    while (true) {
        int ack = i2c.read_(deviceAddress, response, next_bytes);
	//printf("Read ack: %d with bytes %d\n",ack,next_bytes);
	if (ack != 0) { return -1; }
        uint8_t packetLSB = response[0]; 
        uint8_t packetMSB = response[1]; 
        //uint8_t channelNumber = response[2]; 
        //uint8_t sNumber = response[3]; 
        dataLength = ((uint16_t)packetMSB << 8 | packetLSB);
        int dataLength_msb = (dataLength >> 15);
        dataLength &= ~(1 << 15); //Clear the MSbit.  
      	//printf("dataLength %d\n",dataLength);
      	if ( dataLength == 0 ) { return -2; } 
	//std::cout << " " << std::endl;
	//printf("i2c.read_ with %d bytes has ack %d\r\n",next_bytes,ack); //0 success!
        //printf("Header: %d,%d,%d,%d \r\n",packetLSB,packetMSB,channelNumber,sNumber);
        //printf("Bytes in packet: %d \r\n",dataLength);
        //printf("MSB Flag: %d \r\n",dataLength_msb);
        //printf("Data: ");
        //for (int ii = 4; ii < next_bytes; ++ii) {
        //    printf("%d ",response[ii]);    
        //}
        //printf("\r\n");
        if ( dataLength_msb == 0 ) { total_bytes += next_bytes; }
        else { total_bytes += next_bytes-4; }
        if (total_bytes >= dataLength) { return 0; }
        next_bytes = dataLength-total_bytes+4;       
    	wait_ms(10);
    }
}



int BNO080::send_command(COMMAND command, uint8_t op1,uint8_t op2,uint8_t op3 )
{
  shtpData[0] = SHTP_REPORT_COMMAND_REQUEST; //Command Request
  shtpData[1] = snumber++; //Increments automatically each function call
  shtpData[2] = command; //Command    
  for (int ii = 3; ii < 12; ++ii) { shtpData[ii] = 0; }
    switch (command) {
        case ME_CALIBRATION:
            shtpData[3] = op1;
            shtpData[4] = op2;
            shtpData[5] = op3;
            break;    
    }
  int ack = send(CONTROL,shtpData,12,true);
  std::cout << "IMU: ************** Command " << command << " sent with ack " << ack << std::endl;
  if (ack != 1) { error(); }
  wait_ms(100);
  return ack;  
}



int BNO080::request_report(REPORT report, uint16_t timeBetweenReports, uint32_t specificConfig)
{
    long microsBetweenReports = (long)timeBetweenReports * 1000L;
    shtpData[0] = SHTP_REPORT_SET_FEATURE_COMMAND; //Set feature command. Reference page 55
    shtpData[1] = report; //Feature Report ID. 0x01 = Accelerometer, 0x05 = Rotation vector
    shtpData[2] = 0; //Feature flags
    shtpData[3] = 0; //Change sensitivity (LSB)
    shtpData[4] = 0; //Change sensitivity (MSB)
    shtpData[5] = (microsBetweenReports >> 0) & 0xFF; //Report interval (LSB) in microseconds. 0x7A120 = 500ms
    shtpData[6] = (microsBetweenReports >> 8) & 0xFF; //Report interval
    shtpData[7] = (microsBetweenReports >> 16) & 0xFF; //Report interval
    shtpData[8] = (microsBetweenReports >> 24) & 0xFF; //Report interval (MSB)
    shtpData[9] = 0; //Batch Interval (LSB)
    shtpData[10] = 0; //Batch Interval
    shtpData[11] = 0; //Batch Interval
    shtpData[12] = 0; //Batch Interval (MSB)
    shtpData[13] = (specificConfig >> 0) & 0xFF; //Sensor-specific config (LSB)
    shtpData[14] = (specificConfig >> 8) & 0xFF; //Sensor-specific config
    shtpData[15] = (specificConfig >> 16) & 0xFF; //Sensor-specific config
    shtpData[16] = (specificConfig >> 24) & 0xFF; //Sensor-specific config (MSB)
    int ack = send(CONTROL,shtpData,17,true);
    std::cout << "IMU: ******************** Report " << report << " requested with ack " << ack << std::endl;
    if (ack != 1) { error(); }
    if (timeBetweenReports == 0) { enabled[report] = false; }
    else { enabled[report]  = true; }
    return ack;
}


bool BNO080::get_reports() {
	return false; //REMOVE WHEN IMU IS PLACED ON NEW PCBs
	if (!interrupt()) { 
		++failed_interrupts;
		if (failed_interrupts >= 40) {
			std::cout << "IMU: ************* Continue anyway..." << std::endl;
			failed_interrupts = 0;	
		} else {
			return false; 
		}
	}
	failed_interrupts = 0;
	bool ack = read();
	if (ack != 0) { 
		++failed_reads;
		std::cout << "IMU: ***************** FAILED READ!!! (" << failed_reads << ")" << std::endl;
		if (failed_reads % 10 == 0) {
			good_reads = 0;
			reset();
		}
		if (failed_reads % 4 == 0) {
			if (enabled[ACCELEROMETER]) { 
				std::cout << "IMU: ***************** RESETTING ACCELEROMETER " << std::endl;
				good_reads = 0;
				request_report(BNO080::ACCELEROMETER,75); 
			} else {
				std::cout << "AAAA" << std::endl;
			}
			if (enabled[LINEAR_ACCELERATION]) { 
				std::cout << "IMU: ***************** RESETTING LINEAR ACCELERATION!!!"<< std::endl;
				good_reads = 0;
				request_report(BNO080::LINEAR_ACCELERATION,75); 
			} else {
				std::cout << "BBBB" << std::endl;
			}
		}
		return false;
	}
	if (response[2] == CONTROL && response[4] == 241) {
            if (response[6] == SAVE_DCD) {
                wait_ms(1000);
            }
        } else if (response[2] == CONTROL && response[4] == 252) {
        } else if (response[2] == REPORTS) {
            uint16_t offset = 9;
            int16_t qPoint = 14;
            uint8_t type, status;
            uint16_t raw_data[5];
            float float_data[5]; 
            while (offset < dataLength) {
                type = response[offset+0];
                if (type == MAGNETIC_FIELD || type == MAGNETIC_FIELD_UN) {
                    qPoint = 4;
                } else if (type == ROTATION_VECTOR || type == GAME_ROTATION_VECTOR) {
                    qPoint = 14;    
                } else if (type == ACCELEROMETER || type == LINEAR_ACCELERATION) {
                    qPoint = 8;    
                } else if (type == GYROSCOPE) {
                    qPoint = 9;    
                }
                status = response[offset+2] & 0x03; //Get status bits
		for (int ii = 0, jj = 4;ii < 5; ++ii, jj += 2) {
                    raw_data[ii] = (uint16_t)response[offset + jj+1] << 8 | response[offset + jj];
                    float_data[ii] = (int16_t)raw_data[ii];
                    float_data[ii] *= pow(2.0f, qPoint * -1);
                }

		if (type == MAGNETIC_FIELD || type == MAGNETIC_FIELD_UN) {
                    //printf("MAGNETIC FIELD: %f,%f,%f with status %d \r\n",float_data[0],float_data[1],float_data[2],status);   
			mag.set(float_data,3);
			mag.status = status;
    			offset += 10;
                } else if (type == GAME_ROTATION_VECTOR) { 
		    //rot_status = status;
		    	game.set(float_data,4);
			game.status = status;	
                    //printf("GAME ROTATION VECTOR: %f,%f,%f,%f with status %d \r\n",float_data[3],float_data[0],float_data[1],float_data[2],status); 
                    //double norm = sqrt(float_data[0]*float_data[0] + float_data[1]*float_data[1] + float_data[2]*float_data[2]);
                    //double vx = float_data[0]/norm, vy = float_data[1]/norm, vz = float_data[2]/norm;
                    //float theta = 2*atan2(norm,(double)float_data[3])*180./3.14159;
                    //printf("Vector Angle: %f with axis: %f,%f,%f \r\n",theta,vx,vy,vz);
		    //std::cout << "Rotated gravity vector: " << rot_gravity << std::endl;
                    offset += 12;
                } else if (type == ROTATION_VECTOR) {
			rot.set(float_data,4);
			rot.status = status;
		    //rot_status = status;	
                    //printf("TRUE ROTATION VECTOR: %f,%f,%f,%f with status %d \r\n",float_data[3],float_data[0],float_data[1],float_data[2],status);
                    //double norm = sqrt(float_data[0]*float_data[0] + float_data[1]*float_data[1] + float_data[2]*float_data[2]);
                    //double vx = float_data[0]/norm, vy = float_data[1]/norm, vz = float_data[2]/norm;
                    //float theta = 2*atan2(norm,(double)float_data[3])*180./3.14159;
                    //printf("Vector Angle: %f with axis: %f,%f,%f \r\n",theta,vx,vy,vz);
		    //std::cout << "Rotated gravity vector: " << rot_gravity << std::endl;
                    offset += 14;
                } else if (type == ACCELEROMETER) {
			accel.set(float_data,3);
			accel.status = status;
                    //printf("Accelerometer: %f,%f,%f with status %d \r\n",float_data[0],float_data[1],float_data[2],status); 
                    offset += 10;
                } else if (type == LINEAR_ACCELERATION) {
			linear.set(float_data,3);
			linear.status = status;
			offset += 10;	
		}else if (type == GYROSCOPE) {
			gyro.set(float_data,3);
			gyro.status = status;
                    //printf("Gyroscope: %f,%f,%f with status %d \r\n",float_data[0],float_data[1],float_data[2],status); 
                    	offset += 10;
                } else {
			return false;
                }
            }
	    ++good_reads;
	    failed_reads = 0;
	    return true;
        }
       return false;	
}
