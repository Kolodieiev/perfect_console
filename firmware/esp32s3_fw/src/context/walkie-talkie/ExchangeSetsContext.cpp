#include "ExchangeSetsContext.h"
#include "meow/lib/qr/QR_Gen.h"

const char STR_RECEIVE_SETS[] = "Отримати налаштування";
const char STR_SEND_SETS[] = "Поділитися налаштуваннями";

ExchangeSetsContext::ExchangeSetsContext(const String &sets_file_name) : _file_name{sets_file_name}
{
    _is_receiver = _file_name.isEmpty() ? true : false;
}

ExchangeSetsContext::~ExchangeSetsContext()
{
}

bool ExchangeSetsContext::loop()
{
    return false;
}

void ExchangeSetsContext::update()
{
}
