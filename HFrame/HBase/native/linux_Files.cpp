

enum
{
    U_ISOFS_SUPER_MAGIC = 0x9660,   // linux/iso_fs.h
    U_MSDOS_SUPER_MAGIC = 0x4d44,   // linux/msdos_fs.h
    U_NFS_SUPER_MAGIC = 0x6969,     // linux/nfs_fs.h
    U_SMB_SUPER_MAGIC = 0x517B      // linux/smb_fs.h
};

bool File::isOnCDRomDrive() const
{
    struct statfs buf;

    return statfs (getFullPathName().toUTF8(), &buf) == 0
             && buf.f_type == (short) U_ISOFS_SUPER_MAGIC;
}

bool File::isOnHardDisk() const
{
    struct statfs buf;

    if (statfs (getFullPathName().toUTF8(), &buf) == 0)
    {
        switch (buf.f_type)
        {
            case U_ISOFS_SUPER_MAGIC:   // CD-ROM
            case U_MSDOS_SUPER_MAGIC:   // Probably floppy (but could be mounted FAT filesystem)
            case U_NFS_SUPER_MAGIC:     // Network NFS
            case U_SMB_SUPER_MAGIC:     // Network Samba
                return false;

            default: break;
        }
    }

    // Assume so if this fails for some reason
    return true;
}

bool File::isOnRemovableDrive() const
{
    HAssertfalse; // xxx not implemented for linux!
    return false;
}

String File::getVersion() const
{
    return {}; // xxx not yet implemented
}

//==============================================================================
static File resolveXDGFolder (const char* const type, const char* const fallbackFolder)
{
    StringArray confLines;
    File ("~/.config/user-dirs.dirs").readLines (confLines);

    for (int i = 0; i < confLines.size(); ++i)
    {
        const String line (confLines[i].trimStart());

        if (line.startsWith (type))
        {
            // eg. resolve XDG_MUSIC_DIR="$HOME/Music" to /home/user/Music
            const File f (line.replace ("$HOME", File ("~").getFullPathName())
                              .fromFirstOccurrenceOf ("=", false, false)
                              .trim().unquoted());

            if (f.isDirectory())
                return f;
        }
    }

    return File (fallbackFolder);
}

const char* const* argv = nullptr;
int argc = 0;

File File::getSpecialLocation (const SpecialLocationType type)
{
    switch (type)
    {
        case userHomeDirectory:
        {
            if (const char* homeDir = getenv ("HOME"))
                return File (CharPointer_UTF8 (homeDir));

            if (auto* pw = getpwuid (getuid()))
                return File (CharPointer_UTF8 (pw->pw_dir));

            return {};
        }

        case userDocumentsDirectory:          return resolveXDGFolder ("XDG_DOCUMENTS_DIR", "~/Documents");
        case userMusicDirectory:              return resolveXDGFolder ("XDG_MUSIC_DIR",     "~/Music");
        case userMoviesDirectory:             return resolveXDGFolder ("XDG_VIDEOS_DIR",    "~/Videos");
        case userPicturesDirectory:           return resolveXDGFolder ("XDG_PICTURES_DIR",  "~/Pictures");
        case userDesktopDirectory:            return resolveXDGFolder ("XDG_DESKTOP_DIR",   "~/Desktop");
        case userApplicationDataDirectory:    return resolveXDGFolder ("XDG_CONFIG_HOME",   "~/.config");
        case commonDocumentsDirectory:
        case commonApplicationDataDirectory:  return File ("/opt");
        case globalApplicationsDirectory:     return File ("/usr");

        case tempDirectory:
        {
            if (const char* tmpDir = getenv ("TMPDIR"))
                return File (CharPointer_UTF8 (tmpDir));

            return File ("/tmp");
        }

        case invokedExecutableFile:
            if (argv != nullptr && argc > 0)
                return File (CharPointer_UTF8 (argv[0]));
            // deliberate fall-through...

        case currentExecutableFile:
        case currentApplicationFile:
           #if ! HSTANDALONE_APPLICATION
            return getExecutableFile();
           #endif
            // deliberate fall-through if this is not a shared-library

        case hostApplicationPath:
        {
            const File f ("/proc/self/exe");
            return f.isSymbolicLink() ? f.getLinkedTarget() : getExecutableFile();
        }

        default:
            HAssertfalse; // unknown type?
            break;
    }

    return {};
}

//==============================================================================
bool File::moveToTrash() const
{
    if (! exists())
        return true;

    File trashCan ("~/.Trash");

    if (! trashCan.isDirectory())
        trashCan = "~/.local/share/Trash/files";

    if (! trashCan.isDirectory())
        return false;

    return moveFileTo (trashCan.getNonexistentChildFile (getFileNameWithoutExtension(),
                                                         getFileExtension()));
}

//==============================================================================
static bool isFileExecutable (const String& filename)
{
    statStruct info;

    return stat (filename, info)
            && S_ISREG (info.st_mode)
            && access (filename.toUTF8(), X_OK) == 0;
}

bool Process::openDocument (const String& fileName, const String& parameters)
{
    auto cmdString = fileName.replace (" ", "\\ ", false);
    cmdString << " " << parameters;

    if (cmdString.startsWithIgnoreCase ("file:")
         || File::createFileWithoutCheckingPath (fileName).isDirectory()
         || ! isFileExecutable (fileName))
    {
        StringArray cmdLines;

        for (auto browserName : { "xdg-open", "/etc/alternatives/x-www-browser", "firefox", "mozilla",
                                  "google-chrome", "chromium-browser", "opera", "konqueror" })
        {
            cmdLines.add (String (browserName) + " " + cmdString.trim());
        }

        cmdString = cmdLines.joinIntoString (" || ");
    }

    const char* const argv[4] = { "/bin/sh", "-c", cmdString.toUTF8(), 0 };

    auto cpid = fork();

    if (cpid == 0)
    {
        setsid();

        // Child process
        execve (argv[0], (char**) argv, environ);
        exit (0);
    }

    return cpid >= 0;
}

void File::revealToUser() const
{
    if (isDirectory())
        startAsProcess();
    else if (getParentDirectory().exists())
        getParentDirectory().startAsProcess();
}


