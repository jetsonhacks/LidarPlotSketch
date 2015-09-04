#define NULL 0
#include <lidarlite.h>

LidarLite::LidarLite()
{
    kI2CBus = 1 ;           // Default I2C bus for Lidar-Lite on Jetson TK1
    error = 0 ;
}

LidarLite::~LidarLite()
{
    closeLidarLite() ;
}

// Returns true if device file descriptor opens correctly, false otherwise

bool LidarLite::openLidarLite()
{
    char fileNameBuffer[32];
    sprintf(fileNameBuffer,"/dev/i2c-%d", kI2CBus);
    kI2CFileDescriptor = open(fileNameBuffer, O_RDWR);
    if (kI2CFileDescriptor < 0) {
        // Could not open the file
       error = errno ;
       return false ;
    }
    if (ioctl(kI2CFileDescriptor, I2C_SLAVE, kLidarLiteI2CAddress) < 0) {
        // Could not open the device on the bus
        error = errno ;
        return false ;
    }
    return true ;
}

void LidarLite::closeLidarLite()
{
    if (kI2CFileDescriptor > 0) {
        close(kI2CFileDescriptor);
        // WARNING - This is not quite right, need to check for error first
        kI2CFileDescriptor = -1 ;
    }
}

// Read the given register on the Lidar-Lite
int LidarLite::readLidarLite(int readRegister)
{
   int toReturn = i2c_smbus_read_byte_data(kI2CFileDescriptor, readRegister);
   // was originally 10000 ; 10 ms
    usleep(1000) ;
    if (toReturn < 0) {
        error = errno ;
        toReturn = -1 ;
    }
    return toReturn ;
}

// Write the the given value to the given register on the Lidar-Lite
int LidarLite::writeLidarLite(int writeRegister, int writeValue)
{
    int toReturn = i2c_smbus_write_byte_data(kI2CFileDescriptor, writeRegister, writeValue);
    usleep(10000);
    if (toReturn < 0) {
        error = errno ;
        toReturn = -1 ;
    }
    return toReturn ;

}

// Return the current calculated distance in centimeters
int LidarLite::getDistance()
{
    int ioResult ;
    int msb, lsb ;
    ioResult = writeLidarLite(kLidarLiteCommandControlRegister,kLidarLiteMeasure);
    if (ioResult < 0) {
        return ioResult ;
    }
    ioResult = readLidarLite(kLidarLiteCalculateDistanceMSB);
    if (ioResult < 0) {
        return ioResult ;
    } else {
        msb = ioResult ;
    }
    ioResult = readLidarLite(kLidarLiteCalculateDistanceLSB);
    if (ioResult < 0) {
        return ioResult ;
    } else {
        lsb = ioResult ;
    }

    int distance = (msb << 8) + lsb ;

    return distance ;
}

// Return the previous measurement in centimeters
int LidarLite::getPreviousDistance() {

    int ioResult ;
    int msb, lsb ;
    ioResult = readLidarLite(kLidarLitePreviousMeasuredDistanceMSB);
    if (ioResult < 0) {
        return ioResult ;
    } else {
        msb = ioResult ;
    }
    ioResult = readLidarLite(kLidarLitePreviousMeasuredDistanceLSB);
    if (ioResult < 0) {
        return ioResult ;
    } else {
        lsb = ioResult ;
    }

    int distance = (msb << 8) + lsb ;

    return distance ;
}

// Return the velocity (rate of change) in centimeters; +/-
int LidarLite::getVelocity()
{
  int ioResult = readLidarLite(kLidarLiteVelocityMeasurementOutput);
  // 8-bit 2's complement
  if (ioResult == 255) {
      return 0 ;
  }
  if (ioResult > 127) {
      return ioResult - 256 ;
  }
  return ioResult ;
}

// Return the Lidar-Lite hardware version
int LidarLite::getHardwareVersion()
{
    return readLidarLite(kLidarLiteHardwareVersion) ;
 }

// Return the Lidar-Lite software version
int LidarLite::getSoftwareVersion() {
    return readLidarLite(kLidarLiteSoftwareVersion) ;
}

// Return the last i/o error
int LidarLite::getError()
{
    return error ;
}
