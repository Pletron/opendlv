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

#ifndef LIDAR_LIDAR_HPP_
#define LIDAR_LIDAR_HPP_

#include <memory>
#include <list>

#include "opendavinci/odcore/base/module/DataTriggeredConferenceClientModule.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/wrapper/SerialPort.h"

#include "opendlvdata/GeneratedHeaders_opendlvdata.h"

namespace opendlv {
namespace proxy {
namespace lidar {

class Device;

/**
 * This class provides...
 */
class Lidar : public odcore::base::module::DataTriggeredConferenceClientModule,
              public odcore::io::StringListener {
 public:
  Lidar(int32_t const &, char **);
  Lidar(Lidar const &) = delete;
  Lidar &operator=(Lidar const &) = delete;
  virtual ~Lidar();

  //odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
  virtual void nextString(const std::string &s);
  virtual void SendData();
  virtual void ConvertToDistances();
  virtual void WriteToFile();
  virtual void nextContainer(odcore::data::Container &c);

 private:
  void setUp();
  void tearDown();

  bool m_firstHeader;
  bool m_indicator;
  bool m_startConfirmed;
  uint32_t m_counter;
  std::vector<opendlv::coordinate::Spherical3> m_freshCoordinates;
  double m_distances[361];
  double m_position[3];
  unsigned char m_measurements[1000];
  unsigned char m_startResponse[10];
  unsigned char m_measurementHeader[7];
  unsigned char m_buffer[10];

  std::unique_ptr<Device> m_device;
  std::shared_ptr<odcore::wrapper::SerialPort> m_sick;
};

} // lidar
} // proxy
} // opendlv

#endif
