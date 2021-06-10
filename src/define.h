#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/progdlg.h>

wxDECLARE_EVENT(EVT_COMMAND_CLEARMAILVIEW, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_LOGGEDIN, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_PERFORMUPDATE, wxCommandEvent);
wxDECLARE_EVENT(EVT_COMMAND_MAINFRAME_ENABLE, wxCommandEvent);

enum {
    ID_EVT_CLEARMAILVIEW,
    ID_EVT_LOGGEDIN,
    ID_EVT_PERFORMUPDATE,
    ID_EVT_ONREGISTER,
    ID_GenKeys,
    ID_Login,
    ID_Registration,
    ID_Delete_Account,
    ID_Update,
    ID_Data_Get,
    ID_Write_Mail,
    ID_Delete_Mail,
    ID_Minimize,
    ID_Main_Panel,
    ID_Sender_Text,
    ID_Account_Text,
    ID_TaskbarIcon_Clicked,
    ID_TaskbarIcon_Show,
    ID_TaskbarIcon_Close,
    ID_Mail_Folder_Change,
    UPDATE_THREAD_FINISH,
    PULSE_PROGRESS,
    REGISTER_THREAD_FINISH,
    ID_Edit_IP,
    ID_Edit_Username,
    ID_Edit_Email,
    ID_Edit_Password,
    ID_Edit_Password_Repeat,
    ID_Button_Register,
    ID_Button_PasswdReset,
    ID_Button_Login,
    ID_Logout,
    ID_Edit_To,
    ID_Edit_Caption,
    ID_Edit_Mail,
    ID_Button_Send,
    ID_Timer_Mail_Sync,
    ID_ListBox_Inbox,
    ID_Text_Mail,
    ID_MENU_CONTEXT_INBOX_1,
    ID_MENU_CONTEXT_INBOX_2,
    ID_MENU_CONTEXT_INBOX_3,
    ID_EVT_MAINFRAME_ENABLE
};