#ifndef wtfDEFS_H
#define wtfDEFS_H

#include "debug.h"

// GUI elements' ids

// main frame tabs
#define wtfMAIN_TAB         1
#define wtfTORRENT_TAB      2

// main frame menu
// File menu items
#define wtfFILE_MENU_OPEN       1
#define wtfFILE_MENU_SAVE       2
#define wtfFILE_MENU_SAVE_AS    3

// main tab controls
#define wtfFILES_SETS_LIST      11

// main tab buttons
#define wtfADD_NEW_SET_BUTTON   11
#define wtfEDIT_SET_BUTTON      12
#define wtfMANAGE_NS_BUTTON     13
#define wtfDELETE_SET_BUTTON    14
#define wtfRENAME_FILES_BUTTON  15

// files set context menu items' ids
#define wtfSET_CM_ADD_NEW       11
#define wtfSET_CM_EDIT          12
#define wtfSET_CM_MANAGE        13
#define wtfSET_CM_DELETE        14
#define wtfSET_CM_RENAME        15
#define wtfSET_CM_HARDLINK      16

// torrent tab buttons
#define wtfTT_SELECT_TORRENTS_BUTTON    21
#define wtfTT_ADD_TORRENTS_BUTTON       22
#define wtfTT_CHANGE_DIR_BUTTON         23
#define wtfTT_PROCESS_SINGLE_BUTTON     24
#define wtfTT_PROCESS_MULTI_BUTTON      25

// torrent tab text controls
#define wtfTT_TORRENTS_TEXTCTRL     21
#define wtfTT_DIRECTORY_TEXTCTRL    22
#define wtfTT_SUBDIR_CHECKBOX       23

// EditFilesSet dialog
#define wtfEFSD_PULSE_STEPS         100
#define wtfEFSD_TITLE_TEXTCTRL      1
#define wtfEFSD_DIR_TEXTCTRL        2
#define wtfEFSD_CHANGE_DIR_BUTTON   3
#define wtfEFSD_BROWSE_TREE         4
#define wtfEFSD_FILES_LIST          5
#define wtfEFSD_ADD_FILES_BUTTON        6
#define wtfEFSD_ADD_ALL_FILES_BUTTON    7
#define wtfEFSD_DEL_FILES_BUTTON        8
#define wtfEFSD_DEL_ALL_FILES_BUTTON    9

// EditNamesSets dialog
#define wtfENSsD_SETS_LIST          1
#define wtfENSsD_SETS_LIST_STYLE    (wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_EDIT_LABELS)
#define wtfENSsD_ADD_BUTTON         2
#define wtfENSsD_EDIT_BUTTON        3
#define wtfENSsD_DELETE_BUTTON      4
#define wtfENSsD_MARK_MAIN_BUTTON   5
#define wtfENSsD_CM_ADD             6
#define wtfENSsD_CM_EDIT            7
#define wtfENSsD_CM_DELETE          8
#define wtfENSsD_CM_MARK_MAIN       9

// EditNamesSet dialog
#define wtfENSD_TITLE               1
#define wtfENSD_DIR                 2
#define wtfENSD_BROWSE_DIR_BUTTON   3
#define wtfENSD_FILES_LIST          4
#define wtfENSD_DISAMBIGUATE_BUTTON 5
#define wtfENSD_ML_EDIT_BUTTON      6
#define wtfENSD_ML_TORRENT_BUTTON   7

// wtfRenamePreview dialog
#define wtfRP_LIST                  1
#define wtfRP_CHECKBOX_ALLNS        2
#define wtfRP_CHECKBOX_COPY         3
#define wtfRP_CHECKBOX_CREATE_DIRS  4
#define wtfRP_CHECKBOX_OVERWRITE    5
#define wtfRP_RENAME_BUTTON         6
#define wtfRP_LINK_BUTTON           7
#define wtfRP_SYMLINK_BUTTON        8

#define wtfRP_OPTION_ALLNS          1
#define wtfRP_OPTION_COPY           2
#define wtfRP_OPTION_CREATE_DIRS    4
#define wtfRP_OPTION_OVERWRITE      8

#define wtfEV_LIST      1

// wtfRenamerQuestion dialog
#define wtfID_OK           1
#define wtfID_RETRY        2
#define wtfID_IGNORE       3
#define wtfID_IGNORE_ALL   4
#define wtfID_STOP         5
#define wtfID_CANCEL       6
#define wtfID_ABORT        7

#define wtfOK           1
#define wtfRETRY        2
#define wtfIGNORE       4
#define wtfIGNORE_ALL   8
#define wtfSTOP        16
#define wtfCANCEL      32
#define wtfABORT       64

// end of GUI elements' ids

// error codes
// FilesSets class error codes
#ifndef W_FSETS_ERROR_CODES
#define W_FSETS_ERROR_CODES

#endif // #ifndef W_FSETS_ERROR_CODES

// end of error codes

#endif // #ifndef wtfDEFS_H

