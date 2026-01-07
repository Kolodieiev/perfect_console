#pragma once
#pragma GCC optimize("O3")

#include <WebServer.h>
#include <stdint.h>

#include "pixeler/manager/FileManager.h"

namespace pixeler
{
  class FileServer
  {
  public:
    enum ServerMode : uint8_t
    {
      SERVER_MODE_RECEIVE = 0,
      SERVER_MODE_SEND,
      SERVER_MODE_SEND_FILE
    };

    ~FileServer();

    /**
     * @brief Запускає файловий сервер з раніше налаштованими параметрами.
     * Якщо встановлено з'єднання з маршрутизатором, сервер буде запущено на виділеному IP.
     * Якщо з'єднання не встановлено раніше, буде створено точку доступу із заданими налаштуваннями.
     *
     * @param server_path І'мя каталогу або файлу, для якого буде запущено файловий сервер.
     * @param mode Режим, в якому буде запущено файловий сервер. Може мати значення SERVER_MODE_SEND/SERVER_MODE_SEND_FILE/SERVER_MODE_RECEIVE.
     * @return true - Якщо файловий сервер успішно запущено.
     * @return false - Інакше.
     */
    bool begin(const char* server_path, ServerMode mode);

    /**
     * @brief Зупиняє файловий сервер. Та, якщо було запущено точку доступу, відключає WiFi-модуль.
     *
     */
    void stop();

    /**
     * @brief Повертає стан прапору, який вказує на те, чи працює зараз файловий сервер.
     *
     * @return true - Якщо сервер працює.
     * @return false - Інакше.
     */
    bool isWorking() const;

    /**
     * @brief Встановлює SSID з яким буде створено точку доступу, якщо відсутнє підключення до маршрутизатора.
     *
     * @param ssid
     */
    void setSSID(const char* ssid);

    /**
     * @brief Встановлює пароль, який буде використано для точки доступу, якщо відсутнє підключення до маршрутизатора.
     *
     * @param pwd
     */
    void setPWD(const char* pwd);

    /**
     * @brief Повертає виділену IP-адресу сервера на маршрутизаторі або IP-адресу сервера на точці доступу.
     *
     * @return String - IP-адреса сервера, якщо сервер працює. Порожній рядок - інакше.
     */
    String getAddress() const;

    /**
     * @brief Повертає значення режиму роботи, яке налаштовано для сервера.
     *
     * @return ServerMode
     */
    ServerMode getServerMode() const;

  private:
    static void fileServerTask(void* params);
    void startWebServer(void* params);

    void handleReceive();
    void handleSend();
    void handleSendFile();
    void handleFile();
    void handle404();

  private:
    String _server_ip;
    String _server_path;
    String _ssid;
    String _pwd;

    FILE* _in_file{nullptr};
    FileStream* _out_file_stream{nullptr};
    WebServer* _server = nullptr;

    unsigned long _last_delay_ts = 0;

    ServerMode _server_mode{SERVER_MODE_RECEIVE};
    //
    bool _must_work{false};
    bool _is_working{false};
  };
}  // namespace pixeler