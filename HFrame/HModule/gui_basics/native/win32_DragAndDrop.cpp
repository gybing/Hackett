/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace DragAndDropHelpers
{
    //==============================================================================
    struct JuceDropSource   : public ComBaseClassHelper<IDropSource>
    {
        JuceDropSource() {}

        HCOMRESULT QueryContinueDrag (BOOL escapePressed, DWORD keys) override
        {
            if (escapePressed)
                return DRAGDROP_S_CANCEL;

            if ((keys & (MK_LBUTTON | MK_RBUTTON)) == 0)
                return DRAGDROP_S_DROP;

            return S_OK;
        }

        HCOMRESULT GiveFeedback (DWORD) override
        {
            return DRAGDROP_S_USEDEFAULTCURSORS;
        }
    };

    //==============================================================================
    struct JuceEnumFormatEtc   : public ComBaseClassHelper<IEnumFORMATETC>
    {
        JuceEnumFormatEtc (const FORMATETC* f)  : format (f) {}

        HCOMRESULT Clone (IEnumFORMATETC** result) override
        {
            if (result == 0)
                return E_POINTER;

            auto newOne = new JuceEnumFormatEtc (format);
            newOne->index = index;
            *result = newOne;
            return S_OK;
        }

        HCOMRESULT Next (ULONG celt, LPFORMATETC lpFormatEtc, ULONG* pceltFetched) override
        {
            if (pceltFetched != nullptr)
                *pceltFetched = 0;
            else if (celt != 1)
                return S_FALSE;

            if (index == 0 && celt > 0 && lpFormatEtc != 0)
            {
                copyFormatEtc (lpFormatEtc [0], *format);
                ++index;

                if (pceltFetched != nullptr)
                    *pceltFetched = 1;

                return S_OK;
            }

            return S_FALSE;
        }

        HCOMRESULT Skip (ULONG celt) override
        {
            if (index + (int) celt >= 1)
                return S_FALSE;

            index += celt;
            return S_OK;
        }

        HCOMRESULT Reset() override
        {
            index = 0;
            return S_OK;
        }

    private:
        const FORMATETC* const format;
        int index = 0;

        static void copyFormatEtc (FORMATETC& dest, const FORMATETC& source)
        {
            dest = source;

            if (source.ptd != 0)
            {
                dest.ptd = (DVTARGETDEVICE*) CoTaskMemAlloc (sizeof (DVTARGETDEVICE));
                *(dest.ptd) = *(source.ptd);
            }
        }

        HDECLARE_NON_COPYABLE (JuceEnumFormatEtc)
    };

    //==============================================================================
    class JuceDataObject  : public ComBaseClassHelper <IDataObject>
    {
    public:
        JuceDataObject (JuceDropSource* s, const FORMATETC* f, const STGMEDIUM* m)
            : dropSource (s), format (f), medium (m)
        {
        }

        ~JuceDataObject()
        {
            HAssert (refCount == 0);
        }

        HCOMRESULT GetData (FORMATETC* pFormatEtc, STGMEDIUM* pMedium)
        {
            if ((pFormatEtc->tymed & format->tymed) != 0
                 && pFormatEtc->cfFormat == format->cfFormat
                 && pFormatEtc->dwAspect == format->dwAspect)
            {
                pMedium->tymed = format->tymed;
                pMedium->pUnkForRelease = 0;

                if (format->tymed == TYMED_HGLOBAL)
                {
                    auto len = GlobalSize (medium->hGlobal);
                    void* const src = GlobalLock (medium->hGlobal);
                    void* const dst = GlobalAlloc (GMEM_FIXED, len);

                    memcpy (dst, src, len);

                    GlobalUnlock (medium->hGlobal);

                    pMedium->hGlobal = dst;
                    return S_OK;
                }
            }

            return DV_E_FORMATETC;
        }

        HCOMRESULT QueryGetData (FORMATETC* f)
        {
            if (f == 0)
                return E_INVALIDARG;

            if (f->tymed == format->tymed
                  && f->cfFormat == format->cfFormat
                  && f->dwAspect == format->dwAspect)
                return S_OK;

            return DV_E_FORMATETC;
        }

        HCOMRESULT GetCanonicalFormatEtc (FORMATETC*, FORMATETC* pFormatEtcOut)
        {
            pFormatEtcOut->ptd = 0;
            return E_NOTIMPL;
        }

        HCOMRESULT EnumFormatEtc (DWORD direction, IEnumFORMATETC** result)
        {
            if (result == 0)
                return E_POINTER;

            if (direction == DATADIR_GET)
            {
                *result = new JuceEnumFormatEtc (format);
                return S_OK;
            }

            *result = 0;
            return E_NOTIMPL;
        }

        HCOMRESULT GetDataHere (FORMATETC*, STGMEDIUM*)                  { return DATA_E_FORMATETC; }
        HCOMRESULT SetData (FORMATETC*, STGMEDIUM*, BOOL)                { return E_NOTIMPL; }
        HCOMRESULT DAdvise (FORMATETC*, DWORD, IAdviseSink*, DWORD*)     { return OLE_E_ADVISENOTSUPPORTED; }
        HCOMRESULT DUnadvise (DWORD)                                     { return E_NOTIMPL; }
        HCOMRESULT EnumDAdvise (IEnumSTATDATA**)                         { return OLE_E_ADVISENOTSUPPORTED; }

    private:
        JuceDropSource* const dropSource;
        const FORMATETC* const format;
        const STGMEDIUM* const medium;

        HDECLARE_NON_COPYABLE (JuceDataObject)
    };

    //==============================================================================
    HDROP createHDrop (const StringArray& fileNames)
    {
        int totalBytes = 0;
        for (int i = fileNames.size(); --i >= 0;)
            totalBytes += (int) CharPointer_UTF16::getBytesRequiredFor (fileNames[i].getCharPointer()) + sizeof (WCHAR);

        HDROP hDrop = (HDROP) GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof (DROPFILES) + totalBytes + 4);

        if (hDrop != 0)
        {
            auto pDropFiles = (LPDROPFILES) GlobalLock (hDrop);
            pDropFiles->pFiles = sizeof (DROPFILES);
            pDropFiles->fWide = true;

            auto* fname = reinterpret_cast<WCHAR*> (addBytesToPointer (pDropFiles, sizeof (DROPFILES)));

            for (int i = 0; i < fileNames.size(); ++i)
            {
                auto bytesWritten = fileNames[i].copyToUTF16 (fname, 2048);
                fname = reinterpret_cast<WCHAR*> (addBytesToPointer (fname, bytesWritten));
            }

            *fname = 0;

            GlobalUnlock (hDrop);
        }

        return hDrop;
    }

    struct DragAndDropJob   : public ThreadPoolJob
    {
        DragAndDropJob (FORMATETC f, STGMEDIUM m, DWORD d, std::function<void()> cb)
            : ThreadPoolJob ("DragAndDrop"),
              format (f), medium (m), whatToDo (d),
              completionCallback (cb)
        {
        }

        JobStatus runJob() override
        {
            OleInitialize (0);

            auto source = new JuceDropSource();
            auto data = new JuceDataObject (source, &format, &medium);

            DWORD effect;
            DoDragDrop (data, source, whatToDo, &effect);

            data->Release();
            source->Release();

            OleUninitialize();

            if (completionCallback != nullptr)
                MessageManager::callAsync (completionCallback);

            return jobHasFinished;
        }

        FORMATETC format;
        STGMEDIUM medium;
        DWORD whatToDo;

        std::function<void()> completionCallback;
    };

    class ThreadPoolHolder   : private DeletedAtShutdown
    {
    public:
        ThreadPoolHolder() = default;

        ~ThreadPoolHolder()
        {
            // Wait forever if there's a job running. The user needs to cancel the transfer
            // in the GUI.
            pool.removeAllJobs (true, -1);

            clearSingletonInstance();
        }

        HDECLARE_SINGLETON_SINGLETHREADED (ThreadPoolHolder, false)

        // We need to make sure we don't do simultaneous text and file drag and drops,
        // so use a pool that can only run a single job.
        ThreadPool pool { 1 };
    };

    HIMPLEMENT_SINGLETON (ThreadPoolHolder)
}

//==============================================================================
bool DragAndDropContainer::performExternalDragDropOfFiles (const StringArray& files, const bool canMove,
                                                           Component*, std::function<void()> callback)
{
    if (files.isEmpty())
        return false;

    FORMATETC format = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medium = { TYMED_HGLOBAL, { 0 }, 0 };

    medium.hGlobal = DragAndDropHelpers::createHDrop (files);

    auto& pool = DragAndDropHelpers::ThreadPoolHolder::getInstance()->pool;
    pool.addJob (new DragAndDropHelpers::DragAndDropJob (format, medium,
                                                         canMove ? (DROPEFFECT_COPY | DROPEFFECT_MOVE) : DROPEFFECT_COPY,
                                                         callback),
                true);

    return true;
}

bool DragAndDropContainer::performExternalDragDropOfText (const String& text, Component*, std::function<void()> callback)
{
    if (text.isEmpty())
        return false;

    FORMATETC format = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medium = { TYMED_HGLOBAL, { 0 }, 0 };

    auto numBytes = CharPointer_UTF16::getBytesRequiredFor (text.getCharPointer());

    medium.hGlobal = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, numBytes + 2);
    auto* data = static_cast<WCHAR*> (GlobalLock (medium.hGlobal));

    text.copyToUTF16 (data, numBytes + 2);
    format.cfFormat = CF_UNICODETEXT;

    GlobalUnlock (medium.hGlobal);

    auto& pool = DragAndDropHelpers::ThreadPoolHolder::getInstance()->pool;
    pool.addJob (new DragAndDropHelpers::DragAndDropJob (format,
                                                        medium,
                                                        DROPEFFECT_COPY | DROPEFFECT_MOVE,
                                                        callback),
                 true);

    return true;
}


