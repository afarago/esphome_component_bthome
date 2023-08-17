/*
 Beethowen
 Beethowen over ESPNow virtual sensors for ESPHome

 Author: Attila Farago

 */

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include "esphome/components/bthome_base/bthome_base_common.h"
#include "esphome/components/bthome_base/bthome_encoder.h"

#include "esphome/components/beethowen_base/beethowen_base_common.h"
#include "esphome/components/beethowen_base/meshrc_bthome_over_espnow.h"

#include "esphome/components/beethowen_base/meshrc_bthome_over_espnow.h"

#include "beethowen_transmitter_hub.h"

using namespace std;

namespace esphome
{
  namespace beethowen_transmitter
  {
    static const char *const TAG = "beethowen_transmitter";

// Europe 1-13, US 1-11, Japan 1-14
#define MAX_WIFI_CHANNELS 11

    void BeethowenTransmitterHub::setup()
    {
#define GLOBAL_BEETHOWEN_PACKETID_PREFS_ID 127671120UL
      this->prefs_packetid_state_ = global_preferences->make_preference<uint8_t>(GLOBAL_BEETHOWEN_PACKETID_PREFS_ID, false); // save it to rtc
#define GLOBAL_BEETHOWEN_PREFS_ID 127671119UL
      this->prefs_state_ = global_preferences->make_preference<server_address_and_channel_t>(GLOBAL_BEETHOWEN_PREFS_ID, true); // save it to flash

      // setup wifinow hooks
      beethowen_base::on_command([&](const uint8_t command, const uint8_t *buffer, const int size)
                                 { beethowen_on_command_(command, buffer, size); });

      // Load the state from storage
      if (this->restore_from_flash_)
        this->restore_state_();
      else
        ESP_LOGD(TAG, "Initial setup - started searching for Beethowen receiving server...");

      // restore packetid state from RTC
      restore_packetid_state_rtc_();
    }

    void BeethowenTransmitterHub::add_sensor(bthome_base::bthome_measurement_t measurement_type, sensor::Sensor *sensor)
    {
      BTHomeTypedSensor sobj = {measurement_type, sensor, nullptr};
      my_sensors_.push_back(sobj);
      sensor->add_on_state_callback([this, sobj](bool state)
                                    { this->sensor_has_updated(sobj); });
    }

    void BeethowenTransmitterHub::add_sensor(bthome_base::bthome_measurement_t measurement_type, binary_sensor::BinarySensor *binary_sensor)
    {
      BTHomeTypedSensor sobj = {measurement_type, nullptr, binary_sensor};
      my_sensors_.push_back(sobj);
      binary_sensor->add_on_state_callback([this, sobj](float state)
                                           { this->sensor_has_updated(sobj); });
    }

    void BeethowenTransmitterHub::beethowen_on_command_(const uint8_t command, const uint8_t *buffer, const int size)
    {
      ESP_LOGD(TAG, "Command received: 0x%02x, from: %s", command, bthome_base::addr_to_str(beethowen_base::sender).c_str());

      switch (command)
      {
      case beethowen_base::BeethowenCommand_FindServerRequestAck:
      {
        beethowen_base::beethowen_command_find_t *buffer2 = (beethowen_base::beethowen_command_find_t *)buffer;

        // validate remote passkey
        if (remote_expected_passkey_ != 0 && buffer2->header.passkey != remote_expected_passkey_)
        {
          ESP_LOGD(TAG, "Invalid remote found, passkey does not match expected passkey");
          // TODO: send error command with error code
          return;
        }

        server_channel_ = buffer2->server_channel;
        set_server_address(bthome_base::addr_to_uint64(beethowen_base::sender));
        server_found_ = true;
        // this->reinit_server_after_set();

        if (this->restore_from_flash_)
          this->save_state_(this->get_server_data());
        // ESP_LOGI(TAG, "Found server at {address} %s on {channel} %d with {passkey} %04X", bthome_base::addr64_to_str(server_address_).c_str(), buffer2->server_channel);
      }
      break;

      case beethowen_base::BeethowenCommand_DataAck:
      {
        beethowen_base::beethowen_command_data_ack_t *buffer2 = (beethowen_base::beethowen_command_data_ack_t *)buffer;
        ESP_LOGD(TAG, "Data packet ACKed %d", buffer2->packet_id_acked);
        this->last_packet_id_acked = buffer2->packet_id_acked;
      }
      break;

      default:
        ESP_LOGW(TAG, "Unknown command type 0x%02x", command);
        break;
      }
    }

    void BeethowenTransmitterHub::loop()
    {
      this->send_datacmd_loop();
    }

    void BeethowenTransmitterHub::update()
    {
      if (!is_server_found())
      {
        if (initial_server_checkin_completed_ || server_channel_ == 0)
        {
          server_channel_ += 2; // adjacent channels might overlap, just do this skipping sequence of 1-3-5-7-9-11-2-4-6-8-10-1
          if (server_channel_ > MAX_WIFI_CHANNELS)
            server_channel_ = ((server_channel_ - 1) % MAX_WIFI_CHANNELS) + 1; // brings 10->1 and 11->2
        }
        else
        {
          initial_server_checkin_completed_ = true;
        }

        connect_to_wifi(server_channel_, connect_persistent_);

        ESP_LOGD(TAG, "trying to find server on {channel} %d, {local_passkey_} 0x%04X, {remote_expected_passkey_} 0x%04X", server_channel_, local_passkey_, remote_expected_passkey_);
        beethowen_base::send_command_find(beethowen_base::broadcast, local_passkey_, server_channel_, true);
      }
      else
      {
        this->check_auto_send_data();
      }
    }

    void BeethowenTransmitterHub::reinit_server_after_set()
    {
      ESP_LOGI(TAG, "Connecting server to {address} %s on {channel} %d",
               bthome_base::addr64_to_str(this->server_address_).c_str(), this->server_channel_);
      connect_to_wifi(this->server_channel_, this->connect_persistent_);
    }

    void BeethowenTransmitterHub::connect_to_wifi(uint8_t channel, bool persistent)
    {
      // ESP_LOGD(TAG, "connect_to_wifi channel {channel}, persistent {persistent}")
      if (WiFi.isConnected())
        WiFi.disconnect();
      beethowen_base::setupwifi(channel, persistent);
      beethowen_base::begin(false);
    }

    bool BeethowenTransmitterHub::send_data(bool complete_only)
    {
      // send data - success only if not currently sending, otherwise failure as we will not queue this
      if (!beethowen_base::sending)
        return this->send_datacmd_(complete_only, true);
      else
        return false;
    }

    bool BeethowenTransmitterHub::send_datacmd_(optional<bool> add_complete_data, bool add_events)
    // data == null -> do not send data
    // data == false -> send any data
    // data == truew -> send complete
    {
      bool success = false;
      if (this->is_server_found())
      {
        encoder.resetMeasurement();

        // add the packet id as first measurement
        ++this->send_packet_id_;
        encoder.addMeasurementValue(bthome_base::BTHOME_PACKET_ID_VALUE, this->send_packet_id_);
        this->save_packetid_state_rtc_(this->send_packet_id_);

        bool has_meaningful_measurements = false;

        // collect data
        if (add_complete_data != nullopt)
        {
          bool has_outstanding_measurements = false;
          for (auto btsensor_struct : my_sensors_)
          {
            // ESP_LOGD(TAG, ".send {measurement_type} %d, {has_sensor_state} %d", btsensor_struct.measurement_type, has_sensor_state(btsensor_struct));
            if (has_sensor_state(btsensor_struct))
            {
              if (is_sensor_binary(btsensor_struct))
                encoder.addMeasurementState(btsensor_struct.measurement_type, get_sensor_binarystate(btsensor_struct).value());
              else
                encoder.addMeasurementValue(btsensor_struct.measurement_type, get_sensor_state(btsensor_struct).value());
              has_meaningful_measurements = true;
            }
            else
            {
              has_outstanding_measurements = true;
            }
          }
          if (add_complete_data && has_outstanding_measurements)
          {
            if (!add_events)
            {
              return false;
            }
            else
            {
              // if complete_only + add_events are used - skip all collected data from sensors
              encoder.resetMeasurement();
              has_meaningful_measurements = false;
            }
          }

          // TODO: after an outgoing send invalidate somehow -- cache all data and wait for another cycle, or wait at least 1 sec // or what
          // ESP_LOGD(TAG, "send {do_complete_send} %d, {has_outstanding_measurements} %d", do_complete_send, has_outstanding_measurements);
        }

        if (add_events)
        {
          // add any queued events
          // NOTE: this might/will break the numeric order, to be addressed for efficiency
          if (this->queued_events_.size() > 0)
          {
            this->send_datacmd_awaiting_events_ = 0;
            for (auto event : this->queued_events_)
            {
              encoder.addMeasurementEvent(event.device_type, event.event_type, event.steps);
              this->send_datacmd_awaiting_events_++;
            }
            has_meaningful_measurements = true;
          }
        }

        // perform send only if there is anything to send
        if (has_meaningful_measurements)
        {
          // assemble the packet
          uint8_t *bthome_data;
          uint8_t bthome_data_len;
          encoder.buildPaket(bthome_data, bthome_data_len);

          this->on_send_started_callback_.call();
          beethowen_base::send_command_data(get_server_address_arr(), local_passkey_, bthome_data, bthome_data_len);
          this->send_datacmd_awaiting_ = true;
        }
      }

      return success;
    }

    void BeethowenTransmitterHub::send_datacmd_loop()
    {
      // loop - if send awaited and finished
      if (this->is_server_found() && this->send_datacmd_awaiting_ && !beethowen_base::sending)
      {
        this->send_datacmd_awaiting_ = false;
        bool success = beethowen_base::sending_success;

        // auto dataAckOption = this->get_wait_data_ack_option();
        // if (dataAckOption != nullptr)
        // {
        //   // wait until server ack // TODO WiP
        //   auto maxTime = dataAckOption.timeOut; // 400;
        //   uint32_t delayUntil = millis() + maxTime;
        //   while (millis() <= delayUntil && last_packet_id_acked == this->send_packet_id_)
        //     yield();
        // }

        if (success)
        {
          // upon success - remove queued events waiting to be sent
          while (this->send_datacmd_awaiting_events_ && !this->queued_events_.empty())
          {
            this->queued_events_.pop_front();
            this->send_datacmd_awaiting_events_--;
          }
        }
        else
        {
          // reset server found
          ESP_LOGD(TAG, "ESPNow send failed, resetting server_found.");
          server_found_ = false;                     // invalidate server found
          initial_server_checkin_completed_ = false; // but first try it again on the last used channel
          // TODO: rework this logic, maybe repeat or git this one more chance?
        }

        int count = encoder.get_count();
        bool has_outstanding_measurements = my_sensors_.size() == count;
        ESP_LOGD(TAG, "Sending finished {success} %d, {has_outstanding_measurements} %d, {count} %d",
                 success, has_outstanding_measurements, count);

        if (success)
          this->on_send_finished_callback_.call(has_outstanding_measurements);
        else
          this->on_send_failed_callback_.call();

        this->last_send_millis_ = millis();

        // check if any events are queued, start sending them on success
        if (success && !this->queued_events_.empty())
          this->send_datacmd_(nullopt, true);
      }
    }

    void BeethowenTransmitterHub::restore_state_()
    {
      server_address_and_channel_t recovered{};
      bool restored = this->prefs_state_.load(&recovered);
      if (restored)
        this->set_server_data(recovered);
    }

    void BeethowenTransmitterHub::save_state_(server_address_and_channel_t server_address_and_channel)
    {
      ESP_LOGV(TAG, "%s: Saving state", this->device_id_.c_str());
      if (!this->prefs_state_.save(&server_address_and_channel))
        ESP_LOGW(TAG, "Failed to save state");
    }

    void BeethowenTransmitterHub::restore_packetid_state_rtc_()
    {
      this->prefs_packetid_state_.load(&this->send_packet_id_);
    }

    void BeethowenTransmitterHub::save_packetid_state_rtc_(uint8_t packet_id)
    {
      if (!this->prefs_packetid_state_.save(&packet_id))
        ESP_LOGW(TAG, "Failed to save packetid state");
    }

    void BeethowenTransmitterHub::check_auto_send_data()
    {
      // ESP_LOGD(TAG, "update {auto_send_} %d, {auto_send_done_} %d, {server_channel_} %d, ", auto_send_, auto_send_done_, server_channel_);
      if (is_server_found() && this->auto_send_data_ &&
          (!this->last_send_millis_ || (millis() - this->last_send_millis_ > BEETHOWEN_MINIMUM_TIMEOUT_AUTO_SEND)))
      {
        if (this->send_data(true))
          this->last_send_millis_ = millis();
      }
    }

    bool BeethowenTransmitterHub::send_event(bthome_measurement_t device_type, uint8_t event_type, uint8_t value)
    {
      // first queue the event
      if (this->queued_events_.size() >= BEETHOWEN_MAX_EVENT_QUEUE_LEN)
        return false;

      this->queued_events_.push_back({device_type, event_type, value});

      // then start sending the event - only events
      if (!beethowen_base::sending)
        this->send_datacmd_(nullopt, true);

      // regardless of success, return true as we will have this queued
      return true;
    }
  }
}
