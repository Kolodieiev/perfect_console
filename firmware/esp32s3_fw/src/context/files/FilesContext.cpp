#include "FilesContext.h"

#include "meow/lib/qr/QR_Gen.h"
#include "meow/manager/settings/SettingsManager.h"
//
#include "meow/ui/widget/progress/ProgressBar.h"
#include "meow/ui/widget/menu/item/MenuItem.h"

#include "../WidgetCreator.h"
#include "./res/folder.h"

#define UPD_TRACK_INF_INTERVAL 1000UL
#define PADDING_BOTT 40

const char STR_SIZE[] = "File size:";

bool FilesContext::loop()
{
    return true;
}

FilesContext::FilesContext()
{
    _dir_img = new Image(1);
    _dir_img->setTranspColor(TFT_TRANSPARENT);
    _dir_img->init(16, 16);
    _dir_img->setSrc(FOLDER_IMG);
    _dir_img->setTransparency(true);

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    if (!_sd.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    _f_mgr.setTaskDoneHandler(taskDone, this);

    showFilesTmpl();
    indexCurDir();
    fillFilesTmpl();
}

//-------------------------------------------------------------------------------------------

void FilesContext::showSDErrTmpl()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_SD_ERR);
    layout->addWidget(_msg_lbl);

    _mode = MODE_SD_UNCONN;

    setLayout(layout);
}

void FilesContext::showServerTmpl()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();

    String header_str;

    if (_server.getServerMode() == FileServer::SERVER_MODE_SEND)
        header_str = STR_EXPORT;
    else
        header_str = STR_IMPORT;

    header_str += ": ";
    header_str += _server.getAddress().c_str();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, header_str.c_str());
    layout->addWidget(_msg_lbl);
    _msg_lbl->setHeight(_msg_lbl->getCharHgt() + 4);

    if (_qr_img_buff)
    {
        _qr_img = new Image(ID_QR_IMG);
        layout->addWidget(_qr_img);
        _qr_img->init(_qr_width, _qr_width);
        _qr_img->setSrc(_qr_img_buff);
        _qr_img->setPos((D_WIDTH - _qr_width) / 2, (D_HEIGHT - _qr_width) / 2);
    }

    _mode = MODE_FILE_SERVER;
    setLayout(layout);
}

void FilesContext::showCopyingTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_COPYING, 2);
    layout->addWidget(_msg_lbl);
    _msg_lbl->setHeight(32);
    _msg_lbl->setPos(0, D_HEIGHT / 2 - _msg_lbl->getHeight() - 2);

    _task_progress = new ProgressBar(ID_PROGRESS);
    layout->addWidget(_task_progress);
    _task_progress->setBackColor(TFT_BLACK);
    _task_progress->setProgressColor(TFT_ORANGE);
    _task_progress->setBorderColor(TFT_WHITE);
    _task_progress->setMax(100);
    _task_progress->setWidth(D_WIDTH - 5 * 8);
    _task_progress->setHeight(20);
    _task_progress->setProgress(0);
    _task_progress->setPos((D_WIDTH - _task_progress->getWidth()) / 2, D_HEIGHT / 2 + 2);

    _mode = MODE_COPYING;

    setLayout(layout);
}

void FilesContext::showRemovingTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_REMOVING, 2);
    layout->addWidget(_msg_lbl);

    _mode = MODE_REMOVING;

    setLayout(layout);
}

void FilesContext::showCancelingTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _msg_lbl = creator.getStatusMsgLable(ID_MSG_LBL, STR_CANCELING, 2);
    layout->addWidget(_msg_lbl);

    _mode = MODE_CANCELING;
    setLayout(layout);
}

void FilesContext::showFilesTmpl()
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    layout->setBackColor(TFT_BLACK);

    _files_list = creator.getDynamicMenu(ID_DYNAMIC_MENU);
    layout->addWidget(_files_list);
    _files_list->setHeight(D_HEIGHT - PADDING_BOTT);
    _files_list->setItemHeight((_files_list->getHeight() - 2) / MENU_ITEMS_NUM);
    _files_list->setWidth(D_WIDTH - SCROLLBAR_WIDTH);

    _files_list->setOnNextItemsLoadHandler(onNextItemsLoad, this);
    _files_list->setOnPrevItemsLoadHandler(onPrevItemsLoad, this);

    _scrollbar = new ScrollBar(ID_SCROLLBAR);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT - PADDING_BOTT);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, 0);
    _scrollbar->setBackColor(COLOR_MAIN_BACK);

    Label *size_title_lbl = new Label(ID_SIZE_TITLE_LBL);
    layout->addWidget(size_title_lbl);
    size_title_lbl->setText(STR_SIZE);
    size_title_lbl->setFontID(4);
    size_title_lbl->setTextColor(TFT_WHITE);
    size_title_lbl->initWidthToFit();
    size_title_lbl->setPos(0, D_HEIGHT - size_title_lbl->getHeight() * 2 - 4);

    _file_size_lbl = new Label(ID_SIZE_LBL);
    layout->addWidget(_file_size_lbl);
    _file_size_lbl->setText("0");
    _file_size_lbl->setFontID(4);
    _file_size_lbl->setTextColor(TFT_WHITE);
    _file_size_lbl->initWidthToFit();
    _file_size_lbl->setPos(size_title_lbl->getXPos() + size_title_lbl->getWidth() + 2, size_title_lbl->getYPos());

    _file_pos_lbl = new Label(ID_FILE_POS_LBL);
    layout->addWidget(_file_pos_lbl);
    _file_pos_lbl->setText("[0/0]");
    _file_pos_lbl->setFontID(4);
    _file_pos_lbl->setTextColor(TFT_WHITE);
    _file_pos_lbl->initWidthToFit();
    _file_pos_lbl->setPos(0, D_HEIGHT - _file_pos_lbl->getHeight() - 2);

    _mode = MODE_NAVIGATION;

    setLayout(layout);
}

//-------------------------------------------------------------------------------------------

void FilesContext::makePathFromBreadcrumbs(String &out_str)
{
    out_str = "";
    for (uint8_t i{0}; i < _breadcrumbs.size(); ++i)
        out_str += _breadcrumbs[i];
}

void FilesContext::showContextMenu()
{
    _mode = MODE_CONTEXT_MENU;
    _files_list->disable();

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_CNTXT_MENU);
    getLayout()->addWidget(_context_menu);
    _context_menu->setItemHeight(18);
    _context_menu->setWidth((float)D_WIDTH / 2.2);
    _context_menu->setBackColor(TFT_BLACK);
    _context_menu->setBorder(true);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setLoopState(true);

    if (_has_moving_file || _has_copying_file)
    {
        // Якщо є файл для переміщення додати відповідний пункт меню
        MenuItem *paste_item = creator.getMenuItem(ID_ITEM_PASTE);
        _context_menu->addItem(paste_item);

        Label *paste_lbl = creator.getItemLabel(STR_PASTE, 2);
        paste_item->setLbl(paste_lbl);
        paste_lbl->setTextOffset(1);
    }

    // новий каталог
    MenuItem *new_dir_item = creator.getMenuItem(ID_ITEM_NEW_DIR);
    _context_menu->addItem(new_dir_item);

    Label *new_dir_lbl = creator.getItemLabel(STR_NEW_DIR, 2);
    new_dir_item->setLbl(new_dir_lbl);
    new_dir_lbl->setTextOffset(1);

    // контекст для вибраного файлу
    uint16_t id = _files_list->getCurrItemID();

    if (id > 0)
    {
        // перейменувати
        MenuItem *rename_item = creator.getMenuItem(ID_ITEM_RENAME);
        _context_menu->addItem(rename_item);

        Label *rename_lbl = creator.getItemLabel(STR_RENAME, 2);
        rename_item->setLbl(rename_lbl);
        rename_lbl->setTextOffset(1);

        // копіювати
        if (_files.size() >= id && !_files[id - 1].isDir())
        {
            MenuItem *copy_item = creator.getMenuItem(ID_ITEM_COPY);
            _context_menu->addItem(copy_item);

            Label *copy_lbl = creator.getItemLabel(STR_COPY, 2);
            copy_item->setLbl(copy_lbl);
            copy_lbl->setTextOffset(1);
        }

        // перемістити
        MenuItem *move_item = creator.getMenuItem(ID_ITEM_MOVE);
        _context_menu->addItem(move_item);

        Label *move_lbl = creator.getItemLabel(STR_MOVE, 2);
        move_item->setLbl(move_lbl);
        move_lbl->setTextOffset(1);

        // видалити
        MenuItem *delete_item = creator.getMenuItem(ID_ITEM_REMOVE);
        _context_menu->addItem(delete_item);

        Label *delete_lbl = creator.getItemLabel(STR_DELETE, 2);
        delete_item->setLbl(delete_lbl);
        delete_lbl->setTextOffset(1);
    }

    // імпорт
    MenuItem *import_item = creator.getMenuItem(ID_ITEM_IMPORT);
    _context_menu->addItem(import_item);

    Label *import_lbl = creator.getItemLabel(STR_IMPORT, 2);
    import_item->setLbl(import_lbl);
    import_lbl->setTextOffset(1);

    // експорт
    MenuItem *export_item = creator.getMenuItem(ID_ITEM_EXPORT);
    _context_menu->addItem(export_item);

    Label *export_lbl = creator.getItemLabel(STR_EXPORT, 2);
    export_item->setLbl(export_lbl);
    export_lbl->setTextOffset(1);

    // оновити
    MenuItem *upd_item = creator.getMenuItem(ID_ITEM_UPDATE);
    _context_menu->addItem(upd_item);

    Label *upd_lbl = creator.getItemLabel(STR_UPDATE, 2);
    upd_item->setLbl(upd_lbl);
    upd_lbl->setTextOffset(1);

    //
    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth() - 1,
                          D_HEIGHT - PADDING_BOTT - _context_menu->getHeight() - 2);
}

void FilesContext::hideContextMenu()
{
    getLayout()->delWidgetByID(ID_CNTXT_MENU);
    _mode = MODE_NAVIGATION;
    _files_list->enable();
}

//-------------------------------------------------------------------------------------------

void FilesContext::showDialog(Mode mode)
{
    WidgetCreator creator;
    IWidgetContainer *layout = creator.getEmptyLayout();

    _dialog_txt = new TextBox(ID_DIALOG_TXT);
    _dialog_txt->setTextOffset(5);
    _dialog_txt->setWidth(D_WIDTH - 10);
    _dialog_txt->setHeight(40);
    _dialog_txt->setBackColor(TFT_WHITE);
    _dialog_txt->setTextColor(TFT_BLACK);
    _dialog_txt->setFontID(2);
    _dialog_txt->setTextSize(2);
    _dialog_txt->setPos(5, 0);
    _dialog_txt->setCornerRadius(3);

    _mode = mode;
    if (_mode == MODE_RENAME_DIALOG)
    {
        _old_name = _files_list->getCurrItemText();
        _dialog_txt->setText(_old_name);
    }

    _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);

    layout->setBackColor(TFT_BLACK);
    layout->addWidget(_dialog_txt);
    layout->addWidget(_keyboard);

    setLayout(layout);
}

void FilesContext::hideDialog()
{
    showFilesTmpl();
    if (_dialog_success_res)
        indexCurDir();
    fillFilesTmpl();
}

void FilesContext::saveDialogResult()
{
    if (_mode == MODE_NEW_DIR_DIALOG)
    {
        String dir_path;
        makePathFromBreadcrumbs(dir_path);
        dir_path += "/";
        dir_path += _dialog_txt->getText();

        _dialog_success_res = _f_mgr.createDir(dir_path.c_str());
        showResultToast(_dialog_success_res);
    }
    else if (_mode = MODE_RENAME_DIALOG)
    {
        String old_name;
        String new_name;

        makePathFromBreadcrumbs(old_name);
        new_name = old_name;

        old_name += "/";
        old_name += _old_name;
        _old_name = "";

        new_name += "/";
        new_name += _dialog_txt->getText();

        _dialog_success_res = _f_mgr.rename(old_name.c_str(), new_name.c_str());
        showResultToast(_dialog_success_res);
    }

    hideDialog();
}

void FilesContext::keyboardClickHandler()
{
    // uint16_t id = _keyboard->getCurrentBtnID();
    // if (id ==)
    // {
    // }

    // Можна оброблювати по ID кнопки, але в даному випадку зручніше оброблювати текст,
    // Тому що ніякі керуючі кнопки не використовуються.

    _dialog_txt->addChars(_keyboard->getCurrBtnTxt().c_str());
}

//-------------------------------------------------------------------------------------------

void FilesContext::prepareFileMoving()
{
    makePathFromBreadcrumbs(_path_from);
    _name_from = _files_list->getCurrItemText();

    _has_moving_file = true;
    _has_copying_file = false;

    hideContextMenu();
}

void FilesContext::prepareFileCopying()
{
    makePathFromBreadcrumbs(_path_from);
    _name_from = _files_list->getCurrItemText();

    _has_moving_file = false;
    _has_copying_file = true;

    hideContextMenu();
}

void FilesContext::pasteFile()
{
    String old_file_path = _path_from;
    old_file_path += "/";
    old_file_path += _name_from;

    String new_file_path;
    makePathFromBreadcrumbs(new_file_path);

    new_file_path += "/";
    new_file_path += _name_from;

    if (_has_moving_file)
    {
        if (_f_mgr.exists(old_file_path.c_str()) && _f_mgr.rename(old_file_path.c_str(), new_file_path.c_str()))
        {
            indexCurDir();
            fillFilesTmpl();
            hideContextMenu();
            showResultToast(true);
        }
        else
            showResultToast(false);
    }
    else if (_has_copying_file)
    {
        if (!_f_mgr.fileExist(old_file_path.c_str()) || !_f_mgr.startCopyFile(old_file_path.c_str(), new_file_path.c_str()))
        {
            showResultToast(false);
        }
        else
        {
            if (old_file_path.equals(new_file_path))
            {
                new_file_path += "_copy";

                while (_f_mgr.fileExist(new_file_path.c_str(), true))
                    new_file_path += "_copy";
            }

            _copy_to_path = new_file_path;

            showCopyingTmpl();
        }
    }

    _has_moving_file = false;
    _has_copying_file = false;

    _path_from = "";
    _name_from = "";
}

void FilesContext::removeFile()
{
    String file_name;
    makePathFromBreadcrumbs(file_name);

    file_name += "/";
    file_name += _files_list->getCurrItemText();

    if (_f_mgr.startRemoving(file_name.c_str()))
        showRemovingTmpl();
}

//-------------------------------------------------------------------------------------------

void FilesContext::update()
{
    if (_mode == MODE_SD_UNCONN)
    {
        if (_input.isReleased(BtnID::BTN_BACK))
        {
            _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
            openContextByID(ID_CONTEXT_MENU);
        }

        return;
    }

    if (_input.isPressed(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, PRESS_LOCK);
        if (_mode == MODE_NAVIGATION)
            showContextMenu();
        else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            saveDialogResult();
    }
    else if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);

        if (_mode == MODE_NAVIGATION)
            openContextByID(ID_CONTEXT_MENU);
        else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            hideDialog();
    }
    else if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        up();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        down();
    }
    else if (_input.isHolded(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, HOLD_LOCK);
        if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            _keyboard->focusRight();
    }
    else if (_input.isHolded(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, HOLD_LOCK);
        if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
            _keyboard->focusLeft();
    }
    else if (_input.isReleased(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);
        ok();
    }
    else if (_input.isReleased(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
        back();
    }

    if (_f_mgr.isWorking())
    {
        if ((millis() - _upd_msg_time) > UPD_TRACK_INF_INTERVAL)
        {
            String upd_txt;
            String upd_progress;

            if (_upd_counter > 2)
                _upd_counter = 0;
            else
                ++_upd_counter;

            for (uint8_t i{0}; i < _upd_counter; ++i)
                upd_progress += ".";

            if (_mode == MODE_CANCELING)
            {
                upd_txt = STR_CANCELING;
                upd_txt += upd_progress;
                _msg_lbl->setText(upd_txt);
            }
            else if (_mode == MODE_COPYING)
            {
                _task_progress->setProgress(_f_mgr.getCopyProgress());
                _upd_msg_time = millis();
            }
            else if (_mode == MODE_REMOVING)
            {
                upd_txt = STR_REMOVING;
                upd_txt += upd_progress;
                _msg_lbl->setText(upd_txt);
            }

            _upd_msg_time = millis();
        }
    }
}

void FilesContext::ok()
{
    if (_mode == MODE_NAVIGATION)
        openNextLevel();
    else if (_mode == MODE_CONTEXT_MENU)
    {
        uint16_t id = _context_menu->getCurrItemID();

        if (id == ID_ITEM_UPDATE)
        {
            hideContextMenu();
            indexCurDir();
            fillFilesTmpl();
        }
        else if (id == ID_ITEM_REMOVE)
            removeFile();
        else if (id == ID_ITEM_MOVE)
            prepareFileMoving();
        else if (id == ID_ITEM_COPY)
            prepareFileCopying();
        else if (id == ID_ITEM_PASTE)
            pasteFile();
        else if (id == ID_ITEM_NEW_DIR)
            showDialog(MODE_NEW_DIR_DIALOG);
        else if (id == ID_ITEM_RENAME)
            showDialog(MODE_RENAME_DIALOG);
        else if (id == ID_ITEM_IMPORT)
            startFileServer(FileServer::SERVER_MODE_RECEIVE);
        else if (id == ID_ITEM_EXPORT)
            startFileServer(FileServer::SERVER_MODE_SEND);
    }
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        keyboardClickHandler();
}

void FilesContext::back()
{
    if (_f_mgr.isWorking() && _mode != MODE_CANCELING)
    {
        showCancelingTmpl();
        _f_mgr.cancel();
    }
    else if (_mode == MODE_CONTEXT_MENU)
        hideContextMenu();
    else if (_mode == MODE_NAVIGATION)
        openPrevlevel();
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        _dialog_txt->removeLastChar();
    else if (_mode == MODE_FILE_SERVER)
        stopFileServer();
}

void FilesContext::up()
{
    if (_mode == MODE_NAVIGATION)
    {
        if (_files_list->focusUp())
        {
            updateFileInfo();
            _scrollbar->scrollUp();
        }
    }
    else if (_mode == MODE_CONTEXT_MENU)
        _context_menu->focusUp();
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        _keyboard->focusUp();
}

void FilesContext::down()
{
    if (_mode == MODE_NAVIGATION)
    {
        if (_files_list->focusDown())
        {
            updateFileInfo();
            _scrollbar->scrollDown();
        }
    }
    else if (_mode == MODE_CONTEXT_MENU)
        _context_menu->focusDown();
    else if (_mode == MODE_NEW_DIR_DIALOG || _mode == MODE_RENAME_DIALOG)
        _keyboard->focusDown();
}

void FilesContext::updateFileInfo()
{
    String file_name;
    makePathFromBreadcrumbs(file_name);
    file_name += "/";
    file_name += _files_list->getCurrItemText();

    String str_size;
    double kb_size = _f_mgr.getFileSize(file_name.c_str()) / 1024;

    if (kb_size == 0)
    {
        str_size = "0";
    }
    else if (kb_size < 1024)
    {
        str_size = String(kb_size);
        str_size += "kb";
    }
    else
    {
        str_size = String(kb_size / 1024);
        str_size += "mb";
    }

    _file_size_lbl->setText(str_size.c_str());
    _file_size_lbl->updateWidthToFit();

    String pos = "[";
    pos += String(_files_list->getCurrItemID());
    pos += "/";
    pos += String(_files.size());
    pos += "]";
    _file_pos_lbl->setText(pos.c_str());
    _file_pos_lbl->updateWidthToFit();
}

void FilesContext::openNextLevel()
{
    String next_dir = "/";
    next_dir += _files_list->getCurrItemText();

    String next_dir_path;
    makePathFromBreadcrumbs(next_dir_path);
    next_dir_path += next_dir;

    if (!_f_mgr.dirExist(next_dir_path.c_str(), true))
        return;

    _breadcrumbs.push_back(next_dir);
    indexCurDir();
    fillFilesTmpl();
}

void FilesContext::openPrevlevel()
{
    if (_breadcrumbs.size() > 0)
    {
        _breadcrumbs.pop_back();
        indexCurDir();
        fillFilesTmpl();
    }
    else
    {
        openContextByID(ID_CONTEXT_MENU);
    }
}

void FilesContext::indexCurDir()
{
    String dir_path;
    makePathFromBreadcrumbs(dir_path);
    _f_mgr.indexAll(_files, dir_path.c_str());
}

void FilesContext::fillFilesTmpl()
{
    _files_list->delWidgets();

    std::vector<MenuItem *> items;

    makeMenuFilesItems(items, 0, _files_list->getItemsNumOnScreen());

    for (size_t i = 0; i < items.size(); ++i)
        _files_list->addItem(items[i]);

    _scrollbar->setValue(0);
    _scrollbar->setMax(_files.size());

    updateFileInfo();
}

void FilesContext::startFileServer(FileServer::ServerMode mode)
{
    SettingsManager sm;
    String ssid = sm.get(STR_PREF_FS_AP_SSID);
    String pwd = sm.get(STR_PREF_FS_AP_PWD);

    if (ssid.isEmpty())
        ssid = STR_DEF_SSID;
    if (pwd.isEmpty())
        pwd = STR_DEF_PWD;

    _server.setSSID(ssid.c_str());
    _server.setPWD(pwd.c_str());
    //
    String cur_path;
    makePathFromBreadcrumbs(cur_path);
    if (_server.begin(cur_path.c_str(), mode))
    {
        QR_Gen gen;
        String addr = _server.getAddress();
        _qr_img_buff = gen.generateQR(addr.c_str(), 3);
        _qr_width = gen.getImageWidth();

        showServerTmpl();
    }
}

void FilesContext::stopFileServer()
{
    if (!_is_back_eabled)
        return;

    _server.stop();

    showFilesTmpl();
    indexCurDir();
    fillFilesTmpl();

    free(_qr_img_buff);
}

void FilesContext::taskDoneHandler(bool result)
{
    showResultToast(result);

    showFilesTmpl();
    indexCurDir();
    fillFilesTmpl();
}

void FilesContext::taskDone(bool result, void *arg)
{
    FilesContext *this_ptr = static_cast<FilesContext *>(arg);
    this_ptr->taskDoneHandler(result);
}

//-------------------------------------------------------------------------------------------

void FilesContext::makeMenuFilesItems(std::vector<MenuItem *> &items, uint16_t file_pos, uint8_t size)
{
    if (file_pos >= _files.size())
        return;

    uint16_t read_to = file_pos + size;

    if (read_to > _files.size())
        read_to = _files.size();

    WidgetCreator creator;
    items.clear();
    items.reserve(read_to - file_pos);

    for (uint16_t i = file_pos; i < read_to; ++i)
    {
        ++file_pos;

        MenuItem *item = creator.getMenuItem(file_pos);
        items.push_back(item);

        Label *lbl = new Label(1);
        item->setLbl(lbl);
        lbl->setTickerInFocus(true);

        if (_files[i].isDir())
            item->setImg(_dir_img);

        lbl->setText(_files[i].getName());
    }
}

//-------------------------------------------------------------------------------------------

void FilesContext::handleNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    if (!cur_id)
        return;

    makeMenuFilesItems(items, cur_id, size);
}

void FilesContext::onNextItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    FilesContext *this_ptr = static_cast<FilesContext *>(arg);
    this_ptr->handleNextItemsLoad(items, size, cur_id);
}

//

void FilesContext::handlePrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id)
{
    // Не отримали ID
    if (!cur_id)
        return;

    uint16_t item_pos = cur_id - 1;

    // Вже перший елемент
    if (!item_pos)
        return;

    // Вирахувати першу позицію звідки потрібно читати список.
    if (cur_id > size)
        item_pos = cur_id - size - 1;
    else
    {
        item_pos = 0;
        // Вирівняти скролбар, якщо меню було завантажене не з першого елемента
        _scrollbar->setValue(cur_id);
    }

    makeMenuFilesItems(items, item_pos, size);
}

void FilesContext::onPrevItemsLoad(std::vector<MenuItem *> &items, uint8_t size, uint16_t cur_id, void *arg)
{
    FilesContext *this_ptr = static_cast<FilesContext *>(arg);
    this_ptr->handlePrevItemsLoad(items, size, cur_id);
}

void FilesContext::showResultToast(bool result)
{
    if (result)
        showToast(STR_SUCCSESS, 1500);
    else
        showToast(STR_FAIL, 1500);
}
