#include "WavTrack.h"

#define DEF_VOLUME 128
#define MAX_VOLUME 100

namespace pixeler
{
  WavTrack::WavTrack(const uint8_t* data_buf, uint32_t data_size) : _data_buf{data_buf}, _data_size{data_size}, _volume{DEF_VOLUME} {}

  void WavTrack::play()
  {
    if (_data_buf)
      _is_playing = true;

    _current_sample = 0;
  }

  void WavTrack::stop()
  {
    _is_playing = false;
  }

  int16_t WavTrack::getNextSample()
  {
    if (!_is_playing)
      return 0;

    int16_t raw_sample = *reinterpret_cast<const int16_t*>(_data_buf + _current_sample);
    int32_t sample = (static_cast<int32_t>(raw_sample) * _volume) >> 8;

    if (std::abs(sample) < _cached_threshold)
      sample = 0;

    _current_sample += 2;

    if (_current_sample >= _data_size)
    {
      if (!_on_repeate)
        _is_playing = false;
      _current_sample = 0;
    }

    return static_cast<int16_t>(sample);
  }

  void WavTrack::setOnRepeat(bool repeate)
  {
    _on_repeate = repeate;
  }

  bool WavTrack::isPlaying() const
  {
    return _is_playing;
  }

  void WavTrack::setVolume(uint8_t volume)
  {
    _volume = (static_cast<uint32_t>(volume) * 256) / 100;
    _cached_threshold = (_volume * _filtration_lvl) >> 8;
  }

  uint8_t WavTrack::getVolume() const
  {
    return _volume * MAX_VOLUME;
  }

  void WavTrack::setFiltrationLvl(uint16_t lvl)
  {
    _filtration_lvl = lvl;
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