#include "AltQNativeHelper.h"

#ifdef JUCE_WINDOWS

#include "windows.h"
#include "ShlObj.h"

Image Icon2Image(HICON hIcon)
{
    BITMAP bm;
    ICONINFO iconInfo;
    GetIconInfo(hIcon, &iconInfo);
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
    int width = bm.bmWidth;
    int height = bm.bmHeight;
    int bytesPerScanLine = (width * 3 + 3) & 0xFFFFFFFC;
    int size = bytesPerScanLine * height;
    BITMAPINFO infoheader;
    infoheader.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoheader.bmiHeader.biWidth = width;
    infoheader.bmiHeader.biHeight = height;
    infoheader.bmiHeader.biPlanes = 1;
    infoheader.bmiHeader.biBitCount = 24;
    infoheader.bmiHeader.biCompression = BI_RGB;
    infoheader.bmiHeader.biSizeImage = size;
    // allocate Memory for Icon RGB data plus Icon mask plus ARGB buffer for the resulting image
    MemoryBlock* pMemBlock = new MemoryBlock((size * 2 + height*width * 4)*sizeof(uint8));
    uint8* pixelsIconRGB = (uint8*)pMemBlock->getData();
    uint8* alphaPixels = (uint8*)(pixelsIconRGB + size);
    uint32* imagePixels = (uint32*)(alphaPixels + size);
    HDC hDC = CreateCompatibleDC(NULL);
    // Get Icon RGB data
    HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, (HGDIOBJ)iconInfo.hbmColor);
    if (!GetDIBits(hDC, iconInfo.hbmColor, 0, height, (LPVOID)pixelsIconRGB, &infoheader, DIB_RGB_COLORS))
        return Image();
    SelectObject(hDC, hBmpOld);
    // now get the mask
    if (!GetDIBits(hDC, iconInfo.hbmMask, 0, height, (LPVOID)alphaPixels, &infoheader, DIB_RGB_COLORS))
        return Image();
    int x = 0;
    int currentSrcPos = 0;
    int currentDestPos = 0;
    int linePosSrc = 0;
    int linePosDest = 0;
    int lsSrc = width * 3;
    int vsDest = height - 1;

    Image pImage(Image::ARGB, width, height, true);
    for (int y = 0; y < height; y++)
    {
        linePosSrc = (vsDest - y)*lsSrc;
        linePosDest = y*width;
        for (x = 0; x < width; x++)
        {
            //pixels from Icon are stored in BGR vertical and horizontal flipped order
            currentDestPos = linePosDest + x;
            currentSrcPos = linePosSrc + x * 3;
            // BGR -> ARGB
            //((alphaPixels[currentSrcPos]?0:255) << 0x18))) & 0xffffffffL)
            pImage.setPixelAt(x, y, Colour((pixelsIconRGB[currentSrcPos + 2] /*Red*/), (pixelsIconRGB[currentSrcPos + 1] /*Green*/), pixelsIconRGB[currentSrcPos] /*Blue*/, (juce::uint8)(255 - alphaPixels[currentSrcPos])));
        }
    }
    deleteAndZero(pMemBlock);
    DeleteDC(hDC);
    return pImage;
}


Image AltQNative::GetExcuteFileImage(const File& path)
{
    SHFILEINFO FileInfo;
    DWORD_PTR dwRet = ::SHGetFileInfoW(path.getFullPathName().toWideCharPointer(), 0, &FileInfo, sizeof(SHFILEINFO), SHGFI_ICON);
    return dwRet ? Icon2Image(FileInfo.hIcon) : Image();
}

#endif