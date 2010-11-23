#ifndef wtfDIALOGS_H
#define wtfDIALOGS_H

/*
 * This header file contains declarations for Torrent Files
 * Utility by Wintermute common dialogs
 */

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/dialog.h"
#   include "wx/listbox.h"
#   include "wx/listctrl.h"
#   include "wx/textctrl.h"
#   include "wx/treectrl.h"
#   include "wx/dir.h"
#   include "wx/dnd.h"
#   include "wx/hashset.h"
#   include "wx/progdlg.h"
#   include "wx/event.h"
#   include "wx/file.h"
#   include "wx/gdicmn.h"
#endif //precompiled headers

#include "defs.h"
#include "fsets.h"
#include "genc.h"
#include "wtf.h"
#include "util.h"


class wtfDirTraverser : public wxDirTraverser
{
DECLARE_ABSTRACT_CLASS(wtfDirTraverser)
private:
    wxTreeCtrl *m_tree;
    wxArrayString m_files;
    wxArrayString m_dirs;
    int m_len;

public:
    wtfDirTraverser(wxTreeCtrl *tree);

    void SetLen(int len);

    virtual wxDirTraverseResult OnFile(const wxString& filename);
    virtual wxDirTraverseResult OnDir(const wxString& dirname);
    bool Flush(wxTreeItemId root);
};

class wtfDirTreeData : public wxTreeItemData
{
DECLARE_ABSTRACT_CLASS(wtfDirTreeData)
private:
    char m_state;
/*
 * m_state =
 *   0 - file entry
 *   1 - directory entry, no need traverse
 *   2 - directory entry, need traverse
 */

public:

#ifdef wtfDEBUG_MEMORY_LEAKS
    static int Count;
#endif // wtfDEBUG_MEMORY_LEAKS

    wtfDirTreeData();// : m_state(0) { wtfDirTreeData::Count++; wxLogMessage(wxT("Created instance N %d of class wtfDirTreeData"), wtfDirTreeData::Count); }
    wtfDirTreeData(char state);// : m_state(state) { wtfDirTreeData::Count++; wxLogMessage(wxT("Created instance N %d of class wtfDirTreeData"), wtfDirTreeData::Count); }
    ~wtfDirTreeData();// { wxLogMessage(wxT("Deleted instance N %d of class wtfDirTreeData"), wtfDirTreeData::Count); wtfDirTreeData::Count--; }

    bool NeedTraverse()
    {
        return (m_state == 2);
    }
    void SetState(char state)
    {
        m_state = state;
    }
};

class wtfFilesDropTarget;

class wtfDirTree : public wxTreeCtrl
{
DECLARE_ABSTRACT_CLASS(wtfDirTree)
private:
    wxString m_dir;
    wtfDirTraverser *m_traverser;
    wtfFilesDropTarget *m_dnd;

public:
    wtfDirTree(wxWindow* parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS);
    ~wtfDirTree();

    void SetDnD(wtfFilesDropTarget *dnd) { m_dnd = dnd; }

    bool NeedTraverse(const wxTreeItemId& item);
    bool TraverseDir(const wxTreeItemId& item);
    bool SetFSDir(const wxString& dir);
#ifdef wtfDND_FULL
    void StartDrag(wxTreeEvent& event);
#endif // wtfDND_FULL
    void ExpandItem(wxTreeEvent& event);
    wxString GetFSDir();
    wxString GetDirPath(const wxTreeItemId& item);

    DECLARE_EVENT_TABLE()
};

WX_DECLARE_HASH_SET( wxString, wxStringHash, wxStringEqual, FilesHS );

class wtfChosenFiles : public wxListBox
{
DECLARE_ABSTRACT_CLASS(wtfChosenFiles)
private:
    wtfDirTree *m_source;
    FilesHS m_files;

    unsigned int GetInsertIndex(const wxString& filename);
    void AddItems(const wxArrayTreeItemIds& items);

public:
    wtfChosenFiles(wtfDirTree *source, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    ~wtfChosenFiles();

    int AddFile(const wxString& filename);
    int AddFile(const wxTreeItemId& item);

    wxString GetFile(unsigned int n);
    bool DeleteFile(unsigned int n);
    void ClearList();
    void AddFiles(bool all = false);
    void DeleteFiles(bool all = false);
    void DeleteNonExistedFiles();
};

class wtfDnDTraverser : public wxDirTraverser
{
DECLARE_ABSTRACT_CLASS(wtfDnDTraverser)
private:
    wtfChosenFiles *m_target;
    int m_len;
    unsigned int *m_pr;
    wxProgressDialog *m_dlg;

public:
    wtfDnDTraverser(wtfChosenFiles *target) { m_target = target; m_len = 0; m_cont = true; }

    void SetProgressData(wxProgressDialog *dlg, unsigned int *pr, int len);

    virtual wxDirTraverseResult OnFile(const wxString& filename);
    virtual wxDirTraverseResult OnDir(const wxString& dirname);

    bool m_cont;
};

class wtfFilesDropTarget : public wxFileDropTarget
{
DECLARE_ABSTRACT_CLASS(wtfFilesDropTarget)
private:
    wtfChosenFiles *m_target;
    wxString m_dir;
    int m_len;
    wtfDnDTraverser *m_traverser;

public:
    wtfFilesDropTarget(wtfChosenFiles *target) : wxFileDropTarget() { m_target = target; m_len = 0; m_traverser = new wtfDnDTraverser(m_target); }
    ~wtfFilesDropTarget();

    void SetBaseDir(const wxString& dir) { m_dir = dir; m_len = m_dir.Len(); }
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& files);
};

// This class is editing files set dialog
// (normally appears when user click "Add new set..." or "Edit set..." button)
class EditFilesSet : public wxDialog
{
DECLARE_ABSTRACT_CLASS(EditFilesSet)
private:
    wxTextCtrl *m_title;
    wxTextCtrl *m_dir;
    wtfDirTree *m_tree;
    wtfChosenFiles *m_files;
    wtfFilesDropTarget *m_dnd;

public:
    EditFilesSet(wxWindow *parent, const wxString& title);
    EditFilesSet(wxWindow *parent, const wxString& title, const wxString& setTitle, const wxString& setDir = wxT(""));
//    ~EditFilesSet();

    bool SetFSDir(const wxString& dir);
    bool SetFSTitle(const wxString& title);
    bool AddFSFile(const wxString& filename);
    void ChooseDir(wxCommandEvent& event);
    void AddFiles(wxCommandEvent& event);
    void DelFiles(wxCommandEvent& event);
    void CheckData(wxCommandEvent& event);
    wxString GetFSTitle();
    wxString GetFSDir();
    wxString GetFirstFSFile(int *i);
    wxString GetNextFSFile(int *i);

    DECLARE_EVENT_TABLE()
};

// This class is editing names sets dialog
// (normally appears when user click "Manage names sets..." button)
class EditNamesSets : public wxDialog
{
DECLARE_ABSTRACT_CLASS(EditNamesSets)
private:
    wtfMainTab *m_mainTab;
    bool new_set;
    FilesSet *fset;
    wxListCtrl *m_setsList;
    wxButton *m_btnEdit, *m_btnDelete, *m_btnMarkMain;
    wxMenu *m_menu;
    long m_selected;

public:
    EditNamesSets(wxWindow *parent, const wxString& title, wtfMainTab *mtab, FilesSet *fs = NULL);
    ~EditNamesSets();

    void SelectSet(wxListEvent& event);
    void DeselectSet(wxListEvent& event);
    void OnStartLabelEdit(wxListEvent& event);
    void SetNSTitle(wxListEvent& event);
    void ShowSetContextMenu(wxListEvent& event);
    void ShowContextMenu(wxMouseEvent& event);
    void SetMainId(wxCommandEvent& event);
    void AddNS(wxCommandEvent& event);
    void EditNS(wxCommandEvent& event);
    static bool DoEditNS(wxWindow *parent, FilesSet *fset, NamesSet *nset, const wxString &title);
    void DeleteNS(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

typedef union {
    wxString *str;
    wxArrayString *arr;
} wtfVariantValue;

class wtfVariant : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfPrefixes)
private:
    int m_type;
    wtfVariantValue m_value;
public:
    wtfVariant();
    wtfVariant(int type);
    wtfVariant(int type, wtfVariantValue value);
    ~wtfVariant();

    wtfVariantValue GetValue();
    int GetType();
    void AddString(const wxString &str);
    void Replace(const wxString &str);

    size_t GetCount();
    wxString& GetItem(size_t);
};

WX_DECLARE_STRING_HASH_MAP( wtfVariant*, wtfVariants );
WX_DEFINE_ARRAY_SIZE_T( size_t, wtfVarPath );

class wtfPrefixes : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfPrefixes)
private:
    wtfVariants m_prefixes;
public:
    wtfPrefixes();
    ~wtfPrefixes();

    void AddString(const wxString &str);
    void Clear();
    int Count(const wxString &str);
    wtfVariant* GetPrefix(const wxString &str);
    size_t GetVariants(wxArrayString *arr, const wxString& prefix = wxT(""));
};

class EditNamesSet;

class wtfEditVariants : public wxDialog
{
DECLARE_ABSTRACT_CLASS(wtfEditVariants)
private:
    wxTextCtrl *m_list;
    EditNamesSet *m_parent;

public:
    wtfEditVariants(wxWindow *parent, const wxString& title);

    bool ListModified();
    void AddString(const wxString& item);
    void DoOk(wxCommandEvent& event);

    void ClearList();

    DECLARE_EVENT_TABLE()
};

// This class is editing names set dialog
// (normally appears when user click "Edit names set..." button on the EditNamesSets dialog window)
class EditNamesSet : public wxDialog
{
DECLARE_ABSTRACT_CLASS(EditNamesSet)
private:
    wxTextCtrl *m_title;
    wxTextCtrl *m_dir;
    wxListCtrl *m_filesList;
    wxTextCtrl *m_editCtrl;
    size_t m_editLen;
    wtfPrefixes *m_prefixes;
    int start_edit;
    NamesSet *nset;
    NamesSet *fset;
    wtfGEncoding *genc;
    wtfEditVariants *m_listDlg;

public:
    EditNamesSet(wxWindow *parent, const wxString& title, NamesSet *ns = NULL);
    ~EditNamesSet();

    void ListOnSize(wxSizeEvent &event);
    void StartEdit(wxListEvent &event);
    void OnBeginLabelEdit(wxListEvent &event);
    void OnEndLabelEdit(wxListEvent &event);
    void DoOk(wxCommandEvent& event);
    wxString GetNSDir();

    void AddFileName(const int id, const wxString& file, const wxString& name);
    void ClearVariants();
    void AddVariant(const wxString& str);
    void ChooseDir(wxCommandEvent& event);
    void SetNSDir(const wxString& dir);
    void UpdateFilesList();

    void DoDisambiguate(wxCommandEvent& event);
    void DoEditList(wxCommandEvent& event);
    void OnTextUpdated(wxCommandEvent& event);

// Manage list of predefined names
    void LoadNamesFromTorrent(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

class wtfRenamerQuestion : public wxDialog
{
DECLARE_ABSTRACT_CLASS(wtfRenamerQuestion)
public:
	wtfRenamerQuestion();
	wtfRenamerQuestion(wxWindow* parent, const wxString& title, const wxString& message, long style = wtfOK | wtfCANCEL);

	bool Create(wxWindow* parent, const wxString& title, const wxString& message, long style = wtfOK | wtfCANCEL);
	void OnButtonPressed(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE()
};

class wtfRenamerOptions : public wsdgRenamerOptions
{
DECLARE_ABSTRACT_CLASS(wtfRenamerOptions)
public:
	bool m_mustUndo, m_skipAll;
	wtfRenamerOptions();
	wtfRenamerOptions(long flags);

	virtual wtfRenamerAnswer OnError(wtfRenamerAction *action, int error, const wxString& msg = wxEmptyString);

	bool MustUndo() const;
};

class wtfRenamePreview : public wxDialog
{
DECLARE_ABSTRACT_CLASS(wtfRenamePreview)
private:
    wxListCtrl *m_list;
    long m_options;
    FilesSet *m_fs;
    NamesSet *m_ns;
    int m_mid, m_rid;

public:
    wtfRenamePreview(wxWindow *parent, const wxString& title, const wxString& message, FilesSet* fs, int nsid);
    ~wtfRenamePreview();

    void OnOptionChange(wxCommandEvent& event);
    void AddItem(const wxString& oldname, const wxString& newname);
    void UpdateList(bool full);

    void OnButtonPressed(wxCommandEvent& event);
};

#endif // #ifndef wtfDIALOGS_H

