#include "SetsEditorContext.h"

const char STR_EDIT_SETS[] = "Редагувати налаштування";
const char STR_NEW_SETS[] = "Нове налаштуваннями";
const char STR_GENERATE_KEY[] = "Згенерувати ключ";

SetsEditorContext::SetsEditorContext(String &sets_file_name, bool is_new) : _file_name{sets_file_name}, _is_new{is_new}
{
}

SetsEditorContext::~SetsEditorContext()
{
}

bool SetsEditorContext::loop()
{
    return true;
}

void SetsEditorContext::update()
{
}
