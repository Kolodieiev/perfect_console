#pragma once

#include "pixeler/game/DataStream.h"
//
#include "pixeler/game/IGameScene.h"
#include "pixeler/game/online/GameClient.h"
#include "pixeler/game/online/GameServer.h"
#include "pixeler/manager/WiFiManager.h"
#include "pixeler/context/IContext.h"
#include "pixeler/widget/keyboard/Keyboard.h"
#include "pixeler/widget/menu/FixedMenu.h"
#include "pixeler/widget/scrollbar/ScrollBar.h"
#include "pixeler/widget/text/TextBox.h"

using namespace pixeler;

namespace test_server
{
  class TestServerContext : public IContext
  {
  public:
    TestServerContext();
    virtual ~TestServerContext();

  protected:
    virtual bool loop() override;
    virtual void update() override;

  private:
    enum WidgetID : uint8_t  // Ід віджетів
    {
      ID_MAIN_MENU = 1,
      ID_ROLE_MENU,
      ID_LOBBY_MENU,
      ID_WIFI_LIST_MENU,
      ID_CONTEXT_LBL,
      ID_SSID_TXT,
      ID_PWD_TXT,
      ID_KEYBOARD,
      ID_CLIENT_LIST_MENU,
      ID_LOBBY_CONTEXT_MENU,
      ID_PREF_MAIN_MENU,
    };

    enum ItemID : uint8_t  // Ід пунктів меню
    {
      // MAIN
      ID_ITEM_ONLINE = 1,
      ID_ITEM_OFFLINE,
      ID_ITEM_PREF,
      // ROLE
      ID_ITEM_SERVER,
      ID_ITEM_CLIENT,
      // PREF
      ID_ITEM_NICK,
      ID_ITEM_SERV_NAME,
      ID_ITEM_SERV_PWD,
      // CONT_MENU
      ID_ITEM_DISC_CLIENT,
      ID_ITEM_START_GAME,
      ID_ITEM_END_GAME,
      ID_ITEM_CLOSE_LOBBY,
      ID_ITEM_OPEN_LOBBY,
    };

    enum ContextID : uint8_t  // Ід контексту
    {
      ID_CONT_MAIN = 1,
      ID_CONT_ROLE,
      ID_CONT_WIFI_LIST,
      ID_CONT_CLIENT_LOBBY,
      ID_CONT_CONN_DIALOG,
      ID_CONT_CONN_TO_AP,
      ID_CONT_GAME,
      //
      ID_CONT_SERVER_LOBBY,
      ID_CONT_LOBBY_CONT_MENU,
      ID_CONT_CLIENT_CONFIRM,
      //
      ID_CONT_PREF_MAIN,
      ID_CONT_PREF_NICK,
      ID_CONT_PREF_SERV_NAME,
      ID_CONT_PREF_SERV_PWD,
    };

    void addDialog(EmptyLayout* layout, const char* title_txt, const char* start_txt);
    void showMainMenu();
    void handleMainBtns();
    void showSelectRole();
    void handleRoleBtns();
    void showWifiList();
    void handleWifiListBtns();
    void readWifiScan();
    void showConnectDialog();
    void showConnToAP();
    void handleConnToAPBtns();
    void updateConnectState(wl_status_t conn_status);
    void handleServerData(const UdpPacket* packet);
    void handleServerConn();
    void handleServerDisconn();

    void showClientLobby();
    void handleClientLobbyBtns();

    void showServerLobby();
    void updateClientsList(const ClientWrapper* disconnected_client = nullptr);
    void handleServerLobbyBtns();
    void openServerLobby();

    void showServLobbyContMenu();
    void hideServLobbyContMenu();
    void handleLobbyContMenuBtns();

    void handleClientConn(const ClientWrapper* cl_wrap, ConfirmResultHandler_t result_handler);
    void showClientConn();
    void handleClientConnBtns();
    void handleClientDisconn(const ClientWrapper* cl_wrap);
    void showPrefMain();
    void handlePrefMainBtns();
    void showPrefNick();
    void showPrefServName();
    void showPrefServPwd();
    void handleDialBtns();
    void handlePrefSaveBtns();
    void showMainScene();
    void updateGame();

    static void clientDisconnHandler(const ClientWrapper* cl_wrap, void* arg);
    static void serverConnHandler(void* arg);
    static void serverDisconnHandler(void* arg);
    static void onServerData(const UdpPacket* packet, void* arg);
    static void wifiConnDoneHandler(void* arg, wl_status_t conn_status);
    static void wifiScanDoneHandler(void* arg);
    static void clientConfirmHandler(const ClientWrapper* cl_wrap, ConfirmResultHandler_t result_handler, void* arg);

  private:
    GameServer _server;
    GameClient _client;

    ConfirmResultHandler_t _confirm_result_handler{nullptr};

    String _client_nick;
    String _serv_ssid;
    String _serv_pwd;

    DataStream _stored_objs{1};  // Перенесення об'єктів між сценами не буде відбуватися. Тому виділяємо тільки 1 байт

    const ClientWrapper* _conn_client_wrap{nullptr};
    TextBox* _dialog_txt;
    Keyboard* _keyboard;
    Image* _img_back_normal;
    Image* _img_back_hover;
    FixedMenu* _main_menu;
    FixedMenu* _role_menu;
    FixedMenu* _lobby_menu;
    FixedMenu* _pref_menu;
    FixedMenu* _wifi_list_menu;
    FixedMenu* _clients_list_menu;
    FixedMenu* _lobby_context_menu;
    IGameScene* _scene;

    ContextID _context_id{ID_CONT_MAIN};

    bool _is_server{false};
    bool _is_client{false};
  };
}  // namespace test_server