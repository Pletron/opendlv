/**
 * Copyright (C) 2015 Chalmers REVERE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <chrono>
#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
 
#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendlv/data/environment/WGS84Coordinate.h"

#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include "linguistics/v2vcam/buffer.hpp"
#include "linguistics/v2vcam/v2vcam.hpp"


namespace opendlv {
namespace knowledge {
namespace linguistics {
namespace v2vcam {

/**
  * Constructor.
  *
  * @param a_argc Number of command line arguments.
  * @param a_argv Command line arguments.
  */
V2vCam::V2vCam(int32_t const &a_argc, char **a_argv)
    : TimeTriggeredConferenceClientModule(
      a_argc, a_argv, "knowledge-linguistics-v2vcam"),
    m_sendLog(),
    m_receiveLog(),
    m_timeType2004()
{
  struct stat st;
  if (::stat("var/application/knowledge/linguistics/v2vcam", &st) == -1) {
    ::system("mkdir -p ./var/application/knowledge/linguistics/v2vcam");
    std::cout<<"Created dir"<<std::endl;
  }
  odcore::data::TimeStamp nu;

  std::stringstream filenameSend;
  std::stringstream filenameReceive;

  filenameSend << "/" << nu.getYYYYMMDD_HHMMSS() << " cam send.log";
  filenameReceive << "/" << nu.getYYYYMMDD_HHMMSS() << " cam receive.log";
  m_sendLog.open("var/application/knowledge/linguistics/v2vcam" 
    + filenameSend.str(), std::ios::out | std::ios::app);
  m_receiveLog.open("var/application/knowledge/linguistics/v2vcam" 
    + filenameReceive.str(), std::ios::out | std::ios::app);

  std::string header = "#message id, \
      station id, \
      generation delta time, \
      container mask,\
      station type, \
      latitude, \
      longitude, \
      semi major confidence, \
      semi minor confidence, \
      semi major orientation, \
      altitude, \
      heading, \
      heading confidence, \
      speed, speed confidence, \
      vehicle length, \
      vehicle width, \
      longitudinal acc, \
      longitudinal acc conf, \
      yaw rate value,\
      yaw rate conf, \
      vehicle role";

  m_sendLog << header << std::endl;
  m_receiveLog << header << std::endl;


  std::tm tm = {
      0, // tm_sec
      0, // tm_min
      0, // tm_hour
      1, // tm_mday
      0, // tm_mon
      2004 -1900, // tm_year
      4, // tm_wday
      0, // tm_yday
      -1, // tm_isdst
      0, // tm_gmtoff (NOTE: only in glibc)
      nullptr // time zone (NOTE: only in glibc)
      };

  m_timeType2004 = timegm(&tm);
}

V2vCam::~V2vCam()
{
  m_sendLog.close();
  m_receiveLog.close();
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode V2vCam::body()
{

  while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
      odcore::data::dmcp::ModuleStateMessage::RUNNING) {
    std::shared_ptr<opendlv::Buffer> outBuffer(new opendlv::Buffer());
    // Reverser for big and little endian specification of V2V.
    outBuffer->Reversed();
    outBuffer->AppendByte(GetMessageId()); //messageId
    outBuffer->AppendInteger(GetStationId()); //stationId
    outBuffer->AppendInteger(GenerateGenerationDeltaTime()); //generationDeltaTime
    outBuffer->AppendByte(GetContainerMask()); //containerMask
    outBuffer->AppendInteger(GetStationType()); //stationType                
    outBuffer->AppendInteger(GetLatitude()); //latitude
    outBuffer->AppendInteger(GetLongitude()); //longitude
    outBuffer->AppendInteger(GetSemiMajorConfidence()); //semiMajorConfidence
    outBuffer->AppendInteger(GetSemiMinorConfidence()); //semiMinorConfidence
    outBuffer->AppendInteger(GetSemiMajorOrientation()); //semiMajorOrientation
    outBuffer->AppendInteger(GetAltitude()); //altitude
    outBuffer->AppendInteger(GetHeading()); //heading value
    outBuffer->AppendInteger(GetHeadingConfidence()); //headingConfidence
    outBuffer->AppendInteger(GetSpeed()); //speedValue
    outBuffer->AppendInteger(GetSpeedConfidence()); //speedConfidence        
    outBuffer->AppendInteger(GetVehicleLength()); //vehicleLength
    outBuffer->AppendInteger(GetVehicleWidth()); //vehicleWidth
    outBuffer->AppendInteger(GetLongitudinalAcc()); //longitudinalAcc
    outBuffer->AppendInteger(GetLongitudinalAccConf()); //longitudinalAccConf
    outBuffer->AppendInteger(GetYawRateValue()); //yawRateValue
    outBuffer->AppendInteger(GetYawRateConfidence()); //yawRateConfidence        
    outBuffer->AppendInteger(GetVehicleRole()); //vehicleRole

  

    std::vector<unsigned char> bytes = outBuffer->GetBytes();
    std::string bytesString(bytes.begin(),bytes.end());
    // std::cout<< bytesString << std::endl;
    opendlv::knowledge::Message nextMessage(bytesString.size(),bytesString);
    odcore::data::Container c(nextMessage);
    getConference().send(c);

    m_sendLog << std::to_string(GetMessageId()) +
        + "," + std::to_string(GetStationId()) +
        + "," + std::to_string(GenerateGenerationDeltaTime()) +
        + "," + std::to_string(GetContainerMask()) +
        + "," + std::to_string(GetStationType()) +
        + "," + std::to_string(GetLatitude()) +
        + "," + std::to_string(GetLongitude()) +
        + "," + std::to_string(GetSemiMajorConfidence()) +
        + "," + std::to_string(GetSemiMinorConfidence()) +
        + "," + std::to_string(GetSemiMajorOrientation()) +
        + "," + std::to_string(GetAltitude()) +
        + "," + std::to_string(GetHeading()) +
        + "," + std::to_string(GetHeadingConfidence()) +
        + "," + std::to_string(GetSpeed())
        + "," + std::to_string(GetSpeedConfidence()) +
        + "," + std::to_string(GetVehicleLength()) +
        + "," + std::to_string(GetVehicleWidth()) +
        + "," + std::to_string(GetLongitudinalAcc()) +
        + "," + std::to_string(GetLongitudinalAccConf()) +
        + "," + std::to_string(GetYawRateValue()) +
        + "," + std::to_string(GetYawRateConfidence()) +
        + "," + std::to_string(GetVehicleRole());
    m_sendLog << std::endl;
    // std::cout<< "Latitude: " << m_latitude << " Longitude: " << m_longitude << std::endl;

    std::string output = "*** Cam object ***\n";

      output += "Message Id: " + std::to_string(GetMessageId()) + "\n";
      output += "Station Id: " + std::to_string(GetStationId()) + "\n";
      output += "Generation delta time: " 
          + std::to_string(GenerateGenerationDeltaTime()) + "\n";
      output += "Container mask: " + std::to_string(GetContainerMask()) + "\n";
      output += "Station type: " + std::to_string(GetStationType()) + "\n";
      output += "Latitude: " + std::to_string(GetLatitude()) + "\n";
      output += "Longitude: " + std::to_string(GetLongitude()) + "\n";
      output += "Semi major confidence: " 
          + std::to_string(GetSemiMajorConfidence()) + "\n";
      output += "Semi minor confidence: " 
          + std::to_string(GetSemiMinorConfidence()) + "\n";
      output += "Semi major orientation: " 
          + std::to_string(GetSemiMajorOrientation()) + "\n";
      output += "Altitude: " + std::to_string(GetAltitude()) + "\n";
      output += "Heading: " + std::to_string(GetHeading()) + "\n";
      output += "Heading confidence: " 
          + std::to_string(GetHeadingConfidence()) + "\n";
      output += "Speed: " + std::to_string(GetSpeed()) + "\n";
      output += "Speed confidence: " + std::to_string(GetSpeedConfidence()) + "\n";
      output += "Vehicle length: " + std::to_string(GetVehicleLength()) + "\n";
      output += "Vehicle width: " + std::to_string(GetVehicleWidth()) + "\n";
      output += "Longitudinal acc: " + std::to_string(GetLongitudinalAcc()) + "\n";
      output += "Longitudinal acc conf: " 
          + std::to_string(GetLongitudinalAccConf()) + "\n";
      output += "Yaw rate value: " + std::to_string(GetYawRateValue()) + "\n";
      output += "Yaw rate confidence: " 
          + std::to_string(GetYawRateConfidence()) + "\n";
      output += "Vehicle role: " + std::to_string(GetVehicleRole()) + "\n";
      std::cout << output;
  }
  return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

/**
 * Receives geolocation-, voice containers.
 * Sends .
 */
void V2vCam::nextContainer(odcore::data::Container &c)
{
  if (c.getDataType() == opendlv::sensation::Geolocation::ID()) {
    opendlv::sensation::Geolocation geolocation = 
        c.getData<opendlv::sensation::Geolocation>();
    ReadGeolocation(geolocation);
  } else if (c.getDataType() == opendlv::model::DynamicState::ID()) {
    opendlv::model::DynamicState dynamicState = 
        c.getData<opendlv::model::DynamicState>();
    int16_t frameId = dynamicState.getFrameId();
    if (frameId == 0) {
      ReadDynamicState(dynamicState);
    }
  } else if(c.getDataType() == opendlv::sensation::Voice::ID()) {
    opendlv::sensation::Voice voice = c.getData<opendlv::sensation::Voice>();
    ReadVoice(voice);
  }
}

void V2vCam::ReadDynamicState(
    opendlv::model::DynamicState const &a_dynamicState)
{
  m_speed = a_dynamicState.getVelocity().getX();
  m_speedConfidence = a_dynamicState.getVelocityConfidence().getX();
  m_yawRateValue = a_dynamicState.getAngularVelocity().getZ();
  m_yawRateConfidence = a_dynamicState.getAngularVelocityConfidence().getZ();
  m_longitudinalAcc = a_dynamicState.getAcceleration().getX();
  m_longitudinalAccConf = a_dynamicState.getAccelerationConfidence().getX();
}

void V2vCam::ReadGeolocation(
    opendlv::sensation::Geolocation const &a_geolocation)
{
  m_latitude = a_geolocation.getLatitude();
  m_longitude = a_geolocation.getLongitude();
  m_altitude = a_geolocation.getAltitude();
  m_heading = a_geolocation.getHeading();
  m_headingConfidence = a_geolocation.getHeadingConfidence();
}

void V2vCam::ReadVoice(opendlv::sensation::Voice const &a_voice)
{
  if(strcmp(a_voice.getType().c_str(),"cam") == 0){
    std::string dataString = a_voice.getData();
    std::vector<unsigned char> data(dataString.begin(), dataString.end());
    std::shared_ptr<Buffer const> buffer(new Buffer(data));
    std::shared_ptr<Buffer::Iterator> inIterator = buffer->GetIterator();
    //Long and little endian reverser
    inIterator->ItReversed();

    unsigned char messageId = inIterator->ReadByte();
    int32_t stationId = inIterator->ReadInteger();
    int32_t generationDeltaTime = inIterator->ReadInteger();
    unsigned char containerMask =  inIterator->ReadByte();
    int32_t stationType = inIterator->ReadInteger();
    int32_t latitude = inIterator->ReadInteger();
    int32_t longitude = inIterator->ReadInteger();
    int32_t semiMajorConfidence = inIterator->ReadInteger();
    int32_t semiMinorConfidence = inIterator->ReadInteger();
    int32_t semiMajorOrientation = inIterator->ReadInteger();
    int32_t altitude = inIterator->ReadInteger();
    int32_t heading = inIterator->ReadInteger();
    int32_t headingConfidence = inIterator->ReadInteger();
    int32_t speed = inIterator->ReadInteger();
    int32_t speedConfidence = inIterator->ReadInteger();
    int32_t vehicleLength = inIterator->ReadInteger();
    int32_t vehicleWidth = inIterator->ReadInteger();
    int32_t longitudinalAcc = inIterator->ReadInteger();
    int32_t longitudinalAccConf = inIterator->ReadInteger();
    int32_t yawRateValue = inIterator->ReadInteger();
    int32_t yawRateConfidence = inIterator->ReadInteger();
    int32_t vehicleRole = inIterator->ReadInteger();

    std::string output = "*** Cam object ***\n";

    output += "Message Id: " + std::to_string(messageId) + "\n";
    output += "Station Id: " + std::to_string(stationId) + "\n";
    output += "Generation delta time: " 
        + std::to_string(generationDeltaTime) + "\n";
    output += "Container mask: " + std::to_string(containerMask) + "\n";
    output += "Station type: " + std::to_string(stationType) + "\n";
    output += "Latitude: " + std::to_string(latitude) + "\n";
    output += "Longitude: " + std::to_string(longitude) + "\n";
    output += "Semi major confidence: " 
        + std::to_string(semiMajorConfidence) + "\n";
    output += "Semi minor confidence: " 
        + std::to_string(semiMinorConfidence) + "\n";
    output += "Semi major orientation: " 
        + std::to_string(semiMajorOrientation) + "\n";
    output += "Altitude: " + std::to_string(altitude) + "\n";
    output += "Heading: " + std::to_string(heading) + "\n";
    output += "Heading confidence: " 
        + std::to_string(headingConfidence) + "\n";
    output += "Speed: " + std::to_string(speed) + "\n";
    output += "Speed confidence: " + std::to_string(speedConfidence) + "\n";
    output += "Vehicle length: " + std::to_string(vehicleLength) + "\n";
    output += "Vehicle width: " + std::to_string(vehicleWidth) + "\n";
    output += "Longitudinal acc: " + std::to_string(longitudinalAcc) + "\n";
    output += "Longitudinal acc conf: " 
        + std::to_string(longitudinalAccConf) + "\n";
    output += "Yaw rate value: " + std::to_string(yawRateValue) + "\n";
    output += "Yaw rate confidence: " 
        + std::to_string(yawRateConfidence) + "\n";
    output += "Vehicle role: " + std::to_string(vehicleRole) + "\n";
    std::cout << output;

    m_receiveLog << std::to_string(messageId) +
        + "," + std::to_string(stationId) +
        + "," + std::to_string(generationDeltaTime) +
        + "," + std::to_string(containerMask) +
        + "," + std::to_string(stationType) +
        + "," + std::to_string(latitude) +
        + "," + std::to_string(longitude) +
        + "," + std::to_string(semiMajorConfidence) +
        + "," + std::to_string(semiMinorConfidence) +
        + "," + std::to_string(semiMajorOrientation) +
        + "," + std::to_string(altitude) +
        + "," + std::to_string(heading) +
        + "," + std::to_string(headingConfidence) +
        + "," + std::to_string(speed)
        + "," + std::to_string(speedConfidence) +
        + "," + std::to_string(vehicleLength) +
        + "," + std::to_string(vehicleWidth) +
        + "," + std::to_string(longitudinalAcc) +
        + "," + std::to_string(longitudinalAccConf) +
        + "," + std::to_string(yawRateValue) +
        + "," + std::to_string(yawRateConfidence) +
        + "," + std::to_string(vehicleRole);
        m_receiveLog << std::endl; 

  } else {
    std::cout << "Message type not CAM." << std::endl;
  }
}

void V2vCam::SendWGS84Coordinate()
{
  opendlv::data::environment::WGS84Coordinate coordPacket(m_latitude, 
      opendlv::data::environment::WGS84Coordinate::NORTH, m_longitude, 
      opendlv::data::environment::WGS84Coordinate::EAST);
  // std::cout << std::setprecision(11) << coordPacket.getLatitude() 
  //     << " " << coordPacket.getLongitude() << std::endl;
  odcore::data::Container nextC(coordPacket);
  getConference().send(nextC);
}


void V2vCam::setUp()
{
  odcore::base::KeyValueConfiguration kv = getKeyValueConfiguration();

  // std::string const type = kv.getValue<std::string>("proxy-v2v.type");
  
  m_stationId = kv.getValue<int32_t>("knowledge-linguistics-v2vcam.stationId");
  m_stationType = 
      kv.getValue<int32_t>("knowledge-linguistics-v2vcam.stationType");
  m_vehicleLength = 
      kv.getValue<int32_t>("knowledge-linguistics-v2vcam.vehicleLength");
  m_vehicleWidth = 
      kv.getValue<int32_t>("knowledge-linguistics-v2vcam.vehicleWidth");
  m_vehicleRole = 
      kv.getValue<int32_t>("knowledge-linguistics-v2vcam.vehicleRole");
  
}

void V2vCam::tearDown()
{
}


unsigned char V2vCam::GetMessageId() const
{
  return m_messageId;
}

int32_t V2vCam::GetStationId() const
{
  return m_stationId;
}

int32_t V2vCam::GenerateGenerationDeltaTime()
{
  std::chrono::system_clock::time_point start2004TimePoint = 
      std::chrono::system_clock::from_time_t(m_timeType2004);
  unsigned long millisecondsSince2004Epoch =
      std::chrono::system_clock::now().time_since_epoch() /
      std::chrono::milliseconds(1) 
      - start2004TimePoint.time_since_epoch() / std::chrono::milliseconds(1);
  m_generationDeltaTime = millisecondsSince2004Epoch%65536;
  return m_generationDeltaTime;
}

unsigned char V2vCam::V2vCam::GetContainerMask() const
{
  return m_containerMask;
}

int32_t V2vCam::GetStationType() const
{
  return m_stationType;
}

int32_t V2vCam::GetLatitude() const
{
  int32_t scale = std::pow(10,7);
  return static_cast<int32_t>(std::round(m_latitude*scale));
}

int32_t V2vCam::GetLongitude() const
{
  int32_t scale = std::pow(10,7);
  return static_cast<int32_t>(std::round(m_longitude*scale));
}

int32_t V2vCam::GetSemiMajorConfidence() const
{
  return m_semiMajorConfidence;
}

int32_t V2vCam::GetSemiMinorConfidence() const
{
  return m_semiMinorConfidence;
}

int32_t V2vCam::GetSemiMajorOrientation() const
{
  return m_semiMajorOrientation;
}

int32_t V2vCam::GetAltitude() const
{
  int32_t scale = std::pow(10,2);
  return static_cast<int32_t>(std::round(m_altitude*scale));
}

int32_t V2vCam::GetHeading() const
{
  double scale = std::pow(10,1)*opendlv::Constants::RAD2DEG;
  double val = static_cast<double>(m_heading);
  return static_cast<int32_t>(std::round(val*scale));
}

int32_t V2vCam::GetHeadingConfidence() const
{
  double scale = std::pow(10,1)*opendlv::Constants::RAD2DEG;
  double val = static_cast<double>(m_headingConfidence);
  return static_cast<int32_t>(std::round(val*scale));
}

int32_t V2vCam::GetSpeed() const
{
  int32_t scale = std::pow(10,2);
  return static_cast<int32_t>(std::round(m_speed*scale));
}

int32_t V2vCam::GetSpeedConfidence() const
{
  int32_t scale = std::pow(10,2);
  return static_cast<int32_t>(std::round(m_speedConfidence*scale));
}

int32_t V2vCam::GetVehicleLength() const
{
  return m_vehicleLength;
}

int32_t V2vCam::GetVehicleWidth() const
{
  return m_vehicleWidth;
}

int32_t V2vCam::GetLongitudinalAcc() const
{
  int32_t scale = std::pow(10,1);
  return static_cast<int32_t> (m_longitudinalAcc*scale);
}

int32_t V2vCam::GetLongitudinalAccConf() const
{
  int32_t scale = std::pow(10,1);
  return static_cast<int32_t> (m_longitudinalAccConf*scale);
}

int32_t V2vCam::GetYawRateValue() const
{
  int32_t scale = std::pow(10,2);
  double conversion = opendlv::Constants::RAD2DEG;
  return static_cast<int32_t> (m_yawRateValue*scale*conversion);
}

int32_t V2vCam::GetYawRateConfidence() const
{
  double conversion = opendlv::Constants::RAD2DEG;
  double val = m_yawRateConfidence * conversion;
  if(m_yawRateConfidence < 0){
    return 8;
  }else if (val <= 0.01){
    return 0;
  }else if (val <= 0.05){
    return 1;
  }else if (val <= 0.1){
    return 2;
  }else if (val <= 1){
    return 3;
  }else if (val <= 5){
    return 4;
  }else if (val <= 10){
    return 5;
  }else if (val <= 100){
    return 6;
  }else {
    return 7;
  }
}

int32_t V2vCam::GetVehicleRole() const
{
  return m_vehicleRole;
}

} // v2vcam
} // linguistics
} // knowledge
} // opendlv
