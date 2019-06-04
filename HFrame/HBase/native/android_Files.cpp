

#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
  METHOD (constructor, "<init>",     "(Landroid/content/Context;Landroid/media/MediaScannerConnection$MediaScannerConnectionClient;)V") \
  METHOD (connect,     "connect",    "()V") \
  METHOD (disconnect,  "disconnect", "()V") \
  METHOD (scanFile,    "scanFile",   "(Ljava/lang/String;Ljava/lang/String;)V") \

DECLARE_JNI_CLASS (MediaScannerConnection, "android/media/MediaScannerConnection")
#undef JNI_CLASS_MEMBERS

#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
 METHOD (query,            "query",            "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;") \
 METHOD (openInputStream,  "openInputStream",  "(Landroid/net/Uri;)Ljava/io/InputStream;") \
 METHOD (openOutputStream, "openOutputStream", "(Landroid/net/Uri;)Ljava/io/OutputStream;")

DECLARE_JNI_CLASS (ContentResolver, "android/content/ContentResolver")
#undef JNI_CLASS_MEMBERS

#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
 METHOD (moveToFirst,     "moveToFirst",     "()Z") \
 METHOD (getColumnIndex,  "getColumnIndex",  "(Ljava/lang/String;)I") \
 METHOD (getString,       "getString",       "(I)Ljava/lang/String;") \
 METHOD (close,           "close",           "()V") \

DECLARE_JNI_CLASS (AndroidCursor, "android/database/Cursor")
#undef JNI_CLASS_MEMBERS

#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
 STATICMETHOD (getExternalStorageDirectory, "getExternalStorageDirectory", "()Ljava/io/File;") \
 STATICMETHOD (getExternalStoragePublicDirectory, "getExternalStoragePublicDirectory", "(Ljava/lang/String;)Ljava/io/File;") \
 STATICMETHOD (getDataDirectory, "getDataDirectory", "()Ljava/io/File;")

DECLARE_JNI_CLASS (AndroidEnvironment, "android/os/Environment")
#undef JNI_CLASS_MEMBERS

#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
 METHOD (close, "close", "()V") \
 METHOD (flush, "flush", "()V") \
 METHOD (write, "write", "([BII)V")

DECLARE_JNI_CLASS (AndroidOutputStream, "java/io/OutputStream")
#undef JNI_CLASS_MEMBERS

#define JNI_CLASS_MEMBERS(METHOD, STATICMETHOD, FIELD, STATICFIELD, CALLBACK) \
 FIELD (publicSourceDir, "publicSourceDir", "Ljava/lang/String;") \
 FIELD (dataDir, "dataDir", "Ljava/lang/String;")

DECLARE_JNI_CLASS (AndroidApplicationInfo, "android/content/pm/ApplicationInfo")
#undef JNI_CLASS_MEMBERS

//==============================================================================
static File HFile (LocalRef<jobject> obj)
{
    auto* env = getEnv();

    if (env->IsInstanceOf (obj.get(), JavaFile) != 0)
        return File (HString (LocalRef<jstring> ((jstring) env->CallObjectMethod (obj.get(),
                                                                                     JavaFile.getAbsolutePath))));

    return {};
}

static File getWellKnownFolder (const char* folderId)
{
    auto* env = getEnv();
    auto fieldId = env->GetStaticFieldID (AndroidEnvironment, folderId, "Ljava/lang/String;");

    if (fieldId == 0)
    {
        // unknown field in environment
        HAssertfalse;
        return {};
    }

    LocalRef<jobject> fieldValue (env->GetStaticObjectField (AndroidEnvironment, fieldId));

    if (fieldValue == nullptr)
        return {};

    LocalRef<jobject> downloadFolder (env->CallStaticObjectMethod (AndroidEnvironment,
                                                                   AndroidEnvironment.getExternalStoragePublicDirectory,
                                                                   fieldValue.get()));

    return (downloadFolder ? HFile (downloadFolder) : File());
}

static LocalRef<jobject> urlToUri (const URL& url)
{
    return LocalRef<jobject> (getEnv()->CallStaticObjectMethod (AndroidUri, AndroidUri.parse,
                                                                javaString (url.toString (true)).get()));
}

//==============================================================================
struct AndroidContentUriResolver
{
public:
    static LocalRef<jobject> getStreamForContentUri (const URL& url, bool inputStream)
    {
        // only use this method for content URIs
        HAssert (url.getScheme() == "content");
        auto* env = getEnv();

        LocalRef<jobject> contentResolver (env->CallObjectMethod (getAppContext().get(), AndroidContext.getContentResolver));

        if (contentResolver)
            return LocalRef<jobject> ((env->CallObjectMethod (contentResolver.get(),
                                                              inputStream ? ContentResolver.openInputStream
                                                                          : ContentResolver.openOutputStream,
                                                              urlToUri (url).get())));

        return LocalRef<jobject>();
    }

    static File getLocalFileFromContentUri (const URL& url)
    {
        // only use this method for content URIs
        HAssert (url.getScheme() == "content");

        auto authority  = url.getDomain();
        auto documentId = URL::removeEscapeChars (url.getSubPath().fromFirstOccurrenceOf ("/", false, false));
        auto tokens = StringArray::fromTokens (documentId, ":", "");

        if (authority == "com.android.externalstorage.documents")
        {
            auto storageId  = tokens[0];
            auto subpath    = tokens[1];

            auto storagePath = getStorageDevicePath (storageId);

            if (storagePath != File())
                return storagePath.getChildFile (subpath);
        }
        else if (authority == "com.android.providers.downloads.documents")
        {
            auto type       = tokens[0];
            auto downloadId = tokens[1];

            if (type.equalsIgnoreCase ("raw"))
            {
                return File (downloadId);
            }
            else if (type.equalsIgnoreCase ("downloads"))
            {
                auto subDownloadPath = url.getSubPath().fromFirstOccurrenceOf ("tree/downloads", false, false);
                return File (getWellKnownFolder ("DIRECTORY_DOWNLOADS").getFullPathName() + "/" + subDownloadPath);
            }
            else
            {
                return getLocalFileFromContentUri (URL ("content://downloads/public_downloads/" + documentId));
            }
        }
        else if (authority == "com.android.providers.media.documents" && documentId.isNotEmpty())
        {
            auto type    = tokens[0];
            auto mediaId = tokens[1];

            if (type == "image")
                type = "images";

            return getCursorDataColumn (URL ("content://media/external/" + type + "/media"),
                                        "_id=?", StringArray {mediaId});
        }

        return getCursorDataColumn (url);
    }

    static String getFileNameFromContentUri (const URL& url)
    {
        auto uri = urlToUri (url);
        auto* env = getEnv();
        LocalRef<jobject> contentResolver (env->CallObjectMethod (getAppContext().get(), AndroidContext.getContentResolver));

        if (contentResolver == 0)
            return {};

        auto filename = getStringUsingDataColumn ("_display_name", env, uri, contentResolver);

        // Fallback to "_data" column
        if (filename.isEmpty())
        {
            auto path = getStringUsingDataColumn ("_data", env, uri, contentResolver);
            filename = path.fromLastOccurrenceOf ("/", false, true);
        }

        return filename;
    }

private:
    //==============================================================================
    static String getCursorDataColumn (const URL& url, const String& selection = {},
                                       const StringArray& selectionArgs = {})
    {
        auto uri = urlToUri (url);
        auto* env = getEnv();
        LocalRef<jobject> contentResolver (env->CallObjectMethod (getAppContext().get(), AndroidContext.getContentResolver));

        if (contentResolver)
        {
            LocalRef<jstring> columnName (javaString ("_data"));
            LocalRef<jobjectArray> projection (env->NewObjectArray (1, JavaString, columnName.get()));

            LocalRef<jobjectArray> args;

            if (selection.isNotEmpty())
            {
                args = LocalRef<jobjectArray> (env->NewObjectArray (selectionArgs.size(), JavaString, javaString ("").get()));

                for (int i = 0; i < selectionArgs.size(); ++i)
                    env->SetObjectArrayElement (args.get(), i, javaString (selectionArgs[i]).get());
            }

            LocalRef<jstring> jSelection (selection.isNotEmpty() ? javaString (selection) : LocalRef<jstring>());
            LocalRef<jobject> cursor (env->CallObjectMethod (contentResolver.get(), ContentResolver.query,
                                                             uri.get(), projection.get(), jSelection.get(),
                                                             args.get(), nullptr));

            if (jniCheckHasExceptionOccurredAndClear())
            {
                // An exception has occurred, have you acquired RuntimePermission::readExternalStorage permission?
                HAssertfalse;
                return {};
            }

            if (cursor)
            {
                if (env->CallBooleanMethod (cursor.get(), AndroidCursor.moveToFirst) != 0)
                {
                    auto columnIndex = env->CallIntMethod (cursor.get(), AndroidCursor.getColumnIndex, columnName.get());

                    if (columnIndex >= 0)
                    {
                        LocalRef<jstring> value ((jstring) env->CallObjectMethod (cursor.get(), AndroidCursor.getString, columnIndex));

                        if (value)
                            return HString (value.get());
                    }
                }

                env->CallVoidMethod (cursor.get(), AndroidCursor.close);
            }
        }

        return {};
    }

    //==============================================================================
    static File getStorageDevicePath (const String& storageId)
    {
        // check for the primary alias
        if (storageId == "primary")
            return getPrimaryStorageDirectory();

        auto storageDevices = getSecondaryStorageDirectories();

        for (auto storageDevice : storageDevices)
            if (getStorageIdForMountPoint (storageDevice) == storageId)
                return storageDevice;

        return {};
    }

    static File getPrimaryStorageDirectory()
    {
        auto* env = getEnv();
        return HFile (LocalRef<jobject> (env->CallStaticObjectMethod (AndroidEnvironment, AndroidEnvironment.getExternalStorageDirectory)));
    }

    static Array<File> getSecondaryStorageDirectories()
    {
        Array<File> results;

        if (getAndroidSDKVersion() >= 19)
        {
            auto* env = getEnv();
            static jmethodID m = (env->GetMethodID (AndroidContext, "getExternalFilesDirs",
                                                    "(Ljava/lang/String;)[Ljava/io/File;"));
            if (m == 0)
                return {};

            auto paths = convertFileArray (LocalRef<jobject> (env->CallObjectMethod (getAppContext().get(), m, nullptr)));

            for (auto path : paths)
                results.add (getMountPointForFile (path));
        }
        else
        {
            // on older SDKs other external storages are located "next" to the primary
            // storage mount point
            auto mountFolder = getMountPointForFile (getPrimaryStorageDirectory())
                                    .getParentDirectory();

            // don't include every folder. Only folders which are actually mountpoints
            statStruct info;
            if (! stat (mountFolder.getFullPathName(), info))
                return {};

            auto rootFsDevice = info.st_dev;
            DirectoryIterator iter (mountFolder, false, "*", File::findDirectories);

            while (iter.next())
            {
                auto candidate = iter.getFile();

                if (stat (candidate.getFullPathName(), info)
                      && info.st_dev != rootFsDevice)
                    results.add (candidate);
            }

        }

        return results;
    }

    //==============================================================================
    static String getStorageIdForMountPoint (const File& mountpoint)
    {
        // currently this seems to work fine, but something
        // more intelligent may be needed in the future
        return mountpoint.getFileName();
    }

    static File getMountPointForFile (const File& file)
    {
        statStruct info;

        if (stat (file.getFullPathName(), info))
        {
            auto dev  = info.st_dev;
            File mountPoint = file;

            for (;;)
            {
                auto parent = mountPoint.getParentDirectory();

                if (parent == mountPoint)
                    break;

                stat (parent.getFullPathName(), info);

                if (info.st_dev != dev)
                    break;

                mountPoint = parent;
            }

            return mountPoint;
        }

        return {};
    }

    //==============================================================================
    static Array<File> convertFileArray (LocalRef<jobject> obj)
    {
        auto* env = getEnv();
        int n = (int) env->GetArrayLength ((jobjectArray) obj.get());
        Array<File> files;

        for (int i = 0; i < n; ++i)
            files.add (HFile (LocalRef<jobject> (env->GetObjectArrayElement ((jobjectArray) obj.get(),
                                                                                 (jsize) i))));

        return files;
    }

    //==============================================================================
    static String getStringUsingDataColumn (const String& columnNameToUse, JNIEnv* env,
                                            const LocalRef<jobject>& uri,
                                            const LocalRef<jobject>& contentResolver)
    {
        LocalRef<jstring> columnName (javaString (columnNameToUse));
        LocalRef<jobjectArray> projection (env->NewObjectArray (1, JavaString, columnName.get()));

        LocalRef<jobject> cursor (env->CallObjectMethod (contentResolver.get(), ContentResolver.query,
                                                         uri.get(), projection.get(), nullptr,
                                                         nullptr, nullptr));

        if (jniCheckHasExceptionOccurredAndClear())
        {
            // An exception has occurred, have you acquired RuntimePermission::readExternalStorage permission?
            HAssertfalse;
            return {};
        }

        if (cursor == 0)
            return {};

        String fileName;

        if (env->CallBooleanMethod (cursor.get(), AndroidCursor.moveToFirst) != 0)
        {
            auto columnIndex = env->CallIntMethod (cursor.get(), AndroidCursor.getColumnIndex, columnName.get());

            if (columnIndex >= 0)
            {
                LocalRef<jstring> value ((jstring) env->CallObjectMethod (cursor.get(), AndroidCursor.getString, columnIndex));

                if (value)
                    fileName = HString (value.get());

            }
        }

        env->CallVoidMethod (cursor.get(), AndroidCursor.close);

        return fileName;
    }
};

//==============================================================================
struct AndroidContentUriOutputStream :  public OutputStream
{
    AndroidContentUriOutputStream (LocalRef<jobject>&& outputStream)
        : stream (outputStream)
    {
    }

    ~AndroidContentUriOutputStream()
    {
        stream.callVoidMethod (AndroidOutputStream.close);
    }

    void flush() override
    {
        stream.callVoidMethod (AndroidOutputStream.flush);
    }

    bool setPosition (int64 newPos) override
    {
        return (newPos == pos);
    }

    int64 getPosition() override
    {
        return pos;
    }

    bool write (const void* dataToWrite, size_t numberOfBytes) override
    {
        if (numberOfBytes == 0)
            return true;

        JNIEnv* env = getEnv();

        jbyteArray javaArray = env->NewByteArray ((jsize) numberOfBytes);
        env->SetByteArrayRegion (javaArray, 0, (jsize) numberOfBytes, (const jbyte*) dataToWrite);

        stream.callVoidMethod (AndroidOutputStream.write, javaArray, 0, (jint) numberOfBytes);
        env->DeleteLocalRef (javaArray);

        pos += static_cast<int64> (numberOfBytes);
        return true;
    }

    GlobalRef stream;
    int64 pos = 0;
};

OutputStream* CreateContentURIOutputStream (const URL& url)
{
    auto stream = AndroidContentUriResolver::getStreamForContentUri (url, false);

    return (stream.get() != 0 ? new AndroidContentUriOutputStream (std::move (stream)) : nullptr);
}

//==============================================================================
class MediaScannerConnectionClient : public AndroidInterfaceImplementer
{
public:
    virtual void onMediaScannerConnected() = 0;
    virtual void onScanCompleted() = 0;

private:
    jobject invoke (jobject proxy, jobject method, jobjectArray args) override
    {
        auto* env = getEnv();

        auto methodName = HString ((jstring) env->CallObjectMethod (method, JavaMethod.getName));

        if (methodName == "onMediaScannerConnected")
        {
            onMediaScannerConnected();
            return nullptr;
        }
        else if (methodName == "onScanCompleted")
        {
            onScanCompleted();
            return nullptr;
        }

        return AndroidInterfaceImplementer::invoke (proxy, method, args);
    }
};

//==============================================================================
bool File::isOnCDRomDrive() const
{
    return false;
}

bool File::isOnHardDisk() const
{
    return true;
}

bool File::isOnRemovableDrive() const
{
    return false;
}

String File::getVersion() const
{
    return {};
}

static File getDocumentsDirectory()
{
    auto* env = getEnv();

    if (getAndroidSDKVersion() >= 19)
        return getWellKnownFolder ("DIRECTORY_DOCUMENTS");

    return HFile (LocalRef<jobject> (env->CallStaticObjectMethod (AndroidEnvironment, AndroidEnvironment.getDataDirectory)));
}

static File getAppDataDir (bool dataDir)
{
    auto* env = getEnv();

    LocalRef<jobject> applicationInfo (env->CallObjectMethod (getAppContext().get(), AndroidContext.getApplicationInfo));
    LocalRef<jobject> jString (env->GetObjectField (applicationInfo.get(), dataDir ? AndroidApplicationInfo.dataDir : AndroidApplicationInfo.publicSourceDir));

    return  {HString ((jstring) jString.get())};
}

File File::getSpecialLocation (const SpecialLocationType type)
{
    switch (type)
    {
        case userHomeDirectory:
        case userApplicationDataDirectory:
        case userDesktopDirectory:
        case commonApplicationDataDirectory:
        {
            static File appDataDir = getAppDataDir (true);
            return appDataDir;
        }

        case userDocumentsDirectory:
        case commonDocumentsDirectory:
        {
            static auto docsDir = getDocumentsDirectory();
            return docsDir;
        }

        case userPicturesDirectory:
        {
            static auto picturesDir = getWellKnownFolder ("DIRECTORY_PICTURES");
            return picturesDir;
        }

        case userMusicDirectory:
        {
            static auto musicDir = getWellKnownFolder ("DIRECTORY_MUSIC");
            return musicDir;
        }
        case userMoviesDirectory:
        {
            static auto moviesDir = getWellKnownFolder ("DIRECTORY_MOVIES");
            return moviesDir;
        }

        case globalApplicationsDirectory:
            return File ("/system/app");

        case tempDirectory:
        {
            File tmp = getSpecialLocation (commonApplicationDataDirectory).getChildFile (".temp");
            tmp.createDirectory();
            return File (tmp.getFullPathName());
        }

        case invokedExecutableFile:
        case currentExecutableFile:
        case currentApplicationFile:
        case hostApplicationPath:
            return getAppDataDir (false);

        default:
            HAssertfalse; // unknown type?
            break;
    }

    return {};
}

bool File::moveToTrash() const
{
    if (! exists())
        return true;

    // TODO
    return false;
}

API bool CALLTYPE Process::openDocument (const String& fileName, const String&)
{
    URL targetURL (fileName);
    auto* env = getEnv();

    const LocalRef<jstring> action (javaString ("android.intent.action.VIEW"));
    LocalRef<jobject> intent (env->NewObject (AndroidIntent, AndroidIntent.constructWithUri, action.get(), urlToUri (targetURL).get()));

    env->CallVoidMethod (getCurrentActivity(), AndroidContext.startActivity, intent.get());
    return true;
}

void File::revealToUser() const
{
}

//==============================================================================
class SingleMediaScanner : public MediaScannerConnectionClient
{
public:
    SingleMediaScanner (const String& filename)
        : msc (LocalRef<jobject> (getEnv()->NewObject (MediaScannerConnection,
                                                       MediaScannerConnection.constructor,
                                                       getAppContext().get(),
                                                       CreateJavaInterface (this, "android/media/MediaScannerConnection$MediaScannerConnectionClient").get()))),
          file (filename)
    {
        getEnv()->CallVoidMethod (msc.get(), MediaScannerConnection.connect);
    }

    void onMediaScannerConnected() override
    {
        auto* env = getEnv();

        env->CallVoidMethod (msc.get(), MediaScannerConnection.scanFile, javaString (file).get(), 0);
    }

    void onScanCompleted() override
    {
        getEnv()->CallVoidMethod (msc.get(), MediaScannerConnection.disconnect);
    }

private:
    GlobalRef msc;
    String file;
};

void FileOutputStream::flushInternal()
{
    if (fileHandle != 0)
    {
        if (fsync (getFD (fileHandle)) == -1)
            status = getResultForErrno();

        // This stuff tells the OS to asynchronously update the metadata
        // that the OS has cached about the file - this metadata is used
        // when the device is acting as a USB drive, and unless it's explicitly
        // refreshed, it'll get out of step with the real file.
        new SingleMediaScanner (file.getFullPathName());
    }
}


