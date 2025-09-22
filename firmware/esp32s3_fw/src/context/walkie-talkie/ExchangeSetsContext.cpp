#include "ExchangeSetsContext.h"
#include <HTTPClient.h>
#include "meow/lib/qr/QR_Gen.h"
#include "meow/manager/SettingsManager.h"
#include "meow/manager/WiFiManager.h"
#include "LoraSettingsStruct.h"
#include "../WidgetCreator.h"

const char STR_SEND_SETS[] = "Поділитися: ";

ExchangeSetsContext::ExchangeSetsContext(const String &sets_filename)
{
    if (sets_filename.isEmpty())
        startClient();
    else
        startServer(sets_filename);
}

ExchangeSetsContext::~ExchangeSetsContext()
{
    _server.stop();
    free(_qr_img_buff);
}

bool ExchangeSetsContext::loop()
{
    return true;
}

void ExchangeSetsContext::update()
{
    if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        release();
    }
}

void ExchangeSetsContext::startClient()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    String sets_path = SettingsManager::getSettingsDirPath(STR_LORA_SETS_DIR);
    if (sets_path.isEmpty() || !_wifi.isConnected())
    {
        showToast(STR_FAIL);
        return;
    }

    String file_url = "http://";
    file_url += STR_REM_IP;
    HTTPClient http;

    const char *headers[] = {"Content-Disposition"};
    http.collectHeaders(headers, 1);
    http.begin(file_url);

    long unsigned ts = millis();
    while (millis() - ts < 2000 && http.GET() != 200)
        delay(50);

    WiFiClient *stream = http.getStreamPtr();
    if (stream)
    {
        String disposition = http.header("Content-Disposition");
        String filename = getFilenameFromDisposition(disposition);

        sets_path += "/";
        sets_path += filename;

        uint8_t buffer[sizeof(LoraSettings)];
        int read_len = stream->readBytes(buffer, sizeof(buffer));
        if (read_len == sizeof(LoraSettings))
        {
            if (_fs.writeFile(sets_path.c_str(), buffer, sizeof(LoraSettings)))
            {
                release();
                return;
            }
        }
    }

    showToast(STR_FAIL);
}

String ExchangeSetsContext::getFilenameFromDisposition(const String &disposition)
{
    int pos = disposition.indexOf("filename=");
    if (pos == -1)
        return "";

    pos += 9;

    if (disposition.charAt(pos) == '"' || disposition.charAt(pos) == '\'')
    {
        char quote = disposition.charAt(pos);
        int end = disposition.indexOf(quote, pos + 1);
        if (end > pos)
        {
            return disposition.substring(pos + 1, end);
        }
    }

    int end = disposition.indexOf(';', pos);
    if (end == -1)
        end = disposition.length();
    return disposition.substring(pos, end);
}

void ExchangeSetsContext::startServer(const String &sets_filename)
{
    String filepath = SettingsManager::getSettingsFilePath(sets_filename.c_str(), STR_LORA_SETS_DIR);
    filepath += STR_LORA_SETS_EXT;

    String ssid = SettingsManager::get(STR_PREF_FS_AP_SSID);
    if (ssid.isEmpty())
        ssid = STR_DEF_SSID;

    String pwd = SettingsManager::get(STR_PREF_FS_AP_PWD);
    if (pwd.isEmpty())
        pwd = STR_DEF_PWD;

    _server.setSSID(ssid.c_str());
    _server.setPWD(pwd.c_str());

    if (!_server.begin(filepath.c_str(), FileServer::SERVER_MODE_SEND_FILE))
        showToast(STR_FAIL);
    else
        showServerTmpl();
}

void ExchangeSetsContext::showServerTmpl()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    String header_str = STR_SEND_SETS;
    header_str += ": ";
    header_str += _server.getAddress().c_str();

    Label *msg_lbl = creator.getStatusMsgLable(ID_HEADER_LBL, header_str.c_str());
    layout->addWidget(msg_lbl);
    msg_lbl->setHeight(msg_lbl->getCharHgt() + 4);
    msg_lbl->setTicker(true);

    QR_Gen gen;
    _qr_img_buff = gen.generateQR(_server.getAddress().c_str(), 3);
    if (_qr_img_buff)
    {
        Image *_qr_img = new Image(ID_QR_IMG);
        layout->addWidget(_qr_img);

        uint16_t _qr_width = gen.getImageWidth();
        _qr_img->init(_qr_width, _qr_width);
        _qr_img->setSrc(_qr_img_buff);
        _qr_img->setPos((D_WIDTH - _qr_width) / 2, (D_HEIGHT - _qr_width) / 2);
    }
}
