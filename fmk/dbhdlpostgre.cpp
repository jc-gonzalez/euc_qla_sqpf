/******************************************************************************
 * File:    dbhdlpostgre.cpp
 *          This file is part of QLA Processing Framework
 *
 * Domain:  QPF.libQPF.dbhdlpostgre
 *
 * Version:  2.0
 *
 * Date:    2015/07/01
 *
 * Author:   J C Gonzalez
 *
 * Copyright (C) 2015-2018 Euclid SOC Team @ ESAC
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Implement MessageHandler class
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#include <iterator>
#include <arpa/inet.h>
#include <fstream>

#include "dbhdlpostgre.h"

#include "tools.h"
#include "str.h"
#include "json.hpp"
using json = nlohmann::json;

//----------------------------------------------------------------------
// Constructor: DBHdlPostgreSQL
//----------------------------------------------------------------------
DBHdlPostgreSQL::DBHdlPostgreSQL(ProcessingNetwork & _net, Logger & _logger)
    : net(_net), logger(_logger), conn(0), res(0)
{
}

//----------------------------------------------------------------------
// Method: openConnection
// Open a connection to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::openConnection(const char * data)
{
    // Generate new connection to database
    const char * connStr = data;
    if (data == 0) {
        if (!connectionParamsSet) {         
            logger.fatal("DB Connection parameters not set!");
        } else {
            setDbConnectionParams("host=" + getDbHost() +
                                  " port=" + getDbPort() +
                                  " dbname=" + getDbName() +
                                  " user=" + getDbUser() +
                                  " password=" + getDbPasswd());
            connStr = getDbConnectionParams().c_str();
            //logger.info("Connection parameters set to: %s", connStr);
        }
    }
    if (conn != 0) {
        logger.warn("Connection will be re-open at thread %ld", pthread_self());
        PQfinish(conn);
	conn = 0;
    }
    conn = PQconnectdb(connStr);
    if (PQstatus(conn) != CONNECTION_OK) {
        logger.fatal("Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
    }
    return true;
}

//----------------------------------------------------------------------
// Method: closeConnection
// Closes the connection to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::closeConnection(const char * data)
{
    UNUSED(data);

    PQfinish(conn);
    conn = 0;

    return true;
}

//----------------------------------------------------------------------
// Method: storeProducts
// Saves a set of products' metadata to the database
//----------------------------------------------------------------------
int DBHdlPostgreSQL::storeProducts(ProductMetaList & prodList)
{
    bool result;
    int nInsProd = 0;
    std::stringstream ss;

    for (auto & m : prodList) {
        // Get report content
        std::string prodUrl(m["url"]);
        std::string repFile(str::mid(prodUrl, 7));
        std::string repContent("{}");
        if (str::right(repFile, 4) == "json") {
            std::ifstream t(repFile);
            std::stringstream buffer;
            buffer << t.rdbuf();
            repContent = buffer.str();
            if (repContent.empty()) { repContent = "{}"; }
        }
 
        ss.str("");
        ss << "INSERT INTO products_info "
           << "(product_id, product_type, product_status_id, "
            "product_version, product_size, creator_id, "
           << "obs_id, soc_id, "
           << "instrument_id, obsmode_id, signature, start_time, "
            "end_time, registration_time, url, report) "
           << "VALUES ("
           << str::quoted(m["productId"]) << ", "
           << str::quoted(m["productType"]) << ", "
           << str::quoted("OK") << ", "
           << str::quoted(m["productVersion"]) << ", "
           << m["size"] << ", "
           << str::quoted("SOC_QLA_TEST") << ", "
           << str::quoted(m["obsIdStr"]) << ", "
           << str::quoted(m["obsIdStr"]) << ", "
           << str::quoted(m["instrument"]) << ", "
           << str::quoted(m["obsMode"]) << ", "
           << str::quoted(m["signature"]) << ", "
           << str::quoted(str::tagToTimestamp(m["startTime"])) << ", "
           << str::quoted(str::tagToTimestamp(m["endTime"])) << ", "
           << str::quoted(str::tagToTimestamp(timeTag())) << ", "
           << str::quoted(prodUrl) << ", "
           << str::quoted(repContent) << "::json) "
           << "ON CONFLICT (product_id) DO UPDATE "
           << "SET report=" << str::quoted(repContent) << "::json;";
        try { result = runCmd(ss.str()); }
        catch(...) { logger.error("Cannot store product!"); }
        PQclear(res);
        nInsProd++;
    }


    result = runCmd("refresh materialized view products_info_filter;");
    PQclear(res);
    UNUSED(result);

    return nInsProd;
}

//----------------------------------------------------------------------
// Method: retrieveProducts
// Retrieves a set of products from the database, according to
// pre-defined criteria
//----------------------------------------------------------------------
int  DBHdlPostgreSQL::retrieveProducts(ProductMetaList & prodList,
                                       std::string criteria)
{
    bool result;
    std::string cmd(
                "SELECT p.product_id, p.product_type, s.status_desc, p.product_version, "
                "p.product_size, c.creator_desc, i.instrument, m.obsmode_desc, "
                "p.start_time, p.end_time, p.registration_time, p.url "
                "FROM (((products_info AS p "
                "  INNER JOIN creators AS c "
                "  ON p.creator_id = c.creator_id) "
                "    INNER JOIN instruments AS i "
                "    ON p.instrument_id = i.instrument_id) "
                "      INNER JOIN observation_modes AS m "
                "      ON p.obsmode_id = m.obsmode_id) "
                "        INNER JOIN product_status AS s "
                "        ON p.product_status_id = s.product_status_id "
                "ORDER BY p.id ");
    cmd += criteria + ";";

    try { result = runCmd(cmd); } catch(...) { throw; }

    // Transfer the data to the product list argument
    prodList.clear();
    ProductMeta m;
    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        m["productId"]      = std::string(PQgetvalue(res, i, 0));
        m["productType"]    = std::string(PQgetvalue(res, i, 1));
        m["productStatus"]  = std::string(PQgetvalue(res, i, 2));
        m["productVersion"] = std::string(PQgetvalue(res, i, 3));
        m["productSize"]    = *((int*)(PQgetvalue(res, i, 4)));
        m["creator"]        = std::string(PQgetvalue(res, i, 5));
        m["instrument"]     = std::string(PQgetvalue(res, i, 6));
        m["signature"]      = std::string(PQgetvalue(res, i, 7));
        m["startTime"]      = std::string(PQgetvalue(res, i, 8));
        m["endTime"]        = std::string(PQgetvalue(res, i, 9));
        m["regTime"]        = std::string(PQgetvalue(res, i, 10));
        m["url"]            = std::string(PQgetvalue(res, i, 11));
        prodList.push_back(m);
    }
    PQclear(res);

    UNUSED(result);

    return nRows;
}

//----------------------------------------------------------------------
// Method: storeTask
// Stores a task information to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeTask(TaskInfo & task)
{
    bool result = true;

    std::string registrationTime(tagToTimestamp(preciseTimeTag()));
    std::stringstream ss;

    ss.str("");
    ss << "INSERT INTO tasks_info ";
    ss << "(task_id, task_status_id, task_progress, task_exitcode, ";
    ss << "task_path, task_size, registration_time, start_time, task_info, task_data) ";
    ss << "VALUES (";
    ss << str::quoted(task["taskName"].get<std::string>()) << ", ";
    ss << task["taskStatus"].get<int>() << ", ";
    ss << 0 << ", ";
    ss << task["taskExitCode"].get<int>() << ", ";
    ss << str::quoted(task["taskPath"].get<std::string>()) << ", ";
    ss << 0 << ", ";
    ss << str::quoted(registrationTime) << ", ";
    ss << str::quoted(task["taskStart"].get<std::string>()) << ", ";
    ss << str::quoted(task.dump()) << "::json, ";
    ss << str::quoted(task["taskData"].dump()) << "::json);";
        
    try { result = runCmd(ss.str()); } catch(...) { throw; }

    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: checkTask
// Returns true if an entry for a task exists in the DB
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::checkTask(std::string taskId)
{
    bool result;
    std::string cmd("SELECT t.task_id FROM tasks_info AS t "
                    "WHERE t.task_id=" + str::quoted(taskId) + ";");

    try { result = runCmd(cmd); } catch(...) { throw; }

    result &= (PQntuples(res) > 0);
    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: updateTask
// Updates the information for a given task in the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::updateTask(TaskInfo & task)
{
    bool result = true;
    bool mustUpdate = true;

    std::string taskName = task["taskName"].get<std::string>().substr(15);
    json taskData = task["taskData"];
    std::string id = taskData["Id"];

    std::vector<std::string> updates {eqkv("task_id", id)};
        
    if (checkTask(taskName)) {
        // Present, so this is task registered with old name as ID,
        // that must change to the actual ID
        result = updateTable("tasks_info",
                             eqkv("task_id",taskName),
                             updates);
        // Once changed the task_id, the update must still be done
    }

    if (result) {
	int iTaskStatus = task["taskStatus"].get<int>();
        TaskStatus taskStatus(TaskStatusEnum(iTaskStatus));
        updates.clear();
        updates.push_back(eqkv("task_status_id", iTaskStatus));
        updates.push_back(eqkv("task_progress", task["taskProgress"].get<int>()));
        
        bool isFinished = ((iTaskStatus == TASK_STOPPED) || (iTaskStatus == TASK_FINISHED) || 
			   (iTaskStatus == TASK_FAILED) || (iTaskStatus == TASK_UNKNOWN_STATE));

        //if (isFinished) {
	updates.push_back(eqkv("start_time", task["taskStart"].get<std::string>()));
	updates.push_back(eqkv("end_time", task["taskEnd"].get<std::string>()));
	updates.push_back(eqkv("task_path", task["taskPath"].get<std::string>())); 
	updates.push_back(eqkv("task_data", task["taskData"]));
	updates.push_back(eqkv("task_info", task.dump())); 
	//}

        result &= updateTable("tasks_info", eqkv("task_id", id), updates);
    }
     
    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: retrieveTask
// Retrieves a task information from the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveTask(TaskInfo & task)
{
    UNUSED(task);

    return false;
}

//----------------------------------------------------------------------
// Method: storeTask
// Stores a task information to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::saveTaskStatusSpectra(std::string & agName, TskStatSpectra & tss)
{
    bool result = true;

    std::stringstream ss;
    ss.str("");
    ss << "INSERT INTO task_status_spectra "
       << "(agent_id, running, waiting, paused, stopped, failed, finished, total) "
       << "VALUES ("
       << str::quoted(agName) << ", "
       << tss.running << ", "
       << tss.scheduled << ", "
       << tss.paused << ", "
       << tss.stopped << ", "
       << tss.failed << ", "
       << tss.finished << ", "
       << tss.total << ") "
       << " ON CONFLICT(agent_id) DO UPDATE SET "
       << "running = " << tss.running << ", "
       << "waiting = " << tss.scheduled << ", "
       << "paused = " << tss.paused << ", "
       << "stopped = " << tss.stopped << ", "
       << "failed = " << tss.failed << ", "
       << "finished = " << tss.finished << ", "
       << "total = " << tss.total << ";";
    
    try { result = runCmd(ss.str()); } catch(...) { throw; }

    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: storeState
// Stores a new state to the database, for a given node and session
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeState(std::string session, std::string node, std::string newState)
{
    bool result = true;

    std::string registrationTime(tagToTimestamp(preciseTimeTag()));
    std::string cmd("INSERT INTO qpfstates (timestmp, sessionname, nodename, state) VALUES (" +
                    str::quoted(registrationTime) + ", " +
                    str::quoted(session) + ", " +
                    str::quoted(node) + ", " +
                    str::quoted(newState) + ");");

    try { result = runCmd(cmd); } catch(...) { throw; }

    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: getCurrentState
// Gets the list of nodes with its states for a given session
//----------------------------------------------------------------------
std::vector< std::vector<std::string> > DBHdlPostgreSQL::getCurrentState(std::string session)
{
    bool result = true;
    std::vector< std::vector<std::string> > table;

    std::string cmd("SELECT nodename, state FROM qpfstates "
                    "WHERE sessionname = " + str::quoted(session) +
                    "ORDER BY qpfstate_id;");
    try {
        result = runCmd(cmd);
        if (result) { result = fillWithResult(table); }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return table;
}

//----------------------------------------------------------------------
// Method: getLatestState
// Gets the last registered session name and state of the Event Manager
//----------------------------------------------------------------------
std::pair<std::string, std::string> DBHdlPostgreSQL::getLatestState()
{
    bool result = true;
    std::pair<std::string, std::string> p;

    std::string cmd("SELECT sessionname, state FROM qpfstates "
                    "WHERE nodename = 'EvtMng' "
                    "ORDER BY qpfstate_id DESC LIMIT 1;");
    try {
        result = runCmd(cmd);
        if (result) {
            p.first   = std::string(PQgetvalue(res, 0, 0));
            p.second  = std::string(PQgetvalue(res, 0, 1));
        }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return p;
}

//----------------------------------------------------------------------
// Method: addICommand
// Stores a new internal command into the database
//----------------------------------------------------------------------
void DBHdlPostgreSQL::addICommand(std::string target,
                                  std::string source,
                                  std::string content)
{
    bool result = true;

    std::string registrationTime(tagToTimestamp(preciseTimeTag()));
    std::string cmd("INSERT INTO icommands "
                    "(cmd_date, cmd_source, cmd_target, cmd_executed, cmd_content) "
                    "VALUES (" + str::quoted(registrationTime) +
                    ", " + str::quoted(source) +
                    ", " + str::quoted(target) +
                    ", false, " + str::quoted(content) + ");");
    try {
        result = runCmd(cmd);
    } catch(...) {
        throw;
    }

    PQclear(res);
    UNUSED(result);
}

//----------------------------------------------------------------------
// Method: getICommand
// Stores a new state to the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::getICommand(std::string target,
                                  int & id,
                                  std::string & source,
                                  std::string & content)
{
    bool result = true;

    std::string cmd("SELECT cmd.id, cmd.cmd_source, cmd.cmd_content "
                    " FROM icommands cmd "
                    " WHERE cmd.cmd_target = " + str::quoted(target) +
                    " AND cmd.cmd_executed = false "
                    " AND cmd_date + '15 sec'::interval > current_timestamp "
                    " ORDER BY cmd.id LIMIT 1;");

    try {
        result = runCmd(cmd);
        result &= (PQntuples(res) > 0);
        if (result) {
            id      = atoi(PQgetvalue(res, 0, 0));
            source  = std::string(PQgetvalue(res, 0, 1));
            content = std::string(PQgetvalue(res, 0, 2));
        }
    } catch(...) {
        result = false;
    }

    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: markICommandAsDone
// Sets the executed flag to true
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::markICommandAsDone(int id)
{
    bool result = true;
    std::string cmd("UPDATE icommands SET cmd_executed = true "
                    " WHERE id = " + str::quoted(str::toStr<int>(id)) + ";");

    try { result = runCmd(cmd); } catch(...) { result = false; }

    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: removeICommand
// Remove the command using its id
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::removeICommand(int id)
{
    bool result = true;
    std::string cmd("DELETE FROM icommands "
                    " WHERE id = " + str::quoted(str::toStr<int>(id)) + ";");

    try { result = runCmd(cmd); } catch(...) { result = false; }

    return result;
}

//----------------------------------------------------------------------
// Method: storeMsg
// Stores a message into the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeMsg(std::string from,
                               MessageString & msg,
                               bool isBroadcast)
{
    bool result = true;
    /*
    if (from.empty()) { from = "all"; }

    MessageBase m(msg);
    std::string cmd("INSERT INTO transmissions "
                    "(msg_date, msg_from, msg_to, msg_type, msg_bcast, msg_content) "
                    "VALUES (" +
                    str::quoted(str::tagToTimestamp(timeTag())) + ", " +
                    str::quoted(from) + ", " +
                    str::quoted(m["header.target"]) + ", " +
                    str::quoted(m["header.type"]) + ", " +
                    str::quoted(isBroadcast ? "Y" : "N") + ", " +
                    str::quoted(msg) + ")");

    try { result = runCmd(cmd); } catch(...) { throw; }

    */
    return result;
}

//----------------------------------------------------------------------
// Method: retrieveMsgs
// Retrieves a set of messages from the database, according to
// pre-defined criteria
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveMsgs(std::vector<std::pair<std::string,
                                   MessageString> > & msgList,
                                   std::string criteria)
{
    bool result = true;

    std::string cmd("SELECT m.msg_date, m.msg_content "
                    "FROM transmissions as m "
                    "ORDER BY m.msg_date ");
    cmd += criteria + ";";

    try { result = runCmd(cmd); } catch(...) { throw; }

    // Transfer the data to the msgList argument
    msgList.clear();
    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        std::string msg_date(PQgetvalue(res, i, 1));
        MessageString m = std::string(PQgetvalue(res, i, 2));
        msgList.push_back(std::make_pair(msg_date, m));
    }
    PQclear(res);

    return result;
}

//----------------------------------------------------------------------
// Method: getTable
// Get the content (all the records) from a given table
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::getTable(std::string tName,
                               std::vector< std::vector<std::string> > & table)
{
    return  (runCmd(std::string("SELECT * FROM " + tName)) && fillWithResult(table));
}

//----------------------------------------------------------------------
// Method: getQuery
// Get the result (all the records) of a given query
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::getQuery(std::string qry,
                               std::vector< std::vector<std::string> > & table)
{
    return  (runCmd(qry) && fillWithResult(table));
}

//----------------------------------------------------------------------
// Method: getNumRowsInTable
// Get the content (all the records) from a given table
//----------------------------------------------------------------------
int DBHdlPostgreSQL::getNumRowsInTable(std::string tName)
{
    int nRows = -1;
    if (runCmd(std::string("SELECT COUNT(*) FROM " + tName))) {
        nRows = atoi(PQgetvalue(res, 0, 0));
    }
    PQclear(res);
    return nRows;
}

//----------------------------------------------------------------------
// Method: runCmd
// Runs a SQL command
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::runCmd(std::string cmd, bool clear)
{
    // Run the command
    if (clear) { PQclear(res); }
    res = PQexec(conn, cmd.c_str());
    if ((PQresultStatus(res) != PGRES_COMMAND_OK) &&
            (PQresultStatus(res) != PGRES_TUPLES_OK)) {
        logger.error("Failed cmd '" + cmd + "': " +
                     std::string(PQerrorMessage(conn)));
        PQclear(res);
        PQfinish(conn);
    }
    return true;
}

//----------------------------------------------------------------------
// Method: fillWithResult
// Retrieves the content of the returned n-tuples (after and SQL command)
// into the table parameter
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::fillWithResult(std::vector< std::vector<std::string> > & table)
{
    // Transfer the data to the table argument
    table.clear();
    int nFields = PQnfields(res);
    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        std::vector<std::string> row;
        for (int j = 0; j < nFields; ++j) {
            row.push_back(std::string(PQgetvalue(res, i, j)));
        }
        table.push_back(row);
    }
    PQclear(res);

    return true;
}

//----------------------------------------------------------------------
// Method: quotedValue
// Returns the value used in the update string
//----------------------------------------------------------------------
std::string DBHdlPostgreSQL::eqkv(std::string k, int x) 
{ return k + " = " + std::to_string(x); }

std::string DBHdlPostgreSQL::eqkv(std::string k, double x) 
{ return k + " = " + std::to_string(x); }

std::string DBHdlPostgreSQL::eqkv(std::string k, std::string x) 
{ return k + " = " + str::quoted(x); }

std::string DBHdlPostgreSQL::eqkv(std::string k, json x) 
{ return k + " = " + str::quoted(x.dump()) + "::json"; }

//----------------------------------------------------------------------
// Method: updateTable
// Method to update a series of fields of a table
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::updateTable(std::string table, std::string cond,
                                  std::vector<std::string> & newValues)
{
    std::string newValStr = newValues.at(0);
    for (int i = 1; i < newValues.size(); ++i) {
        newValStr += ", " + newValues.at(i);
    }
    std::string cmd("UPDATE " + table + " SET " + newValStr);
    if (!cond.empty()) { cmd += " WHERE " + cond; }
    //TRC("PSQL> " << cmd);
    return runCmd(cmd);
}

//----------------------------------------------------------------------
// Method: getVersionCounter
// Returns the process version counter for a given processor
//----------------------------------------------------------------------
int DBHdlPostgreSQL::getVersionCounter(std::string & procName)
{
    bool result;
    std::string cmd("SELECT counter FROM pvc "
                    "WHERE name LIKE " +
                    str::quoted(procName + "%") + " ORDER BY id "
                    "DESC LIMIT 1;");

    try { result = runCmd(cmd); } catch(...) { throw; }

    int cnt = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    return cnt;
}

//----------------------------------------------------------------------
// Method: checkSignature
// Check if a product with the same signature exists in the archive
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::checkSignature(std::string & sgnt, std::string & ptype, 
                                     std::string & ver)
{
    bool result = true;

    std::string cmd("SELECT product_version FROM products_info "
                    "WHERE signature LIKE " + str::quoted(sgnt + "%") +
                    " AND product_type = " + str::quoted(ptype) +
                    " AND ((NOW() - registration_time) > INTERVAL '10 sec')"
                    " ORDER BY id "
                    "DESC LIMIT 1;");

    //TRC("CHECKING VERSION: " + cmd);
    // The 10 sec of margin are taken to avoid to count as existing, by
    // the TskOrc, products that where just inserted, by the DataMng,
    // into the local archive, since TskOrc and DataMng are receiving
    // almost at the same time the same INDATA message

    try {
        result = runCmd(cmd);
        result = PQntuples(res) > 0;
        if (result) {
            ver = std::string(PQgetvalue(res, 0, 0));
        }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: retrieveRestartableTasks
// Retrieve tasks with status SCHEDULED and RUNNING
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveRestartableTasks(std::map<int,TaskInfo> & tasks)
{
    bool result = true;
    /*
    std::string cmd("SELECT id, task_info from tasks_info "
                    "WHERE task_status_id = " + std::to_string(TASK_SCHEDULED) +
                    "   OR task_status_id = " + std::to_string(TASK_RUNNING) +
                    "ORDER BY id;");

    try {
        result = runCmd(cmd);
        result = PQntuples(res) > 0;
        if (!result) {
                PQclear(res);
                return false;
        }
    } catch(...) {
        throw;
    }

    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i) {
        int id = atoi(PQgetvalue(res, i, 0));
        std::string content(PQgetvalue(res, i, 1));
        logger.debug("TaskInfo content is: " + content);
        std::string scontent(PQgetvalue(res, i, 1));
        json::Value tiv(scontent);
        TaskInfo * ti = new TaskInfo(tiv);
        tasks[id] = *ti;
        logger.debug("#########>>>>>> FOUND task with id = " + std::to_string(id));// +
        //                    " and status " + std::to_string(ti->taskStatus()));
    }

    // Register count of tasks with the statuses that are
    // to be restarted
    // SCHEDULED tasks have not been assigned to any TaskAgent yet
    std::map<TaskStatus, std::string> statuses {{TASK_RUNNING, "running"}};
    for (auto & kv: statuses) {
        const TaskStatus & s = kv.first;
        std::string & ss = kv.second;
        for (auto & ag: net.agentNames) {
            try {
                result = runCmd("SELECT COUNT(*) from tasks_info " 
                                " WHERE task_status_id = " + std::to_string(s) +
                                " AND task_data#>>'{Info,Agent}' = '" + ag + "';",
                                true);
                int numAb = atoi(PQgetvalue(res, 0, 0));
                TRC("Number of aborted tasks found for agent " + ag +
                    " and status " + TaskStatusName[s] + ": " +
                    std::to_string(numAb));
                // if (numAb > 0) {
                //     std::string cmd("UPDATE task_status_spectra "
                //                     "SET " + ss + " = " + ss + " - " + std::to_string(numAb) +
                //                     ", total = total - " + std::to_string(numAb) +
                //                     " WHERE agent_id = '" + ag + "';");
                //     TRC("Correcting: " + cmd);
                //     result = runCmd(cmd, true);
                // }
            } catch(...) {
                throw;
            }            
        }
    }
    
    // Clear status for these tasks
    cmd = ("UPDATE tasks_info "
           "SET   task_status_id = " + std::to_string(TASK_ABORTED) +
           "WHERE id IN "
           "(SELECT id from tasks_info "
           " WHERE task_status_id = " + std::to_string(TASK_SCHEDULED) +
           "    OR task_status_id = " + std::to_string(TASK_RUNNING) +
           " ORDER BY id);");
    
    try { result = runCmd(cmd, true); } catch(...) { throw; }    

    PQclear(res);
*/
    return result;
}

//----------------------------------------------------------------------
// Method: storeNodeState
// Stores the state of a node in the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeNodeState(std::string node,
                                     std::string new_state)
{
    bool result = true;

    std::string cmd("WITH upsert AS (UPDATE node_states "
                    "SET node_state=" + str::quoted(new_state) + " "
                    "WHERE node_name=" + str::quoted(node) + " "
                    "RETURNING *) "
                    "INSERT INTO node_states (node_name, node_state) "
                    "SELECT " + str::quoted(node) + ", " + str::quoted(new_state) + " "
                    "WHERE NOT EXISTS (SELECT * FROM upsert)");

    try { result = runCmd(cmd); } catch(...) { throw; }

    return result;
}

//----------------------------------------------------------------------
// Method: retrieveNodeState
// Retrieves the state of a node from the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveNodeState(std::string node,
                                        std::string & state)
{
    bool result = true;

    std::string cmd("SELECT node_state FROM node_states WHERE node_name=" + 
                    str::quoted(node));

    try {
        result = runCmd(cmd);
        result = PQntuples(res) > 0;
        if (result) {
            state = std::string(PQgetvalue(res, 0, 0));
        }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return result;
}

//----------------------------------------------------------------------
// Method: storeVar
// Stores a variable in the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::storeVar(std::string var,
                               std::string filter,
                               std::string new_value)
{
    bool result = true;
    std::string flt("");

    if (!filter.empty()) { 
        flt = "AND filter=" + str::quoted(filter) + " "; 
    }

    std::string cmd("WITH upsert AS (UPDATE qpf_vars "
                    "SET var_value=" + str::quoted(new_value) + " "
                    "WHERE var_name=" + str::quoted(var) + " " + flt + 
                    "RETURNING *) "
                    "INSERT INTO qpf_vars (var_name, filter, var_value) "
                    "SELECT " + str::quoted(var) + ", " + 
                    str::quoted(filter) + ", " +str::quoted(new_value) + " "
                    "WHERE NOT EXISTS (SELECT * FROM upsert)");

    try { result = runCmd(cmd); } catch(...) { throw; }

    return result;
}

//----------------------------------------------------------------------
// Method: retrieveVar
// Retrieves the value of a var from the database
//----------------------------------------------------------------------
bool DBHdlPostgreSQL::retrieveVar(std::string var,
                                  std::string filter,
                                  std::string & value)
{
    bool result = true;
    std::string flt("");

    if (!filter.empty()) { 
        flt = " AND filter=" + filter; 
    } else {
        filter = "''";
    }

    std::string cmd("SELECT var_value FROM qpf_vars WHERE var_name=" + 
                    str::quoted(var) + flt);

    try {
        result = runCmd(cmd);
        result = PQntuples(res) > 0;
        if (result) {
            value = std::string(PQgetvalue(res, 0, 0));
        }
    } catch(...) {
        throw;
    }

    PQclear(res);
    return result;
}
