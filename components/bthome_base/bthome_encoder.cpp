/*
 BTHome V2 protocol packages encoder

 Based on BTHomeV2-ESP32-example DIY Sensor by Christos Baltatzidis
 https://github.com/Chreece/BTHomeV2-ESP32-example

 Author: Attila Farago
 */

#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <cstring>

#include "bthome_base_common.h"
#include "bthome_encoder.h"

// #include "esphome\core\log.h" //TODO: temporary only for development
// using namespace esphome;

namespace bthome_base
{
  BTHomeEncoder::BTHomeEncoder(uint16_t measurement_max_len)
  {
    // ESPHome cannot throw exceptions, just absorb this
    measurement_max_len_ = min(measurement_max_len, (uint16_t)MAX_BUFFER_LEN);
  }

  void BTHomeEncoder::resetMeasurement()
  {
    this->m_sensorDataIdx = 0;
    this->last_object_id = 0;
    this->m_sortEnable = false;
    this->m_count = 0;
  }

  bool BTHomeEncoder::addMeasurementState(bthome_measurement_t sensor_id, uint8_t state, uint8_t steps)
  {
    // if ((this->m_sensorDataIdx + 2 + (steps > 0 ? 1 : 0)) <= (MEASUREMENT_MAX_LEN - (this->m_encryptEnable ? 8 : 0))) {
    if ((this->m_sensorDataIdx + 2 + (steps > 0 ? 1 : 0)) > (measurement_max_len_))
      return false;

    this->m_count++;
    this->m_sensorData[this->m_sensorDataIdx] = static_cast<uint8_t>(sensor_id & 0xff);
    this->m_sensorDataIdx++;
    this->m_sensorData[this->m_sensorDataIdx] = static_cast<uint8_t>(state & 0xff);
    this->m_sensorDataIdx++;
    if (steps > 0)
    {
      this->m_sensorData[this->m_sensorDataIdx] = static_cast<uint8_t>(steps & 0xff);
      this->m_sensorDataIdx++;
    }
    if (!this->m_sortEnable)
    {
      if (sensor_id < this->last_object_id)
        this->m_sortEnable = true;
    }
    last_object_id = sensor_id;

    return true;
  }

  bool BTHomeEncoder::addMeasurementValueRaw(bthome_measurement_t sensor_id, uint64_t raw_value, uint8_t size)
  {
    this->m_count++;
    this->m_sensorData[this->m_sensorDataIdx] = static_cast<uint8_t>(sensor_id & 0xff);
    this->m_sensorDataIdx++;
    for (uint8_t i = 0; i < size; i++)
    {
      this->m_sensorData[this->m_sensorDataIdx] = static_cast<uint8_t>((raw_value >> (8 * i)) & 0xff);
      this->m_sensorDataIdx++;
    }
    if (!this->m_sortEnable)
    {
      if (sensor_id < this->last_object_id)
        this->m_sortEnable = true;
    }
    last_object_id = sensor_id;

    return true;
  }

  bool BTHomeEncoder::addMeasurementValue(bthome_measurement_t sensor_id, uint64_t value)
  {
    BTHomeDataFormat dataformat = getDataFormat(sensor_id);
    uint8_t size = dataformat.len_in_bytes;
    uint16_t factor = dataformat.factor_multiple;

    // if ((this->m_sensorDataIdx + size + 1) <= (MEASUREMENT_MAX_LEN - (this->m_encryptEnable ? 8 : 0))) {
    if (size == 0)
      return false;
    if ((this->m_sensorDataIdx + size + 1) > (measurement_max_len_))
      return false;

    uint64_t value2 = value * factor;
    return addMeasurementValueRaw(sensor_id, value2, size);
  }

  bool BTHomeEncoder::addMeasurementValue(bthome_measurement_t sensor_id, float value)
  {
    BTHomeDataFormat dataformat = getDataFormat(sensor_id);
    uint8_t size = dataformat.len_in_bytes;
    uint16_t factor = dataformat.factor_multiple;

    // if ((this->m_sensorDataIdx + size + 1) <= (MEASUREMENT_MAX_LEN - (this->m_encryptEnable ? 8 : 0))) {
    if (size == 0)
      return false;
    if ((this->m_sensorDataIdx + size + 1) > (measurement_max_len_))
      return false;

    uint64_t value2;
    switch (dataformat.data_format)
    {
    case HaBleType_uint:
    {
      value2 = static_cast<uint64_t>(value * factor);
    }
    break;
    case HaBleType_sint:
    {
      int64_t value2s = static_cast<int64_t>(value * factor);
      value2 = signextend<uint64_t, 64>(value2s);
    }
    break;
    default:
      // cannot encode this
      return false;
    }

    // perform actual data add to the send array
    return addMeasurementValueRaw(sensor_id, value2, size);
  }

  void BTHomeEncoder::sortSensorData()
  {
    uint8_t i, j, k, data_block_num;

    struct DATA_BLOCK
    {
      bthome_measurement_t object_id;
      uint8_t data[4];
      uint8_t data_len;
    };
    struct DATA_BLOCK data_block[measurement_max_len_ / 2 + 1];
    struct DATA_BLOCK temp_data_block;

    for (i = 0, j = 0, data_block_num = 0; j < this->m_sensorDataIdx; i++)
    {
      // copy the object id
      data_block[i].object_id = this->m_sensorData[j];
      data_block_num++;
      // copy the data length
      //  if (this->m_sensorData[j] == EVENT_DIMMER) {
      //    if (this->m_sensorData[j + 1] == EVENT_DIMMER_NONE)
      //      data_block[i].data_len = 1;
      //    else
      //      data_block[i].data_len = 2;
      //  }
      //  else
      {
        data_block[i].data_len = getDataFormat(this->m_sensorData[j]).len_in_bytes;
      }
      // copy the data
      for (k = 0; k < data_block[i].data_len; k++)
      {
        data_block[i].data[k] = this->m_sensorData[j + 1 + k];
      }
      // move to the next object id location
      j = j + data_block[i].data_len + 1;
    }

    if (data_block_num > 1)
    {
      // bubble sort
      for (i = 0; i < data_block_num - 1; i++)
      {
        for (j = 0; j < data_block_num - 1 - i; j++)
        {
          if (data_block[j].object_id > data_block[j + 1].object_id)
          {
            memcpy(&temp_data_block, &data_block[j], sizeof(struct DATA_BLOCK));
            memcpy(&data_block[j], &data_block[j + 1], sizeof(struct DATA_BLOCK));
            memcpy(&data_block[j + 1], &temp_data_block, sizeof(struct DATA_BLOCK));
          }
        }
      }
      // copy the new order to m_sensorData array
      for (i = 0, j = 0; i < data_block_num && j < this->m_sensorDataIdx; i++)
      {
        this->m_sensorData[j] = data_block[i].object_id;
        for (k = 0; k < data_block[i].data_len; k++)
        {
          this->m_sensorData[j + 1 + k] = data_block[i].data[k];
        }
        j = j + data_block[i].data_len + 1;
      }
    }
  }

  uint8_t *BTHomeEncoder::buildPaket(uint8_t *&beethowen_data_out, uint8_t &beethowen_data_len_out)
  {
    // const uint8_t *BTHomeEncoder::buildPaket() {
    // the Object ids have to be applied in numerical order (from low to high)
    if (this->m_sortEnable)
      sortSensorData();

    beethowen_data_out = this->m_sensorData;
    beethowen_data_len_out = this->m_sensorDataIdx;

    return this->m_sensorData;
  }

  /*
  void BTHomeEncoder::buildPaket_orig(bool trigger_based_device) {
    //the Object ids have to be applied in numerical order (from low to high)
    if (this->m_sortEnable) sortSensorData();

    // Create the BLE Device
    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

    std::string payloadData = "";
    std::string serviceData = "";
    uint8_t i;

    //head
    payloadData += FLAG1;
    payloadData += FLAG2;
    payloadData += FLAG3;
    //local name: move to the response packet

    serviceData += SERVICE_DATA;  // DO NOT CHANGE -- Service Data - 16-bit UUID
    serviceData += UUID1;  // DO NOT CHANGE -- UUID
    serviceData += UUID2;  // DO NOT CHANGE -- UUID
    // The encryption
    if (this->m_encryptEnable) {
      if (trigger_based_device)
        serviceData += ENCRYPT_TRIGGER_BASE;
      else
        serviceData += ENCRYPT;

      uint8_t ciphertext[BLE_ADVERT_MAX_LEN];
      uint8_t encryptionTag[MIC_LEN];
      //buildNonce
      uint8_t nonce[NONCE_LEN];
      uint8_t* countPtr  = (uint8_t*)(&this->m_encryptCount);
      esp_read_mac(&nonce[0], ESP_MAC_BT);
      nonce[6] = UUID1;
      nonce[7] = UUID2;
      nonce[8] = ENCRYPT;
      memcpy(&nonce[9], countPtr, 4);
      //encrypt sensorData
      mbedtls_ccm_encrypt_and_tag(&this->m_encryptCTX, this->m_sensorDataIdx, nonce, NONCE_LEN, 0, 0,
                                  &this->m_sensorData[0], &ciphertext[0], encryptionTag,
                                  MIC_LEN);
      for (i = 0; i < this->m_sensorDataIdx; i++)
      {
        serviceData += ciphertext[i];
      }
      //writeCounter
      serviceData += nonce[9];
      serviceData += nonce[10];
      serviceData += nonce[11];
      serviceData += nonce[12];
      this->m_encryptCount++;
      //writeMIC
      serviceData += encryptionTag[0];
      serviceData += encryptionTag[1];
      serviceData += encryptionTag[2];
      serviceData += encryptionTag[3];
    }
    else {
      if (trigger_based_device)
        serviceData += NO_ENCRYPT_TRIGGER_BASE;
      else
        serviceData += NO_ENCRYPT;
      for (i = 0; i < this->m_sensorDataIdx; i++)
      {
        serviceData += this->m_sensorData[i]; // Add the sensor data to the Service Data
      }
    }

    byte sd_length = serviceData.length(); // Generate the length of the Service Data
    payloadData += sd_length;         // Add the length of the Service Data
    payloadData += serviceData;             // Finalize the packet

    oAdvertisementData.addData(payloadData);
    pAdvertising->setAdvertisementData(oAdvertisementData);

    //fill the local name into oScanResponseData
    if (!this->dev_name.isEmpty()) {
      int dn_length = this->dev_name.length() + 1;
      if (dn_length > 28) dn_length = 28;//BLE_ADVERT_MAX_LEN - FLAG = 31 - 3
      oScanResponseData.setName(this->dev_name.substring(0, dn_length - 1).c_str());
    }
    pAdvertising->setScanResponseData(oScanResponseData);

    //  pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
    //     Advertising mode. Can be one of following constants:
    //   - BLE_GAP_CONN_MODE_NON (non-connectable; 3.C.9.3.2).
    //   - BLE_GAP_CONN_MODE_DIR (directed-connectable; 3.C.9.3.3).
    //   - BLE_GAP_CONN_MODE_UND (undirected-connectable; 3.C.9.3.4).
    pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
  }
  */
}