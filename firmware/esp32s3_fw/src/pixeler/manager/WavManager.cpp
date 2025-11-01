#pragma GCC optimize("O3")
#include "WavManager.h"

#include <cmath>
#include <limits>

#include "I2SOutManager.h"

namespace pixeler
{
  WavManager::~WavManager()
  {
    if (_task_handle)
    {
      _params.cmd = TaskParams::CMD_STOP;
      vTaskDelete(_task_handle);
    }

    for (auto mix_it = _mix.begin(), last_it = _mix.end(); mix_it != last_it; ++mix_it)
      delete mix_it->second;
  }

  uint16_t WavManager::addToMix(WavTrack* track)
  {
    if (!track)
    {
      log_e("Track ptr не може бути null");
      esp_restart();
    }

    ++_track_id;
    _mix.insert(std::pair<uint16_t, WavTrack*>(_track_id, track));
    return _track_id;
  }

  void WavManager::removeFromMix(uint16_t id)
  {
    _mix.erase(id);
  }

  void WavManager::startMix()
  {
    if (!_i2s_out.isInited())
      esp_restart();

    if (_is_playing)
      return;

    _is_playing = true;

    xTaskCreatePinnedToCore(mixTask, "mixTask", (1024 / 2) * 10, this, 10, &_task_handle, 0);
  }

  void WavManager::pauseResume()
  {
    if (_params.cmd == TaskParams::CMD_NONE)
    {
      _params.cmd = TaskParams::CMD_PAUSE;
    }
    else if (_params.cmd == TaskParams::CMD_PAUSE)
    {
      _params.cmd = TaskParams::CMD_NONE;
    }
  }

  void WavManager::mixTask(void* params)
  {
    int16_t _samples_buf[256];

    WavManager* self = static_cast<WavManager*>(params);

    int16_t sample;
    uint32_t cycles_counter = 0;

    while (self->_params.cmd != TaskParams::CMD_STOP)
    {
      if (self->_params.cmd != TaskParams::CMD_PAUSE)
      {
        for (uint32_t i{0}; i < 256u; i += 2u)
        {
          sample = 0;

          for (auto it = self->_mix.begin(), last_it = self->_mix.end(); it != last_it;)
          {
            if (it->second->isPlaying())
            {
              sample += it->second->getNextSample();
              ++it;
            }
            else
            {
              delete it->second;
              it = self->_mix.erase(it);
            }
          }

          _samples_buf[i] = sample;
          _samples_buf[i + 1] = sample;
        }

        _i2s_out.write(_samples_buf, sizeof(uint16_t) * 256);
      }

      ++cycles_counter;

      if (cycles_counter > 50u)
      {
        cycles_counter = 0;
        delay(1);
      }
    }

    vTaskDelete(NULL);
  }

  void WavTrack::play()
  {
    if (_data_buf)
      _is_playing = true;

    _current_sample = 0;
  }

  int16_t WavTrack::getNextSample()
  {
    if (!_is_playing)
      return 0;

    int16_t ret = *reinterpret_cast<const int16_t*>(_data_buf + _current_sample);
    float temp_val = static_cast<float>(ret) * _volume;

    if (temp_val > 32767.0f)
      temp_val = 32767.0f;
    else if (temp_val < -32768.0f)
      temp_val = -32768.0f;

    ret = static_cast<int16_t>(temp_val);

    if (std::abs(ret) > _volume * _filtration_lvl)
    {
      int8_t high = ret;
      ret >>= 8;
      int8_t low = ret;

      ret = high;
      ret <<= 8;
      ret |= low;
    }
    else
    {
      ret = 0;
    }

    _current_sample += 2;

    if (_current_sample >= _data_size)
    {
      if (!_on_repeate)
        _is_playing = false;

      _current_sample = 0;
    }

    return ret;
  }

  void WavTrack::setVolume(uint8_t volume)
  {
    if (volume < MAX_VOLUME)
      _volume = (float)volume / 100;
    else
      _volume = DEF_VOLUME;
  }

  WavTrack* WavTrack::clone()
  {
    try
    {
      return new WavTrack(*this);
    }
    catch (const std::bad_alloc& e)
    {
      log_e("Помилка клонування звукової доріжки");
      esp_restart();
    }
  }
}  // namespace pixeler