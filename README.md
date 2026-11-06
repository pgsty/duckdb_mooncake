# duckdb_mooncake

[![Website: mooncake.pgsty.com](https://img.shields.io/badge/Website-mooncake.pgsty.com-slategray?style=flat&logo=cilium&logoColor=white)](https://mooncake.pgsty.com/)
[![Docs: duckdb_mooncake](https://img.shields.io/badge/Docs-duckdb__mooncake-slategray?style=flat)](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)
[![Version: v0.0.2](https://img.shields.io/badge/Version-v0.0.2-slategray?style=flat)](https://github.com/pgsty/duckdb_mooncake/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-slategray?style=flat&logo=opensourceinitiative&logoColor=green)](https://github.com/pgsty/duckdb_mooncake/blob/main/LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/pgsty/duckdb_mooncake?style=flat&logo=github&logoColor=black&color=slategray)](https://star-history.com/#pgsty/duckdb_mooncake&Date)
[![DuckDB: v2.0.0](https://img.shields.io/badge/DuckDB-v2.0.0-%23FFF000?style=flat&logo=duckdb&logoColor=black&labelColor=FFF000)](https://duckdb.org/)

**`duckdb_mooncake`** is a DuckDB storage and catalog extension that reads real-time Apache Iceberg tables written by [`moonlink`][moonlink-link] with sub-second LSN-consistent freshness, powering vectorized analytical queries in [`pg_mooncake`](https://github.com/pgsty/pg_mooncake) and standalone DuckDB.

[Website](https://mooncake.pgsty.com/) | [Documentation](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/) | [pg_mooncake](https://github.com/pgsty/pg_mooncake) | [moonlink][moonlink-link] | [Blog](https://mooncake.pgsty.com/blog/) | [PGEXT](https://pgext.cloud/ext/pg_mooncake) | [Releases](https://github.com/pgsty/duckdb_mooncake/releases) | [Issues](https://github.com/pgsty/duckdb_mooncake/issues) | [Upstream](https://github.com/Mooncake-Labs/duckdb_mooncake)

---

## Mooncake Suite

`duckdb_mooncake` is the DuckDB storage and scan engine of the **Mooncake** family maintained by [PGSTY](https://pgsty.com):

- **[`pg_mooncake`](https://github.com/pgsty/pg_mooncake)** ([Docs](https://mooncake.pgsty.com/docs/intro/) / [Meta](https://pgext.cloud/ext/pg_mooncake)): PostgreSQL extension (`v0.3.0`, MIT) providing `mooncake.create_table()`, logical replication workers, and transparent query routing via `pg_duckdb` `1.2.0`.
- **[`moonlink`][moonlink-link]** ([Docs](https://mooncake.pgsty.com/docs/intro/moonlink/)): Rust real-time Iceberg v3 ingestion engine (`v0.0.1`, BSL-1.1 with `pg_mooncake` Linking & Distribution Exception; client crates MIT).
- **[`duckdb_mooncake`](https://github.com/pgsty/duckdb_mooncake)** ([Docs](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)): DuckDB `v2.0.0` storage & catalog scanner extension (`v0.0.2`, MIT) merging in-memory Arrow IPC buffers, Roaring deletion vectors, and flushed Parquet files with LSN snapshot consistency.

---

## Version v0.0.2

Version `v0.0.2` is the first PGSTY release, targeting **DuckDB `v2.0.0`** (`d54cbbe3df17e8e5ce68312f483b4155836112aa`) and [`pg_mooncake v0.3.0`](https://github.com/pgsty/pg_mooncake/releases/tag/v0.3.0):

- **DuckDB `v2.0.0` C++ Storage & Catalog Migration**: Adapts `MooncakeCatalog`, `MooncakeTransactionManager`, `MultiFileReader`, `TableFilterSet` iteration, and `DUCKDB_CPP_EXTENSION_ENTRY` to DuckDB `2.0.0`.
- **PostgreSQL LSN Snapshot Barrier**: Resolves `mooncake_get_postgres_lsn()` dynamically via `dlsym(RTLD_DEFAULT, ...)` when loaded inside PostgreSQL (`pg_duckdb`) so `moonlink_create_scan_snapshot` waits for the current PostgreSQL WAL flush LSN before scanning.
- **Filter Pushdown Column-Index Remapping**: Remaps `IS NULL`, `IS NOT NULL`, and dynamic `TableFilterSet` keys from projection column order to physical Parquet file column indices before initializing `ParquetReader` scans.
- **Pinned & Audited FFI Client**: Pins `moonlink_ffi` (`moonlink_rpc`) to [`moonlink v0.0.1`](https://github.com/pgsty/moonlink/releases/tag/v0.0.1) (`b298ef7444890e3c8af5544e350232518bcfd8e5`) and upgrades fallback `CRoaring` to `v5.5.4`.

See [Adapting pg_mooncake to DuckDB 2.0 & PostgreSQL 18](https://mooncake.pgsty.com/blog/post/duckdb-2-adaptation/) and the [duckdb_mooncake Architecture Guide](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/) for details.

## Installation

When used with [`pg_mooncake v0.3.0`](https://github.com/pgsty/pg_mooncake), `mooncake.duckdb_extension` (`v0.0.2`, compiled for DuckDB `v2.0.0`) is staged into the local extension repository (`$(pg_config --sharedir)/extension/pg_mooncake`) and loaded automatically when `mooncake.extension_repository` is configured.

In standalone DuckDB, `duckdb_mooncake` can also be installed using the `INSTALL` command (note: the upstream DuckDB community repository currently hosts legacy `v0.0.1` for DuckDB `1.4.x`; for DuckDB `2.0.0`, load the locally built `mooncake.duckdb_extension`):
```sql
INSTALL duckdb_mooncake FROM community;
```

## Usage

Mooncake databases can be attached using the `ATTACH` command, after which tables can be queried using standard SQL.

The example below attaches to the moonlink database `'postgres'`, from a moonlink instance listening at `'/var/lib/postgresql/data/pg_mooncake/moonlink.sock'`. This moonlink instance comes prepopulated with a table named `public.c`:
```sql
D ATTACH DATABASE 'mooncake' (TYPE mooncake, URI '/var/lib/postgresql/data/pg_mooncake/moonlink.sock', DATABASE 'postgres');
D SELECT * FROM mooncake.public.c;
┌───────┬─────────┐
│  id   │   val   │
│ int32 │ varchar │
├───────┼─────────┤
│     1 │ Hello   │
│     2 │ World   │
└───────┴─────────┘
```

## Building

To build, type:
```bash
git submodule update --init --recursive
GEN=ninja make
```

To run, run the bundled `duckdb` shell:
```bash
./build/release/duckdb
```

---

## Documentation

Full documentation is available at **[mooncake.pgsty.com/docs/intro/duckdb-mooncake](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)**:

- [duckdb_mooncake Scanner Architecture (`v0.0.2`)](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)
- [Mooncake End-to-End Architecture](https://mooncake.pgsty.com/docs/intro/architecture/)
- [Adapting pg_mooncake to DuckDB 2.0 & PostgreSQL 18](https://mooncake.pgsty.com/blog/post/duckdb-2-adaptation/)
- [Unlocking DuckDB's Parallel Parquet Reader for Custom Table Functions](https://mooncake.pgsty.com/blog/post/duckdb-parquet/)

---

## License

This software is licensed under the **MIT License ([MIT](LICENSE))**.

- Copyright (c) 2018–2025 Stichting DuckDB Foundation
- Copyright (c) 2025 Mooncake Labs, Inc.
- Copyright (c) 2026 PGSTY PTE. LTD.
- Maintained by [Ruohang Feng](mailto:rh@vonng.com) @ [PGSTY](https://pgsty.com)

[moonlink-link]: https://github.com/pgsty/moonlink
