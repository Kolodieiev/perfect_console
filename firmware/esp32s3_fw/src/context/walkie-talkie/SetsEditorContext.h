#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

using namespace meow;

class SetsEditorContext : public IContext
{
public:
    SetsEditorContext(String &sets_file_name, bool is_new = true);
    virtual ~SetsEditorContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    String _file_name;
    bool _is_new;
};
