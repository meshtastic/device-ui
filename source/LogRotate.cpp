#include "LogRotate.h"
#include "ILog.h"
#include <ctime>

#define FILE_PREFIX "log_"

LogRotate::LogRotate(fs::FS &fs, const char *logDir, uint32_t maxLen, uint32_t maxSize, uint32_t maxFiles, uint32_t maxFileSize)
    : _fs(fs), numFiles(0), minLogNum(0), currentLogNum(0), currentSize(0), currentCount(0), totalSize(0), rootDirName(logDir),
      c_maxLen(maxLen), c_maxSize(maxSize), c_maxFiles(maxFiles), c_maxFileSize(maxFileSize)
{
    //init();
}

void LogRotate::init(void)
{
    if (!_fs.exists(rootDirName)) {
        _fs.mkdir(rootDirName);
    } else {
        scanLogDir(numFiles, minLogNum, currentLogNum, currentSize, totalSize);
        if (currentSize > c_maxFileSize - c_maxLen) {
            ILOG_DEBUG("currentSize(%d) > c_maxFileSize(%d) - c_maxLen(%d)", currentSize, c_maxFileSize, c_maxLen);
            ILOG_DEBUG("numFiles(%d) > c_maxFiles(%d) || totalSize(%d) >= c_maxSize(%d)", numFiles, c_maxFiles, totalSize,
                       c_maxSize);
            while ((numFiles > c_maxFiles || totalSize >= c_maxSize) && removeLog())
                ;
            numFiles++;
            currentLogNum++;
            currentSize = 0;
        }
    }

    ILOG_INFO("LogRotate: found %d log files using %d bytes (%d%%).", numFiles, totalSize, (totalSize * 100) / c_maxSize);
    if (minLogNum == 0) {
        numFiles = 1;
        minLogNum = 1;
        currentLogNum = 1;
    }
    currentLogName = logFileName(currentLogNum);
    ILOG_INFO("Logging to %s", currentLogName.c_str());
}

/**
 * retrieve entry from list of logFiles
 */

bool LogRotate::read(ILogEntry &entry)
{
    if (!rootDir) {
        rootDir = _fs.open(rootDirName);
        if (!rootDir)
            return false;
    }
    if (!currentFile) {
        currentFile = rootDir.openNextFile();
        if (!currentFile) {
            rootDir.close();
            return false;
        }
        if (currentFile.isDirectory()) {
            currentFile.close();
            return read(entry);
        }
        currentCount++;
        ILOG_DEBUG("-> reading %s (%d bytes)", currentFile.name(), currentFile.size());
    }

    // elegant way to let the logentry do its work it knows best and pass just a temporary function for reading
    if (!entry.deserialize([this](uint8_t *buf, size_t size) { return this->currentFile.read(buf, size); })) {
        currentFile.close();
        return read(entry);
    } else
        return true;
}

bool LogRotate::write(const ILogEntry &entry)
{
    time_t start = millis();
    if (currentSize + entry.size() >= c_maxFileSize || totalSize + entry.size() >= c_maxSize) {
        // log rotation
        ILOG_DEBUG("LogRotation: %d >= %d || %d >= %d", currentSize + entry.size(), c_maxFileSize, totalSize + entry.size(),
                   c_maxSize);
        numFiles++;
        currentSize = 0;
        currentLogNum++;
        currentLogName = logFileName(currentLogNum);
        while ((numFiles >= c_maxFiles || totalSize + entry.size() > c_maxSize) && removeLog())
            ;
    }

    // elegant way to let the logentry do its work it knows best and pass just a temporary function for writing
    File file = _fs.open(currentLogName, FILE_APPEND);
    entry.serialize([&file](const uint8_t *buf, size_t size) { return file.write(buf, size); });
    file.close();

    currentSize += entry.size();
    totalSize += entry.size();
    currentCount = currentLogNum;

    ILOG_DEBUG("LogRotate: %d bytes written in %d ms to %s (%d/%d bytes, total: %d)", entry.size(), millis() - start,
               currentLogName.c_str(), currentSize, c_maxFileSize, totalSize);
    return true;
}

/**
 * remove all log files
 */
bool LogRotate::clear(void)
{
    time_t start = millis();
    File root = _fs.open(rootDirName);

    int count = 0;
    bool error = false;
    File file = root.openNextFile();
    while (file) {
        String name = rootDirName + '/' + file.name();
        if (!file.isDirectory()) {
            file.close();
            if (_fs.remove(name)) {
                count++;
                ILOG_DEBUG("removed %s", name.c_str());
            } else {
                ILOG_ERROR("failed to remove %s!", name.c_str());
                error = true;
            }
        }
        else {
            file.close();
        }
        file = root.openNextFile();
    }
    ILOG_DEBUG("removed %d logs in %d ms", count, millis() - start);

    numFiles = 1;
    minLogNum = 1;
    currentLogNum = 1;
    currentSize = 0;
    totalSize = 0;
    currentLogName = logFileName(currentLogNum);
    return error;
}

/**
 * Return number of log files
 */
uint32_t LogRotate::count(void)
{
    return numFiles;
}

/**
 * Return current log number that is processed for reading
 */
uint32_t LogRotate::current(void)
{
    return currentCount;
}

/**
 * Generate a log file name based on num
 */
String LogRotate::logFileName(uint32_t num)
{
    char filename[40];
    sprintf(filename, "%s/" FILE_PREFIX "%06d.log", rootDirName.c_str(), num);
    return filename;
}

/**
 * remove the oldest log
 */
size_t LogRotate::removeLog(void)
{
    ILOG_DEBUG("removeLog minLogNum=%d, numFiles=%d, totalSize=%d", minLogNum, numFiles, totalSize);
    size_t size = 0;
    if (minLogNum > 0) {
        String log = logFileName(minLogNum);
        File file = _fs.open(log, FILE_READ);
        size = file.size();
        file.close();
        if (_fs.remove(log)) {
            ILOG_DEBUG("removed %s, freeing %d bytes", log.c_str(), size);
            totalSize -= size;
            numFiles--;
        } else {
            ILOG_ERROR("failed to remove %s", log.c_str());
            size = 0;
        }
        minLogNum++;
    }
    return size;
}

/**
 * Scan the log directory (rootDirName) for files and return the following
 * @param num number of logs
 * @param minLog log starting number
 * @param maxLog log ending number
 * @param logSize size of last log
 * @param total total size of all logs
 */
void LogRotate::scanLogDir(uint32_t &num, uint32_t &minLog, uint32_t &maxLog, uint32_t &logSize, uint32_t &total)
{
    num = maxLog = logSize = 0;
    minLog = UINT32_MAX;

    ILOG_DEBUG("scanning log folder %s", rootDirName);
    if (!rootDir)
        rootDir = _fs.open(rootDirName);
    File file = rootDir.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            num++;
            size_t size = file.size();
            total += size;
            ILOG_DEBUG(" %s(%d bytes)", file.name(), size);
            uint32_t logNum = 0;
            if (sscanf(file.name(), "%*s" FILE_PREFIX "%u.log", &logNum) > 0 && logNum > 0) {
                if (logNum < minLog) {
                    minLog = logNum;
                }
                if (logNum > maxLog) {
                    maxLog = logNum;
                    logSize = size;
                }
            }
        }
        file.close();
        file = rootDir.openNextFile();
    }
    rootDir.close();

    if (minLog == UINT32_MAX)
        minLog = 0;
}
