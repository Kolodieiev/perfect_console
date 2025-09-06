#include "ExchangeSetsContext.h"
#include "meow/lib/qr/QR_Gen.h"

const char STR_RECEIVE_SETS[] = "Отримати налаштування";
const char STR_SEND_SETS[] = "Поділитися налаштуваннями";

ExchangeSetsContext::ExchangeSetsContext(String &sets_file_name, bool is_receiver) : _file_name{sets_file_name}, _is_receiver{is_receiver}
{
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
