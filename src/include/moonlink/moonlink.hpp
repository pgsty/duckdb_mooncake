#pragma once

#include "moonlink/moonlink_ffi.hpp"

namespace duckdb {

class Moonlink {
public:
	Moonlink(string uri, string database);

	DataPtr GetTableSchema(const string &schema, const string &table);

	StreamPtr Connect() const;
	DataPtr ScanTableBegin(Stream &stream, const string &schema, const string &table, uint64_t lsn);

private:
	string uri;
	string database;
};

} // namespace duckdb
