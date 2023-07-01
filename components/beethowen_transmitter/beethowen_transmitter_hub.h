/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago
 */

#pragma once

#include <vector>
#include <map>

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome
{
  namespace beethowen_transmitter
  {
    class BeethowenTransmitterHub : public PollingComponent
    {
    public:
#define BEETHOWEN_TAKT_TIME 100
#define BEETHOWEN_MINIMUM_TIMEOUT_AUTO_SEND (10 * 1000)
      BeethowenTransmitterHub() : PollingComponent(BEETHOWEN_TAKT_TIME)
      {
      }

      inline uint64_t get_server_address() { return this->server_address_; };
      inline uint8_t *get_server_address_arr() { return this->server_address_arr_; };
      inline void set_server_address(uint64_t value)
      {
        server_address_ = value;

        server_address_arr_[0] = (uint8_t)(server_address_ >> 40) & 0xff;
        server_address_arr_[1] = (uint8_t)(server_address_ >> 32) & 0xff;
        server_address_arr_[2] = (uint8_t)(server_address_ >> 24) & 0xff;
        server_address_arr_[3] = (uint8_t)(server_address_ >> 16) & 0xff;
        server_address_arr_[4] = (uint8_t)(server_address_ >> 8) & 0xff;
        server_address_arr_[5] = (uint8_t)(server_address_ >> 0) & 0xff;
      };
      inline uint8_t get_server_channel() { return server_channel_; };
      inline void set_server_channel(uint8_t value) { server_channel_ = value; };
      inline uint64_t get_server_data() { return uint64_t(server_channel_ & 0xFF) << 48 | server_address_; };
      inline void set_server_data(uint64_t value)
      {
        server_channel_ = (uint8_t)(value >> 48) & 0xff;
        set_server_address(value & 0xffffffffffffLL);
        this->server_found_ = true;
        this->init_server_after_set();
      };

      inline void set_connect_persistent(bool value) { connect_persistent_ = value; };
      inline void set_auto_send(bool value) { auto_send_ = value; };
      inline void set_local_passkey(uint16_t value) { local_passkey_ = value; };
      inline void set_remote_expected_passkey(uint16_t value) { remote_expected_passkey_ = value; };

      void setup() override;
      void update() override;
      // void loop() override;

      float get_setup_priority() const override { return setup_priority::DATA; }

      void add_sensor(uint8_t measurement_type, sensor::Sensor *sensor) { my_sensors.push_back({measurement_type, sensor, nullptr}); }
      void add_sensor(uint8_t measurement_type, binary_sensor::BinarySensor *binary_sensor) { my_sensors.push_back({measurement_type, nullptr, binary_sensor}); }

      bool send(bool do_complete_send);

      void add_on_send_finished_callback(std::function<void(bool)> callback)
      {
        this->on_send_finished_callback_.add(std::move(callback));
      }
      void add_on_send_failed_callback(std::function<void()> callback)
      {
        this->on_send_failed_callback_.add(std::move(callback));
      }

    protected:
      void beethowen_on_command_(const uint8_t command, const uint8_t *buffer, const int size);
      bool is_server_found() { return this->server_found_; }
      void connect_to_wifi(uint8_t channel, bool persistent);
      void init_server_after_set();

      CallbackManager<void(bool)> on_send_finished_callback_;
      CallbackManager<void()> on_send_failed_callback_;

    private:
      uint64_t server_address_{0};
      uint8_t server_address_arr_[6];
      uint8_t server_channel_{0};
      bool server_found_{false};
      bool initial_server_checkin_completed_{false};
      bool connect_persistent_{false};
      bool auto_send_{false};
      uint16_t local_passkey_{0};
      uint16_t remote_expected_passkey_{0};

      uint32_t last_send_millis_{0};

      typedef struct
      {
        uint8_t measurement_type;
        sensor::Sensor *sensor;
        binary_sensor::BinarySensor *binary_sensor;
      } BTHomeTypedSensor;

      bool has_sensor_state(BTHomeTypedSensor sensor_struct)
      {
        if (sensor_struct.sensor)
          return sensor_struct.sensor->has_state();
        if (sensor_struct.binary_sensor)
          return sensor_struct.binary_sensor->has_state();
        return false;
      }

      optional<float> get_sensor_state(BTHomeTypedSensor sensor_struct)
      {
        if (sensor_struct.sensor && sensor_struct.sensor->has_state())
          return sensor_struct.sensor->get_state();
        if (sensor_struct.binary_sensor->has_state())
          return sensor_struct.binary_sensor->state;
        return nullopt;
      }

      std::vector<BTHomeTypedSensor> my_sensors;
    };

  }
}
