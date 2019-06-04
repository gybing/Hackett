

/*
    Note that a lot of methods that you'd expect to find in this file actually
    live in posix_SharedCode.h!
*/

//==============================================================================
bool File::copyInternal (const File& dest) const
{
    HAUTORELEASEPOOL
    {
        NSFileManager* fm = [NSFileManager defaultManager];

        return [fm fileExistsAtPath: HStringToNS (fullPath)]
                && [fm copyItemAtPath: HStringToNS (fullPath)
                               toPath: HStringToNS (dest.getFullPathName())
                                error: nil];
    }
}

void File::findFileSystemRoots (Array<File>& destArray)
{
    destArray.add (File ("/"));
}


//==============================================================================
namespace MacFileHelpers
{
    static bool isFileOnDriveType (const File& f, const char* const* types)
    {
        struct statfs buf;

        if (doStatFS (f, buf))
        {
            const String type (buf.f_fstypename);

            while (*types != nullptr)
                if (type.equalsIgnoreCase (*types++))
                    return true;
        }

        return false;
    }

    static bool isHiddenFile (const String& path)
    {
       #if HMAC
        HAUTORELEASEPOOL
        {
            NSNumber* hidden = nil;
            NSError* err = nil;

            return [createNSURLFromFile (path) getResourceValue: &hidden forKey: NSURLIsHiddenKey error: &err]
                     && [hidden boolValue];
        }
       #else
        return File (path).getFileName().startsWithChar ('.');
       #endif
    }

   #if HIOS
    static String getIOSSystemLocation (NSSearchPathDirectory type)
    {
        return nsStringToJuce ([NSSearchPathForDirectoriesInDomains (type, NSUserDomainMask, YES)
                                objectAtIndex: 0]);
    }
   #else
    static bool launchExecutable (const String& pathAndArguments)
    {
        auto cpid = fork();

        if (cpid == 0)
        {
            const char* const argv[4] = { "/bin/sh", "-c", pathAndArguments.toUTF8(), nullptr };

            // Child process
            if (execve (argv[0], (char**) argv, nullptr) < 0)
                exit (0);
        }
        else
        {
            if (cpid < 0)
                return false;
        }

        return true;
    }
   #endif
}

bool File::isOnCDRomDrive() const
{
    static const char* const cdTypes[] = { "cd9660", "cdfs", "cddafs", "udf", nullptr };

    return MacFileHelpers::isFileOnDriveType (*this, cdTypes);
}

bool File::isOnHardDisk() const
{
    static const char* const nonHDTypes[] = { "nfs", "smbfs", "ramfs", nullptr };

    return ! (isOnCDRomDrive() || MacFileHelpers::isFileOnDriveType (*this, nonHDTypes));
}

bool File::isOnRemovableDrive() const
{
   #if HIOS
    return false; // xxx is this possible?
   #else
    HAUTORELEASEPOOL
    {
        BOOL removable = false;

        [[NSWorkspace sharedWorkspace]
               getFileSystemInfoForPath: HStringToNS (getFullPathName())
                            isRemovable: &removable
                             isWritable: nil
                          isUnmountable: nil
                            description: nil
                                   type: nil];

        return removable;
    }
   #endif
}

bool File::isHidden() const
{
    return MacFileHelpers::isHiddenFile (getFullPathName());
}

//==============================================================================
const char* const* argv = nullptr;
int argc = 0;

File File::getSpecialLocation (const SpecialLocationType type)
{
    HAUTORELEASEPOOL
    {
        String resultPath;

        switch (type)
        {
            case userHomeDirectory:                 resultPath = nsStringToJuce (NSHomeDirectory()); break;

          #if HIOS
            case userDocumentsDirectory:            resultPath = MacFileHelpers::getIOSSystemLocation (NSDocumentDirectory); break;
            case userDesktopDirectory:              resultPath = MacFileHelpers::getIOSSystemLocation (NSDesktopDirectory); break;

            case tempDirectory:
            {
                File tmp (MacFileHelpers::getIOSSystemLocation (NSCachesDirectory));
                tmp = tmp.getChildFile (getExecutableFile().getFileNameWithoutExtension());
                tmp.createDirectory();
                return tmp.getFullPathName();
            }

          #else
            case userDocumentsDirectory:            resultPath = "~/Documents"; break;
            case userDesktopDirectory:              resultPath = "~/Desktop"; break;

            case tempDirectory:
            {
                File tmp ("~/Library/Caches/" + getExecutableFile().getFileNameWithoutExtension());
                tmp.createDirectory();
                return File (tmp.getFullPathName());
            }
          #endif
            case userMusicDirectory:                resultPath = "~/Music"; break;
            case userMoviesDirectory:               resultPath = "~/Movies"; break;
            case userPicturesDirectory:             resultPath = "~/Pictures"; break;
            case userApplicationDataDirectory:      resultPath = "~/Library"; break;
            case commonApplicationDataDirectory:    resultPath = "/Library"; break;
            case commonDocumentsDirectory:          resultPath = "/Users/Shared"; break;
            case globalApplicationsDirectory:       resultPath = "/Applications"; break;

            case invokedExecutableFile:
                if (argv != nullptr && argc > 0)
                    return File::getCurrentWorkingDirectory().getChildFile (String (argv[0]));
                // deliberate fall-through...

            case currentExecutableFile:
                return getExecutableFile();

            case currentApplicationFile:
            {
                const File exe (getExecutableFile());
                const File parent (exe.getParentDirectory());

               #if HIOS
                return parent;
               #else
                return parent.getFullPathName().endsWithIgnoreCase ("Contents/MacOS")
                        ? parent.getParentDirectory().getParentDirectory()
                        : exe;
               #endif
            }

            case hostApplicationPath:
            {
                unsigned int size = 8192;
                HeapBlock<char> buffer;
                buffer.calloc (size + 8);

                _NSGetExecutablePath (buffer.get(), &size);
                return File (String::fromUTF8 (buffer, (int) size));
            }

            default:
                HAssertfalse; // unknown type?
                break;
        }

        if (resultPath.isNotEmpty())
            return File (resultPath.convertToPrecomposedUnicode());
    }

    return {};
}

//==============================================================================
String File::getVersion() const
{
    HAUTORELEASEPOOL
    {
        if (NSBundle* bundle = [NSBundle bundleWithPath: HStringToNS (getFullPathName())])
            if (NSDictionary* info = [bundle infoDictionary])
                if (NSString* name = [info valueForKey: nsStringLiteral ("CFBundleShortVersionString")])
                    return nsStringToJuce (name);
    }

    return {};
}

//==============================================================================
static NSString* getFileLink (const String& path)
{
    return [[NSFileManager defaultManager] destinationOfSymbolicLinkAtPath: HStringToNS (path) error: nil];
}

bool File::isSymbolicLink() const
{
    return getFileLink (fullPath) != nil;
}

String File::getNativeLinkedTarget() const
{
    if (NSString* dest = getFileLink (fullPath))
        return nsStringToJuce (dest);

    return {};
}

//==============================================================================
bool File::moveToTrash() const
{
    if (! exists())
        return true;

    HAUTORELEASEPOOL
    {
       #if (defined (__IPHONE_11_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_11_0) \
         || (defined (MAC_OS_X_VERSION_10_8) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_8)
        NSError* error = nil;
        return [[NSFileManager defaultManager] trashItemAtURL: createNSURLFromFile (*this)
                                             resultingItemURL: nil
                                                        error: &error];
       #elif HIOS
        return deleteFile();
       #else
        [[NSWorkspace sharedWorkspace] recycleURLs: [NSArray arrayWithObject: createNSURLFromFile (*this)]
                                 completionHandler: nil];

        // recycleURLs is async, so we need to block until it has finished. We can't use a
        // run-loop here because it'd dispatch unexpected messages, so have to do this very
        // nasty bodge. But this is only needed for support of pre-10.8 versions.
        for (int retries = 100; --retries >= 0;)
        {
            if (! exists())
                return true;

            Thread::sleep (5);
        }

        return false;
       #endif
    }
}

//==============================================================================
class DirectoryIterator::NativeIterator::Pimpl
{
public:
    Pimpl (const File& directory, const String& wildcard)
        : parentDir (File::addTrailingSeparator (directory.getFullPathName())),
          wildCard (wildcard)
    {
        HAUTORELEASEPOOL
        {
            enumerator = [[[NSFileManager defaultManager] enumeratorAtPath: HStringToNS (directory.getFullPathName())] retain];
        }
    }

    ~Pimpl()
    {
        [enumerator release];
    }

    bool next (String& filenameFound,
               bool* const isDir, bool* const isHidden, int64* const fileSize,
               Time* const modTime, Time* const creationTime, bool* const isReadOnly)
    {
        HAUTORELEASEPOOL
        {
            const char* wildcardUTF8 = nullptr;

            for (;;)
            {
                if (enumerator == nil)
                    return false;

                NSString* file = [enumerator nextObject];

                if (file == nil)
                    return false;

                [enumerator skipDescendents];
                filenameFound = nsStringToJuce (file).convertToPrecomposedUnicode();

                if (wildcardUTF8 == nullptr)
                    wildcardUTF8 = wildCard.toUTF8();

                if (fnmatch (wildcardUTF8, filenameFound.toUTF8(), FNM_CASEFOLD) != 0)
                    continue;

                auto fullPath = parentDir + filenameFound;
                updateStatInfoForFile (fullPath, isDir, fileSize, modTime, creationTime, isReadOnly);

                if (isHidden != nullptr)
                    *isHidden = MacFileHelpers::isHiddenFile (fullPath);

                return true;
            }
        }
    }

private:
    String parentDir, wildCard;
    NSDirectoryEnumerator* enumerator = nil;

    HDECLARE_NON_COPYABLE (Pimpl)
};

DirectoryIterator::NativeIterator::NativeIterator (const File& directory, const String& wildcard)
    : pimpl (new DirectoryIterator::NativeIterator::Pimpl (directory, wildcard))
{
}

DirectoryIterator::NativeIterator::~NativeIterator()
{
}

bool DirectoryIterator::NativeIterator::next (String& filenameFound,
                                              bool* const isDir, bool* const isHidden, int64* const fileSize,
                                              Time* const modTime, Time* const creationTime, bool* const isReadOnly)
{
    return pimpl->next (filenameFound, isDir, isHidden, fileSize, modTime, creationTime, isReadOnly);
}


//==============================================================================
bool CALLTYPE Process::openDocument (const String& fileName, const String& parameters)
{
    HAUTORELEASEPOOL
    {
        NSString* fileNameAsNS (HStringToNS (fileName));
        NSURL* filenameAsURL = File::createFileWithoutCheckingPath (fileName).exists() ? [NSURL fileURLWithPath: fileNameAsNS]
                                                                                       : [NSURL URLWithString: fileNameAsNS];

      #if HIOS
        ignoreUnused (parameters);

       #if (! defined __IPHONE_OS_VERSION_MIN_REQUIRED) || (! defined __IPHONE_10_0) || (__IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_10_0)
        return [[UIApplication sharedApplication] openURL: filenameAsURL];
       #else
        [[UIApplication sharedApplication] openURL: filenameAsURL options: @{} completionHandler: nil];
        return true;
       #endif
      #else
        NSWorkspace* workspace = [NSWorkspace sharedWorkspace];

        if (parameters.isEmpty())
            // NB: the length check here is because of strange failures involving long filenames,
            // probably due to filesystem name length limitations..
            return (fileName.length() < 1024 && [workspace openFile: HStringToNS (fileName)])
                    || [workspace openURL: filenameAsURL];

        const File file (fileName);

        if (file.isBundle())
        {
            StringArray params;
            params.addTokens (parameters, true);

            NSMutableDictionary* dict = [[NSMutableDictionary new] autorelease];

            NSMutableArray* paramArray = [[NSMutableArray new] autorelease];

            for (int i = 0; i < params.size(); ++i)
                [paramArray addObject: HStringToNS (params[i])];

            [dict setObject: paramArray
                     forKey: nsStringLiteral ("NSWorkspaceLaunchConfigurationArguments")];

            return [workspace launchApplicationAtURL: filenameAsURL
                                             options: NSWorkspaceLaunchDefault | NSWorkspaceLaunchNewInstance
                                       configuration: dict
                                               error: nil];
        }

        if (file.exists())
            return MacFileHelpers::launchExecutable ("\"" + fileName + "\" " + parameters);

        return false;
      #endif
    }
}

void File::revealToUser() const
{
   #if ! HIOS
    if (exists())
        [[NSWorkspace sharedWorkspace] selectFile: HStringToNS (getFullPathName()) inFileViewerRootedAtPath: nsEmptyString()];
    else if (getParentDirectory().exists())
        getParentDirectory().revealToUser();
   #endif
}

//==============================================================================
OSType File::getMacOSType() const
{
    HAUTORELEASEPOOL
    {
        NSDictionary* fileDict = [[NSFileManager defaultManager] attributesOfItemAtPath: HStringToNS (getFullPathName()) error: nil];
        return [fileDict fileHFSTypeCode];
    }
}

bool File::isBundle() const
{
   #if HIOS
    return false; // xxx can't find a sensible way to do this without trying to open the bundle..
   #else
    HAUTORELEASEPOOL
    {
        return [[NSWorkspace sharedWorkspace] isFilePackageAtPath: HStringToNS (getFullPathName())];
    }
   #endif
}

#if HMAC
void File::addToDock() const
{
    // check that it's not already there...
    if (! getOutputFromCommand ("defaults read com.apple.dock persistent-apps").containsIgnoreCase (getFullPathName()))
    {
        runSystemCommand ("defaults write com.apple.dock persistent-apps -array-add \"<dict><key>tile-data</key><dict><key>file-data</key><dict><key>_CFURLString</key><string>"
                                 + getFullPathName() + "</string><key>_CFURLStringType</key><integer>0</integer></dict></dict></dict>\"");

        runSystemCommand ("osascript -e \"tell application \\\"Dock\\\" to quit\"");
    }
}
#endif


