#coding:utf-8
import os 
import os.path 
import shutil 
import time, datetime
import re

# 第一个就是把某一目录下的所有文件复制到指定目录中：
def copyFiles(sourceDir,  targetDir): 
    if sourceDir.find(".svn") > 0: 
        return 
    for file in os.listdir(sourceDir): 
        sourceFile = os.path.join(sourceDir,  file) 
        targetFile = os.path.join(targetDir,  file) 
        if os.path.isfile(sourceFile): 
            if not os.path.exists(targetDir):  
                os.makedirs(targetDir)  
            if not os.path.exists(targetFile) or(os.path.exists(targetFile) and (os.path.getsize(targetFile) != os.path.getsize(sourceFile))):  
                    open(targetFile, "wb").write(open(sourceFile, "rb").read()) 
        if os.path.isdir(sourceFile): 
            First_Directory = False 
            copyFiles(sourceFile, targetFile)

# 删除一级目录下的所有文件：
def removeFileInFirstDir(targetDir): 
    for file in os.listdir(targetDir): 
        targetFile = os.path.join(targetDir,  file) 
        if os.path.isfile(targetFile): 
            os.remove(targetFile)

# 复制一级目录下的所有文件到指定目录：
def coverFiles(sourceDir,  targetDir): 
        for file in os.listdir(sourceDir): 
            sourceFile = os.path.join(sourceDir,  file) 
            targetFile = os.path.join(targetDir,  file) 
            #cover the files 
            if os.path.isfile(sourceFile): 
                open(targetFile, "wb").write(open(sourceFile, "rb").read())

# 复制指定文件到目录：
def moveFileto(sourceDir,  targetDir): 
    shutil.copy(sourceDir,  targetDir)

# 往指定目录写文本文件：
def writeVersionInfo(targetDir): 
    open(targetDir, "wb").write("Revison:")

#
def renameDir(sourceDir, oldId, newId):
    for parent, dirnames, filenames in os.walk(sourceDir):    
        for dirname in dirnames:
            renameDir(os.path.join(parent, dirname), oldId, newId)
            if dirname.find(oldId)!=-1:
                newName = dirname.replace(oldId, newId)
                print(dirname, "---->", newName)
                os.rename(os.path.join(parent, dirname), os.path.join(parent, newName))

#
def renameFile(sourceDir, oldId, newId):
    for parent, dirnames, filenames in os.walk(sourceDir):
        for filename in filenames:  #输出文件信息
            if filename.find(oldId)!=-1:
                newName = filename.replace(oldId, newId)
                print(filename, "---->", newName)
                os.rename(os.path.join(parent, filename), os.path.join(parent, newName))

#
def renameFileLoop(sourceDir, oldId, newId):
    for parent, dirnames, filenames in os.walk(sourceDir):
        for dirname in dirnames:
            renameFile(os.path.join(parent, dirname), oldId, newId)

def removeFile(sourceDir, fileName):
    for parent, dirnames, filenames in os.walk(sourceDir):    
        for filename in filenames:  #输出文件信息
            if filename == fileName:
                os.remove(filename)

# 返回当前的日期，以便在创建指定目录的时候用：
def getCurTime(): 
    nowTime = time.localtime() 
    year = str(nowTime.tm_year) 
    month = str(nowTime.tm_mon) 
    if len(month) < 2: 
        month = '0' + month 
    day =  str(nowTime.tm_yday) 
    if len(day) < 2: 
        day = '0' + day 
    return (year + '-' + month + '-' + day)

def listFiles(dirPath):
    fileList=[]
    for root,dirs,files in os.walk(dirPath):
        for fileObj in files:
            fileList.append(os.path.join(root,fileObj))
    return fileList

def ReplaceContent(sourceDir, textsrc, textdst):
    regex = ur'FUNC_SYS_ADD_ACCDETAIL'
    fileList = listFiles(sourceDir)
    for fileObj in fileList:
        f = open(fileObj,'r+')
        if f:
            all_the_lines=f.readlines()
            f.seek(0)
            f.truncate()
            for line in all_the_lines:
                oldline = line
                f.write(line.replace(textsrc,textdst))
            #f.close()  

def syncDir(sourceFoldername, destFoldername):
    os.system("rd " + destFoldername +" /s /q")
    copyFiles(sourceFoldername, destFoldername)
    syncReplaceOther(destFoldername)
    
def syncReplaceOther(destFoldername):

    ReplaceContent(destFoldername, "juce_getTypefaceForFont","HGetTypefaceForFont")

    ReplaceContent(destFoldername, "juce_InterlockedExchange","HInterlockedExchange")
    ReplaceContent(destFoldername, "juce_InterlockedIncrement","HInterlockedIncrement")
    ReplaceContent(destFoldername, "juce_InterlockedDecrement","HInterlockedDecrement")
    ReplaceContent(destFoldername, "juce_InterlockedExchangeAdd","HInterlockedExchangeAdd")
    ReplaceContent(destFoldername, "juce_InterlockedCompareExchange","HInterlockedCompareExchange")
    ReplaceContent(destFoldername, "juce_InterlockedCompareExchange64","HInterlockedCompareExchange64")
    ReplaceContent(destFoldername, "juce_MemoryBarrier","HMemoryBarrier")
    ReplaceContent(destFoldername, "Process::isRunningUnderDebugger","Process::HRunningUnderDebugger")
    ReplaceContent(destFoldername, "JUCE_CALLTYPE isRunningUnderDebugger","HCALLTYPE HRunningUnderDebugger")
    ReplaceContent(destFoldername, "JUCE_CALLTYPE","HCALLTYPE")
    ReplaceContent(destFoldername, "JUCE_API","HAPI")
    ReplaceContent(destFoldername, "Projucer","Projector")

    ReplaceContent(destFoldername, "juce_", "")
    ReplaceContent(destFoldername, "JUCE_", "H")
    ReplaceContent(destFoldername, "JUCE", "H")
    ReplaceContent(destFoldername, "jucer_", "")
    ReplaceContent(destFoldername, "JUCER_", "H")
    ReplaceContent(destFoldername, "JUCER", "H")
    ReplaceContent(destFoldername, "juce","H")
    
    renameFileLoop(destFoldername, "juce_", "")
    renameFileLoop(destFoldername, "Projucer", "Projector")
    renameFileLoop(destFoldername, "jucer_", "")
    renameDir(destFoldername, "juce_", "")
    renameDir(destFoldername, "jucer_", "")
    renameDir(destFoldername, "Projucer", "Projector")

# 主程序入口
if  __name__ =="__main__":

    juceSourceDir = "D:\\SRC\\Hackett\\JUCE"
    destDir = "D:\\SRC\\Hackett\\HFoundation\\JUCE"

    #os.system("git -C " + juceSourceDir + " pull")

    syncDir(juceSourceDir + "\\modules",destDir + "\\modules")
    syncDir(juceSourceDir + "\\examples\\Assets", destDir + "\\examples\\Assets")
    syncDir(juceSourceDir + "\\examples\\Audio", destDir + "\\examples\\Audio")
    syncDir(juceSourceDir + "\\examples\\BLOCKS", destDir + "\\examples\\BLOCKS")
    syncDir(juceSourceDir + "\\examples\\DemoRunner", destDir + "\\examples\\DemoRunner")
    syncDir(juceSourceDir + "\\examples\\DSP", destDir + "\\examples\\DSP")
    syncDir(juceSourceDir + "\\examples\\GUI", destDir + "\\examples\\GUI")
    syncDir(juceSourceDir + "\\examples\\Plugins", destDir + "\\examples\\Plugins")
    syncDir(juceSourceDir + "\\examples\\Utilities", destDir + "\\examples\\Utilities")

    syncDir(juceSourceDir + "\\extras\\AudioPerformanceTest", destDir + "\\extras\\AudioPerformanceTest")
    syncDir(juceSourceDir + "\\extras\\AudioPluginHost", destDir + "\\extras\\AudioPluginHost")
    syncDir(juceSourceDir + "\\extras\\BinaryBuilder", destDir + "\\extras\\BinaryBuilder")
    syncDir(juceSourceDir + "\\extras\\BLOCKS", destDir + "\\extras\\BLOCKS")
    syncDir(juceSourceDir + "\\extras\\NetworkGraphicsDemo", destDir + "\\extras\\NetworkGraphicsDemo")
    syncDir(juceSourceDir + "\\extras\\Projucer", destDir + "\\extras\\Projucer")
    syncDir(juceSourceDir + "\\extras\\UnitTestRunner", destDir + "\\extras\\UnitTestRunner")
    syncDir(juceSourceDir + "\\extras\\WindowsDLL", destDir + "\\extras\\WindowsDLL")


    #removeFile(destDir + "\\modules","module_info")
    
    #syncDir("D:\\SRC\\Hackett\\trunk\\projects\\AutoQ\\AutoQ\\Source",destDir + "\\Projects\\Source")

    print "all sucess" 