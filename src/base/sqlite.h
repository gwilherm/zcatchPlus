#ifndef BASE_SQLITE_H
#define BASE_SQLITE_H
#include <mutex>
#include <chrono>
#include <engine/external/sqlite/sqlite3.h>
#include "system.h"

/**
 * @brief opens a new database file.
 * @details [long description]
 *
 * @param filename [description]
 * @param ppDb [description]
 *
 * @return [description]
 */
int sqlite_open(const char *filename, sqlite3 **DbHandle);

/**
 * @brief closes database connection.
 * @details
 *
 * @param DbHandle database handle
 * @return [description]
 */
int sqlite_close(sqlite3* DbHandle);

/**
 * @brief Lock a mutex assiciated to a sqlite database
 * @details
 *
 * @param MutexForDB mutex handle for the database
 * @param MiliSecondsToLock miliseconds to lock the database, default = -1
 * 		  meaning, that will be locked as long as the other party is not finished.
 *
 * @return true if locking attempt was successful, false else - NON-blocking
 */
bool sqlite_lock(std::timed_mutex* MutexForDB, int MiliSecondsToLock = -1);


/**
 * @brief Locks a mutex, blocks current thread if the mutex is not available
 * @details [long description]
 *
 * @param MutexForDB std::timed_mutex
 * @return [description]
 */
void sqlite_block_lock(std::timed_mutex* MutexForDB);

/**
 * @brief Unlock given mutex object
 * @details please pass the a pointer to that object.
 *
 * @param MutexForDB [description]
 */
void sqlite_unlock(std::timed_mutex* MutexForDB);

/**
 * @brief checks if given statement can be applied to given database handle.
 * @details given sqlite3_stmt struct is constructed.
 * pzTail returns a pointer to the unused portion of the zSqlQuery statement.
 *
 * @param DbHandle sqlite3 handle to given database.
 * @param zSqlQuery sql query
 * @param ppStatement filled struct
 * @param pzTail pointer to the unused portion of the zSqlQuery
 * @return SQLITE_OK
 * 		   SQLITE_ERROR SQL error or missing database
 * 		   SQLITE_PERM Access permission denied
 * 		   SQLITE_BUSY The database file is locked
 * 		   SQLITE_LOCKED A table in the database is locked
 * 		   SQLITE_CORRUPT The database disk image is malformed
 * 		   SQLITE_CANTOPEN Unable to open the database file
 * 		   SQLITE_EMPTY Database is empty
 * 		   SQLITE_NOTADB File opened that is not a database file
 */
int sqlite_prepare_statement(sqlite3* DbHandle, const char* zSqlQuery, sqlite3_stmt **ppStatement, const char **pzTail);

/**
 * @brief Executes the statement, which has to be prepared before execution.
 * @details
 *
 * @param Statement sqlite3:stmt struct pointer.
 * @return SQLITE_DONE when the query executes successfully
 * 		   SQLITE_BUSY timed out waiting for database to become accessible.
 * 		   SQLITE_ROW when the query executed successfully and the database returns a row with data(while loop through it)
 * 		   Else: Error state or look into sqlite3.h
 */
int sqlite_step(sqlite3_stmt* Statement);

/**
 * @brief Directly executes a query on given database through handle
 * @details Normally called after locking the database mutex.
 *
 * @param DbHandle handle
 * @param SqlStatement statement in sql
 * @param ErrMsg pointer where to write the error message to.
 * @return [description]
 */
int sqlite_exec(sqlite3* DbHandle, const char* SqlStatement, char** ErrMsg);

/**
 * @brief
 * @details [long description]
 *
 * @param DbHandle [description]
 * @param ms [description]
 *
 * @return [description]
 */
int sqlite_busy_timeout(sqlite3* DbHandle, int ms);

/**
 * @brief Bind text to statement using ?1 ?2 ?3 ... in the statement
 * @details [long description]
 *
 * @param SqlStatement Statement object to fill
 * @param pos pisition indicating the digit after the ?(1..)
 * @param value integer to insert.
 * @return [description]
 */
int sqlite_bind_int(sqlite3_stmt* SqlStatement, int pos, int value);

/**
 * @brief Bind text to statement using ?1 ?2 ?3 ... in the statement
 * @details [long description]
 *
 * @param SqlStatement Statement object to fill
 * @param pos pisition indicating the digit after the ?(1..)
 * @param value String to insert.
 * @return [description]
 */
int sqlite_bind_text(sqlite3_stmt* SqlStatement, int pos, const char* value);

int sqlite_bind_double(sqlite3_stmt *SqlStatement, int pos, double value);

/**
 * @brief retrieve error message
 * @details [long description]
 *
 * @param DbHandle [description]
 * @return [description]
 */
const char* sqlite_errmsg(sqlite3* DbHandle);

/**
 * @brief Destroy statement object
 * @details [long description]
 *
 * @param Statement [description]
 * @return [description]
 */
int sqlite_finalize(sqlite3_stmt *Statement);

/**
 * @brief retrieve from current SQLITE_ROW the integer data.
 * @details [long description]
 *
 * @param Statement [description]
 * @param Column [description]
 *
 * @return [description]
 */
int  sqlite_column_int(sqlite3_stmt* Statement, int Column);


/**
 * @brief retrieve from current SQLITE_ROW the text data.
 * @details [long description]
 *
 * @param Statement [description]
 * @param Column [description]
 *
 * @return [description]
 */
const unsigned char * sqlite_column_text(sqlite3_stmt* Statement, int Column);



/**
 * @brief Returns sqlite query from given statement.
 * @details [long description]
 *
 * @param pStmt [description]
 * @return [description]
 */
char *sqlite_expand(sqlite3_stmt *pStmt);


/**
 * @brief frees allocated memory from sqlite error messages
 * @details [long description]
 *
 * @param ErrMsg [description]
 */
void sqlite_free(void *ErrMsg);

#endif
