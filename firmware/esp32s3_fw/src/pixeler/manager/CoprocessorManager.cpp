#pragma GCC optimize("O3")
#include "CoprocessorManager.h"

#include "../manager/I2C_Manager.h"

namespace pixeler
{
#ifdef HAS_COPROCESSOR

  bool CoprocessorManager::connect()
  {
    if (_is_connected)
      return true;

    if (!_i2c.begin() || !_i2c.hasConnect(COPROCESSOR_ADDR))
    {
      log_e("Помилка підключення до копроцесора");
      _is_connected = false;
      return _is_connected;
    }

    log_i("З'єднання з копроцесором встановлено");

    _is_connected = true;
    return _is_connected;
  }

  bool CoprocessorManager::reconnect()
  {
    _i2c.end();
    _is_connected = false;
    return connect();
  }

  bool CoprocessorManager::sendCmd(const void* cmd_data, size_t data_size, uint8_t resp_delay) const
  {
    if (!_is_connected)
      return false;

    if (!_i2c.write(COPROCESSOR_ADDR, cmd_data, data_size))
    {
      log_e("Копроцесор не відповідає");
      return false;
    }
    delay(resp_delay);
    return true;
  }

  bool CoprocessorManager::readData(void* out_buf, size_t data_size) const
  {
    if (!_is_connected)
      return false;

    if (!_i2c.read(COPROCESSOR_ADDR, out_buf, data_size))
    {
      log_e("Не вдалося прочитати очікувану кількість байтів");
      return false;
    }

    return true;
  }

  CoprocessorManager _ccpu;

#endif  // HAS_COPROCESSOR
}  // namespace pixeler
