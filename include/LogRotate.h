#pragma once

#include "FS.h"
#include "ILogEntry.h"
#include <stdint.h>

/**
 * Generic LogRotate class that writes log-rotation like files into (arduino) FS storage file system
 * @param fs arduino file system FS/LittleFS or derived classes
 * @param logDir directory to store the logs (absolute path)
 * @param maxLen the maximum length of the variable log entry length
 * The maximum storage is limited by:
 * @param maxSize the total storage in bytes (default is 200kB)
 * @param maxFiles number of log files (default is 50)
 * @param maxFileSize per log file (default size is 4000 bytes to fit into a physical block
 *                    including fs descriptor data)
 *
 * If the maximum storage is exceeded then old files are deleted to fit the new log entry.
 * Note: for performance reasons the logs are not renumbered
 */
class LogRotate
{
  public:
    LogRotate(fs::FS &fs, const char *logDir, uint32_t maxLen,
              //               uint32_t maxSize = 204800, uint32_t maxFiles = 50, uint32_t maxFileSize = 4000);
              uint32_t maxSize = 600, uint32_t maxFiles = 10, uint32_t maxFileSize = 300);

    // initialize the log directory
    void init(void);
    // write a log entry to fs
    bool write(const ILogEntry &entry);
    // read the next log entry from fs
    bool read(ILogEntry &entry);
    // remove all logs from fs
    bool clear(void);
    // request log count
    uint32_t count(void);
    // request current log number
    uint32_t current(void);

  private:
    LogRotate(const LogRotate &) = delete;
    LogRotate &operator=(const LogRotate &) = delete;

    // create filename from number
    String logFileName(uint32_t num);
    // remove oldest log and return freed size
    size_t removeLog(void);
    // scan all files in logdir
    void scanLogDir(uint32_t &num, uint32_t &minLog, uint32_t &maxLog, uint32_t &size, uint32_t &total);

    fs::FS &_fs;
    uint32_t numFiles;      // number of log files
    uint32_t minLogNum;     // logfile with smallest number
    uint32_t currentLogNum; // current log number
    uint32_t currentSize;   // size of current written log file
    uint32_t currentCount;  // currently read log
    uint32_t totalSize;     // size of all logs

    File rootDir;          // directory (for reading logs)
    File currentFile;      // current file (when reading)
    String rootDirName;    // path of log directory
    String currentLogName; // current log file name (when writing)

    const uint32_t c_maxLen;      // maximum size a single log entry could be
    const uint32_t c_maxSize;     // max storage size in bytes (default is 100kB)
    const uint32_t c_maxFiles;    // max log files number (default is 30)
    const uint32_t c_maxFileSize; // max file size per log file
};