#pragma once
#include <Arduino.h>

#include "FileManager.h"

namespace pixeler
{
  /**
   * @brief Дозволяє зберігати та завантажувати окремі налаштування до бінарних файлів на карту пам'яті.
   * Бінарні файли з налаштуваннями зберігаються у папці "/.data/preferences"
   *
   */
  class SettingsManager
  {
  public:
    /**
     * @brief Зберігає налаштування до бінарного файлу.
     *
     * @param pref_name Ім'я налаштування.
     * @param value Значення налаштування.
     * @param subdir Ім'я підкаталогу з налаштуваннями.
     * @return true - якщо операція виконана успішно.
     * @return false - якщо операція завершилася невдачею.
     */
    static bool set(const char* pref_name, const char* value, const char* subdir = "");

    /**
     * @brief Читає налаштування з бінарного файлу.
     *
     * @param pref_name Ім'я налаштування.
     * @param subdir Ім'я підкаталогу з налаштуваннями.
     * @return String - Рядок, що містить дані налаштування, або порожній рядок, якщо файл з налаштуванням не вдалося прочитати.
     */
    static String get(const char* pref_name, const char* subdir = "");

    /**
     * @brief Повертає повний шлях до файла в каталозі налаштувань.
     * Якщо каталог налаштувань не існує, буде виконана спроба його створення.
     *
     * @param pref_name Шлях до файла з налаштуваннями.
     * @param subdir Ім'я підкаталогу з налаштуваннями.
     * @return String - Рядок, що містить повний шлях до файла, або порожній рядок, у разі помилки.
     */
    static String getSettingsFilePath(const char* pref_name, const char* subdir = "");

    /**
     * @brief Повертає повний шлях до каталогу налаштувань.
     *
     * @param sub_dirname Ім'я підкаталогу в каталозі налаштувань, якщо потрібно.
     * @return String - Рядок, що містить повний шлях до каталогу, або порожній рядок, у разі помилки.
     */
    static String getSettingsDirPath(const char* sub_dirname = "");

    /**
     * @brief Завантажує структуру налаштувань з карти пам'яті.
     *
     * @param out_data_struct Вказівник на пам'ять, куди будуть записані дані з файлу.
     * @param data_struct_size Очікуваний розмір даних.
     * @param data_filename Ім'я файлу налаштувань.
     * @param data_dirname Ім'я підкаталогу налаштувань.
     * @return true - Якщо розмір очікуваних даних співпадає з розміром прочитаних даних.
     * @return false - Інакше.
     */
    static bool load(void* out_data_struct, size_t data_struct_size, const char* data_filename, const char* data_dirname = "");

    /**
     * @brief Зберігає структуру налаштувань на карту пам'яті.
     *
     * @param data_struct Вказівник на пам'ять, звідки будуть скопійовані дані до файлу.
     * @param data_struct_size Розмір даних.
     * @param data_filename Ім'я файлу налаштувань.
     * @param data_dirname Ім'я підкаталогу налаштувань.
     * @return true - Якщо розмір даних співпадає з розміром записаних даних.
     * @return false - Інакше.
     */
    static bool save(const void* data_struct, size_t data_struct_size, const char* data_filename, const char* data_dirname = "");
  };
}  // namespace pixeler