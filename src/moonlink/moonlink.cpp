#include "moonlink/moonlink.hpp"

namespace duckdb {

Moonlink::Moonlink(string uri, string database) : uri(std::move(uri)), database(std::move(database)) {
}

StreamPtr Moonlink::Connect() const {
	return StreamPtr(moonlink_connect(uri.c_str()).Unwrap());
}

DataPtr Moonlink::GetTableSchema(const string &schema, const string &table) {
	auto stream = Connect();
	return DataPtr(moonlink_get_table_schema(stream.get(), database.c_str(), schema.c_str(), table.c_str()).Unwrap());
}

DataPtr Moonlink::ScanTableBegin(Stream &stream, const string &schema, const string &table, uint64_t lsn) {
	return DataPtr(moonlink_scan_table_begin(&stream, database.c_str(), schema.c_str(), table.c_str(), lsn).Unwrap());
}

} // namespace duckdb
