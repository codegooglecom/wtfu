#ifndef wtfUTIL_H
#define wtfUTIL_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/dynarray.h"
#   include "wx/dynlib.h"
#   include "wx/file.h"
#   include "wx/filename.h"
#   include "wx/dir.h"
#endif //precompiled headers

//#define wtfUTIL_H_DEBUG


WX_DECLARE_HASH_MAP(size_t, size_t, wxIntegerHash, wxIntegerEqual, CorrM );

WX_DECLARE_STRING_HASH_MAP(size_t, wtfArrayNames);

class wtfFileNameComponents : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfFileNameComponents)
private:
    wxString m_filename;
    CorrM m_offsets;
    size_t m_len;
    size_t m_id;

public:
    wtfFileNameComponents(const wxString& filename, const wxString& ignore = wxT(" _."));
    ~wtfFileNameComponents();

    size_t Len() const;
    wxString GetPart(const size_t from, const size_t len);
    size_t FindPart(const wxString& part) const;
    void SetId(size_t id);
    size_t GetId() const;
};

WX_DECLARE_OBJARRAY(wtfFileNameComponents, wtfArrayFNC);

bool Disambiguate(wxArrayString& files, wxArrayString& names, CorrM *corr, const wxString& ignore = wxT(" _."));

void wtfMakeFilesList(const wxString& text, wxArrayString *list);


#define W_RENAMER_RENAME                1
#define W_RENAMER_CREATE_HARDLINK       2
#define W_RENAMER_CREATE_SYMLINK        3

#define W_RENAMER_ERROR_NOT_SUPPORTED           1
#define W_RENAMER_ERROR_DIR_NOT_EXISTS          2
#define W_RENAMER_ERROR_NOT_DIR                 3
#define W_RENAMER_ERROR_FILE_NOT_EXISTS         4
#define W_RENAMER_ERROR_CROSS_FS                5
#define W_RENAMER_ERROR_NOT_PERMITTED           6
#define W_RENAMER_ERROR_SYS_ERROR               7
#define W_RENAMER_ERROR_FILE_EXISTS             8
#define W_RENAMER_ERROR_CONFLICT                9

#define W_RENAMER_OPTION_CREATE_DIRS       1

typedef enum {
	wtfRenamerActionCreateDir,
	wtfRenamerActionRemoveDir,
	wtfRenamerActionCopyFile,
	wtfRenamerActionRenameFile,
	wtfRenamerActionCreateHardLink,
	wtfRenamerActionCreateSymlink,
	wtfRenamerActionUnlinkFile
} wtfRenamerActionType;

typedef struct {
	wxString *path;
	wxString *newpath;
} wtfRenamerActionData;

typedef enum {
	wtfRenamerAnswerStop,
	wtfRenamerAnswerContinue,
	wtfRenamerAnswerRetry,
	wtfRenamerAnswerSkip
} wtfRenamerAnswer;

class wtfRenamerAction;

class wsdgRenamerOptions : public wxObject
{
DECLARE_ABSTRACT_CLASS(wsdgRenamerOptions)
public:
	long m_flags;

	wsdgRenamerOptions();
	wsdgRenamerOptions(long flags);

	virtual wtfRenamerAnswer OnError(wtfRenamerAction *action, int error, const wxString& msg = wxEmptyString);
};

static wsdgRenamerOptions wsdgRenamerDefOpts;

class wtfRenamerAction : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfRenamerAction)
public:
	wtfRenamerActionType	m_type;
	wtfRenamerActionData	m_data;
	int			m_error;

	wtfRenamerAction(const wtfRenamerActionType type, const wtfRenamerActionData& data);
	~wtfRenamerAction();

	int PerformAction();
	int GetError() const { return m_error; }
};

WX_DECLARE_OBJARRAY(wtfRenamerAction, wtfRenamerProcess);

class wtfRenamer : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfRenamer)
private:
	wxArrayString	m_files,
			m_names;
	wsdgRenamerOptions	*m_opts;
	wtfRenamerProcess	m_process;
	int		m_error;
	wtfRenamerProcess	m_undo;
#ifdef __WXMSW__
	static bool m_hasHLS;
	static wxDynamicLibrary	*m_lib;
	static bool m_HLShasW;
	static bool m_HLShasA;
	typedef BOOL (__stdcall *m_HLSfuncW)(LPCWSTR,LPCWSTR,LPSECURITY_ATTRIBUTES);
	typedef BOOL (__stdcall *m_HLSfuncA)(LPCSTR,LPCSTR,LPSECURITY_ATTRIBUTES);
#endif

	int DoRename(int mode = W_RENAMER_RENAME, long flags = 0, bool undo = false);
public:
	wtfRenamer(const wxArrayString& files, const wxArrayString& names);
	~wtfRenamer();

	static bool HasSymlinkSupport() { return false; }
	static bool HasHardlinkSupport();

	static bool CanCreateHardlink(const wxString& oldname, const wxString& newname);
	static bool CanCreateSymlink(const wxString& WXUNUSED(oldname), const wxString& WXUNUSED(newname)) { return false; }

	static int doGetFileName(const wxString& name, wxFileName* filename);

	static int doRename(const wxString& oldname, const wxString& newname);
	static int doCreateHardlink(const wxString& oldname, const wxString& newname);
	static int doCreateSymlink(const wxString& WXUNUSED(oldname), const wxString& WXUNUSED(newname)) { return W_RENAMER_ERROR_NOT_SUPPORTED; }

	void SetOptions(wsdgRenamerOptions* opts);

	int CreateRenameProcess(int mode = W_RENAMER_RENAME, long flags = 0);
	bool Rename(int mode = W_RENAMER_RENAME, long flags = 0);
	bool Undo();
	int GetError() const { return m_error; }

	static void Free();
};

#endif // #ifndef wtfUTIL_H

