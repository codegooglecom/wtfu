//---------------------------------------------------------------------------
#ifndef wtf_h
#define wtf_h

//---------------------------------------------------------------------------
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/file.h"
#   include "wx/notebook.h"
#   include "wx/listctrl.h"
#   include "wx/filename.h"
#   include "wx/dir.h"
#   include "wx/filefn.h"
#   include "wx/longlong.h"
#endif //precompiled headers

#include "defs.h"
#include "bencode.h"
#include "util.h"
#include "fsets.h"

class wtfFrame;

class wtfApp : public wxApp
{
private:    // User declarations
    wtfFrame *m_frame;
//        FilesSets *fsets;

public:     // User declarations
    wtfGEncoding *genc;

    virtual bool OnInit();
    virtual int OnExit();
};

DECLARE_APP(wtfApp)

class wtfMainTab;
class wtfTorrentTab;

class wtfFrame : public wxFrame
{
DECLARE_ABSTRACT_CLASS(wtfFrame)
private:
    wxNotebook *m_tabs;
    wtfMainTab *m_mainTab;
    wtfTorrentTab *m_torrentTab;

public:
    wtfFrame();

    void OnMenuExit(wxCommandEvent& event);
    void OnCloseExit(wxCloseEvent& event);

    void LoadFilesSets(wxCommandEvent& event);
    void SaveFilesSets(wxCommandEvent& event);
    void AddSet(wxCommandEvent& event);
    void EditSet(wxCommandEvent& event);
    void ManageNamesSets(wxCommandEvent& event);
    void DeleteSet(wxCommandEvent& event);
    void RenameFiles(wxCommandEvent& event);

    void TTAddTorrents(wxCommandEvent& event);
    void TTChangeDir(wxCommandEvent& event);
    void TTProcessSingle(wxCommandEvent& event);
    void TTProcessMulti(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

class FilesSetsCtrl;

class wtfMainTab : public wxPanel
{
DECLARE_ABSTRACT_CLASS(wtfMainTab)
private:
    FilesSetsCtrl *m_setsList;
    wxButton *m_btnEditSet;
    wxButton *m_btnEditNames;
    wxButton *m_btnDelSet;
    wxButton *m_btnRename;
    FilesSets *fsets;
    long m_selected;
    wxString m_filename;
    bool m_modified;

public:
    wtfMainTab(wxWindow *parent);
    ~wtfMainTab();

    bool LoadFilesSets(const wxString& filename);
    bool SaveFilesSets(bool force = false, bool name = false);
    void SelectSet(wxListEvent& event);
    void DeselectSet(wxListEvent& event);
    void SetFSTitle(wxListEvent& event);
    bool AddSet();
    bool EditSet(wxCommandEvent& event);
    static bool DoEditSet(wxWindow *parent, FilesSet *fset, const wxString& title = wxT("Editing files set..."));
    bool DoAddSet(FilesSet *fset);
    void ManageNamesSets();
    bool DeleteSet();
    bool RenameFiles();

    int ConfirmExit();
    void SetModifiedFlag(bool flag = true);

    DECLARE_EVENT_TABLE()
};

class FilesSetsCtrl : public wxListCtrl
{
DECLARE_ABSTRACT_CLASS(FilesSetsCtrl)
private:
    wxMenu *menu;
    bool m_ctxmenu;

public:
    FilesSetsCtrl(wxWindow* parent);
    ~FilesSetsCtrl();

    void ShowContextMenu(wxContextMenuEvent& event);
    void ShowSetContextMenu(wxListEvent& event);

    DECLARE_EVENT_TABLE()
};

WX_DEFINE_ARRAY_SIZE_T(size_t, wtfArraySizeT);
WX_DECLARE_STRING_HASH_MAP(wtfArraySizeT*, wtfFilesListBySize);
WX_DECLARE_STRING_HASH_MAP(wtfFilesListBySize*, wtfFilesList);

WX_DECLARE_STRING_HASH_MAP(size_t, wtfNamesToFiles);

class wtfTorrentTab : public wxPanel
{
DECLARE_ABSTRACT_CLASS(wtfTorrentTab)
private:
    wtfMainTab *m_mainTab;
    wxTextCtrl *m_torrents;
    wxTextCtrl *m_directory;
    wxCheckBox *m_subdirs;
    wtfGEncoding *genc;

public:
    wtfTorrentTab(wxWindow *parent, wtfMainTab *mtab);
    void AddTorrents(const bool clear = true);
    bool SetSourceDir(const wxString& dir);
    bool ProcessSingle();
    bool ProcessMulti();

    wxString DumpData(wbenData &data, const size_t sp = 0);

};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
