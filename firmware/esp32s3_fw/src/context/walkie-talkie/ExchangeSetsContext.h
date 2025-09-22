#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "meow/lib/server/file_server/FileServer.h"

using namespace meow;

class ExchangeSetsContext : public IContext
{
public:
    ExchangeSetsContext(const String &sets_filename);
    virtual ~ExchangeSetsContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Mode : uint8_t
    {
        MODE_MAIN = 0,
        MODE_CTX_MENU,
    };

    enum Widget_ID : uint8_t
    {
        ID_HEADER_LBL = 1,
        ID_QR_IMG,
        //
    };

    void startClient();
    void startServer(const String &sets_filename);
    void showServerTmpl();
    String getFilenameFromDisposition(const String &disposition);
    //

private:
    FileServer _server;
    uint16_t *_qr_img_buff{nullptr};
};
