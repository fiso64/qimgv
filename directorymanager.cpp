#include "directorymanager.h"

DirectoryManager::DirectoryManager() :
    currentPos(-1),
    startDir("")
{
    readSettings();
    setCurrentDir(startDir);
    connect(globalSettings, SIGNAL(settingsChanged()), this, SLOT(applySettingsChanges()));
}

void DirectoryManager::setCurrentDir(QString path) {
    if(currentDir.exists()) {
        if(currentDir.path() != path) {
            changePath(path);
        }
    }
    else changePath(path);
}

void DirectoryManager::changePath(QString path) {
    currentDir.setPath(path);
    if(currentDir.isReadable())
        globalSettings->s.setValue("lastDir", path);
    currentDir.setNameFilters(filters);
    currentPos = -1;
    fileNameList = currentDir.entryList();
    emit directoryChanged(path);
}

// full paths array
QStringList DirectoryManager::getFileList() {
    QStringList files = currentDir.entryList();
    QString dirPath = currentDir.absolutePath() + "/";
    for(int i=0; i<fileNameList.length(); i++) {
        files.replace(i, dirPath+files.at(i));
    }
    return files;
}

QFileInfoList DirectoryManager::getFileInfoList() {
    return currentDir.entryInfoList();
}

bool DirectoryManager::existsInCurrentDir(QString file) {
    return fileNameList.contains(file, Qt::CaseInsensitive);
}

void DirectoryManager::setFile(QString path) {
    FileInfo *info = loadInfo(path);
    setCurrentDir(info->getDirectoryPath());
    currentPos = fileNameList.indexOf(info->getFileName());
    globalSettings->s.setValue("lastPosition", currentPos);
    //return info;
}

void DirectoryManager::setCurrentPos(int pos) {
    currentPos = pos;
    globalSettings->s.setValue("lastPosition", currentPos);
}

bool DirectoryManager::isValidFile(QString path) {
    QFile file(path);
    QString extension = "nope";
    if(path.contains('.')) {
        extension = "*.";
        QStringList dotSplit = path.split(".",QString::SkipEmptyParts);
        extension.append(dotSplit.at(dotSplit.length()-1));
    }
    if( file.exists() && filters.contains(extension, Qt::CaseInsensitive) ) {
        return true;
    }
    else return false;
}

QString DirectoryManager::currentFileName() {
    return fileNameList.at(currentPos);
}

QString DirectoryManager::currentDirectory() {
    return currentDir.absolutePath();
}

int DirectoryManager::currentFilePos() {
    return currentPos;
}

bool DirectoryManager::containsFiles() {
    return !fileNameList.empty();
}

void DirectoryManager::readSettings() {
    startDir = globalSettings->s.value("lastDir",
                                       currentDir.homePath()).toString();
    filters = globalSettings->supportedFormats();
    currentDir.setNameFilters(filters);
    switch(globalSettings->sortingMode()) {
        case 1: currentDir.setSorting(QDir::Name | QDir::Reversed); break;
        case 2: currentDir.setSorting(QDir::Time); break;
        case 3: currentDir.setSorting(QDir::Time | QDir::Reversed); break;
        default: currentDir.setSorting(QDir::Name); break;
    }
}

void DirectoryManager::applySettingsChanges() {
    QDir::SortFlags flags;
    switch(globalSettings->sortingMode()) {
        case 1: flags = QDir::SortFlags(QDir::Name | QDir::Reversed); break;
        case 2: flags = QDir::SortFlags(QDir::Time); break;
        case 3: flags = QDir::SortFlags(QDir::Time | QDir::Reversed); break;
        default: flags = QDir::SortFlags(QDir::Name); break;
    }
    if(currentDir.sorting() != flags) {
        currentDir.setSorting(flags);
        fileNameList = currentDir.entryList();
        emit directorySortingChanged(); //for now, sorting dir will cause full cache reload TODO
    }
}

int DirectoryManager::nextPos() {
    if(currentPos<0) {
        currentPos=0;
    } else if(++currentPos>=fileNameList.length()) {
        currentPos = fileNameList.length()-1;
    }
    return currentPos;
}

int DirectoryManager::prevPos() {
    if(--currentPos<0) {
        currentPos = 0;
    }
    return currentPos;
}

int DirectoryManager::peekNext(int offset) {
    int pos = currentPos+offset;
    if(pos>=fileNameList.length()) {
        pos = fileNameList.length()-1;
    }
    return pos;
}

int DirectoryManager::peekPrev(int offset) {
    int pos = currentPos-offset;
    if(pos<0) {
        pos = 0;
    }
    return pos;
}

int DirectoryManager::lastPos() {
    return fileNameList.length()-1;
}

FileInfo* DirectoryManager::loadInfo(QString path) {
    FileInfo *info = new FileInfo(path);
    return info;
}
