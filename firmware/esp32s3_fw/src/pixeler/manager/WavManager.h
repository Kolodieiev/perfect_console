#pragma once
#pragma GCC optimize("O3")
#include <list>
#include <unordered_map>

#include "../defines.h"

namespace pixeler
{
  class WavTrack
  {
  public:
    WavTrack(uint8_t* data_buf, uint32_t data_size) : _data_buf{data_buf}, _data_size{data_size} {}

    void play();

    void stop()
    {
      _is_playing = false;
    }
    int16_t getNextSample();

    void setOnRepeat(bool repeate)
    {
      _on_repeate = repeate;
    }

    bool isPlaying() const
    {
      return _is_playing;
    };

    void setVolume(uint8_t volume);

    uint8_t getVolume() const
    {
      return _volume * MAX_VOLUME;
    }

    // Встановити коефіцієнт фільтрації шуму. По замовченню встановлено 1. Без фільтрації.
    void setFiltrationLvl(uint16_t lvl)
    {
      _filtration_lvl = lvl;
    }

    WavTrack* clone();

  private:
    WavTrack() : _data_buf{nullptr}, _data_size{0} {}

  private:
    const float DEF_VOLUME{1.0f};
    float _volume{DEF_VOLUME};

    const uint8_t* _data_buf{nullptr};

    const uint32_t _data_size;
    uint32_t _current_sample{0};

    uint16_t _filtration_lvl{1};

    const uint8_t MAX_VOLUME{100};

    bool _is_playing{true};
    bool _on_repeate{false};
  };

  class WavManager
  {
  public:
    ~WavManager();

    /*!
     * @brief
     *      Додати звукову доріжку до списку міксування. Ресурси зі списку міксування звільняються автоматично.
     * @param  track
     *      Вказівник на передзавантажену доріжку.
     * @return
     *      Ідентифікатор на доріжку в списку міксування.
     */
    uint16_t addToMix(WavTrack* track);

    /*!
     * @brief
     *     Видалити доріжку із списку міксування.
     * @param  id
     *     Ідентифікатор, який було присвоєно доріжці, під час додавання до міксу.
     */
    void removeFromMix(uint16_t id);

    /*!
     * @brief
     *     Стартувати задачу відтворення міксу.
     */
    void startMix();

    /*!
     * @brief
     *    Поставити на паузу/відновити вітворення міксу.
     */
    void pauseResume();

  private:
    static void mixTask(void* params);

  private:
    std::unordered_map<uint16_t, WavTrack*> _mix;
    uint64_t _track_id{0};
    TaskHandle_t _task_handle{nullptr};

    struct TaskParams
    {
      enum CMD : uint8_t
      {
        CMD_NONE = 0,
        CMD_PAUSE,
        CMD_STOP
      };

      CMD cmd{CMD_NONE};
    } _params;

    bool _is_playing{false};
  };
}  // namespace pixeler