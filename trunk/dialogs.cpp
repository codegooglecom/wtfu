#include "dialogs.h"
#include "bencode.h"
#include "util.h"


IMPLEMENT_ABSTRACT_CLASS(EditFilesSet, wxDialog)

BEGIN_EVENT_TABLE(EditFilesSet, wxDialog)
    EVT_BUTTON(wtfEFSD_CHANGE_DIR_BUTTON,       EditFilesSet::ChooseDir)
    EVT_BUTTON(wtfEFSD_ADD_FILES_BUTTON,        EditFilesSet::AddFiles)
    EVT_BUTTON(wtfEFSD_ADD_ALL_FILES_BUTTON,    EditFilesSet::AddFiles)
    EVT_BUTTON(wtfEFSD_DEL_FILES_BUTTON,        EditFilesSet::DelFiles)
    EVT_BUTTON(wtfEFSD_DEL_ALL_FILES_BUTTON,    EditFilesSet::DelFiles)
    EVT_BUTTON(wxID_OK,                         EditFilesSet::CheckData)
END_EVENT_TABLE()

EditFilesSet::EditFilesSet(wxWindow *parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inpsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *inp1sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inp2sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *filessizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *f1sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *f2sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *f3sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);

    wxStaticText *stext = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Title of new set:"));
    int b_top = stext->GetSize().GetHeight();
    inp1sizer->Add(
        stext,
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_title = WXDEBUG_NEW wxTextCtrl(this, wtfEFSD_TITLE_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120, 22), wxTE_NOHIDESEL);
    b_top += m_title->GetSize().GetHeight() + 2 + 2;
    inp1sizer->Add(
        m_title,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    stext = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Source directory:"));
    b_top += stext->GetSize().GetHeight();
    inp1sizer->Add(
        stext,
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_dir = WXDEBUG_NEW wxTextCtrl(this, wtfEFSD_DIR_TEXTCTRL, wxT(""), wxDefaultPosition, wxSize(120, 22), wxTE_NOHIDESEL | wxTE_READONLY);
    b_top += m_dir->GetSize().GetHeight() + 2 + 2;
    inp1sizer->Add(
        m_dir,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    wxButton *btn = WXDEBUG_NEW wxButton(this, wtfEFSD_CHANGE_DIR_BUTTON, wxT("Browse..."));
    b_top -= btn->GetSize().GetHeight();
    inp2sizer->Add(
        btn,
        wxSizerFlags(0).Border(wxTOP, b_top)
    );

    inpsizer->Add(
        inp1sizer,
        wxSizerFlags(1)
    );
    inpsizer->Add(
        inp2sizer,
        wxSizerFlags(0).Border(wxLEFT, 5)
    );

    innersizer->Add(
        inpsizer,
        wxSizerFlags(0).Expand()
    );

    stext = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Directory tree"));
    b_top = stext->GetSize().GetHeight();
    b_top++;
    b_top *= 2;
    f1sizer->Add(
        stext,
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_tree = WXDEBUG_NEW wtfDirTree(this, wxDefaultPosition, wxSize(200, 220), wxTR_MULTIPLE | wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);
    f1sizer->Add(
        m_tree,
        wxSizerFlags(1).Border(wxTOP, 2).Expand()
    );

    f2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfEFSD_ADD_ALL_FILES_BUTTON, wxT(">>"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxTOP, b_top).Expand()
    );
    f2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfEFSD_ADD_FILES_BUTTON, wxT(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxTOP, 2).Expand()
    );
    f2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfEFSD_DEL_ALL_FILES_BUTTON, wxT("<<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxTOP, 2).Expand()
    );
    f2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfEFSD_DEL_FILES_BUTTON, wxT("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxTOP, 2).Expand()
    );

    f3sizer->Add(
        WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Selected files:")),
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_files = WXDEBUG_NEW wtfChosenFiles(m_tree, this, wtfEFSD_FILES_LIST, wxDefaultPosition, wxSize(200, 220));
    m_dnd = WXDEBUG_NEW wtfFilesDropTarget(m_files);
    m_tree->SetDnD(m_dnd);
    m_files->SetDropTarget(m_dnd);
    f3sizer->Add(
        m_files,
        wxSizerFlags(1).Border(wxTOP, 2).Expand()
    );

    filessizer->Add(
        f1sizer,
        wxSizerFlags(1).Expand()
    );
    filessizer->Add(
        f2sizer,
        wxSizerFlags(0).Border(wxLEFT, 3).Expand()
    );
    filessizer->Add(
        f3sizer,
        wxSizerFlags(1).Border(wxLEFT, 3).Expand()
    );

    innersizer->Add(
        filessizer,
        wxSizerFlags(1).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_OK, wxT("&Ok")),
        wxSizerFlags(0).Border(wxLEFT, 20)
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_CANCEL, wxT("&Cancel")),
        wxSizerFlags(0).Border(wxLEFT, 20)
    );

    innersizer->Add(
        btnsizer,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2).Expand()
    );

    topsizer->Add(
        innersizer,
        wxSizerFlags(1).Border(wxALL, 10).Expand()
    );

    SetSizer(topsizer);

    topsizer->Fit(this);
    topsizer->SetSizeHints(this);

    Show(true);
}

EditFilesSet::EditFilesSet(wxWindow *parent, const wxString& title, const wxString& setTitle, const wxString& setDir)
{
    EditFilesSet(parent, title);
    SetFSTitle(setTitle);
    SetFSDir(setDir);
}

bool EditFilesSet::SetFSDir(const wxString& dir)
{
    m_dir->SetValue(dir);
    bool rv = m_tree->SetFSDir(dir);
    m_files->DeleteNonExistedFiles();

    return rv;
}

bool EditFilesSet::SetFSTitle(const wxString& title)
{
    m_title->SetValue(title);
    return true;
}

bool EditFilesSet::AddFSFile(const wxString& filename)
{
    m_files->AddFile(filename);

    return true;
}

void EditFilesSet::ChooseDir(wxCommandEvent& WXUNUSED(event))
{
    wxDirDialog *dlg = WXDEBUG_NEW wxDirDialog(this, wxT("Choose directory:"), GetFSDir(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dlg->ShowModal() == wxID_OK)
    {
        SetFSDir( dlg->GetPath() );
    }
    dlg->Destroy();
}

void EditFilesSet::AddFiles(wxCommandEvent& event)
{
    if (event.GetId() == wtfEFSD_ADD_ALL_FILES_BUTTON)
    {
        m_files->AddFiles(true);
    } else {
        m_files->AddFiles(false);
    }
}

void EditFilesSet::DelFiles(wxCommandEvent& event)
{
    if (event.GetId() == wtfEFSD_DEL_ALL_FILES_BUTTON)
    {
        m_files->DeleteFiles(true);
    } else {
        m_files->DeleteFiles(false);
    }
}

wxString EditFilesSet::GetFSTitle()
{
    return m_title->GetValue();
}

wxString EditFilesSet::GetFSDir()
{
    return m_dir->GetValue();
}

wxString EditFilesSet::GetFirstFSFile(int *i)
{
    if ( !m_files->GetCount() )
    {
        *i = -1;
        return wxEmptyString;
    }
    *i = 0;
    return m_files->GetString(*i);
}

wxString EditFilesSet::GetNextFSFile(int *i)
{
    if ( (*i < 0) || ( *i >= (int) m_files->GetCount() - 1 ) )
    {
        *i = -1;
        return wxEmptyString;
    }
    *i = *i + 1;
    return m_files->GetString(*i);
}

void EditFilesSet::CheckData(wxCommandEvent& WXUNUSED(event))
{
    if ( ( !m_title->GetValue().Len() ) || ( !m_dir->GetValue().Len() ) || ( !m_files->GetCount() ) )
    {
        wxMessageBox(wxT("The title of files set and directory must be specifyed! Besides, you must add at least one file."), wxT("Information"), wxOK);
        return;
    }
    EndModal(wxID_OK);
}


/*
 * Class representing control with selected files
 */

IMPLEMENT_ABSTRACT_CLASS(wtfChosenFiles, wxListBox)

wtfChosenFiles::wtfChosenFiles(wtfDirTree *source, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxListBox(parent, id, pos, size, 0, NULL, wxLB_MULTIPLE | wxLB_NEEDED_SB)
{
    m_source = source;
}

wtfChosenFiles::~wtfChosenFiles()
{
    ClearList();
}

unsigned int wtfChosenFiles::GetInsertIndex(const wxString& filename)
{
    int res = GetCount();

    if ( m_files.count(filename) )
    {
        return ((unsigned int) -1);
    }

    return res;
}

int wtfChosenFiles::AddFile(const wxString& filename)
{
    int n = GetInsertIndex(filename);

    if (n < 0)
    {
        return -1;
    }

    m_files.insert(filename);
    Insert(filename, n);

    return n;
}

int wtfChosenFiles::AddFile(const wxTreeItemId& item)
{
    if (m_source == NULL)
    {
        return -1;
    }
    int n;
    wxString filename;

    if ( m_source->GetChildrenCount(item) )
    {
        return -1;
    }
    filename = m_source->GetDirPath( m_source->GetItemParent(item) );
    if (m_source->GetItemParent(item) != m_source->GetRootItem())
    {
        filename += wxFileName::GetPathTerminators().Left(1);
    }
    filename += m_source->GetItemText(item);
    n = GetInsertIndex(filename);
    if (n < 0)
    {
        return -1;
    }
    m_files.insert(filename);
    Insert(filename, n);

    return n;
}

wxString wtfChosenFiles::GetFile(unsigned int n)
{
    return GetString(n);
}

bool wtfChosenFiles::DeleteFile(unsigned int n)
{
    wxString filename = GetString(n);

    if ( !filename.Len() )
    {
        return false;
    }

    Delete(n);
    m_files.erase(filename);

    return true;
}

void wtfChosenFiles::ClearList()
{
    Clear();
    m_files.clear();
}

void wtfChosenFiles::AddFiles(bool all)
{
    wxArrayTreeItemIds items;
    wxTreeItemId root, item;
    wxTreeItemIdValue cookie;

    if (all)
    {
        root = m_source->GetRootItem();
        item = m_source->GetFirstChild(root, cookie);
        while (item.IsOk())
        {
            items.Add(item);
            item = m_source->GetNextChild(root, cookie);
        }
    } else {
        m_source->GetSelections(items);
    }

    AddItems(items);
}

void wtfChosenFiles::AddItems(const wxArrayTreeItemIds& items)
{
    unsigned int count = items.Count();

    if (count == 0)
    {
        return;
    }

    wxArrayTreeItemIds items1;
    unsigned int i, j;
    unsigned int pr;
    wxString file;
    bool cont = true;
    wxTreeItemIdValue cookie;
    wxTreeItemId item, curr_item;
    wxString curr_path;
    wxString sep = wxFileName::GetPathSeparator();
    wxString term = wxFileName::GetPathTerminators().Left(1);

    wxProgressDialog *prdlg = WXDEBUG_NEW wxProgressDialog(wxT("Adding files..."), wxT("Initializing..."), 100, m_source->GetParent(), wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
    for (i = 0; i < count; i++)
    {
        if ( m_source->GetItemData(items[i]) == NULL )
        {
            AddFile(m_source->GetItemText(items[i]));
        } else {
            items1.Add(items[i]);
        }
    }
    wxTreeItemId bad_item = m_source->GetItemParent( m_source->GetRootItem() );
    wxASSERT(!bad_item.IsOk());

    prdlg->Pulse(wxT("Adding files..."));
    Freeze();
    count = items1.Count();
    i = count;
    pr = 0;
    while (i > 0 && cont)
    {
        while ((i > 0) && cont && ((pr + 1) % wtfEFSD_PULSE_STEPS))
        {
            if (items1[i - 1] != bad_item)
            {
                j = i - 1;
                curr_item = items1[j];
                items1.RemoveAt(j);
                items1.Add(bad_item);
                m_source->TraverseDir(curr_item);
                if (curr_path.Len())
                {
                    curr_path += sep;
                }
                curr_path += m_source->GetItemText(curr_item);
                item = m_source->GetFirstChild(curr_item, cookie);
                while (item.IsOk())
                {
                    if ( m_source->GetItemData(item) == NULL )
                    {
                        AddFile(curr_path + term + m_source->GetItemText(item));
                    } else {
                        items1.Add(item);
                        i++;
                    }
                    item = m_source->GetNextChild(curr_item, cookie);
                }
            } else {
                j = (unsigned int) curr_path.Find(sep[0], true);
                if ((int) j == wxNOT_FOUND)
                {
                    curr_path.Empty();
                } else {
                    curr_path = curr_path.Left(j);
                }
                i--;
                items1.RemoveAt(i);
            }
            pr++;
        }
        cont = prdlg->Pulse(wxT("Adding files... ") + curr_path);
        pr++;
    }
    prdlg->Show(false);
    prdlg->Destroy();

    items1.clear();
    Thaw();
}

void wtfChosenFiles::DeleteFiles(bool all)
{
    wxArrayInt items;

    if (all)
    {
        ClearList();
    } else {
        while ( GetSelections(items) )
        {
            Delete(items[0]);
        }
    }
}

void wtfChosenFiles::DeleteNonExistedFiles()
{
    unsigned int i = 0;
    wxString dir = m_source->GetFSDir() + wxFileName::GetPathSeparator();
    wxString file;

    Freeze();
    while (i < GetCount())
    {
        file = dir + GetString(i);
        if ( wxFile::Exists( file ) )
        {
            i++;
        } else {
            m_files.erase( file );
            Delete(i);
        }
    }
    Thaw();
}


IMPLEMENT_ABSTRACT_CLASS(wtfDirTreeData, wxObject)

wtfDirTreeData::wtfDirTreeData()
    : m_state(0)
{
}

wtfDirTreeData::wtfDirTreeData(char state)
    : m_state(state)
{
}

wtfDirTreeData::~wtfDirTreeData()
{
}


/*
 * Class representing directory (and files) tree (drop source)
 */

IMPLEMENT_ABSTRACT_CLASS(wtfDirTree, wxTreeCtrl)

BEGIN_EVENT_TABLE(wtfDirTree, wxTreeCtrl)
#ifdef wtfDND_FULL
    EVT_TREE_BEGIN_DRAG(wtfEFSD_BROWSE_TREE,        wtfDirTree::StartDrag)
#endif // wtfDND_FULL
    EVT_TREE_ITEM_EXPANDING(wtfEFSD_BROWSE_TREE,    wtfDirTree::ExpandItem)
END_EVENT_TABLE()

wtfDirTree::wtfDirTree(wxWindow* parent, const wxPoint& pos, const wxSize& size, long style)
    : wxTreeCtrl(parent, wtfEFSD_BROWSE_TREE, pos, size, style)
{
    m_traverser = WXDEBUG_NEW wtfDirTraverser(this);

    AddRoot(wxT("root"));
    wtfDirTreeData *data = WXDEBUG_NEW wtfDirTreeData();
    data->SetState(2);
    SetItemData( GetRootItem(), data );
}

wtfDirTree::~wtfDirTree()
{
    delete(m_traverser);
}

bool wtfDirTree::NeedTraverse(const wxTreeItemId& item)
{
    if ( !item.IsOk() )
    {
        return false;
    }

    wtfDirTreeData *data = (wtfDirTreeData *) GetItemData(item);

    if (data == NULL)
    {
        return false;
    }
    return data->NeedTraverse();
}

bool wtfDirTree::SetFSDir(const wxString& dir)
{
    m_dir = dir;

    if ( !m_dir.Len() )
    {
        return true;
    }

    if ( ( m_dir.EndsWith(wxT("\\")) ) || ( m_dir.EndsWith(wxT("/")) ) )
    {
        m_dir = m_dir.Mid(0, m_dir.Len() - 1);
    }

    m_dnd->SetBaseDir(m_dir + wxFileName::GetPathSeparator());

    ( (wtfDirTreeData *) GetItemData( GetRootItem() ) )->SetState(2);

    return TraverseDir( GetRootItem() );
}

void wtfDirTree::ExpandItem(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    if ( NeedTraverse(item) )
    {
        TraverseDir(item);
    }
}

wxString wtfDirTree::GetDirPath(const wxTreeItemId& item)
{
    wxTreeItemId root = GetRootItem();
    wxTreeItemId curr = item;
    wxString res;

    if (curr == root)
    {
        return res;
    }
    res = GetItemText(curr);
    curr = GetItemParent(curr);

    while (curr != root)
    {
        res = GetItemText(curr) + wxFileName::GetPathSeparator() + res;
        curr = GetItemParent(curr);
    }
    return res;
}

bool wtfDirTree::TraverseDir(const wxTreeItemId& item)
{
    if ( !NeedTraverse(item) )
    {
        return false;
    }
    wxString dir( m_dir + wxFileName::GetPathSeparator() + GetDirPath(item) );
    if (item != GetRootItem())
    {
        dir += wxFileName::GetPathSeparator();
    }
    wxDir *t_dir = WXDEBUG_NEW wxDir(dir);

    if ( !t_dir->IsOpened() )
    {
        delete(t_dir);
        return false;
    }

    m_traverser->SetLen( dir.Len() );

    if ( t_dir->Traverse(*m_traverser) != (size_t) -1 )
    {
        m_traverser->Flush(item);
        delete(t_dir);
        return true;
    }
    delete(t_dir);
    return false;
}

#ifdef wtfDND_FULL
void wtfDirTree::StartDrag(wxTreeEvent& WXUNUSED(event))
{
    wxArrayTreeItemIds items;
    wxString file;
    unsigned int count = GetSelections(items);
    unsigned int i;
    wxFileDataObject dndData;

    for (i = 0; i < count; i++)
    {
        file.Empty();
        file += m_dir;
        if ( GetChildrenCount(items[i]) )
        {
            file += wxFileName::GetPathSeparator() + GetDirPath(items[i]);
        } else {
            if (GetItemParent(items[i]) != GetRootItem())
            {
                file += GetDirPath( GetItemParent(items[i]) );
            }
            file += wxFileName::GetPathTerminators().Left(1) + GetItemText(items[i]);
        }
        dndData.AddFile(file);
    }

    wxDropSource dndSource(this);
    dndSource.SetData(dndData);
    dndSource.DoDragDrop(true);
}
#endif // wtfDND_FULL

wxString wtfDirTree::GetFSDir()
{
    return m_dir;
}

/*
 * Directory Traverser class
 */

IMPLEMENT_ABSTRACT_CLASS(wtfDirTraverser, wxObject)

wtfDirTraverser::wtfDirTraverser(wxTreeCtrl *tree)
{
    m_tree = tree;
    m_len = 0;
}

void wtfDirTraverser::SetLen(int len)
{
    m_len = len;
}

wxDirTraverseResult wtfDirTraverser::OnFile(const wxString& filename)
{
    m_files.Add(filename);
    return wxDIR_CONTINUE;
}

wxDirTraverseResult wtfDirTraverser::OnDir(const wxString& dirname)
{
    m_dirs.Add(dirname);

    return wxDIR_IGNORE;
}

bool wtfDirTraverser::Flush(wxTreeItemId root)
{
    m_tree->DeleteChildren(root);
    wxTreeItemId item;
    int c = m_dirs.GetCount();
    int i;
    wtfDirTreeData *data;
    wxString t_path;

    ( (wtfDirTreeData *) m_tree->GetItemData(root) )->SetState(1);

    for (i = 0; i < c; i++)
    {
        t_path.Empty();
        t_path += m_dirs[i].Mid(m_len);
        if ( (t_path.Len()) && (( t_path.EndsWith(wxT("\\")) ) || ( t_path.EndsWith(wxT("/")) )) )
        {
            t_path = t_path.Mid(0, t_path.Len() - 1);
        }
        item = m_tree->AppendItem(root, t_path);

        m_tree->AppendItem(item, wxT("."));
        m_tree->SetItemBold(item, true);

        data = WXDEBUG_NEW wtfDirTreeData(2);
        m_tree->SetItemData( item, data );
    }
    m_dirs.Clear();
    c = m_files.GetCount();
    for (i = 0; i < c; i++)
    {
        m_tree->AppendItem(root, m_files[i].Mid(m_len));
    }
    m_files.Clear();
    return true;
}

/*
 * DropTarget class
 */

IMPLEMENT_ABSTRACT_CLASS(wtfFilesDropTarget, wxObject)

wtfFilesDropTarget::~wtfFilesDropTarget()
{
    delete(m_traverser);
}

bool wtfFilesDropTarget::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxArrayString& files)
{
    unsigned int count = files.GetCount();
    unsigned int i = 0;
    unsigned int j = 0;
    wxString file;
    bool cont = true;
    wxDir tdir;

    wxProgressDialog *prdlg = WXDEBUG_NEW wxProgressDialog(wxT("Adding files..."), wxT("Adding files..."), 100, m_target->GetParent(), wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
    m_traverser->SetProgressData(prdlg, &j, m_len);
    m_target->Freeze();
    while ((i < count) && cont)
    {
        do
        {
            if ((files[i].Len() < m_len) || (files[i].Left(m_len) != m_dir))
            {
                i++;
                j++;
                continue;
            }
            if ( wxFileName::FileExists(files[i]) )
            {
                
                m_target->AddFile(files[i].Mid(m_len));
                j++;
            } else if ( wxFileName::DirExists(files[i]) ) {
                tdir.Open(files[i]);
                if (tdir.IsOpened())
                {
                    tdir.Traverse(*m_traverser);
                }
                cont = m_traverser->m_cont;
                j++;
            } else {
                j++;
            }
            i++;
        } while ((i < count) && cont && ((j + 1) % wtfEFSD_PULSE_STEPS));
        if (!cont)
        {
            break;
        }
        cont = prdlg->Pulse();
    }
    prdlg->Show(false);
    prdlg->Destroy();

    m_target->Thaw();

    return true;
}


IMPLEMENT_ABSTRACT_CLASS(wtfDnDTraverser, wxObject)

wxDirTraverseResult wtfDnDTraverser::OnFile(const wxString& filename)
{
    m_target->AddFile(filename.Mid(m_len));
    *m_pr = *m_pr + 1;
    if ((*m_pr + 1) % wtfEFSD_PULSE_STEPS == 0)
    {
        if (!m_dlg->Pulse())
        {
            m_cont = false;
            return wxDIR_STOP;
        }
    }
    return wxDIR_CONTINUE;
}

wxDirTraverseResult wtfDnDTraverser::OnDir(const wxString& WXUNUSED(dirname))
{
    return wxDIR_CONTINUE;
}

void wtfDnDTraverser::SetProgressData(wxProgressDialog *dlg, unsigned int *pr, int len)
{
    m_pr = pr;
    m_dlg = dlg;
    m_len = len;
    m_cont = true;
}


IMPLEMENT_ABSTRACT_CLASS(EditNamesSets, wxDialog)

BEGIN_EVENT_TABLE(EditNamesSets, wxDialog)
    EVT_LIST_ITEM_SELECTED(wtfENSsD_SETS_LIST,      EditNamesSets::SelectSet)
    EVT_LIST_ITEM_DESELECTED(wtfENSsD_SETS_LIST,    EditNamesSets::DeselectSet)
    EVT_LIST_BEGIN_LABEL_EDIT(wtfENSsD_SETS_LIST,   EditNamesSets::OnStartLabelEdit)
    EVT_LIST_END_LABEL_EDIT(wtfENSsD_SETS_LIST,     EditNamesSets::SetNSTitle)
    EVT_BUTTON(wtfENSsD_ADD_BUTTON,                 EditNamesSets::AddNS)
    EVT_BUTTON(wtfENSsD_EDIT_BUTTON,                EditNamesSets::EditNS)
    EVT_BUTTON(wtfENSsD_DELETE_BUTTON,              EditNamesSets::DeleteNS)
    EVT_BUTTON(wtfENSsD_MARK_MAIN_BUTTON,           EditNamesSets::SetMainId)
    EVT_MENU(wtfENSsD_CM_ADD,                       EditNamesSets::AddNS)
    EVT_MENU(wtfENSsD_CM_EDIT,                      EditNamesSets::EditNS)
    EVT_MENU(wtfENSsD_CM_DELETE,                    EditNamesSets::DeleteNS)
    EVT_MENU(wtfENSsD_CM_MARK_MAIN,                 EditNamesSets::SetMainId)
END_EVENT_TABLE()

EditNamesSets::EditNamesSets(wxWindow *parent, const wxString& title, wtfMainTab *mtab, FilesSet *fs)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
    m_mainTab = mtab;
    if (fs == NULL)
    {
        new_set = true;
        fset = WXDEBUG_NEW FilesSet();
    } else {
        new_set = false;
        fset = fs;
    }
    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *btnsizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);

    m_setsList = WXDEBUG_NEW wxListCtrl(this, wtfENSsD_SETS_LIST, wxDefaultPosition, wxSize(200, 150), wtfENSsD_SETS_LIST_STYLE);
    m_setsList->InsertColumn(0, wxT("Title"));
    innersizer->Add(
        m_setsList,
        wxSizerFlags(1).Expand().Border(wxRIGHT, 10)
    );

    m_setsList->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(EditNamesSets::ShowContextMenu), NULL, this);
    m_setsList->Connect(wtfENSsD_SETS_LIST, wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler(EditNamesSets::ShowSetContextMenu), NULL, this);

    if (!new_set)
    {
        int mid = fset->GetMainId();
        int i = 0;
        int id;
        NamesSet *nset = fset->GetFirstNS(&id);
        while ( (id > 0) && (id < fset->GetNextNSId()) )
        {
            m_setsList->InsertItem( (id == mid) ? 0 : i, nset->GetTitle() );
            m_setsList->SetItemData( (id == mid) ? 0 : i, id);
            i++;
            nset = fset->GetNextNS(&id);
        }
        m_setsList->SetItemBackgroundColour(0, wxColour(167, 249, 157));
    }

    innersizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_OK, wxT("&Ok")),
        wxSizerFlags(0).Border(wxTOP, 20)
    );

    topsizer->Add(
        innersizer,
        wxSizerFlags(1).Expand().Border(wxALL, 20)
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wtfENSsD_ADD_BUTTON, wxT("&Add...")),
        wxSizerFlags(0).Expand()
    );

    m_btnEdit = WXDEBUG_NEW wxButton(this, wtfENSsD_EDIT_BUTTON, wxT("&Edit..."));
    m_btnEdit->Enable(false);
    btnsizer->Add(
        m_btnEdit,
        wxSizerFlags(0).Expand().Border(wxTOP, 4)
    );

    m_btnDelete = WXDEBUG_NEW wxButton(this, wtfENSsD_DELETE_BUTTON, wxT("&Delete..."));
    m_btnDelete->Enable(false);
    btnsizer->Add(
        m_btnDelete,
        wxSizerFlags(0).Expand().Border(wxTOP, 4)
    );

    m_btnMarkMain = WXDEBUG_NEW wxButton(this, wtfENSsD_MARK_MAIN_BUTTON, wxT("Mark as &main..."));
    m_btnMarkMain->Enable(false);
    btnsizer->Add(
        m_btnMarkMain,
        wxSizerFlags(0).Expand().Border(wxTOP, 4)
    );

    topsizer->Add(
        btnsizer,
        wxSizerFlags(0).Border(wxALL, 20).Expand()
    );

    SetSizer(topsizer);

    topsizer->Fit(this);
    topsizer->SetSizeHints(this);

    m_menu = WXDEBUG_NEW wxMenu();

    m_menu->Append(wtfENSsD_CM_ADD,         wxT("Add &new names set..."));
    m_menu->Append(wtfENSsD_CM_EDIT,        wxT("&Edit names set..."));
    m_menu->Append(wtfENSsD_CM_DELETE,      wxT("&Delete names set"));
    m_menu->Append(wtfENSsD_CM_MARK_MAIN,   wxT("Mark as &main"));

    m_selected = -1;
}

EditNamesSets::~EditNamesSets()
{
    if (new_set)
    {
        delete(fset);
    }
    delete(m_menu);
}

void EditNamesSets::SelectSet(wxListEvent& event)
{
    m_selected = event.GetIndex();
    int id = m_setsList->GetItemData(m_selected);
    int mid = fset->GetMainId();
    if (mid <= 0)
    {
        mid = id;
    }

    m_btnEdit->Enable(id != mid);
    m_btnDelete->Enable(id != mid);
    m_btnMarkMain->Enable(id != mid);
}

void EditNamesSets::DeselectSet(wxListEvent& WXUNUSED(event))
{
    m_btnEdit->Enable(false);
    m_btnDelete->Enable(false);
    m_btnMarkMain->Enable(false);
}

void EditNamesSets::OnStartLabelEdit(wxListEvent& WXUNUSED(event))
{
    SetEscapeId(wxID_NONE);
}

void EditNamesSets::SetNSTitle(wxListEvent& event)
{
    SetEscapeId(wxID_CANCEL);
    if ( event.IsEditCancelled() )
    {
        return;
    }

    long i = event.GetIndex();
    int id = (int) m_setsList->GetItemData(i);
    NamesSet *nset = fset->GetNamesSet(id);

    if (nset == NULL)
    {
        return;
    }

    nset->SetTitle( event.GetLabel() );
    if (m_mainTab != NULL)
    {
        m_mainTab->SetModifiedFlag(true);
    }
}

void EditNamesSets::ShowSetContextMenu(wxListEvent& WXUNUSED(event))
{
    m_menu->Enable(wtfENSsD_CM_EDIT,        true);
    m_menu->Enable(wtfENSsD_CM_DELETE,      true);
    m_menu->Enable(wtfENSsD_CM_MARK_MAIN,   true);

    PopupMenu(m_menu);
}

void EditNamesSets::ShowContextMenu(wxMouseEvent& WXUNUSED(event))
{
    m_menu->Enable(wtfENSsD_CM_EDIT,        false);
    m_menu->Enable(wtfENSsD_CM_DELETE,      false);
    m_menu->Enable(wtfENSsD_CM_MARK_MAIN,   false);

    PopupMenu(m_menu);
}

void EditNamesSets::SetMainId(wxCommandEvent& WXUNUSED(event))
{
    int id = m_setsList->GetItemData(m_selected);
    NamesSet *nset = fset->GetNamesSet(id);

    if (nset == NULL)
    {
        return;
    }

    m_setsList->SetItem( 0, 0, nset->GetTitle() );
    m_setsList->SetItemData( 0, id );
    nset = fset->GetFilesSet();
    m_setsList->SetItem( m_selected, 0, nset->GetTitle() );
    m_setsList->SetItemData( m_selected, nset->GetId() );
    fset->SetMainId(id);
    if (m_mainTab != NULL)
    {
        m_mainTab->SetModifiedFlag(true);
    }
}

void EditNamesSets::AddNS(wxCommandEvent& WXUNUSED(event))
{
    NamesSet *nset = WXDEBUG_NEW NamesSet();
    if (!DoEditNS(this, fset, nset, wxT("New names set")))
    {
        delete(nset);
        return;
    }
    fset->AddNamesSet(nset);
    int i = m_setsList->GetItemCount();
    m_setsList->InsertItem(i, nset->GetTitle());
    m_setsList->SetItemData(i, nset->GetId());
    if (m_mainTab != NULL)
    {
        m_mainTab->SetModifiedFlag(true);
    }
}

void EditNamesSets::EditNS(wxCommandEvent& WXUNUSED(event))
{
    int id = m_setsList->GetItemData(m_selected);
    NamesSet *nset = fset->GetNamesSet(id);

    if (nset == NULL)
    {
        return;
    }
    if (!DoEditNS(this, fset, nset, wxT("Edit names set")))
    {
        return;
    }
    if (m_mainTab != NULL)
    {
        m_mainTab->SetModifiedFlag(true);
    }
    m_setsList->SetItem(m_selected, 0, nset->GetTitle());
}

bool EditNamesSets::DoEditNS(wxWindow *parent, FilesSet *fset, NamesSet *nset, const wxString &title)
{
    if (nset == NULL)
    {
        return false;
    }
    NamesSet *fs = fset->GetFilesSet();
    NamesSet *ns = WXDEBUG_NEW NamesSet();
    int id;
    wxString name = nset->GetFirstName(&id);
    ns->SetTitle(nset->GetTitle());
    ns->SetDir(nset->GetDir());
    EditNamesSet *dlg = WXDEBUG_NEW EditNamesSet(parent, title, ns);
    while (id > 0 && id < nset->GetNextId())
    {
        dlg->AddFileName(id, fs->GetName(id), name);
        name = nset->GetNextName(&id);
    }
    name = fs->GetFirstName(&id);
    while (id > 0 && id < fs->GetNextId())
    {
        dlg->AddFileName(id, name, wxT(""));
        name = fs->GetNextName(&id);
    }
    if (dlg->ShowModal() != wxID_OK)
    {
        dlg->Destroy();
        delete(ns);
        return false;
    }
    nset->SetTitle(ns->GetTitle());
    nset->SetDir(ns->GetDir());
    nset->DeleteAllNames();
    name = ns->GetFirstName(&id);
    while (id > 0 && id < ns->GetNextId())
    {
        nset->AddName(id, name);
        name = ns->GetNextName(&id);
    }
    dlg->Destroy();
    delete(ns);
    return true;
}

void EditNamesSets::DeleteNS(wxCommandEvent& WXUNUSED(event))
{
    int id = m_setsList->GetItemData(m_selected);
    wxMessageDialog *dlg = WXDEBUG_NEW wxMessageDialog(this, wxT("Do you really want to delete selected names set?"), wxT("Deleting names set"), wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

    if (dlg->ShowModal() != wxID_YES)
    {
        dlg->Destroy();
        return;
    }
    dlg->Destroy();

    m_setsList->DeleteItem(m_selected);
    m_selected = -1;
    m_menu->Enable(wtfENSsD_CM_EDIT,        true);
    m_menu->Enable(wtfENSsD_CM_DELETE,      true);
    m_menu->Enable(wtfENSsD_CM_MARK_MAIN,   true);
    fset->DeleteNamesSet(id);
    if (m_mainTab != NULL)
    {
        m_mainTab->SetModifiedFlag(true);
    }
}


IMPLEMENT_ABSTRACT_CLASS(wtfVariant, wxObject)

wtfVariant::wtfVariant()
{
    m_type = 0;
    m_value.str = NULL;
}

wtfVariant::wtfVariant(int type)
{
    switch (type)
    {
        case 0:
        case 1:
            m_type = 1;
            m_value.str = WXDEBUG_NEW wxString();
            break;
        case 2:
            m_type = type;
            m_value.arr = WXDEBUG_NEW wxArrayString();
            break;
        default:
            m_type = 0;
    }
}

wtfVariant::wtfVariant(int type, wtfVariantValue value)
{
    switch (type)
    {
        case 0:
            m_type = 1;
            m_value.str = WXDEBUG_NEW wxString();
            break;
        case 1:
            m_type = 1;
            m_value.str = value.str;
            break;
        case 2:
            m_type = 2;
            m_value.arr = value.arr;
            break;
        default:
            m_type = 0;
            m_value.str = NULL;
    }
}

wtfVariant::~wtfVariant()
{
    switch (m_type)
    {
        case 1:
            delete(m_value.str);
            break;
        case 2:
            delete(m_value.arr);
            break;
        default:
            break;
    }
    m_type = 0;
}

wtfVariantValue wtfVariant::GetValue()
{
    return m_value;
}

int wtfVariant::GetType()
{
    return m_type;
}

void wtfVariant::AddString(const wxString &str)
{
    wxString *str2;

    switch (m_type)
    {
        case 1:
            str2 = m_value.str;
            m_type = 2;
            m_value.arr = WXDEBUG_NEW wxArrayString();
            m_value.arr->Add(*str2);
            m_value.arr->Add(str);
            delete(str2);
            break;
        case 2:
            m_value.arr->Add(str);
            break;
        default:
            break;
    }
}

void wtfVariant::Replace(const wxString &str)
{
    size_t i;
    switch (m_type)
    {
        case 1:
            if (m_value.str && m_value.str->Len() > str.Len() && m_value.str->Left(str.Len()).Cmp(str) == 0)
            {
                delete(m_value.str);
                m_value.str = WXDEBUG_NEW wxString(str);
            }
            break;
        case 2:
            i = 0;
            while (i < m_value.arr->Count() && (m_value.arr->Item(i).Len() <= str.Len() || m_value.arr->Item(i).Left(str.Len()).Cmp(str) != 0))
            {
                i++;
            }
            if (i < m_value.arr->Count())
            {
                (*(m_value.arr))[i] = str;
            }
            break;
        default:
            break;
    }
}

size_t wtfVariant::GetCount()
{
    return ((m_type == 1) ? 1 : m_value.arr->GetCount());
}

wxString& wtfVariant::GetItem(size_t i)
{
    return ((m_type == 1) ? *(m_value.str) : m_value.arr->Item(i));
}

IMPLEMENT_ABSTRACT_CLASS(wtfPrefixes, wxObject)

wtfPrefixes::wtfPrefixes()
{
}

wtfPrefixes::~wtfPrefixes()
{
    Clear();
}

void wtfPrefixes::Clear()
{
    wtfVariants::iterator it;

    for (it = m_prefixes.begin(); it != m_prefixes.end(); ++it)
    {
        delete(it->second);
        m_prefixes[it->first] = NULL;
    }
    m_prefixes.clear();
}

void wtfPrefixes::AddString(const wxString &str)
{
    wxString pr = wxT("");
    wtfVariant *v;
    wxString str2 = str;
    size_t l = 0;
    size_t i, len;
    wtfVariantValue value;

    while (l <= str.Len())
    {
        if (m_prefixes.count(pr))
        {
            v = m_prefixes[pr];
            if (l < str.Len() && m_prefixes.count(str.Left(l + 1)))
            {
                if (m_prefixes[str.Left(l + 1)]->GetType() == 1)
                {
                    i = l + 1;
                    len = m_prefixes[str.Left(l + 1)]->GetValue().str->Len();
                    while (i <= str.Len() && i <= len && m_prefixes.count(str.Left(i)))
                    {
                        i++;
                    }
                    if (i <= len)
                    {
                        len = i - 1;
                        i = l;
                        v = m_prefixes[str.Left(i)];
                        while (i < len)
                        {
                            v->Replace(str.Left(len));
                            i++;
                            v = m_prefixes[str.Left(i)];
                        }
                        if (i < str.Len())
                        {
                            v->AddString(str);
                            l = i + 1;
                            pr = str.Left(l);
                        } else {
                            v->AddString(wxT(""));
                            l = i + 1;
                        }
                    } else {
                        l = i - 1;
                        pr = str.Left(l);
                    }
                } else {
                    l++;
                    pr = str.Left(l);
                }
            } else {
                l++;
                if (l > str.Len())
                {
                    v->AddString(wxT(""));
                } else {
                    v->AddString(str);
                    pr = str.Left(l);
                }
            }
        } else {
            while (l < str.Len())
            {
                value.str = WXDEBUG_NEW wxString(str);
                m_prefixes[str.Left(l)] = WXDEBUG_NEW wtfVariant(1, value);
                l++;
            }
            m_prefixes[str] = WXDEBUG_NEW wtfVariant(1);
            l++;
        }
    }
}

int wtfPrefixes::Count(const wxString &str)
{
    return m_prefixes.count(str);
}

wtfVariant* wtfPrefixes::GetPrefix(const wxString &str)
{
    if (!m_prefixes.count(str))
    {
        return NULL;
    }
    return m_prefixes[str];
}

size_t wtfPrefixes::GetVariants(wxArrayString *arr, const wxString& prefix)
{
    if (arr == NULL || !m_prefixes.count(prefix))
    {
        return 0;
    }
    if ((m_prefixes[prefix]->GetType() == 1) && (m_prefixes[prefix]->GetItem(0).Len() == 0))
    {
        arr->Add(prefix);
        return 1;
    }
    size_t res = 0;
    wxString item = prefix;
    wtfVarPath path;
    wxArrayString pp;
    size_t next = 1;
    size_t i;

    path.Add(0);
    pp.Add(item);
    item = m_prefixes[prefix]->GetItem(0);
    while (next > 0)
    {
        if (m_prefixes[item]->GetType() == 1)
        {
            if (m_prefixes[item]->GetItem(0).Len() == 0)
            {
                arr->Add(item);
                res++;
                item = pp[next - 1];
                i = path[next - 1];
                next--;
                i++;
                while ((next > 0) && (i >= m_prefixes[item]->GetCount()))
                {
                    item = pp[next - 1];
                    i = path[next - 1];
                    next--;
                    i++;
                }
                if (next == 0 && i >= m_prefixes[item]->GetCount())
                {
                    return res;
                }
                path[next] = i;
                item = m_prefixes[item]->GetItem(i);
                next++;
            } else {
                if (next >= path.GetCount())
                {
                    path.Add(0);
                    pp.Add(item);
                } else {
                    path[next] = 0;
                    pp[next] = item;
                }
                item = m_prefixes[item]->GetItem(0);
                next++;
            }
        } else {
            if (next >= path.GetCount())
            {
                path.Add(0);
                pp.Add(item);
            } else {
                path[next] = 0;
                pp[next] = item;
            }
            item = m_prefixes[item]->GetItem(0);
            next++;
        }
    }
    return res;
}


IMPLEMENT_ABSTRACT_CLASS(EditNamesSet, wxDialog)

BEGIN_EVENT_TABLE(EditNamesSet, wxDialog)
    EVT_BUTTON(wtfENSD_BROWSE_DIR_BUTTON,       EditNamesSet::ChooseDir)
    EVT_BUTTON(wtfENSD_DISAMBIGUATE_BUTTON,     EditNamesSet::DoDisambiguate)
    EVT_BUTTON(wtfENSD_ML_EDIT_BUTTON,          EditNamesSet::DoEditList)
    EVT_BUTTON(wtfENSD_ML_TORRENT_BUTTON,       EditNamesSet::LoadNamesFromTorrent)
    EVT_BUTTON(wxID_OK,                         EditNamesSet::DoOk)
END_EVENT_TABLE()

EditNamesSet::EditNamesSet(wxWindow *parent, const wxString& title, NamesSet *ns)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
    if (ns == NULL)
    {
        nset = WXDEBUG_NEW NamesSet();
    } else {
        nset = ns;
    }
    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inpsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *inp1sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *inp2sizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *btn2sizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);

    wxStaticText *text = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Title of the names set:"));
    int top = text->GetSize().GetHeight();
    inp1sizer->Add(
        text,
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_title = WXDEBUG_NEW wxTextCtrl(this, wtfENSD_TITLE, wxEmptyString, wxDefaultPosition, wxSize(250, 22));
    top += 2 + m_title->GetSize().GetHeight() + 2;
    inp1sizer->Add(
        m_title,
        wxSizerFlags(0).Expand().Border(wxTOP | wxBOTTOM, 2)
    );

    text = WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("Destination directory:"));
    top += text->GetSize().GetHeight();
    inp1sizer->Add(
        text,
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_dir = WXDEBUG_NEW wxTextCtrl(this, wtfENSD_DIR, wxEmptyString, wxDefaultPosition, wxSize(250, 22));
    top += 2 + m_dir->GetSize().GetHeight();
    inp1sizer->Add(
        m_dir,
        wxSizerFlags(0).Expand().Border(wxTOP | wxBOTTOM, 2)
    );

    wxButton *btn = WXDEBUG_NEW wxButton(this, wtfENSD_BROWSE_DIR_BUTTON, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    top -= btn->GetSize().GetHeight();
    inp2sizer->Add(
        btn,
        wxSizerFlags(0).Border(wxTOP, top)
    );

    inpsizer->Add(
        inp1sizer,
        wxSizerFlags(1)
    );
    inpsizer->Add(
        inp2sizer,
        wxSizerFlags(0).Border(wxLEFT, 10)
    );

    innersizer->Add(
        inpsizer,
        wxSizerFlags(0).Expand()
    );

    m_filesList = WXDEBUG_NEW wxListCtrl(this, wtfENSD_FILES_LIST, wxDefaultPosition, wxSize(250, 212), wxLC_REPORT | wxLC_EDIT_LABELS);
    m_filesList->InsertColumn(0, wxT("Name"));
    m_filesList->InsertColumn(1, wxT("File"));
    m_filesList->InsertColumn(2, wxT("Id"));
    m_filesList->SetColumnWidth(0, 100);
    m_filesList->SetColumnWidth(1, 100);
    m_filesList->SetColumnWidth(2, 30);
    m_filesList->Connect(wtfENSD_FILES_LIST, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(EditNamesSet::StartEdit), NULL, this);
    m_filesList->Connect(wtfENSD_FILES_LIST, wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, wxListEventHandler(EditNamesSet::OnBeginLabelEdit), NULL, this);
    m_filesList->Connect(wtfENSD_FILES_LIST, wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(EditNamesSet::OnEndLabelEdit), NULL, this);
    m_filesList->Connect(wtfENSD_FILES_LIST, wxEVT_SIZE, wxSizeEventHandler(EditNamesSet::ListOnSize), NULL, this);
    innersizer->Add(
        m_filesList,
        wxSizerFlags(1).Expand().Border(wxTOP | wxBOTTOM, 2)
    );

    btn2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfENSD_DISAMBIGUATE_BUTTON, wxT("Disambiguate"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0)
    );

    btn2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfENSD_ML_EDIT_BUTTON, wxT("Edit"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxLEFT, 20)
    );

    btn2sizer->Add(
        WXDEBUG_NEW wxButton(this, wtfENSD_ML_TORRENT_BUTTON, wxT("Load from torrent"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxLEFT, 20)
    );

    innersizer->Add(
        btn2sizer,
        wxSizerFlags(0).Right().Border(wxTOP | wxBOTTOM, 2)
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0)
    );
    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxLEFT, 20)
    );

    innersizer->Add(
        btnsizer,
        wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 2)
    );

    topsizer->Add(
        innersizer,
        wxSizerFlags(1).Expand().Border(wxALL, 20)
    );

    SetSizer(topsizer);

    topsizer->Fit(this);
    topsizer->SetSizeHints(this);

    m_editCtrl = NULL;
    start_edit = 0;
    m_prefixes = WXDEBUG_NEW wtfPrefixes();

    m_title->SetValue(nset->GetTitle());
    m_dir->SetValue(nset->GetDir());

    genc = ((wtfApp *) wxTheApp)->genc;
    fset = WXDEBUG_NEW NamesSet();
    m_listDlg = NULL;
}

EditNamesSet::~EditNamesSet()
{
    delete(m_prefixes);
    delete(fset);
    if (m_listDlg != NULL)
    {
        m_listDlg->Destroy();
        m_listDlg = NULL;
    }
}

void EditNamesSet::DoOk(wxCommandEvent& WXUNUSED(event))
{
    if (!m_title->GetValue().Len())
    {
        wxMessageBox(wxT("Title for the names set must be specifyed!"), wxT("Information"), wxOK);
        return;
    }
    if (!m_dir->GetValue().Len())
    {
        wxMessageBox(wxT("Base directory for the names set must be specifyed!"), wxT("Information"), wxOK);
        return;
    }
    if (nset->IsEmpty())
    {
        wxMessageBox(wxT("You should specify alternative name for at least one file. If you want to delete this names set use \"Delete\" button in the \"Manage names sets\" window"), wxT("Information"), wxOK);
        return;
    }
    nset->SetTitle(m_title->GetValue());
    nset->SetDir(m_dir->GetValue());
    delete(m_prefixes);
    m_prefixes = NULL;
    EndModal(wxID_OK);
}

void EditNamesSet::AddFileName(const int id, const wxString& file, const wxString& name)
{
    if (file.IsEmpty() && name.IsEmpty())
    {
        return;
    }
    if (id <= 0)
    {
        return;
    }
    if (nset->NameExists(id))
    {
        return;
    }
    if (!name.IsEmpty())
    {
        nset->AddName(id, name);
        m_prefixes->AddString(name);
    }
    fset->AddName(id, file);
    int i = id - 1;
    wxString str;
    if (i > m_filesList->GetItemCount())
    {
        i = m_filesList->GetItemCount();
    }
    str.Printf(wxT("%d"), id);
    m_filesList->InsertItem(i, name);
    m_filesList->SetItem(i, 1, file);
    m_filesList->SetItem(i, 2, str);
    m_filesList->SetItemData(i, id);
}

void EditNamesSet::UpdateFilesList()
{
    int id = 1;
    int max = nset->GetNextId();
    int i = 0;

    if (fset->GetNextId() > max)
    {
        max = fset->GetNextId();
    }

    m_filesList->DeleteAllItems();

    while (id < max)
    {
        if (nset->NameExists(id) || fset->NameExists(id))
        {
            m_filesList->InsertItem(i, nset->GetName(id));
            m_filesList->SetItem(i, 1, fset->GetName(id));
            m_filesList->SetItem(i, 2, wxString::Format(wxT("%d"), id));
            m_filesList->SetItemData(i, id);
            i++;
        }
        id++;
    }
}

void EditNamesSet::ClearVariants()
{
    m_prefixes->Clear();
}

void EditNamesSet::AddVariant(const wxString& str)
{
    m_prefixes->AddString(str);
}

void EditNamesSet::ListOnSize(wxSizeEvent& WXUNUSED(event))
{
    wxSize new_size = m_filesList->GetClientSize();
    int w = (int) (new_size.GetWidth() - m_filesList->GetColumnWidth(2)) / 2;

    m_filesList->SetColumnWidth(0, w);
    m_filesList->SetColumnWidth(1, w);
}

void EditNamesSet::OnBeginLabelEdit(wxListEvent &event)
{
    if (!start_edit)
    {
        event.Veto();
    }
}

void EditNamesSet::OnEndLabelEdit(wxListEvent &event)
{
    if (event.IsEditCancelled())
    {
        SetEscapeId(wxID_CANCEL);
        m_editCtrl = NULL;
        return;
    }
    int id = m_filesList->GetItemData(event.GetIndex());
    if (id <= 0)
    {
        SetEscapeId(wxID_CANCEL);
        m_editCtrl = NULL;
        return;
    }
    wxString name = event.GetLabel();
    if (!name.IsEmpty() && nset->NameExists(name))
    {
        event.Veto();
        SetEscapeId(wxID_NONE);
        return;
    }
    SetEscapeId(wxID_CANCEL);
    m_editCtrl = NULL;
    nset->DeleteName(id);
    if (!name.IsEmpty())
    {
        nset->AddName(id, name);
    }
}

void EditNamesSet::StartEdit(wxListEvent &event)
{
    start_edit = 1;
    m_editCtrl = m_filesList->EditLabel(event.GetIndex());
    if (!m_editCtrl)
    {
        return;
    }
    m_editCtrl->Connect(m_editCtrl->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditNamesSet::OnTextUpdated), NULL, this);
    m_editLen = m_editCtrl->GetLastPosition();
    SetEscapeId(wxID_NONE);
    start_edit = 0;
}

void EditNamesSet::OnTextUpdated(wxCommandEvent& WXUNUSED(event))
{
    wtfVariant *v = m_prefixes->GetPrefix(m_editCtrl->GetValue());
    long p = m_editCtrl->GetInsertionPoint();
    size_t len = m_editCtrl->GetLastPosition();
    if (len == 0 || p < len || len < m_editLen || v == NULL || v->GetType() != 1 || v->GetValue().str->Len() == 0)
    {
        m_editLen = len;
        return;
    }
    m_editLen = len;
    m_editCtrl->ChangeValue( *(v->GetValue().str) );
    m_editCtrl->SetSelection(p, v->GetValue().str->Len());
}

wxString EditNamesSet::GetNSDir()
{
	return wxString(nset->GetDir());
}

void EditNamesSet::SetNSDir(const wxString& dir)
{
	nset->SetDir(dir);
	m_dir->SetValue(dir);
}

void EditNamesSet::ChooseDir(wxCommandEvent& WXUNUSED(event))
{
    wxDirDialog *dlg = WXDEBUG_NEW wxDirDialog(this, wxT("Choose directory:"), m_dir->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

    if (dlg->ShowModal() == wxID_OK)
    {
        SetNSDir( dlg->GetPath() );
    }
    dlg->Destroy();
}

void EditNamesSet::LoadNamesFromTorrent(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog *dlg = WXDEBUG_NEW wxFileDialog(this, wxT("Select torrent file"), wxT(""), wxT(""), wxT("BitComet files (*.torrent)|*.torrent"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg->ShowModal() != wxID_OK)
    {
        dlg->Destroy();
        return;
    }

    wtfTorrentFile tf;

    tf.SetGenc(genc);
    tf.Load(dlg->GetPath());
    dlg->Destroy();

    if (tf.Error() != 0)
    {
        return;
    }
    m_prefixes->Clear();
    size_t cnt = tf.GetFilesCount();
    size_t i;

    for (i = 0; i < cnt; i++)
    {
        m_prefixes->AddString(tf.GetFileFullName(i));
    }
}

void EditNamesSet::DoDisambiguate(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString names;
    size_t res = m_prefixes->GetVariants(&names);

    if (res == 0)
    {
        wxMessageBox(wxT("There is no names in list"), wxT("Disambiguate by name"), wxOK);
        return;
    }
    wxArrayString files;
    size_t cnt = 0;
    int id;
    wxString name;

    name = fset->GetFirstName(&id);
    while (id > 0 && id < fset->GetNextId())
    {
        if (name.Len() > 0 && !nset->NameExists(id))
        {
            files.Add(name);
        }
        name = fset->GetNextName(&id);
        cnt++;
    }

    if (cnt == 0)
    {
        wxMessageBox(wxT("There is no files having no alternative name specified"), wxT("Disambiguate by name"), wxOK);
        return;
    }

    CorrM corr;

    if (!Disambiguate(files, names, &corr, wxT("")))
    {
        wxMessageBox(wxT("Cannot disambiguate"), wxT("Disambiguate by name: information"), wxOK);
        return;
    }

    CorrM::iterator it;

    name = wxT("Disambiguated.");
    for (it = corr.begin(); it != corr.end(); ++it)
    {
        id = fset->NameExists(files[it->first]);
        nset->DeleteName(id);
        nset->AddName(id, names[it->second]);
        name += wxT("\n'") + files[it->first] + wxT("' => '") + names[it->second] + wxT("'");
    }
    UpdateFilesList();
    wxMessageBox(name, wxT("disambiguate test"), wxOK);
}

void EditNamesSet::DoEditList(wxCommandEvent& WXUNUSED(event))
{
    if (m_listDlg == NULL)
    {
        m_listDlg = WXDEBUG_NEW wtfEditVariants(this, wxT("Edit list"));
    }
    wxArrayString vars;
    size_t cnt = m_prefixes->GetVariants(&vars);
    size_t i;

    m_listDlg->ClearList();
    for (i = 0; i < cnt; i++)
    {
        m_listDlg->AddString(vars[i]);
    }
    m_listDlg->Show(true);
}


IMPLEMENT_ABSTRACT_CLASS(wtfEditVariants, wxDialog)

BEGIN_EVENT_TABLE(wtfEditVariants, wxDialog)
    EVT_BUTTON(wxID_OK,                         wtfEditVariants::DoOk)
END_EVENT_TABLE()

wtfEditVariants::wtfEditVariants(wxWindow *parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);

    innersizer->Add(
        WXDEBUG_NEW wxStaticText(this, wxID_ANY, wxT("List of predefined names:")),
        wxSizerFlags(0).Border(wxLEFT, 3)
    );

    m_list = WXDEBUG_NEW wxTextCtrl(this, wtfEV_LIST, wxT(""), wxDefaultPosition, wxSize(250, 180), wxTE_MULTILINE | wxHSCROLL);
    innersizer->Add(
        m_list,
        wxSizerFlags(1).Expand().Border(wxTOP, 3)
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0)
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxLEFT, 20)
    );

    innersizer->Add(
        btnsizer,
        wxSizerFlags(0).Border(wxTOP, 3)
    );

    topsizer->Add(
        innersizer,
        wxSizerFlags(1).Expand().Border(wxALL, 20)
    );
    SetSizer(topsizer);

    topsizer->Fit(this);
    topsizer->SetSizeHints(this);

    m_parent = (EditNamesSet *) parent;
}

bool wtfEditVariants::ListModified()
{
    return m_list->IsModified();
}

void wtfEditVariants::AddString(const wxString& item)
{
    m_list->AppendText(item + wxT("\n"));
    m_list->SetModified(false);
}

void wtfEditVariants::DoOk(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
    if (!m_list->IsModified() || m_parent == NULL)
    {
        return;
    }
    wxLogMessage(wxT("List has been modified"));
    wxString list = m_list->GetValue();
    size_t pos = 0;
    size_t i = 0;
    size_t len = list.Len();

    m_parent->ClearVariants();
    while (pos < len)
    {
        while ((i < len) && (wxIscntrl(list[i]) || wxIsspace(list[i])))
        {
            i++;
        }
        if (i >= len)
        {
            return;
        }
        pos = i;
        while ((i < len) && !(wxIscntrl(list[i])))
        {
            i++;
        }
        m_parent->AddVariant(list.Mid(pos, i - pos));
        pos = i;
    }
}

void wtfEditVariants::ClearList()
{
    m_list->SetValue(wxT(""));
    m_list->SetModified(false);
}


IMPLEMENT_ABSTRACT_CLASS(wtfRenamerQuestion, wxDialog)

BEGIN_EVENT_TABLE(wtfRenamerQuestion, wxDialog)
	EVT_CLOSE(wtfRenamerQuestion::OnClose)
END_EVENT_TABLE()

wtfRenamerQuestion::wtfRenamerQuestion()
	: wxDialog()
{
}

wtfRenamerQuestion::wtfRenamerQuestion(wxWindow* parent, const wxString& title, const wxString& message, long style)
	: wxDialog()
{
	this->Create(parent, title, message, style);
}

bool wtfRenamerQuestion::Create(wxWindow* parent, const wxString& title, const wxString& message, long style)
{
	wxDialog::Create(parent, wxID_ANY, title);

	wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
	wxBoxSizer *btnsizer = NULL;

	innersizer->Add(
		WXDEBUG_NEW wxStaticText(this, wxID_ANY, message),
		wxSizerFlags(1).Expand()
	);

	wxButton *btn;
	wxEvtHandler *eh = this->GetEventHandler();
	bool haveesc = false;

	if (style & wtfOK)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_OK, wxT("OK"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
	}

	if (style & wtfRETRY)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_RETRY, wxT("Retry"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
	}

	if (style & wtfIGNORE)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_IGNORE, wxT("Ignore"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
	}

	if (style & wtfIGNORE_ALL)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_IGNORE_ALL, wxT("Ignore all"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
	}

	if (style & wtfSTOP)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_STOP, wxT("Stop"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
	}

	if (style & wtfCANCEL)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_CANCEL, wxT("Cancel"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
		if (!haveesc)
		{
			this->SetEscapeId(btn->GetId());
			haveesc = true;
		}
	}

	if (style & wtfABORT)
	{
		btn = WXDEBUG_NEW wxButton(this, wtfID_ABORT, wxT("Abort"));
		if (btnsizer == NULL)
		{
			btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
			btnsizer->Add(
				btn,
				wxSizerFlags(0)
			);
		} else {
			btnsizer->Add(
				btn,
				wxSizerFlags(0).Border(wxLEFT, 20)
			);
		}
		eh->Connect(btn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamerQuestion::OnButtonPressed), NULL, this);
		if (!haveesc)
		{
			this->SetEscapeId(btn->GetId());
			haveesc = true;
		}
	}

	if (btnsizer != NULL)
	{
		innersizer->Add(
			btnsizer,
			wxSizerFlags(0).Center().Border(wxTOP, 7)
		);
	}

	topsizer->Add(
		innersizer,
		wxSizerFlags(1).Expand().Border(wxALL, 20)
	);

	this->SetSizer(topsizer);
	topsizer->Fit(this);
	topsizer->SetSizeHints(this);

	this->SetEscapeId(wtfID_ABORT);

	return true;
}

void wtfRenamerQuestion::OnButtonPressed(wxCommandEvent& event)
{
	this->EndModal(event.GetId());
}

void wtfRenamerQuestion::OnClose(wxCloseEvent& WXUNUSED(event))
{
	this->EndModal(wtfID_ABORT);
}


IMPLEMENT_ABSTRACT_CLASS(wtfRenamerOptions, wsdgRenamerOptions)

wtfRenamerOptions::wtfRenamerOptions()
	: wsdgRenamerOptions(),
	m_mustUndo(false),
	m_skipAll(false)
{
}

wtfRenamerOptions::wtfRenamerOptions(long flags)
	: wsdgRenamerOptions(flags),
	m_mustUndo(false),
	m_skipAll(false)
{
}

wtfRenamerAnswer wtfRenamerOptions::OnError(wtfRenamerAction *action, int error, const wxString& WXUNUSED(msg))
{
	if (action == NULL)
	{
		return wtfRenamerAnswerStop;
	}
	if (m_skipAll)
	{
		return wtfRenamerAnswerSkip;
	}

	wtfRenamerActionData action_data = action->m_data;
	long style = 0;
	wxString message(wxT("Cannot "));

	switch (action->m_type)
	{
		case wtfRenamerActionCreateDir:
			message += wxT("create directory '");
			message += *(action_data.path);
			message += wxT("'");
			break;
		case wtfRenamerActionRemoveDir:
			message += wxT("remove directory '");
			message += *(action_data.path);
			message += wxT("'");
			break;
		case wtfRenamerActionCopyFile:
			message += wxT("copy file '");
			message += *(action_data.path);
			message += wxT("' to '");
			message += *(action_data.newpath);
			message += wxT("'");
			break;
		case wtfRenamerActionRenameFile:
			message += wxT("rename file '");
			message += *(action_data.path);
			message += wxT("' to '");
			message += *(action_data.newpath);
			message += wxT("'");
			break;
		case wtfRenamerActionCreateHardLink:
			message += wxT("create hard link '");
			message += *(action_data.newpath);
			message += wxT("' to file '");
			message += *(action_data.path);
			message += wxT("'");
			break;
		case wtfRenamerActionCreateSymlink:
			message += wxT("create symbolic link '");
			message += *(action_data.newpath);
			message += wxT("' to directory '");
			message += *(action_data.path);
			message += wxT("'");
			break;
		case wtfRenamerActionUnlinkFile:
			message += wxT("remove file '");
			message += *(action_data.path);
			message += wxT("'");
			break;
		default:
			return wtfRenamerAnswerStop;
	}
	message += wxT("\nError: ");

	switch (error)
	{
		case W_RENAMER_ERROR_NOT_SUPPORTED:
			message += wxT("operation is not supported");
			style = wtfOK | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_DIR_NOT_EXISTS:
			message += wxT("some directory in source (or destination) doesn't exists");
			style = wtfRETRY | wtfIGNORE | wtfIGNORE_ALL | wtfSTOP | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_NOT_DIR:
			message += wxT("some component in source (or destination) is not in fact a directory");
			style = wtfOK | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_FILE_NOT_EXISTS:
			message += wxT("source file doesn't exists");
			style = wtfOK | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_CROSS_FS:
			message += wxT("source and destination are located on different file systems");
			style = wtfOK | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_NOT_PERMITTED:
			message += wxT("operation not permitted (permission denied)");
			style = wtfRETRY | wtfIGNORE | wtfIGNORE_ALL | wtfSTOP | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_SYS_ERROR:
			message += wxT("system error");
			style = wtfRETRY | wtfIGNORE | wtfIGNORE_ALL | wtfSTOP | wtfCANCEL;
			break;
		case W_RENAMER_ERROR_FILE_EXISTS:
			message += wxT("destination file already exists");
			style = wtfRETRY | wtfIGNORE | wtfIGNORE_ALL | wtfSTOP | wtfCANCEL;
			break;
		default:
			message += wxT("unknown error");
			style = wtfOK | wtfCANCEL;
			break;
	}

	wtfRenamerQuestion *dlg = WXDEBUG_NEW wtfRenamerQuestion(NULL, wxT("Renamer error"), message, style);
	int rv = dlg->ShowModal();
	wtfRenamerAnswer ans = wtfRenamerAnswerStop;

	dlg->Destroy();

	switch (rv)
	{
		case wtfID_CANCEL:
		case wtfID_ABORT:
			m_mustUndo = true;
		case wtfID_OK:
		case wtfID_STOP:
			ans = wtfRenamerAnswerStop;
			break;
		case wtfID_RETRY:
			ans = wtfRenamerAnswerRetry;
			break;
		case wtfID_IGNORE_ALL:
			m_skipAll = true;
		case wtfID_IGNORE:
			ans = wtfRenamerAnswerSkip;
			break;
		default:
			break;
	}
	return ans;
}

bool wtfRenamerOptions::MustUndo() const
{
	return m_mustUndo;
}

IMPLEMENT_ABSTRACT_CLASS(wtfRenamePreview, wxDialog)

wtfRenamePreview::wtfRenamePreview(wxWindow *parent, const wxString& title, const wxString& message, FilesSet *fs, int nsid)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
    wxASSERT_MSG(fs != NULL, wxT("fs != NULL assert failed in wtfRenamePreview::wtfRenamePreview"));
    m_fs = fs;
    m_mid = fs->GetMainId();
    m_rid = nsid;
    wxASSERT_MSG(m_mid != 0, wxT("Main names set is not specified"));
    m_ns = fs->GetNamesSet(nsid);
    wxASSERT_MSG(m_ns != NULL, wxT("No such names set"));
    wxASSERT_MSG(m_mid != nsid, wxT("Files set is the names set"));

    wxBoxSizer *topsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *innersizer = WXDEBUG_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *btnsizer = WXDEBUG_NEW wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer *optsizer = WXDEBUG_NEW wxStaticBoxSizer(wxVERTICAL, this, wxT("Options"));

    innersizer->Add(
        optsizer,
        wxSizerFlags(0).Expand().Border(wxBOTTOM, 3)
    );

    wxEvtHandler *eh = GetEventHandler();

    wxCheckBox *opt = WXDEBUG_NEW wxCheckBox(this, wtfRP_CHECKBOX_ALLNS, wxT("Try to locate files using all alternative names"));
    optsizer->Add(
        opt,
        wxSizerFlags(0).Expand().Border(wxBOTTOM, 3)
    );
    eh->Connect(opt->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnOptionChange));

    opt = WXDEBUG_NEW wxCheckBox(this, wtfRP_CHECKBOX_COPY, wxT("Copy files across different file systems (when rename has failed)"));
    optsizer->Add(
        opt,
        wxSizerFlags(0).Expand().Border(wxBOTTOM, 3)
    );
    eh->Connect(opt->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnOptionChange));

    opt = WXDEBUG_NEW wxCheckBox(this, wtfRP_CHECKBOX_CREATE_DIRS, wxT("Create directories if not exists"));
    optsizer->Add(
        opt,
        wxSizerFlags(0).Expand().Border(wxBOTTOM, 3)
    );
    eh->Connect(opt->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnOptionChange));

    opt = WXDEBUG_NEW wxCheckBox(this, wtfRP_CHECKBOX_OVERWRITE, wxT("Overwrite files if exists (not recommended)"));
    optsizer->Add(
        opt,
        wxSizerFlags(0).Expand().Border(wxBOTTOM, 3)
    );
    eh->Connect(opt->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnOptionChange));

    m_options = 0;

    if (message.Len())
    {
        innersizer->Add(
            WXDEBUG_NEW wxStaticText(this, wxID_ANY, message),
            wxSizerFlags(0).Border(wxLEFT | wxBOTTOM, 3)
        );
    }
    m_list = WXDEBUG_NEW wxListCtrl(this, wtfRP_LIST, wxDefaultPosition, wxSize(400, 300), wxLC_REPORT | wxLC_SINGLE_SEL);
    m_list->InsertColumn(0, wxT("#"));
    m_list->InsertColumn(1, wxT("Old name"));
    m_list->InsertColumn(2, wxT("New name"));
    innersizer->Add(
        m_list,
        wxSizerFlags(1).Expand()
    );

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wtfRP_RENAME_BUTTON, wxT("Rename"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0).Border(wxRIGHT, 10)
    );
    eh->Connect(wtfRP_RENAME_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnButtonPressed));

    if (wtfRenamer::HasHardlinkSupport())
    {
        btnsizer->Add(
            WXDEBUG_NEW wxButton(this, wtfRP_LINK_BUTTON, wxT("Create hard links"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
            wxSizerFlags(0).Border(wxRIGHT, 10)
        );
        eh->Connect(wtfRP_LINK_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnButtonPressed));
    }

    if (wtfRenamer::HasSymlinkSupport())
    {
        btnsizer->Add(
            WXDEBUG_NEW wxButton(this, wtfRP_SYMLINK_BUTTON, wxT("Create symbolic links"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
            wxSizerFlags(0).Border(wxRIGHT, 10)
        );
        eh->Connect(wtfRP_SYMLINK_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wtfRenamePreview::OnButtonPressed));
    }

    btnsizer->Add(
        WXDEBUG_NEW wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        wxSizerFlags(0)
    );

    innersizer->Add(
        btnsizer,
        wxSizerFlags(0).Border(wxTOP, 3)
    );

    topsizer->Add(
        innersizer,
        wxSizerFlags(1).Expand().Border(wxALL, 20)
    );
    SetSizer(topsizer);

    topsizer->Fit(this);
    topsizer->SetSizeHints(this);

    UpdateList(true);
}

wtfRenamePreview::~wtfRenamePreview()
{
}

void wtfRenamePreview::AddItem(const wxString& oldname, const wxString& newname)
{
    long i = m_list->GetItemCount();

    m_list->InsertItem(i, wxString::Format(wxT("%d"), i + 1));
    m_list->SetItem(i, 1, oldname);
    m_list->SetItem(i, 2, newname);
    if (i % 2 == 0)
    {
        m_list->SetItemBackgroundColour(i, wxColour(167, 249, 157));
    }
}

inline wxString wtfGetFullPath(const wxString& base, const wxString& path)
{
    wxFileName fn(base + wxFileName::GetPathSeparator() + path);

    fn.Normalize(wxPATH_NORM_ALL ^ wxPATH_NORM_SHORTCUT);
    return fn.GetFullPath();
}

void wtfRenamePreview::UpdateList(bool full)
{
    wxArrayString files;
    wxFileName fname, nname;
    wxString path, dir1, dir2;
    size_t i = 0;
    long num = 0;
    int id, nsid;
    NamesSet *fs;
    bool item_exists, file_exists;

    if (full)
    {
        m_list->Freeze();
        m_list->DeleteAllItems();

        dir2 = m_ns->GetDir();
        path = m_ns->GetFirstName(&id);
        if (m_options & wtfRP_OPTION_ALLNS)
        {
            while (id > 0 && id < m_ns->GetNextId())
            {
                nname.Assign( wtfGetFullPath(dir2, path) );
                item_exists = false;
                fs = m_fs->GetFirstNS(&nsid);
                files.Clear();
                while (nsid > 0 && nsid < m_fs->GetNextNSId())
                {
                    if (fs->NameExists(id))
                    {
                        item_exists = (item_exists) || (nsid != m_rid);
                        fname.Assign( wtfGetFullPath(fs->GetDir(), fs->GetName(id)) );
                        files.Add(fname.GetFullPath());
                    }
                    fs = m_fs->GetNextNS(&nsid);
                }
                i = 0;
                while (i < files.GetCount() && !wxFileName::FileExists(files.Item(i)))
                {
                    i++;
                }
                file_exists = (i < files.GetCount()) ? true : false;
                fs = m_fs->GetFilesSet();
                if (file_exists)
                {
                    path = files.Item(i);
                } else if (fs->NameExists(id)) {
                    path = wtfGetFullPath(fs->GetDir(), fs->GetName(id));
                } else {
                    path.Empty();
                }
                m_list->InsertItem(num, wxString::Format(wxT("%d"), num + 1));
                m_list->SetItem(num, 1, path);
                m_list->SetItem(num, 2, nname.GetFullPath());
                m_list->SetItemData(num, id);

                if (!file_exists)
                {
                    m_list->SetItemBackgroundColour(num, wxColour(255, 0, 0));
                } else if (num % 2) {
                    m_list->SetItemBackgroundColour(num, wxColour(167, 249, 157));
                }
                if (!item_exists)
                {
                    m_list->SetItemTextColour(num, wxColour(160, 160, 160));
                }
                num++;
                path = m_ns->GetNextName(&id);
            }
        } else {
            fs = m_fs->GetFilesSet();
            dir1 = fs->GetDir();
            while (id > 0 && id < m_ns->GetNextId())
            {
                item_exists = fs->NameExists(id);
                if (item_exists)
                {
                    fname.Assign( wtfGetFullPath(dir1, fs->GetName(id)) );
                    nname.Assign( wtfGetFullPath(dir2, path) );
                }
                file_exists = (item_exists) ? fname.FileExists() : false;

                if (item_exists)
                {
                    m_list->InsertItem(num, wxString::Format(wxT("%d"), num + 1));
                    m_list->SetItem(num, 1, fname.GetFullPath());
                    m_list->SetItem(num, 2, nname.GetFullPath());
                    m_list->SetItemData(num, id);
    
                    if (!file_exists)
                    {
                        m_list->SetItemBackgroundColour(num, wxColour(255, 0, 0));
                    } else if (num % 2) {
                        m_list->SetItemBackgroundColour(num, wxColour(167, 249, 157));
                    }
                    num++;
                }
                path = m_ns->GetNextName(&id);
            }
        }
        m_list->Thaw();
    } else {
        m_list->Freeze();

        dir2 = m_ns->GetDir();
        if (m_options & wtfRP_OPTION_ALLNS)
        {
            for (num = 0; num < m_list->GetItemCount(); num++)
            {
                id = (int) m_list->GetItemData(num);
                path = m_ns->GetName(id);
                nname.Assign( wtfGetFullPath(dir2, path) );
                item_exists = false;
                fs = m_fs->GetFirstNS(&nsid);
                files.Clear();
                while (nsid > 0 && nsid < m_fs->GetNextNSId())
                {
                    if (fs->NameExists(id))
                    {
                        item_exists = (item_exists) || (nsid != m_rid);
                        fname.Assign( wtfGetFullPath(fs->GetDir(), fs->GetName(id)) );
                        files.Add(fname.GetFullPath());
                    }
                    fs = m_fs->GetNextNS(&nsid);
                }
                i = 0;
                while (i < files.GetCount() && !wxFileName::FileExists(files.Item(i)))
                {
                    i++;
                }
                file_exists = (i < files.GetCount()) ? true : false;
                fs = m_fs->GetFilesSet();
                if (file_exists)
                {
                    path = files.Item(i);
                } else if (fs->NameExists(id)) {
                    path = wtfGetFullPath(fs->GetDir(), fs->GetName(id));
                } else {
                    path.Empty();
                }
                m_list->SetItem(num, 1, path);
                m_list->SetItem(num, 2, nname.GetFullPath());

                if (!file_exists)
                {
                    m_list->SetItemBackgroundColour(num, wxColour(255, 0, 0));
                } else if (num % 2) {
                    m_list->SetItemBackgroundColour(num, wxColour(167, 249, 157));
                } else {
                    m_list->SetItemBackgroundColour(num, m_list->GetBackgroundColour());
                }
                if (item_exists)
                {
                    m_list->SetItemTextColour(num, m_list->GetTextColour());
                } else {
                    m_list->SetItemTextColour(num, wxColour(160, 160, 160));
                }
            }
        } else {
            fs = m_fs->GetFilesSet();
            dir1 = fs->GetDir();
            for (num = 0; num < m_list->GetItemCount(); num++)
            {
                id = (int) m_list->GetItemData(num);
                path = m_ns->GetName(id);
                nname.Assign( wtfGetFullPath(dir2, path) );
                fname.Assign( wtfGetFullPath(dir1, fs->GetName(id)) );
                file_exists = fname.FileExists();

                m_list->SetItem(num, 1, fname.GetFullPath());
                m_list->SetItem(num, 2, nname.GetFullPath());
    
                if (!file_exists)
                {
                    m_list->SetItemBackgroundColour(num, wxColour(255, 0, 0));
                } else if (num % 2) {
                    m_list->SetItemBackgroundColour(num, wxColour(167, 249, 157));
                } else {
                    m_list->SetItemBackgroundColour(num, m_list->GetBackgroundColour());
                }
            }
        }
        m_list->Thaw();
    }
}

void wtfRenamePreview::OnOptionChange(wxCommandEvent& event)
{
    int flag = 0;
    bool nu = false;
    bool fu = false;

    switch (event.GetId())
    {
        case wtfRP_CHECKBOX_ALLNS:
            flag = wtfRP_OPTION_ALLNS;
            nu = true;
            fu = true;
            break;
        case wtfRP_CHECKBOX_COPY:
            flag = wtfRP_OPTION_COPY;
            break;
	case wtfRP_CHECKBOX_CREATE_DIRS:
            flag = wtfRP_OPTION_CREATE_DIRS;
            break;
        case wtfRP_CHECKBOX_OVERWRITE:
            flag = wtfRP_OPTION_OVERWRITE;
            break;
        default:
            break;
    }

    m_options = m_options ^ flag;
    if (nu)
    {
        UpdateList(fu);
    }
}

void wtfRenamePreview::OnButtonPressed(wxCommandEvent& event)
{
    int mode;

    switch (event.GetId())
    {
        case wtfRP_RENAME_BUTTON:
            mode = W_RENAMER_RENAME;
            break;
        case wtfRP_LINK_BUTTON:
            mode = W_RENAMER_CREATE_HARDLINK;
            break;
        case wtfRP_SYMLINK_BUTTON:
            mode = W_RENAMER_CREATE_SYMLINK;
            break;
        default:
            return;
    }

    wxArrayString files, names, items;
    wxFileName fname, nname;
    wxString path, dir1, dir2;
    size_t i = 0;
    int id, nsid;
    NamesSet *fs;
    bool item_exists, file_exists;

    dir2 = m_ns->GetDir();
    path = m_ns->GetFirstName(&id);
    if (m_options & wtfRP_OPTION_ALLNS)
    {
        while (id > 0 && id < m_ns->GetNextId())
        {
            nname.Assign( wtfGetFullPath(dir2, path) );
            item_exists = false;
            fs = m_fs->GetFirstNS(&nsid);
            while (nsid > 0 && nsid < m_fs->GetNextNSId())
            {
                if (fs->NameExists(id))
                {
                    item_exists = (item_exists) || (nsid != m_rid);
                    fname.Assign( wtfGetFullPath(fs->GetDir(), fs->GetName(id)) );
                    items.Add(fname.GetFullPath());
                }
                fs = m_fs->GetNextNS(&nsid);
            }
            i = 0;
            while (i < items.GetCount() && !wxFileName::FileExists(items.Item(i)))
            {
                i++;
            }
            file_exists = (i < items.GetCount()) ? true : false;
            fs = m_fs->GetFilesSet();
            if (file_exists)
            {
                path = items.Item(i);
                files.Add(path);
                names.Add( nname.GetFullPath() );
            }
            path = m_ns->GetNextName(&id);
            items.Clear();
        }
    } else {
        fs = m_fs->GetFilesSet();
        dir1 = fs->GetDir();
        while (id > 0 && id < m_ns->GetNextId())
        {
            item_exists = fs->NameExists(id);
            if (item_exists)
            {
                fname.Assign( wtfGetFullPath(dir1, fs->GetName(id)) );
                nname.Assign( wtfGetFullPath(dir2, path) );
                if (fname.FileExists())
                {
                    files.Add( fname.GetFullPath() );
                    names.Add( nname.GetFullPath() );
                }
            }
            path = m_ns->GetNextName(&id);
        }
    }

    long flags = 0;

    if (m_options & wtfRP_OPTION_CREATE_DIRS)
    {
        flags = flags | W_RENAMER_OPTION_CREATE_DIRS;
    }

    wtfRenamerOptions opts;
    wtfRenamer renamer(files, names);
    renamer.SetOptions((wsdgRenamerOptions *) &opts);
    bool rv = renamer.Rename(mode, flags);
    int rc = renamer.GetError();

    if (rv)
    {
    	if (mode == W_RENAMER_RENAME)
    	{
    	    m_fs->SetMainId( m_ns->GetId() );
    	}
    	EndModal(event.GetId());
    }
    if (opts.MustUndo())
    {
        renamer.Undo();
    }
}

