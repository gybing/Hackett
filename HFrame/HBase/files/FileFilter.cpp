

FileFilter::FileFilter (const String& filterDescription)
    : description (filterDescription)
{
}

FileFilter::~FileFilter()
{
}

const String& FileFilter::getDescription() const noexcept
{
    return description;
}


