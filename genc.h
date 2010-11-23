/*
 * This header file contains definition of wtfGEncoding,
 * which provides methods for automatic encoding
 * determination.
 *
 * The method of encoding determination is based on the
 * one described in the article "Language Trees and Zipping"
 * (http://xxx.uni-augsburg.de/format/cond-mat/0108530)
 *
 * Suppose we have sequence of bytes (octets). We need to
 * determine encoding. Note that language is also unknown.
 * The statement is that relative enthropy between a sequence
 * (which encoding should be determined) and a sample text
 * has its minimum value for sample text having the same
 * encoding and language. So, we should have enough samples
 * for correct encoding determination (for each language
 * and encoding).
 * For each encoding and language sample text should be
 * long enough.
 * Note that for a particular encoding there may be several
 * sample texts (each in its own language).
 *
 * Relative enthropy is calculated by the following formula:
 *     sum{i=0..255}(q_i * ln(q_i / p_i) / ln(2))
 * Here q_i is probability of occurence of byte with value i
 * in sequence under consideration, and p_i is probability of
 * occurence of byte with value i in sample text.
 *
 * If p_i is equal to 0 and q_i is not 0 for some i, then
 * summand i is assumed to be +infinity. Its quite obvious,
 * since enthropy is average length (in bits) of one byte
 * when sample text is compressed optimally. This optimal
 * compressing algorythm use 0 bits for bytes which never
 * occur in sample text. The relative enthropy between given
 * sequence and sample text is the difference between average
 * length used for one byte of the sequence appended to the
 * sample text and the one used for one byte of sample text
 * itself in compressing algorythm optimal for sample text.
 * Therefore, if in the sequence there is byte with value i
 * and there is no such byte in the sample text, compressing
 * algorythm is not able to compress the sequence at all.
 * This also means that the language and encoding of the
 * given sequence are not the same as ones of the sample text.
 * If p_i and q_i both are equal to 0, summand i is assumed
 * to be 0.
 */

#ifndef WTF_GENCODE_H
#define WTF_GENCODE_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#   include "wx/xml/xml.h"
#   include "wx/strconv.h"
#   include "wx/dynarray.h"
#endif //precompiled headers

#include <cmath>

/*
 * Class wtfGEncoding implements the technique described above.
 * It was designed for Wintermute Torrent files Utility which
 * uses wxWidgets for GUI part.
 *
 * SYNOPSIS:
 * wtfGEncoding genc;   // wtfGEncoding genc(wxT("encdata.xml"));
 * genc.LoadData(wxT("encdata.xml"));
 *
 * char *buf = "given sequence";
 * size_t i = genc.Guess(buf); // no wxT() is need
 *                  // since we usually need to determine
 *                  // encoding of sequence of bytes before
 *                  // transform it to wxString object
 * wxString *str;
 * if (i < genc.GetCount())
 * {
 *     str = new wxString(buf, genc.GetConv(i));
 * } else {
 *     // try another way =)
 * }
 */

typedef struct
{
    wxString *name;
    wxString *lang;
    unsigned long bcount;
    unsigned long *p;
} gencData;

class wtfEncConv : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfEncConv)
private:
    wxMBConv *m_conv;
    bool m_multibyte;
    bool m_delete;
public:
    wtfEncConv(const wxString& encoding);
    ~wtfEncConv();

    bool IsMultibyte();

    wxMBConv& GetConv();
};

WX_DECLARE_STRING_HASH_MAP(wtfEncConv*, wtfConvCache);

#define wtfGEncodingDelta 10

class wtfGEncoding : public wxObject
{
DECLARE_ABSTRACT_CLASS(wtfGEncoding)
private:
    gencData *sample;
    size_t m_count;
    size_t m_size;
    int m_error;
    wtfConvCache *m_convCache;

    bool CheckEncoding(wxXmlNode *enc, const wxString& filename);
    bool CheckEncData(wxXmlNode *data, unsigned long *p, const wxString& filename);

public:
    wtfGEncoding();
    wtfGEncoding(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    ~wtfGEncoding();

    void AddEncoding(const wxString& name, const wxString& lang, unsigned long* p);
    bool LoadData(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    size_t GetCount();
    wxString& GetLanguage(size_t i);
    wxString& GetEncodingName(size_t i);
    wxMBConv& GetConv(size_t i);
    wxMBConv& GetConv(const wxString& encoding);
    wtfEncConv& GetConvInfo(size_t i);
    wtfEncConv& GetConvInfo(const wxString& encoding);

    size_t Guess(const char *seq, const size_t len = 0);

    int error() const;
};

#endif // ifndef WTF_GENCODE_H

