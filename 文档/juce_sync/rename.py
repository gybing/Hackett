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
    renameFileLoop(destFoldername, "*", "1")
    renameDir(destFoldername, "*", "1")

# 主程序入口
if  __name__ =="__main__":

    destDir = ".\\"

    syncDir(destDir,destDir)
    #removeFile(destDir + "\\modules","module_info")
    
    #syncDir("D:\\SRC\\Hackett\\trunk\\projects\\AutoQ\\AutoQ\\Source",destDir + "\\Projects\\Source")

    print "all sucess" 