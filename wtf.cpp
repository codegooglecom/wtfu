//---------------------------------------------------------------------------
#include "wtf.h"

#include "dialogs.h"

IMPLEMENT_APP(wtfApp)

bool wtfApp::OnInit()
{
    genc = WXDEBUG_NEW wtfGEncoding(wxT("encdata.xml"));
    m_frame = WXDEBUG_NEW wtfFrame();

    m_frame->Show(true);
//    m_frame->SetTransparent(128);
    SetTopWindow(m_frame);

    return true;
}

int wtfApp::OnExit()
{
    delete(genc);
    wtfRenamer::Free();
    return 0;
}


IMPLEMENT_ABSTRACT_CLASS(wtfFrame, wxFrame)

BEGIN_EVENT_TABLE(wtfFrame, wxFrame)
    EVT_BUTTON(wtfADD_NEW_SET_BUTTON,       wtfFrame::AddSet)
    EVT_BUTTON(wtfEDIT_SET_BUTTON,          wtfFrame::EditSet)
    EVT_BUTTON(wtfMANAGE_NS_BUTTON,         wtfFrame::ManageNamesSets)
    EVT_BUTTON(wtfDELETE_SET_BUTTON,        wtfFrame::DeleteSet)
    EVT_BUTTON(wtfRENAME_FILES_BUTTON,      wtfFrame::RenameFiles)
    EVT_MENU(wtfSET_CM_ADD_NEW,     wtfFrame::AddSet)
    EVT_MENU(wtfSET_CM_EDIT,        wtfFrame::EditSet)
    EVT_MENU(wtfSET_CM_MANAGE,      wtfFrame::ManageNamesSets)
    EVT_MENU(wtfSET_CM_DELETE,      wtfFrame::DeleteSet)
    EVT_MENU(wtfSET_CM_RENAME,      wtfFrame::RenameFiles)

    EVT_BUTTON(wtfTT_SELECT_TORRENTS_BUTTON,    wtfFrame::TTAddTorrents)
    EVT_BUTTON(wtfTT_ADD_TORRENTS_BUTTON,       wtfFrame::TTAddTorrents)
    EVT_BUTTON(wtfTT_CHANGE_DIR_BUTTON,         wtfFrame::TTChangeDir)
    EVT_BUTTON(wtfTT_PROCESS_SINGLE_BUTTON,     wtfFrame::TTProcessSingle)
    EVT_BUTTON(wtfTT_PROCESS_MULTI_BUTTON,      wtfFrame::TTProcessMulti)

    EVT_MENU(wtfFILE_MENU_OPEN,     wtfFrame::LoadFilesSets)
    EVT_MENU(wtfFILE_MENU_SAVE,     wtfFrame::SaveFilesSets)
    EVT_MENU(wtfFILE_MENU_SAVE_AS,  wtfFrame::SaveFilesSets)
    EVT_MENU(wxID_EXIT,             wtfFrame::OnMenuExit)
    EVT_CLOSE(                      wtfFrame::OnCloseExit)
END_EVENT_TABLE()

wtfFrame::wtfFrame()
    : wxFrame(NULL, wxID_ANY, wxT("Torrent Files utility by Wintermute"), wxDefaultPosition,
        wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxMINIMIZE_BOX | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
// creating menu bar
    wxMenuBar *menuBar = WXDEBUG_NEW wxMenuBar();
    wxMenu *wtfFileMenu = WXDEBUG_NEW wxMenu();
    
    wtfFileMenu->Append(wtfFILE_MENU_OPEN, wxT("&Open files set...\tCtrl-O"));
    wtfFileMenu->Append(wtfFILE_MENU_SAVE, wxT("&Save\tCtrl-S"));
    wtfFileMenu->Append(wtfFILE_MENU_SAVE_AS, wxT("Save &as..."));
    wtfFileMenu->AppendSeparator();
    wtfFileMenu->Append(wxID_EXIT, wxT("&Exit"));

//    wtfFileMenu->Enable(wtfFILE_MENU_SAVE, false);
//    wtfFileMenu->Enable(wtfFILE_MENU_SAVE_AS, false);

    menuBar->Append(wtfFileMenu, wxT("&File"));

    SetMenuBar(menuBar);

// creating tab control via wxNotebook
    wxPanel *panel = WXDEBUG_NEW wxPanel(this);
    wxBoxSizer* m_sizerFrame = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);

    m_tabs = WXDEBUG_NEW wxNotebook(panel, wxID_ANY);

// creating tabs
    m_mainTab = WXDEBUG_NEW wtfMainTab(m_tabs);
    m_torrentTab = WXDEBUG_NEW wtfTorrentTab(m_tabs, m_mainTab);

// adding tabs to the tabs control
    m_tabs->AddPage(m_mainTab, wxT("Manage files sets"));
    m_tabs->AddPage(m_torrentTab, wxT("Make file set from .torrent file..."));

    m_sizerFrame->Add(m_tabs, wxSizerFlags(1).Expand());

    panel->SetSizer(m_sizerFrame);
    m_sizerFrame->Fit(this);
    m_sizerFrame->SetSizeHints(this);

// setting icon for this frame
    SetIcon(wxICON(wtf));

// show frame
    Show(true);
}

void wtfFrame::LoadFilesSets(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog *dlg = WXDEBUG_NEW wxFileDialog(this, wxT("Select files sets file..."),
                                wxT(""), wxT(""),
                                wxT("XML files (*.xml)|*.xml|All files (*.*)|*.*"),
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    int rv = dlg->ShowModal();
    wxString filename = dlg->GetPath();

    dlg->Destroy();

    if (rv != wxID_OK)
    {
        return;
    }
    m_mainTab->LoadFilesSets(filename);
}

void wtfFrame::SaveFilesSets(wxCommandEvent& event)
{
    m_mainTab->SaveFilesSets(true, event.GetId() == wtfFILE_MENU_SAVE_AS);
}

void wtfFrame::AddSet(wxCommandEvent& WXUNUSED(event))
{
    m_mainTab->AddSet();
}

void wtfFrame::EditSet(wxCommandEvent& event)
{
    m_mainTab->EditSet(event);
}

void wtfFrame::ManageNamesSets(wxCommandEvent& WXUNUSED(event))
{
    m_mainTab->ManageNamesSets();
}

void wtfFrame::DeleteSet(wxCommandEvent& WXUNUSED(event))
{
    m_mainTab->DeleteSet();
}

void wtfFrame::RenameFiles(wxCommandEvent& WXUNUSED(event))
{
    m_mainTab->RenameFiles();
}

void wtfFrame::TTAddTorrents(wxCommandEvent& event)
{
#ifdef wtfDEBUG_MEMORY_LEAKS
    wxDebugContext::SetCheckpoint();
    wxLog::SetLogLevel(wxLOG_Max);
#endif // wtfDEBUG_MEMORY_LEAKS
    if (event.GetId() == wtfTT_SELECT_TORRENTS_BUTTON)
    {
        m_torrentTab->AddTorrents(true);
    } else {
        m_torrentTab->AddTorrents(false);
    }
#ifdef wtfDEBUG_MEMORY_LEAKS
    wxDebugContext::Dump();
    wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
}

void wtfFrame::TTChangeDir(wxCommandEvent& WXUNUSED(event))
{
    wxDirDialog *dlg = WXDEBUG_NEW wxDirDialog(this, wxT("Choose directory:"), wxT(""), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dlg->ShowModal() == wxID_OK)
    {
        m_torrentTab->SetSourceDir( dlg->GetPath() );
    }
    dlg->Destroy();
}

void wtfFrame::TTProcessSingle(wxCommandEvent& WXUNUSED(event))
{
    m_torrentTab->ProcessSingle();
}

void wtfFrame::TTProcessMulti(wxCommandEvent& WXUNUSED(event))
{
    m_torrentTab->ProcessMulti();
}

void wtfFrame::OnMenuExit(wxCommandEvent& WXUNUSED(event))
{
    Close(false);
}

void wtfFrame::OnCloseExit(wxCloseEvent& event)
{
    int rv = m_mainTab->ConfirmExit();

    if (rv == wxID_YES)
    {
        m_mainTab->SaveFilesSets(false, false);
    }
    if (rv != wxID_YES && rv != wxID_NO && event.CanVeto())
    {
        event.Veto();
        return;
    }
    Destroy();
}


IMPLEMENT_ABSTRACT_CLASS(wtfMainTab, wxPanel)

BEGIN_EVENT_TABLE(wtfMainTab, wxPanel)
    EVT_LIST_ITEM_SELECTED(wtfFILES_SETS_LIST,      wtfMainTab::SelectSet)
    EVT_LIST_ITEM_DESELECTED(wtfFILES_SETS_LIST,    wtfMainTab::DeselectSet)
    EVT_LIST_END_LABEL_EDIT(wtfFILES_SETS_LIST,     wtfMainTab::SetFSTitle)
END_EVENT_TABLE()

wtfMainTab::wtfMainTab(wxWindow *parent)
    : wxPanel(parent, wtfMAIN_TAB)
{
    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);

    m_setsList = WXDEBUG_NEW FilesSetsCtrl(this);

    m_setsList->InsertColumn(0, wxT("Set name"));
    m_setsList->SetColumnWidth(0, 200);
    m_setsList->InsertColumn(1, wxT("Source path"));
    m_setsList->SetColumnWidth(1, 200);

    m_setsList->SetMinSize(wxSize(410, 300));

// Horizontally and vertically stretchable with a border width 10 and with minimal size of 410 x 300
    innersizer->Add(m_setsList, wxSizerFlags(1).Expand());

    wxBoxSizer *button_sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);

    button_sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfADD_NEW_SET_BUTTON, wxT("Add new set..."),       wxDefaultPosition,   wxSize(-1, -1), wxBU_LEFT | wxNO_BORDER),
        wxSizerFlags(0).Border(wxTOP, 20).Expand());

    m_btnEditSet = WXDEBUG_NEW wxButton(this, wtfEDIT_SET_BUTTON, wxT("Edit set..."),          wxDefaultPosition,   wxSize(-1, -1), wxBU_LEFT | wxNO_BORDER);
    m_btnEditSet->Enable(0);
    button_sizer->Add(
        m_btnEditSet,
        wxSizerFlags(0).Border(wxTOP, 2).Expand());

    m_btnEditNames = WXDEBUG_NEW wxButton(this, wtfMANAGE_NS_BUTTON, wxT("Manage names sets..."), wxDefaultPosition,  wxSize(-1, -1), wxBU_LEFT | wxNO_BORDER);
    m_btnEditNames->Enable(0);
    button_sizer->Add(
        m_btnEditNames,
        wxSizerFlags(0).Border(wxTOP, 2).Expand());

    m_btnDelSet = WXDEBUG_NEW wxButton(this, wtfDELETE_SET_BUTTON, wxT("Delete set..."),        wxDefaultPosition,  wxSize(-1, -1), wxBU_LEFT | wxNO_BORDER);
    m_btnDelSet->Enable(0);
    button_sizer->Add(
        m_btnDelSet,
        wxSizerFlags(0).Border(wxTOP, 2).Expand());

    m_btnRename = WXDEBUG_NEW wxButton(this, wtfRENAME_FILES_BUTTON, wxT("Rename files..."),        wxDefaultPosition,  wxSize(-1, -1), wxBU_LEFT | wxNO_BORDER);
    m_btnRename->Enable(0);
    button_sizer->Add(
        m_btnRename,
        wxSizerFlags(0).Border(wxTOP, 2).Expand());

    innersizer->Add(button_sizer, wxSizerFlags(0).Top().Border(wxLEFT, 60));

    topsizer->Add(innersizer, wxSizerFlags(1).Expand().Border(wxALL, 10));

    SetSizer(topsizer);

    topsizer->SetSizeHints(this);

    fsets = WXDEBUG_NEW FilesSets();

    m_filename = wxT("");
    m_modified = false;
}

wtfMainTab::~wtfMainTab()
{
    delete(fsets);
}

bool wtfMainTab::LoadFilesSets(const wxString& filename)
{
    SaveFilesSets(false, false);
    m_setsList->DeleteAllItems();
    if ( !fsets->Load(filename) )
    {
        return false;
    }
    m_filename = filename;
    m_modified = false;

    int id;
    FilesSet *fset;
    int i = 0;

    fset = fsets->GetFirstFS(&id);
    while ( (id > 0) && (id < fsets->GetNextId()) )
    {
        m_setsList->InsertItem( i, fset->GetTitle() );
        m_setsList->SetItem( i, 1, fset->GetSourceDir() );
        m_setsList->SetItemData( i, (long) id );
        fset = fsets->GetNextFS(&id);
        i++;
    }

    return true;
}

bool wtfMainTab::SaveFilesSets(bool force, bool name)
{
    if (!m_modified && !force)
    {
        return false;
    }
    if (name)
    {
        m_filename = wxT("");
    }
    name = (m_filename.Len() == 0);
    bool should_save = true;
    wxFileDialog *dlg;
    wxFileName fname;
    wxMessageDialog *mdlg;

    do
    {
        if (name)
        {
            fname.Assign(m_filename);
            dlg = WXDEBUG_NEW wxFileDialog(this, wxT("Choose a file"), fname.GetPath(), m_filename, wxT("XML files (*.xml)|*.xml|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        
            if (dlg->ShowModal() == wxID_OK)
            {
                m_filename = dlg->GetPath();
            } else {
                should_save = false;
            }
            dlg->Destroy();
        }
        name = true;
        if (!should_save || m_filename.Len() == 0)
        {
            return true;
        }
        if (fsets->Save(m_filename))
        {
            m_modified = false;
            return true;
        }
        mdlg = WXDEBUG_NEW wxMessageDialog(this, wxT(""), wxT("Save request"), wxYES_NO | wxYES_DEFAULT);
        should_save = (mdlg->ShowModal() == wxID_YES);
        mdlg->Destroy();
        if (!should_save)
        {
            return true;
        }
    } while (should_save);

    return false;
}

void wtfMainTab::SelectSet(wxListEvent& event)
{
    m_selected = event.GetIndex();

    m_btnEditSet->Enable(1);
    m_btnEditNames->Enable(1);
    m_btnDelSet->Enable(1);
    m_btnRename->Enable(1);
}

void wtfMainTab::DeselectSet(wxListEvent& WXUNUSED(event))
{
    m_btnEditSet->Enable(0);
    m_btnEditNames->Enable(0);
    m_btnDelSet->Enable(0);
    m_btnRename->Enable(0);
}

void wtfMainTab::SetFSTitle(wxListEvent& event)
{
    long i = event.GetIndex();
    int id = (int) m_setsList->GetItemData(i);
    FilesSet *fset = fsets->GetSet(id);

    if ((fset == NULL) || ( event.IsEditCancelled() ))
    {
        return;
    }

    fset->SetTitle( event.GetLabel() );
    m_modified = true;

    return;
}

bool wtfMainTab::AddSet()
{
    FilesSet *fset = WXDEBUG_NEW FilesSet();
    NamesSet *nset = WXDEBUG_NEW NamesSet();

    fset->AddNamesSet(nset);
    fset->SetMainId( nset->GetId() );

    if ( !DoEditSet(this, fset, wxT("Creating new files set...")) )
    {
        delete(fset);
        return false;
    }
    fset->SetTitle( nset->GetTitle() );
    if (!DoAddSet(fset))
    {
        delete(fset);
        return false;
    }
    m_modified = true;
    return true;
}

bool wtfMainTab::DoAddSet(FilesSet *fset)
{
    if ( !fsets->AddSet(fset) )
    {
        return false;
    }
    m_modified = true;
    int i = m_setsList->GetItemCount();
    m_setsList->InsertItem( i, fset->GetTitle() );
    m_setsList->SetItem( i, 1, fset->GetSourceDir() );
    m_setsList->SetItemData( i, fset->GetId() );
    return true;
}

bool wtfMainTab::EditSet(wxCommandEvent& WXUNUSED(event))
{
    int id = m_setsList->GetItemData(m_selected);
    FilesSet *fset = fsets->GetSet(id);

    if ( !DoEditSet(this, fset) )
    {
        return false;
    }
    m_setsList->SetItem( m_selected, 0, fset->GetTitle() );
    m_setsList->SetItem( m_selected, 1, fset->GetSourceDir() );
    m_modified = true;
    return true;
}

bool wtfMainTab::DoEditSet(wxWindow *parent, FilesSet *fset, const wxString& title)
{
    if (fset == NULL)
    {
        return false;
    }
    EditFilesSet *dlg = WXDEBUG_NEW EditFilesSet(parent, title);

    dlg->SetFSTitle( fset->GetTitle() );
    dlg->SetFSDir( fset->GetSourceDir() );

    NamesSet *nset = fset->GetFilesSet();

    if (nset == NULL)
    {
        dlg->Destroy();
        return false;
    }

    int id;
    wxString name = nset->GetFirstName(&id);

    while ( (id > 0) && (id < nset->GetNextId()) )
    {
        dlg->AddFSFile(name);
        name = nset->GetNextName(&id);
    }

    int rv = dlg->ShowModal();

    if (rv != wxID_OK)
    {
        dlg->Destroy();
        return false;
    }
// setting files set title, main names set title and dir
    fset->SetTitle( dlg->GetFSTitle() );
    nset->SetTitle( dlg->GetFSTitle() );
    fset->SetSourceDir( dlg->GetFSDir() );

// calculating max NextId through all names sets of the files set
    int newid = 1;
    NamesSet *ns = fset->GetFirstNS(&id);
    while ( (id > 0) && (id < fset->GetNextNSId()) )
    {
        if (newid < ns->GetNextId())
        {
            newid = ns->GetNextId();
        }
        ns = fset->GetNextNS(&id);
    }
    int maxid = newid;

// fetching file names from dialog
    ns = WXDEBUG_NEW NamesSet();
    name = dlg->GetFirstFSFile(&id);
    while (id >= 0)
    {
        if ( !nset->NameExists(name) )
        {
            nset->AddName(newid, name);
            newid++;
        } else {
            ns->AddName(0, name);
        }
        name = dlg->GetNextFSFile(&id);
    }

// deleting names from main names set
    name = nset->GetFirstName(&id);
    while ( (id > 0) && (id < maxid) )
    {
        if ( !ns->NameExists(name) )
        {
            nset->DeleteName(id);
        }
        name = nset->GetNextName(&id);
    }
    name.Clear();
    delete(ns);
    dlg->Destroy();
    return true;
}

void wtfMainTab::ManageNamesSets()
{
    int id = m_setsList->GetItemData(m_selected);
    FilesSet *fset = fsets->GetSet(id);

    if (fset == NULL)
    {
        return;
    }

    EditNamesSets *dlg = WXDEBUG_NEW EditNamesSets(this, wxT("Managing names sets"), this, fset);

    dlg->ShowModal();
    dlg->Destroy();
}

bool wtfMainTab::DeleteSet()
{
    int id = m_setsList->GetItemData(m_selected);
    wxMessageDialog *dlg = WXDEBUG_NEW wxMessageDialog(this, wxT("Do you really want to delete selected files set?"), wxT("Deleting files set"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

    if (dlg->ShowModal() != wxID_YES)
    {
        dlg->Destroy();
        return false;
    }
    dlg->Destroy();

    m_setsList->DeleteItem(m_selected);
    m_selected = -1;
    m_modified = true;
    return fsets->DeleteSet(id);
}

bool wtfMainTab::RenameFiles()
{
    int id = m_setsList->GetItemData(m_selected);
    FilesSet *fset = fsets->GetSet(id);

    if (fset == NULL)
    {
        return false;
    }

    int mid = fset->GetMainId();
    if (mid == 0)
    {
        wxMessageBox(wxT("No files"), wxT("Rename"), wxOK);
        return false;
    }
    wxArrayString *names_sets = WXDEBUG_NEW wxArrayString();
    wxArrayInt nsets_ids;
    NamesSet *nset = fset->GetFirstNS(&id);
    while ((id > 0) && (id < fset->GetNextNSId()))
    {
        if (id != mid)
        {
            names_sets->Add(nset->GetTitle());
            nsets_ids.Add(id);
        }
        nset = fset->GetNextNS(&id);
    }
    if (names_sets->GetCount() == 0)
    {
        wxMessageBox(wxT("No names"), wxT("Rename"), wxOK);
        delete(names_sets);
        return false;
    }
    wxSingleChoiceDialog *sdlg;
    int rid;

    if (names_sets->GetCount() > 1)
    {
        sdlg = WXDEBUG_NEW wxSingleChoiceDialog(this, wxT("Please select names set:"), wxT("Rename to"), *names_sets, NULL, wxDEFAULT_DIALOG_STYLE | wxOK | wxCANCEL);
        if (sdlg->ShowModal() != wxID_OK)
        {
            sdlg->Destroy();
            delete(names_sets);
            return false;
        }
        rid = nsets_ids[ sdlg->GetSelection() ];
        sdlg->Destroy();
    } else {
        rid = nsets_ids[0];
    }
    names_sets->Clear();
    delete(names_sets);

    wtfRenamePreview *dlg = WXDEBUG_NEW wtfRenamePreview(this, wxT("Rename information"), wxT("Here is the list of files that will be renamed and their new names:"), fset, rid);
    dlg->ShowModal();

    return true;
}

int wtfMainTab::ConfirmExit()
{
    if (!m_modified)
    {
        return wxID_NO;
    }
    wxString msg;

    if (m_filename.Len() > 0)
    {
        msg = wxT("File ") + m_filename + wxT(" has been modified.\nDo you want save changes?");
    } else {
        msg = wxT("Do you want save information about new files sets?\nThis information may be used to restore old file names in future.");
    }
    wxMessageDialog *dlg = WXDEBUG_NEW wxMessageDialog(this, msg, wxT("Save changes"), wxYES_NO | wxCANCEL | wxYES_DEFAULT);
    int rv = dlg->ShowModal();
    dlg->Destroy();

    if (rv != wxID_YES && rv != wxID_NO)
    {
        rv = wxID_CANCEL;
    }
    return rv;
}

void wtfMainTab::SetModifiedFlag(bool flag)
{
    m_modified = flag;
}


IMPLEMENT_ABSTRACT_CLASS(FilesSetsCtrl, wxListCtrl)

BEGIN_EVENT_TABLE(FilesSetsCtrl, wxListCtrl)
    EVT_CONTEXT_MENU( FilesSetsCtrl::ShowContextMenu )
    EVT_LIST_ITEM_RIGHT_CLICK(wtfFILES_SETS_LIST,       FilesSetsCtrl::ShowSetContextMenu)
END_EVENT_TABLE()

FilesSetsCtrl::FilesSetsCtrl(wxWindow* parent)
    : wxListCtrl(parent, wtfFILES_SETS_LIST, wxDefaultPosition,
        wxSize(410, 300), wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_HRULES)
{
    menu = WXDEBUG_NEW wxMenu();

    menu->Append(wtfSET_CM_ADD_NEW,     wxT("Add &new set..."));
    menu->Append(wtfSET_CM_EDIT,        wxT("&Edit set..."));
    menu->Append(wtfSET_CM_DELETE,      wxT("&Delete set..."));
    menu->Append(wtfSET_CM_MANAGE,      wxT("&Manage names sets..."));
    menu->AppendSeparator();
    menu->Append(wtfSET_CM_RENAME,      wxT("&Rename files..."));
    menu->Append(wtfSET_CM_HARDLINK,    wxT("Create &hard links..."));
}

FilesSetsCtrl::~FilesSetsCtrl()
{
    delete(menu);
}

void FilesSetsCtrl::ShowContextMenu(wxContextMenuEvent& WXUNUSED(event))
{
    if (m_ctxmenu)
    {
        m_ctxmenu = false;
        return;
    }
    menu->Enable(wtfSET_CM_EDIT,        0);
    menu->Enable(wtfSET_CM_DELETE,      0);
    menu->Enable(wtfSET_CM_MANAGE,      0);
    menu->Enable(wtfSET_CM_RENAME,      0);
    menu->Enable(wtfSET_CM_HARDLINK,    0);

    PopupMenu(menu);
}

void FilesSetsCtrl::ShowSetContextMenu(wxListEvent& WXUNUSED(event))
{
    menu->Enable(wtfSET_CM_EDIT,        1);
    menu->Enable(wtfSET_CM_DELETE,      1);
    menu->Enable(wtfSET_CM_MANAGE,      1);
    menu->Enable(wtfSET_CM_RENAME,      1);
    menu->Enable(wtfSET_CM_HARDLINK,    1);

    PopupMenu(menu);
    m_ctxmenu = true;
}


IMPLEMENT_ABSTRACT_CLASS(wtfTorrentTab, wxPanel)

wtfTorrentTab::wtfTorrentTab(wxWindow *parent, wtfMainTab *mtab)
    : wxPanel(parent, wxID_ANY)
{
    m_mainTab = mtab;
    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inpsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *inp1sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inp2sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inp3sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);

    wxStaticText *text = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Torrent files to process:"));
    inp1sizer->Add(
        text,
        wxSizerFlags(0).Border(wxLEFT, 3).Expand()
    );

    m_torrents = WXDEBUG_NEW wxTextCtrl(this, wtfTT_TORRENTS_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(300, 22), wxTE_NOHIDESEL);
    inp1sizer->Add(
        m_torrents,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    wxButton *btn = WXDEBUG_NEW wxButton(this, wtfTT_SELECT_TORRENTS_BUTTON, wxT("Select..."));
    int top = m_torrents->GetSize().GetHeight() + 2 + text->GetSize().GetHeight() - btn->GetSize().GetHeight();
    inp2sizer->Add(
        btn,
        wxSizerFlags(0).Border(wxTOP, top)
    );
    inp3sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfTT_ADD_TORRENTS_BUTTON, wxT("Add...")),
        wxSizerFlags(0).Border(wxTOP, top)
    );

    text = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Source directory:"));
    inp1sizer->Add(
        text,
        wxSizerFlags(0).Border(wxLEFT, 3).Expand()
    );

    m_directory = WXDEBUG_NEW wxTextCtrl(this, wtfTT_DIRECTORY_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(300, 22), wxTE_NOHIDESEL);
    inp1sizer->Add(
        m_directory,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    btn = WXDEBUG_NEW wxButton(this, wtfTT_CHANGE_DIR_BUTTON, wxT("Browse..."));
    top = m_directory->GetSize().GetHeight() + 2 + text->GetSize().GetHeight() - btn->GetSize().GetHeight();
    inp2sizer->Add(
        btn,
        wxSizerFlags(0).Border(wxTOP, top)
    );

    m_subdirs = WXDEBUG_NEW wxCheckBox(this, wtfTT_SUBDIR_CHECKBOX, wxT("Include subdirectories"));
    m_subdirs->SetValue(1);
    inp1sizer->Add(
        m_subdirs,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    inpsizer->Add(inp1sizer, wxSizerFlags(1));
    inpsizer->Add(inp2sizer, wxSizerFlags(0).Border(wxLEFT, 20));
    inpsizer->Add(inp3sizer, wxSizerFlags(0).Border(wxLEFT, 20));

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wtfTT_PROCESS_SINGLE_BUTTON, wxT("Process single")),
        wxSizerFlags(0).Border(wxRIGHT, 40)
    );
    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wtfTT_PROCESS_MULTI_BUTTON, wxT("Process multiple")),
        wxSizerFlags(0)
    );

    topsizer->Add(inpsizer, wxSizerFlags(1).Border(wxLEFT | wxRIGHT | wxTOP, 20).Expand());
    topsizer->Add(btnsizer, wxSizerFlags(0).Border(wxALL, 20).Left());

    SetSizer(topsizer);

    topsizer->SetSizeHints(this);

    genc = ((wtfApp *) wxTheApp)->genc;
}

void wtfTorrentTab::AddTorrents(const bool clear)
{
    wxFileDialog *dlg = WXDEBUG_NEW wxFileDialog(this, (clear) ? wxT("Select torrents...") : wxT("Select torrents to add..."), wxT(""), wxT(""), wxT("BitComet files (*.torrent)|*.torrent"), wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

    if (dlg->ShowModal() != wxID_OK)
    {
        dlg->Destroy();
        return;
    }

    wxArrayString t_files;
    wxString old_val;
    dlg->GetPaths(t_files);

    if (!clear)
    {
        old_val += m_torrents->GetValue();
    }
    unsigned int i;

    for (i = 0; i < t_files.Count(); i++)
    {
        if ( old_val.Len() )
        {
            old_val += wxT(", ");
        }
        old_val += wxT("\"");
        old_val += t_files[i];
        old_val += wxT("\"");
    }
    m_torrents->SetValue(old_val);
    t_files.Clear();
    dlg->Destroy();
}

bool wtfTorrentTab::SetSourceDir(const wxString& dir)
{
    m_directory->SetValue(dir);
    return true;
}

bool wtfTorrentTab::ProcessSingle()
{
    wxString dir = m_directory->GetValue();

    if (!wxDir::Exists(dir))
    {
        wxLogError(wxT("Process single: Directory '") + dir + wxT("' does not exists"));
        return false;
    }

#ifdef wtfDEBUG_MEMORY_LEAKS
    wxDebugContext::SetCheckpoint();
    wxLog::SetLogLevel(wxLOG_Max);
#endif // wtfDEBUG_MEMORY_LEAKS
    wxString tfiles = m_torrents->GetValue();
    wxArrayString torrents, l_files;
    wxString filename;
    size_t i, j;

#ifdef wtfDEBUG_PROCESS_SINGLE
    wxLogMessage(wxT("Process single: Fetching torrent files to process"));
#endif // wtfDEBUG_PROCESS_SINGLE
// fetching torrent files to process
    wtfMakeFilesList(tfiles, &torrents);
    tfiles.Clear();

#ifdef wtfDEBUG_PROCESS_SINGLE
    j = torrents.GetCount();
    for (i = 0; i < j; i++)
    {
        wxLogMessage(wxT("Process single:   ") + torrents[i] + wxT(","));
    }
#endif // wtfDEBUG_PROCESS_SINGLE

    wxFileName fname;
#ifdef wtfDEBUG_PROCESS_SINGLE
    wxLogMessage(wxT("Process single: Fetching local files"));
#endif // wtfDEBUG_PROCESS_SINGLE
// fetching local files
    int flags = wxDIR_FILES | wxDIR_HIDDEN;

    if (m_subdirs->GetValue())
    {
        flags = flags | wxDIR_DIRS;
    }

    wxDir::GetAllFiles(dir, &l_files, wxT(""), flags);

    wxString ext, size;
    wtfFilesList hl_files, ht_files;

    j = l_files.GetCount();
    for (i = 0; i < j; i++)
    {
        size = wxFileName::GetSize(l_files[i]).ToString();
        wxFileName::SplitPath(l_files[i], NULL, NULL, NULL, &ext);
        fname.Assign(l_files[i]);
        if (fname.MakeRelativeTo(dir))
        {
            l_files[i] = fname.GetFullPath();
        } else {
            wxLogError(wxT("Cannot make file name relative to directory specifyed"));
        }
        if (!hl_files.count(ext))
        {
            hl_files[ext] = WXDEBUG_NEW wtfFilesListBySize();
        }
        if (!hl_files[ext]->count(size))
        {
            (*(hl_files[ext]))[size] = WXDEBUG_NEW wtfArraySizeT();
        }
        (*(hl_files[ext]))[size]->Add(i);
    }

    wtfFilesList::iterator eit;
    wtfFilesListBySize::iterator sit;

#ifdef wtfDEBUG_PROCESS_SINGLE
    for (eit = hl_files.begin(); eit != hl_files.end(); ++eit)
    {
        for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
        {
            j = sit->second->GetCount();
            for (i = 0; i < j; i++)
            {
                wxLogMessage(wxT("Process single: ") + eit->first + wxT("   ") + sit->first + wxT("   ") + l_files[sit->second->Item(i)]);
            }
        }
    }
#endif // wtfDEBUG_PROCESS_SINGLE

    wtfTorrentFile tf;
    size_t t, cnt, maxt;
    wtfNamesToFiles curr, max;
    wtfNamesToFiles::iterator nfit;
    int error;
    wxString msg;
    double fsize, tsize, dsize, maxrat = 0;
    CorrM corr;
    CorrM::iterator cit;
    wxArrayString d_files, d_names;

    tf.SetGenc(genc);
    cnt = torrents.GetCount();
    for (t = 0; t < cnt; t++)
    {
        tf.Load(torrents[t]);
        error = tf.Error();
#ifdef wtfDEBUG_PROCESS_SINGLE
        wxLogMessage(wxT("Process single: Opening torrent file '") + torrents[t] + wxT("'..."));
#endif // wtfDEBUG_PROCESS_SINGLE
        if (error == 0)
        {
            tsize = 0;
            j = tf.GetFilesCount();
            for (i = 0; i < j; i++)
            {
                filename = tf.GetFileName(i);
                size = tf.GetFileSize(i);
                if (size.ToDouble(&dsize))
                {
                    tsize += dsize;
                } else {
                    wxLogError(wxT("Process single: Torrent file '") + torrents[i] + wxT("' files enumeration: Cannot transform '") + size + wxT("' to double"));
                }
                wxFileName::SplitPath(filename, NULL, NULL, NULL, &ext);
                if (!ht_files.count(ext))
                {
                    ht_files[ext] = WXDEBUG_NEW wtfFilesListBySize();
                }
                if (!ht_files[ext]->count(size))
                {
                    (*(ht_files[ext]))[size] = WXDEBUG_NEW wtfArraySizeT();
                }
                (*(ht_files[ext]))[size]->Add(i);
            }
            fsize = 0;
            for (eit = ht_files.begin(); eit != ht_files.end(); ++eit)
            {
                if (!hl_files.count(eit->first))
                {
#ifdef wtfDEBUG_PROCESS_SINGLE
                    wxLogMessage(wxT("Process single: There is no file with extension '") + eit->first + wxT("' under directory specifyed"));
#endif // wtfDEBUG_PROCESS_SINGLE
                    continue;
                }
                for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
                {
                    if (!hl_files[eit->first]->count(sit->first))
                    {
#ifdef wtfDEBUG_PROCESS_SINGLE
                        wxLogMessage(wxT("Process single: There is no file with extension '") + eit->first + wxT("' and size ") + sit->first + wxT(" under directory specifyed"));
#endif // wtfDEBUG_PROCESS_SINGLE
                        continue;
                    }
                    if ((sit->second->GetCount() == 1) && ((*(hl_files[eit->first]))[sit->first]->GetCount() == 1))
                    {
                        j = (*(hl_files[eit->first]))[sit->first]->Item(0);
#ifdef wtfDEBUG_PROCESS_SINGLE
                        wxLogMessage(wxT("Process single:   ") + tf.GetFileName(sit->second->Item(0)) + wxT("(%d) => ") + l_files[j] + wxT("(%d)"), sit->second->Item(0), j);
#endif // wtfDEBUG_PROCESS_SINGLE
                        curr[tf.GetFileFullName(sit->second->Item(0))] = j;
                        if (sit->first.ToDouble(&dsize))
                        {
                            fsize += dsize;
                        } else {
                            wxLogError(wxT("Process single: Torrent file '") + torrents[i] + wxT("' checking: Cannot transform '") + size + wxT("' to double"));
                        }
                    } else {
#ifdef wtfDEBUG_PROCESS_SINGLE
                        wxLogMessage(wxT("Process single: Disambiguation required"));
                        wxLogMessage(wxT("Process single: Torrent files:"));
#endif // wtfDEBUG_PROCESS_SINGLE
                        j = sit->second->GetCount();
                        for (i = 0; i < j; i++)
                        {
#ifdef wtfDEBUG_PROCESS_SINGLE
                            wxLogMessage(wxT("Process single:   ") + tf.GetFileName(sit->second->Item(i)));
#endif // wtfDEBUG_PROCESS_SINGLE
                            d_names.Add(tf.GetFileFullName(sit->second->Item(i)));
                        }
#ifdef wtfDEBUG_PROCESS_SINGLE
                        wxLogMessage(wxT("Process single: Local files:"));
#endif // wtfDEBUG_PROCESS_SINGLE
                        j = (*(hl_files[eit->first]))[sit->first]->GetCount();
                        for (i = 0; i < j; i++)
                        {
#ifdef wtfDEBUG_PROCESS_SINGLE
                            wxLogMessage(wxT("Process single:   ") + l_files[(*(hl_files[eit->first]))[sit->first]->Item(i)]);
#endif // wtfDEBUG_PROCESS_SINGLE
                            d_files.Add(l_files[(*(hl_files[eit->first]))[sit->first]->Item(i)]);
                        }
                        if (Disambiguate(d_files, d_names, &corr, wxT("")))
                        {
#ifdef wtfDEBUG_PROCESS_SINGLE
                            wxLogMessage(wxT("Process single: Successfully disambiguated"));
#endif // wtfDEBUG_PROCESS_SINGLE
                            if (!sit->first.ToDouble(&dsize))
                            {
                                dsize = 0;
#ifdef wtfDEBUG_PROCESS_SINGLE
                                wxLogError(wxT("Torrent file '") + torrents[i] + wxT("' checking: Cannot transform '") + size + wxT("' to double"));
#endif // wtfDEBUG_PROCESS_SINGLE
                            }
                            for (cit = corr.begin(); cit != corr.end(); ++cit)
                            {
                                msg = d_names[cit->second];
                                curr[msg] = (*(hl_files[eit->first]))[sit->first]->Item(cit->first);
                                fsize += dsize;
#ifdef wtfDEBUG_PROCESS_SINGLE
                                wxLogMessage(wxT("Process single:   ") + msg + wxT("(%d) => ") + d_files[cit->first] + wxT("(%d)"), cit->second, curr[msg]);
#endif // wtfDEBUG_PROCESS_SINGLE
                            }
                        } else {
#ifdef wtfDEBUG_PROCESS_SINGLE
                            wxLogMessage(wxT("Process single: Cannot disambiguate"));
#endif // wtfDEBUG_PROCESS_SINGLE
                        }
                        d_files.Clear();
                        d_names.Clear();
                    }
                }
            }
#ifdef wtfDEBUG_PROCESS_SINGLE
            wxLogMessage(wxT("Process single: Total size of files: %.0f (of %.0f)"), fsize, tsize);
            wxLogMessage(wxT("Process single: Map:"));
            for (nfit = curr.begin(); nfit != curr.end(); ++nfit)
            {
                wxLogMessage(wxT("Process single:   ") + nfit->first + wxT(" => %d"), nfit->second);
            }
#endif // wtfDEBUG_PROCESS_SINGLE
            if (fsize > maxrat)
            {
                max.clear();
                for (nfit = curr.begin(); nfit != curr.end(); ++nfit)
                {
                    max[nfit->first] = nfit->second;
                }
                maxrat = fsize;
                maxt = t;
            }
            for (eit = ht_files.begin(); eit != ht_files.end(); ++eit)
            {
                for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
                {
                    delete(sit->second);
                    (*(eit->second))[sit->first] = NULL;
                }
                eit->second->clear();
                delete(eit->second);
                ht_files[eit->first] = NULL;
            }
            ht_files.clear();
            curr.clear();
        }
    }
    for (eit = hl_files.begin(); eit != hl_files.end(); ++eit)
    {
        for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
        {
            delete(sit->second);
            (*(eit->second))[sit->first] = NULL;
        }
        eit->second->clear();
        delete(eit->second);
        hl_files[eit->first] = NULL;
    }
    hl_files.clear();

    if (max.empty())
    {
        torrents.Clear();
        l_files.Clear();
#ifdef wtfDEBUG_PROCESS_SINGLE
        wxLogMessage(wxT("Process single: There is no appropriate torrent for your files under directory specifyed"));
#endif // wtfDEBUG_PROCESS_SINGLE
#ifdef wtfDEBUG_MEMORY_LEAKS
        wxDebugContext::Dump();
        wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
        return false;
    }
#ifdef wtfDEBUG_PROCESS_SINGLE
    wxLogMessage(wxT("Process single: The most appropriate torrent file is '") + torrents[maxt] + wxT("'"));
    wxLogMessage(wxT("Process single: The ratio is %.5f"), maxrat);
#endif // wtfDEBUG_PROCESS_SINGLE

    FilesSet *fset;
    NamesSet *nset;
    corr.clear();
    if (m_mainTab != NULL)
    {
        fset = WXDEBUG_NEW FilesSet();
        nset = WXDEBUG_NEW NamesSet();
        fset->AddNamesSet(nset);
        fset->SetMainId(nset->GetId());
        fset->SetSourceDir(dir);
        for (nfit = max.begin(); nfit != max.end(); ++nfit)
        {
            corr[nfit->second] = 1;
        }
        j = l_files.GetCount();
        for (i = 0; i < j; i++)
        {
            if (corr.count(i))
            {
                fset->AddFile(i + 1, l_files[i]);
            }
        }
        nset = WXDEBUG_NEW NamesSet();
        for (nfit = max.begin(); nfit != max.end(); ++nfit)
        {
            nset->AddName(nfit->second + 1, nfit->first);
        }
        fset->AddNamesSet(nset);
        corr.clear();
        if (!wtfMainTab::DoEditSet(this, fset, wxT("New files set for torrent file '") + torrents[maxt] + wxT("'")))
        {
            delete(fset);
            max.clear();
            torrents.Clear();
            l_files.Clear();
#ifdef wtfDEBUG_MEMORY_LEAKS
            wxDebugContext::Dump();
            wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
            return false;
        }
        if (!EditNamesSets::DoEditNS(this, fset, nset, wxT("Names set for torrent file '") + torrents[maxt] + wxT("'")))
        {
            delete(fset);
            max.clear();
            torrents.Clear();
            l_files.Clear();
#ifdef wtfDEBUG_MEMORY_LEAKS
            wxDebugContext::Dump();
            wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
            return false;
        }
        m_mainTab->DoAddSet(fset);
        m_mainTab->SetModifiedFlag(true);
    }    

    max.clear();
    torrents.Clear();
    l_files.Clear();
#ifdef wtfDEBUG_MEMORY_LEAKS
    wxDebugContext::Dump();
    wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
    return true;
}

wxString wtfTorrentTab::DumpData(wbenData &data, const size_t sp)
{
    wxString res;
    wbenDataDictionary::iterator it;
    wbenDataDictionary *dict;
    wbenDataList *arr;
    size_t i, j;

    switch (data.GetType())
    {
        case wbenDATA_UNDEFINED:
            res = wxT("undef");
            break;
        case wbenDATA_INTEGER:
            res.Printf(wxT("%d"), *(data.GetValue().m_int));
            break;
        case wbenDATA_STRING:
            res = wxT("'") + *(data.GetValue().m_str) + wxT("'");
            break;
        case wbenDATA_LIST:
            res = wxT("[\n");
            arr = data.GetValue().m_list;
            for (i = 0; i < arr->Count(); i++)
            {
                for (j = 0; j < sp + 4; j++)
                {
                    res += wxT(" ");
                }
                res += DumpData(arr->Item(i), sp + 4);
                res += wxT(",\n");
            }
            for (j = 0; j < sp; j++)
            {
                res += wxT(" ");
            }
            res += wxT("]");
            break;
        case wbenDATA_DICTIONARY:
            res = wxT("{\n");
            dict = data.GetValue().m_dict;
            for (it = dict->begin(); it != dict->end(); ++it)
            {
                for (j = 0; j < sp + 4; j++)
                {
                    res += wxT(" ");
                }
                res += wxT("'") + it->first + wxT("' => ");
                res += DumpData(*(it->second), sp + 4 + it->first.Len() + 6);
                res += wxT(",\n");
            }
            for (j = 0; j < sp; j++)
            {
                res += wxT(" ");
            }
            res += wxT("}");
            break;
        default:
            res = wxT("Unknown data type");
            break;
    }
    return res;
}

bool wtfTorrentTab::ProcessMulti()
{
    if (m_mainTab == NULL)
    {
        wxLogError(wxT("m_mainTab is NULL"));
        return false;
    }
    wxString dir = m_directory->GetValue();

    if (!wxDir::Exists(dir))
    {
        wxLogError(wxT("Process multiple: Directory '") + dir + wxT("' does not exists"));
        return false;
    }

#ifdef wtfDEBUG_MEMORY_LEAKS
    wxDebugContext::SetCheckpoint();
    wxLog::SetLogLevel(wxLOG_Max);
#endif // wtfDEBUG_MEMORY_LEAKS
    wxString tfiles = m_torrents->GetValue();
    wxArrayString torrents, l_files;
    wxString filename;
    size_t i, j;

#ifdef wtfDEBUG_PROCESS_MULTI
    wxLogMessage(wxT("Process multiple: Fetching torrent files to process"));
#endif // wtfDEBUG_PROCESS_MULTI
// fetching torrent files to process
    wtfMakeFilesList(tfiles, &torrents);
    tfiles.Clear();

#ifdef wtfDEBUG_PROCESS_MULTI
    j = torrents.GetCount();
    for (i = 0; i < j; i++)
    {
        wxLogMessage(wxT("Process multiple:   ") + torrents[i] + wxT(","));
    }
    wxLogMessage(wxT("Process multiple: Fetching local files"));
#endif // wtfDEBUG_PROCESS_MULTI

    wxFileName fname;

// fetching local files
    int flags = wxDIR_FILES | wxDIR_HIDDEN;

    if (m_subdirs->GetValue())
    {
        flags = flags | wxDIR_DIRS;
    }

    wxDir::GetAllFiles(dir, &l_files, wxT(""), flags);

    wxString ext, size;
    wtfFilesList hl_files, ht_files;

    size_t fcnt = l_files.GetCount();

    if (fcnt == 0)
    {
        wxMessageBox(wxT("There no files under directory specified"), wxT("Process multiple"), wxOK);
        return false;
    }
    for (i = 0; i < fcnt; i++)
    {
        size = wxFileName::GetSize(l_files[i]).ToString();
        wxFileName::SplitPath(l_files[i], NULL, NULL, NULL, &ext);
        fname.Assign(l_files[i]);
        if (fname.MakeRelativeTo(dir))
        {
            l_files[i] = fname.GetFullPath();
        } else {
            wxLogError(wxT("Process multiple: Cannot make file name relative to directory specifyed"));
        }
        if (!hl_files.count(ext))
        {
            hl_files[ext] = WXDEBUG_NEW wtfFilesListBySize();
        }
        if (!hl_files[ext]->count(size))
        {
            (*(hl_files[ext]))[size] = WXDEBUG_NEW wtfArraySizeT();
        }
        (*(hl_files[ext]))[size]->Add(i);
    }

    wtfFilesList::iterator eit;
    wtfFilesListBySize::iterator sit;

#ifdef wtfDEBUG_PROCESS_MULTI
    for (eit = hl_files.begin(); eit != hl_files.end(); ++eit)
    {
        for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
        {
            j = sit->second->GetCount();
            for (i = 0; i < j; i++)
            {
                wxLogMessage(wxT("Process multiple: ") + eit->first + wxT("   ") + sit->first + wxT("   ") + l_files[sit->second->Item(i)]);
            }
        }
    }
#endif // wtfDEBUG_PROCESS_MULTI

    wtfTorrentFile tf;
    size_t t, cnt, ncnt;
    wtfNamesToFiles curr;
    wtfNamesToFiles::iterator nfit;
    int error;
    CorrM corr, f_remained;
    CorrM::iterator cit;
    wxArrayString d_files, d_names;
    FilesSet *fset;
    NamesSet *nset;
    bool cancel = false;

    tf.SetGenc(genc);
    cnt = torrents.GetCount();
    for (t = 0; (t < cnt) && (fcnt > 0); t++)
    {
        tf.Load(torrents[t]);
        error = tf.Error();
#ifdef wtfDEBUG_PROCESS_MULTI
        wxLogMessage(wxT("Process multiple: Opening torrent file '") + torrents[t] + wxT("'..."));
#endif // wtfDEBUG_PROCESS_MULTI
        if (error == 0)
        {
            j = tf.GetFilesCount();
            for (i = 0; i < j; i++)
            {
                filename = tf.GetFileName(i);
                size = tf.GetFileSize(i);
                wxFileName::SplitPath(filename, NULL, NULL, NULL, &ext);
                if (!ht_files.count(ext))
                {
                    ht_files[ext] = WXDEBUG_NEW wtfFilesListBySize();
                }
                if (!ht_files[ext]->count(size))
                {
                    (*(ht_files[ext]))[size] = WXDEBUG_NEW wtfArraySizeT();
                }
                (*(ht_files[ext]))[size]->Add(i);
            }
            ncnt = 0;
            for (eit = ht_files.begin(); eit != ht_files.end(); ++eit)
            {
                if (!hl_files.count(eit->first))
                {
#ifdef wtfDEBUG_PROCESS_MULTI
                    wxLogMessage(wxT("Process multiple: There is no file with extension '") + eit->first + wxT("' under directory specifyed"));
#endif // wtfDEBUG_PROCESS_MULTI
                    continue;
                }
                for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
                {
                    if (!hl_files[eit->first]->count(sit->first))
                    {
#ifdef wtfDEBUG_PROCESS_MULTI
                        wxLogMessage(wxT("Process multiple: There is no file with extension '") + eit->first + wxT("' and size ") + sit->first + wxT(" under directory specifyed"));
#endif // wtfDEBUG_PROCESS_MULTI
                        continue;
                    }
                    if ((sit->second->GetCount() == 1) && ((*(hl_files[eit->first]))[sit->first]->GetCount() == 1))
                    {
                        j = (*(hl_files[eit->first]))[sit->first]->Item(0);
#ifdef wtfDEBUG_PROCESS_MULTI
                        wxLogMessage(wxT("Process multiple:   ") + tf.GetFileName(sit->second->Item(0)) + wxT("(%d) => ") + l_files[j] + wxT("(%d)"), sit->second->Item(0), j);
#endif // wtfDEBUG_PROCESS_MULTI
                        curr[tf.GetFileFullName(sit->second->Item(0))] = j;
                        delete((*(hl_files[eit->first]))[sit->first]);
                        (*(hl_files[eit->first]))[sit->first] = NULL;
                        hl_files[eit->first]->erase(sit->first);
                        ncnt++;
                    } else {
#ifdef wtfDEBUG_PROCESS_MULTI
                        wxLogMessage(wxT("Process multiple: Disambiguation required"));
                        wxLogMessage(wxT("Process multiple: Torrent files:"));
#endif // wtfDEBUG_PROCESS_MULTI
                        j = sit->second->GetCount();
                        for (i = 0; i < j; i++)
                        {
#ifdef wtfDEBUG_PROCESS_MULTI
                            wxLogMessage(wxT("Process multiple:   ") + tf.GetFileName(sit->second->Item(i)));
#endif // wtfDEBUG_PROCESS_MULTI
                            d_names.Add(tf.GetFileFullName(sit->second->Item(i)));
                        }
#ifdef wtfDEBUG_PROCESS_MULTI
                        wxLogMessage(wxT("Process multiple: Local files:"));
#endif // wtfDEBUG_PROCESS_MULTI
                        j = (*(hl_files[eit->first]))[sit->first]->GetCount();
                        for (i = 0; i < j; i++)
                        {
#ifdef wtfDEBUG_PROCESS_MULTI
                            wxLogMessage(wxT("Process multiple:   ") + l_files[(*(hl_files[eit->first]))[sit->first]->Item(i)]);
#endif // wtfDEBUG_PROCESS_MULTI
                            d_files.Add(l_files[(*(hl_files[eit->first]))[sit->first]->Item(i)]);
                            f_remained[i] = (*(hl_files[eit->first]))[sit->first]->Item(i);
                        }
                        if (Disambiguate(d_files, d_names, &corr, wxT("")))
                        {
#ifdef wtfDEBUG_PROCESS_MULTI
                            wxLogMessage(wxT("Process multiple: Successfully disambiguated"));
#endif // wtfDEBUG_PROCESS_MULTI
                            for (cit = corr.begin(); cit != corr.end(); ++cit)
                            {
                                filename = d_names[cit->second];
                                curr[filename] = (*(hl_files[eit->first]))[sit->first]->Item(cit->first);
                                f_remained.erase(cit->first);
                                ncnt++;
#ifdef wtfDEBUG_PROCESS_MULTI
                                wxLogMessage(wxT("Process multiple:   ") + filename + wxT("(%d) => ") + d_files[cit->first] + wxT("(%d)"), cit->second, curr[filename]);
#endif // wtfDEBUG_PROCESS_MULTI
                            }
                            (*(hl_files[eit->first]))[sit->first]->Empty();
                            for (cit = f_remained.begin(); cit != f_remained.end(); ++cit)
                            {
                                (*(hl_files[eit->first]))[sit->first]->Add(cit->second);
                            }
                        } else {
#ifdef wtfDEBUG_PROCESS_MULTI
                            wxLogMessage(wxT("Process multiple: Cannot disambiguate"));
#endif // wtfDEBUG_PROCESS_MULTI
                        }
                        f_remained.clear();
                        d_files.Clear();
                        d_names.Clear();
                    }
                }
            }
#ifdef wtfDEBUG_PROCESS_MULTI
            wxLogMessage(wxT("Process multiple: Total number of files: %d (of %d)"), ncnt, tf.GetFilesCount());
            wxLogMessage(wxT("Process multiple: Map:"));
            for (nfit = curr.begin(); nfit != curr.end(); ++nfit)
            {
                wxLogMessage(wxT("Process multiple:   ") + nfit->first + wxT(" => ") + l_files[nfit->second]);
            }
#endif // wtfDEBUG_PROCESS_MULTI
            for (eit = ht_files.begin(); eit != ht_files.end(); ++eit)
            {
                for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
                {
                    delete(sit->second);
                    (*(eit->second))[sit->first] = NULL;
                }
                eit->second->clear();
                delete(eit->second);
                ht_files[eit->first] = NULL;
            }
            ht_files.clear();
            if (ncnt > 0)
            {
                fcnt -= ncnt;
                corr.clear();
                if (cancel)
                {
                    fset->GetFilesSet()->DeleteAllNames();
                    fset->GetFilesSet()->SetTitle(wxT(""));
                    fset->SetTitle(wxT(""));
                    nset->DeleteAllNames();
                    nset->SetTitle(wxT(""));
                    nset->SetDir(wxT(""));
                    cancel = false;
                } else {
                    fset = WXDEBUG_NEW FilesSet();
                    nset = WXDEBUG_NEW NamesSet();
                    fset->AddNamesSet(nset);
                    fset->SetMainId(nset->GetId());
                    fset->SetSourceDir(dir);
                    nset = WXDEBUG_NEW NamesSet();
                    fset->AddNamesSet(nset);
                }
                for (nfit = curr.begin(); nfit != curr.end(); ++nfit)
                {
                    corr[nfit->second] = 1;
                }
                j = l_files.GetCount();
                ncnt = 1;
                for (i = 0; i < j; i++)
                {
                    if (corr.count(i))
                    {
                        fset->AddFile(ncnt, l_files[i]);
                        corr[i] = ncnt;
                        ncnt++;
                    }
                }
                for (nfit = curr.begin(); nfit != curr.end(); ++nfit)
                {
                    nset->AddName(corr[nfit->second], nfit->first);
                }
                corr.clear();
                if (!wtfMainTab::DoEditSet(this, fset, wxT("New files set for torrent file '") + torrents[t] + wxT("'")))
                {
                    wxMessageDialog *dlg = WXDEBUG_NEW wxMessageDialog(this, wxT("Do you want to stop the process?"), wxT("Process multiple"), wxYES_NO);
                    if (dlg->ShowModal() == wxID_YES)
                    {
                        dlg->Destroy();
                        delete(dlg);
                        delete(fset);
                        for (eit = hl_files.begin(); eit != hl_files.end(); ++eit)
                        {
                            for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
                            {
                                delete(sit->second);
                                (*(eit->second))[sit->first] = NULL;
                            }
                            eit->second->clear();
                            delete(eit->second);
                            hl_files[eit->first] = NULL;
                        }
                        hl_files.clear();
                        curr.clear();
                        torrents.Clear();
                        l_files.Clear();
#ifdef wtfDEBUG_MEMORY_LEAKS
                        wxDebugContext::Dump();
                        wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
                        return false;
                    }
                    dlg->Destroy();
                    delete(dlg);
                    cancel = true;
                }
                if (!cancel && !EditNamesSets::DoEditNS(this, fset, nset, wxT("Names set for torrent file '") + torrents[t] + wxT("'")))
                {
                    wxMessageDialog *dlg = WXDEBUG_NEW wxMessageDialog(this, wxT("Do you want to stop the process?"), wxT("Process multiple"), wxYES_NO);
                    if (dlg->ShowModal() == wxID_YES)
                    {
                        dlg->Destroy();
                        delete(dlg);
                        delete(fset);
                        for (eit = hl_files.begin(); eit != hl_files.end(); ++eit)
                        {
                            for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
                            {
                                delete(sit->second);
                                (*(eit->second))[sit->first] = NULL;
                            }
                            eit->second->clear();
                            delete(eit->second);
                            hl_files[eit->first] = NULL;
                        }
                        hl_files.clear();
                        curr.clear();
                        torrents.Clear();
                        l_files.Clear();
#ifdef wtfDEBUG_MEMORY_LEAKS
                        wxDebugContext::Dump();
                        wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
                        return false;
                    }
                    dlg->Destroy();
                    delete(dlg);
                    cancel = true;
                }
                if (!cancel)
                {
                    m_mainTab->DoAddSet(fset);
                    m_mainTab->SetModifiedFlag(true);
                }
            }
            curr.clear();
        }
    }
    if (cancel)
    {
        delete(fset);
    }
    for (eit = hl_files.begin(); eit != hl_files.end(); ++eit)
    {
        for (sit = eit->second->begin(); sit != eit->second->end(); ++sit)
        {
            delete(sit->second);
            (*(eit->second))[sit->first] = NULL;
        }
        eit->second->clear();
        delete(eit->second);
        hl_files[eit->first] = NULL;
    }
    hl_files.clear();

#ifdef wtfDEBUG_MEMORY_LEAKS
    wxDebugContext::Dump();
    wxDebugContext::PrintStatistics();
#endif // wtfDEBUG_MEMORY_LEAKS
    return true;
}


//---------------------------------------------------------------------------
