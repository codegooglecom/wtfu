/*
 * This module provides interface for encoding data to
 * and decoding data from bencoded strings
 *
 * You may redistribute and/or modify this module free.
 *
 * Author: Alexey Ten aka Wintermute
 * version: 2.0b
 * Copyright (c) 2009
 */

#ifndef W_BENCODE_H
#define W_BENCODE_H

#define W_BENCODE_VERSION '2.0b'

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/hashmap.h"
#   include "wx/dynarray.h"
#   include "wx/file.h"
#   include "wx/filename.h"
#   include "wx/strconv.h"
#endif //precompiled headers

#include "genc.h"

// --------------------------------------------------------- //
// error code constants

#define W_BENCODE_ERROR_UNKNOWN_DATA_TYPE               1
#define W_BENCODE_ERROR_UNEXPECTED_TOKEN                2
#define W_BENCODE_ERROR_NO_END_TAG                      3
#define W_BENCODE_ERROR_ODD_ELEMENTS_IN_DICTIONARY      4
#define W_BENCODE_ERROR_INVALID_KEY                     5


typedef enum {
    wbenDATA_UNDEFINED,
    wbenDATA_INTEGER,
    wbenDATA_STRING,
    wbenDATA_LIST,
    wbenDATA_DICTIONARY
} wbenDataType;

class wbenData;

#define wbenDataInteger long

#define wbenDataString wxString

#ifndef wbenDataList
WX_DECLARE_OBJARRAY(wbenData, wbenDataList);

#define WBEN_INCLUDE_REST() \
    WX_DEFINE_OBJARRAY(wbenDataList);

#endif // #ifndef wbenDataList

#ifndef wbenDataDictionary
WX_DECLARE_STRING_HASH_MAP( wbenData*, wbenDataDictionary );
#endif // #ifndef wbenDataDictionary

typedef union {
    wbenDataInteger     *m_int;
    wbenDataString      *m_str;
    wbenDataList        *m_list;
    wbenDataDictionary  *m_dict;
} wbenDataValue;

typedef enum
{
    wbenSigContinue,
    wbenSigStop
} wbenSignal;

typedef enum
{
    wbenCtxRoot,
    wbenCtxList,
    wbenCtxDict,
    wbenCtxDictKey
} wbenContextType;

WX_DEFINE_ARRAY_CHAR(char, wbenContextPath);

typedef struct {
    char type;
    wbenContextPath *path;
    wbenDataType dtype;
    size_t pos, size, dsize;
    int err;
    char *str;
    wbenSignal sig;
} wbenContext;

class wtfBencode;

typedef void (wtfBencode::*wbenCallbackInteger) (wbenContext*, long);
typedef void (wtfBencode::*wbenCallbackString) (wbenContext*);
typedef void (wtfBencode::*wbenCallbackList) (wbenContext*);
typedef void (wtfBencode::*wbenCallbackDictionary) (wbenContext*);
typedef void (wtfBencode::*wbenCallbackEndTag) (wbenContext *);

#define wbenCbInteger(func) \
    (wbenCallbackInteger)(&func)
#define wbenCbString(func) \
    (wbenCallbackString)(&func)
#define wbenCbList(func) \
    (wbenCallbackList)(&func)
#define wbenCbDictionary(func) \
    (wbenCallbackDictionary)(&func)
#define wbenCbEndTag(func) \
    (wbenCallbackEndTag)(&func)

typedef struct {
    void *obj;
    wbenCallbackInteger     cb_int;
    wbenCallbackString      cb_str;
    wbenCallbackList        cb_list;
    wbenCallbackDictionary  cb_dict;
    wbenCallbackEndTag      cb_endtag;
} wbenCallbacks;

class wtfBencode : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfBencode)
public:
    static bool bdecode(wbenContext *ctx, wbenCallbacks cbs);
    static long _parseInt(wbenContext *ctx, bool issigned = true);
};

class wbenData : public wtfBencode
{
DECLARE_ABSTRACT_CLASS(wbenData)
private:
    wbenDataType m_type;
    wbenDataValue m_data;
    wbenData *m_parent;
    wbenData *m_item;
    int m_error;

public:
    wbenData(wbenDataType dtype);
    wbenData(const wbenData& data);
    wbenData(size_t size, char *str);

    void SetType(wbenDataType dtype);
    void SetValue(wbenDataValue val);
    void SetParent(wbenData *item);

    wbenData* AddItem(const wbenDataType dtype, const wbenDataValue& value);
    wbenData* GetItem(size_t i);
    size_t GetItemCount();

    bool KeyExists(const wbenDataString& key);
    wbenData* GetItem(const wbenDataString& key);
    wbenData* AddKey(const wbenDataString& key);
    void RemoveKey(const wbenDataString& key);

    wbenData* GetParent() const;
    wbenDataType GetType() const;
    wbenDataValue GetValue() const;

// Standard callbacks for default data types
    void wbenCreateInteger(wbenContext *ctx, long value);
    void wbenCreateString(wbenContext *ctx);
    void wbenCreateList(wbenContext *ctx);
    void wbenCreateDictionary(wbenContext *ctx);
    void wbenEndTag(wbenContext *ctx);

    int error();
    int GetError() const;

    void Clear();
    void Undef();

    ~wbenData();
};


#define wtfFILE_NOT_EXISTS              1
#define wtfFILE_ACCESS_DENIED           2
#define wtfFILE_CANNOT_OPEN             3
#define wtfCANNOT_ALLOCATE_MEMORY       4
#define wtfFILE_CANNOT_READ_WHOLE       5
#define wtfWBEN_UNKNOWN_DATA_TYPE       6
#define wtfWBEN_UNEXPECTED_TOKEN        7
#define wtfWBEN_NO_END_TAG              8
#define wtfWBEN_ODD_ELEMENTS            9
#define wtfWBEN_INVALID_KEY             10
#define wtfWBEN_UNKNOWN_ERROR           11
#define wtfTF_INVALID_ROOT_TYPE         12
#define wtfTF_NO_INFO                   13
#define wtfTF_INVALID_INFO_TYPE         14
#define wtfTF_INVALID_NAME_TYPE         15
#define wtfTF_NO_NAME                   16
#define wtfTF_INVALID_LENGTH_TYPE       17
#define wtfTF_NO_LENGTH                 18
#define wtfTF_INVALID_FILES_TYPE        19
#define wtfTF_INVALID_FILES_ENTRY_TYPE  20
#define wtfTF_INVALID_PATH_TYPE         21
#define wtfTF_INVALID_PATH_ENTRY_TYPE   22
#define wtfTF_NO_PATH                   23


class wtfTorrentFile : public wtfBencode
{
DECLARE_ABSTRACT_CLASS(wtfTorrentFile)
private:
    wxString m_encoding;
    wbenData *m_data;
    wbenData *m_item;
    wbenDataList *m_path;
    wbenDataList *m_files;
    int m_error;
    size_t skip_key, enc_key, enc_key_i;
    wtfGEncoding *genc;
    bool genc_delete;

    static int cmp_key(wbenContext *ctx, const size_t len, const char *name);

public:
    wtfTorrentFile();
    wtfTorrentFile(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    ~wtfTorrentFile();

    void SetGenc(wtfGEncoding *ge);
    bool Load(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    int Error() const;
    wbenData& GetData() const;

    size_t GetFilesCount();
    wbenData* GetFileInfo(size_t i);
    wxString& GetFileName(size_t i);
    wxString GetFileFullName(size_t i);
    wxString& GetFileSize(size_t i);

    void wtfCreateInteger(wbenContext *ctx, long value);
    void wtfCreateString(wbenContext *ctx);
    void wtfCreateList(wbenContext *ctx);
    void wtfCreateDictionary(wbenContext *ctx);
    void wtfEndTag(wbenContext *ctx);
    wxString GetPath();
};


#if defined(WBEN_INCLUDE_REST)
#include "wx/arrimpl.cpp"
#endif

#endif // #ifndef W_BENCODE_H

